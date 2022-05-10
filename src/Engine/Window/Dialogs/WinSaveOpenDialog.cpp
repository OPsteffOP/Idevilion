// Folder: Window/Dialogs

#include "EnginePCH.h"
#include "WinSaveOpenDialog.h"

#ifdef BP_PLATFORM_WINDOWS

#include <ShlObj_core.h>

namespace _Internal
{
#pragma comment(lib, "shlwapi.lib")

#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC

	class DialogEventHandler : public IFileDialogEvents
	{
	public:
		IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);
		IFACEMETHODIMP_(ULONG) AddRef();
		IFACEMETHODIMP_(ULONG) Release();

		IFACEMETHODIMP OnFileOk(IFileDialog*) { return S_OK; }
		IFACEMETHODIMP OnFolderChange(IFileDialog*) { return S_OK; }
		IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*) { return S_OK; }
		IFACEMETHODIMP OnHelp(IFileDialog*) { return S_OK; }
		IFACEMETHODIMP OnSelectionChange(IFileDialog*) { return S_OK; }
		IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*) { return S_OK; }
		IFACEMETHODIMP OnTypeChange(IFileDialog*) { return S_OK; }
		IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*) { return S_OK; }

		DialogEventHandler() : _cRef(1) {};
	private:
		long _cRef;
	};

	IFACEMETHODIMP DialogEventHandler::QueryInterface(REFIID riid, void** ppv)
	{
		static const QITAB qit[] = {
			QITABENT(DialogEventHandler, IFileDialogEvents),
			QITABENT(DialogEventHandler, IFileDialogControlEvents),
			{ 0 },
		};
		return QISearch(this, qit, riid, ppv);
	}

	IFACEMETHODIMP_(ULONG) DialogEventHandler::AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	IFACEMETHODIMP_(ULONG) DialogEventHandler::Release()
	{
		long cRef = InterlockedDecrement(&_cRef);
		if (!cRef)
			delete this;
		return cRef;
	}

	HRESULT DialogEventHandler_CreateInstance(REFIID riid, void** ppv)
	{
		*ppv = NULL;
		DialogEventHandler* pDialogEventHandler = new DialogEventHandler();
		HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
		if (SUCCEEDED(hr))
		{
			hr = pDialogEventHandler->QueryInterface(riid, ppv);
			pDialogEventHandler->Release();
		}
		return hr;
	}
}

WinSaveOpenDialog::WinSaveOpenDialog(SaveOpenDialogFlags flags, const std::vector<DialogFileType>& fileTypes, const std::string& defaultExtension, const std::string& defaultFolder)
	: SaveOpenDialog()
	, m_pFileDialog(nullptr)
	, m_pFileDialogEvent(nullptr)
	, m_EventCookie()
{
	// https://docs.microsoft.com/en-us/windows/win32/shell/common-file-dialog#basic-usage
	// https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/winui/shell/appplatform/commonfiledialog/CommonFileDialogApp.cpp

	HRESULT hr;

	m_pFileDialog = NULL;
	hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pFileDialog));
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to open save/open dialog (couldn't create file dialog)");
		return;
	}

	m_pFileDialogEvent = NULL;
	hr = _Internal::DialogEventHandler_CreateInstance(IID_PPV_ARGS(&m_pFileDialogEvent));
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to open save/open dialog (couldn't create file dialog events)");
		return;
	}

	hr = m_pFileDialog->Advise(m_pFileDialogEvent, &m_EventCookie);
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to open save/open dialog (couldn't link file dialog events to file dialog)");
		return;
	}

	hr = m_pFileDialog->SetOptions(ConvertDialogFlags(flags));
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to open save/open dialog (couldn't set file dialog flags)");
		return;
	}

	std::vector<std::pair<std::wstring, std::wstring>> temp;
	std::vector<COMDLG_FILTERSPEC> allowedFileTypes;
	for (const DialogFileType& fileType : fileTypes)
	{
		// Storing it in a temporary vector to make sure the temporary std::wstrings aren't destroyed when leaving this for loop scope
		const std::pair<std::wstring, std::wstring>& tempPair = temp.emplace_back(Utils::StringToWideString(fileType.name), Utils::StringToWideString(fileType.type));

		COMDLG_FILTERSPEC filter;
		filter.pszName = tempPair.first.c_str();
		filter.pszSpec = tempPair.second.c_str();
		allowedFileTypes.push_back(filter);
	}
	hr = m_pFileDialog->SetFileTypes((uint)allowedFileTypes.size(), allowedFileTypes.data());
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to open save/open dialog (couldn't set allowed file types)");
		return;
	}

	hr = m_pFileDialog->SetFileTypeIndex(0);
	if (FAILED(hr))
	{
		LOG_WARNING("Failed to set file type index while creating save/open dialog");
	}

	std::wstring wDefaultExtension = Utils::StringToWideString(defaultExtension);
	hr = m_pFileDialog->SetDefaultExtension(wDefaultExtension.c_str());
	if (FAILED(hr))
	{
		LOG_WARNING("Failed to set default extension while creating save/open dialog, default_extension=%s", defaultExtension.c_str());
	}

	if (!defaultFolder.empty())
	{
		if (std::filesystem::exists(defaultFolder))
		{
			PIDLIST_ABSOLUTE pidl;

			const std::wstring wDefaultFolder = Utils::StringToWideString(defaultFolder);
			hr = SHParseDisplayName(wDefaultFolder.c_str(), NULL, &pidl, SFGAO_FOLDER, NULL);
			if (FAILED(hr))
			{
				LOG_ERROR("Failed to open save/open dialog (couldn't set default folder)");
				return;
			}

			IShellItem* pShellItem;
			hr = SHCreateShellItem(NULL, NULL, pidl, &pShellItem);
			if (FAILED(hr))
			{
				LOG_ERROR("Failed to open save/open dialog (couldn't set default folder)");
				return;
			}

			hr = m_pFileDialog->SetDefaultFolder(pShellItem);
			if (FAILED(hr))
			{
				LOG_ERROR("Failed to open save/open dialog (couldn't set default folder)");
				return;
			}

			hr = m_pFileDialog->SetFolder(pShellItem);
			if (FAILED(hr))
			{
				LOG_WARNING("Couldn't set folder while trying to create save/open dialog");
			}
		}
		else
		{
			LOG_WARNING("Default folder for save/open dialog does not exists, not setting default folder instead, folder=%s", defaultFolder.c_str());
		}
	}
}

WinSaveOpenDialog::~WinSaveOpenDialog()
{
	if (m_pFileDialog != nullptr)
		m_pFileDialog->Unadvise(m_EventCookie);

	SAFE_RELEASE(m_pFileDialogEvent);
	SAFE_RELEASE(m_pFileDialog);
}

void WinSaveOpenDialog::Show()
{
	HRESULT hr = m_pFileDialog->Show(NULL);
	if (SUCCEEDED(hr))
	{
		IShellItem* pShellItem;
		hr = m_pFileDialog->GetResult(&pShellItem);
		if (SUCCEEDED(hr))
		{
			PWSTR pszFilePath = NULL;
			hr = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
			if (SUCCEEDED(hr))
			{
				m_SelectedPath = Utils::WideStringToString(std::wstring(pszFilePath));
				pShellItem->Release();
			}
		}
	}
}

uint WinSaveOpenDialog::ConvertDialogFlags(SaveOpenDialogFlags flags) const
{
	uint result = 0;

	if (flags & SaveOpenDialogFlags::PROMPT_ON_OVERWRITE)
		result |= FOS_OVERWRITEPROMPT;
	if (flags & SaveOpenDialogFlags::FORCE_FILE_TYPES)
		result |= FOS_STRICTFILETYPES;
	if (flags & SaveOpenDialogFlags::NO_CHANGE_DIRECTORY)
		result |= FOS_NOCHANGEDIR;
	if (flags & SaveOpenDialogFlags::SELECT_FOLDERS)
		result |= FOS_PICKFOLDERS;
	if (flags & SaveOpenDialogFlags::ALLOW_MULTISELECT)
		result |= FOS_ALLOWMULTISELECT;
	if (flags & SaveOpenDialogFlags::FORCE_PATH_EXISTANCE)
		result |= FOS_PATHMUSTEXIST;
	if (flags & SaveOpenDialogFlags::FORCE_FILE_EXISTANCE)
		result |= FOS_FILEMUSTEXIST;
	if (flags & SaveOpenDialogFlags::NO_READ_ONLY)
		result |= FOS_NOREADONLYRETURN;

	return result;
}

#endif
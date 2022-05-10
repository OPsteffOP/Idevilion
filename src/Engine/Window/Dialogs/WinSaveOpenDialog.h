// Folder: Window/Dialogs

#pragma once
#ifdef BP_PLATFORM_WINDOWS

#include "SaveOpenDialog.h"

struct IFileDialog;
struct IFileDialogEvents;

class WinSaveOpenDialog : public SaveOpenDialog
{
public:
	WinSaveOpenDialog(SaveOpenDialogFlags flags, const std::vector<DialogFileType>& fileTypes, const std::string& defaultExtension, const std::string& defaultFolder = "");
	virtual ~WinSaveOpenDialog() override;

	virtual void Show() override;

private:
	uint ConvertDialogFlags(SaveOpenDialogFlags flags) const;

private:
	IFileDialog* m_pFileDialog;
	IFileDialogEvents* m_pFileDialogEvent;
	DWORD m_EventCookie;
};

#endif
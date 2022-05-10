// Folder: Window/Dialog

#include "EnginePCH.h"
#include "SaveOpenDialog.h"

#include "WinSaveOpenDialog.h"

SaveOpenDialog* SaveOpenDialog::Create(SaveOpenDialogFlags flags, const std::vector<DialogFileType>& fileTypes, const std::string& defaultExtension, const std::string& defaultFolder)
{
#ifdef BP_PLATFORM_WINDOWS
	return new WinSaveOpenDialog(flags, fileTypes, defaultExtension, defaultFolder);
#endif
}
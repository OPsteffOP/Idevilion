// Folder: Window/Dialog

#pragma once

DEFINE_ENUM_FLAGS(SaveOpenDialogFlags,
	PROMPT_ON_OVERWRITE = 1 << 0,
	FORCE_FILE_TYPES = 1 << 1,
	NO_CHANGE_DIRECTORY = 1 << 2,
	SELECT_FOLDERS = 1 << 3,
	ALLOW_MULTISELECT = 1 << 4,
	FORCE_PATH_EXISTANCE = 1 << 5,
	FORCE_FILE_EXISTANCE = 1 << 6,
	NO_READ_ONLY = 1 << 7
);

class SaveOpenDialog
{
public:
	struct DialogFileType
	{
		std::string name;
		std::string type;
	};

public:
	static SaveOpenDialog* Create(SaveOpenDialogFlags flags, const std::vector<DialogFileType>& fileTypes, const std::string& defaultExtension, const std::string& defaultFolder = "");

	SaveOpenDialog() = default;
	virtual ~SaveOpenDialog() = default;

	virtual void Show() = 0;

	const std::string& GetSelectedPath() { return m_SelectedPath; }

protected:
	std::string m_SelectedPath;
};
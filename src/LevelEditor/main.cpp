#include "LevelEditorPCH.h"

#include "LevelEditor.h"

int main(int argc, char* argv[])
{
	LeakDetector::Initialize();

	{
		CommandLine::Initialize(argc, argv);

		LevelEditor levelEditor;
		levelEditor.Start();
	}

	LeakDetector::GetInstance()->DumpLeaks();
}
#include "DatabaseWriterPCH.h"
#include "DatabaseWriteDialog.h"

int main(int argc, char* argv[])
{
	LeakDetector::Initialize();

	{
		CommandLine::Initialize(argc, argv);

		DatabaseWriteDialog dialog;
		dialog.Start();
	}

	LeakDetector::GetInstance()->DumpLeaks();
	return 0;
}
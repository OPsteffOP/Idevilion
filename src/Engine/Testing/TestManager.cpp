// Folder: Testing

#include "EnginePCH.h"
#include "TestManager.h"

#include <iostream>

TestManager* TestManager::GetInstance()
{
	static TestManager* pInstance = nullptr;
	if (pInstance == nullptr)
	{
		ALLOW_NEXT_ALLOCATION_LEAK();
		pInstance = new TestManager();
	}

	return pInstance;
}

bool TestManager::ExecuteTests()
{
	bool didAllTestsPass = true;

	for (const std::pair<std::string, std::vector<TestData*>>& pair : m_Tests)
	{
		const std::string& testModuleName = pair.first;
		PrintModuleName(testModuleName);

		bool moduleStatus = true;
		for (TestData* pTest : pair.second)
		{
			const bool didTestPass = pTest->m_Func();
			moduleStatus &= didTestPass;

			PrintTestInfo(pTest->m_FuncName);

			if (didTestPass)
				PrintSuccess();
			else
				PrintFailure();
		}

		PrintModuleStatus();
		if (moduleStatus)
			PrintSuccess();
		else
			PrintFailure();

		std::cout << '\n';
		didAllTestsPass &= moduleStatus;
	}

	return didAllTestsPass;
}

void TestManager::AddTest(const std::string& moduleName, TestData* pTest)
{
	m_Tests[moduleName].push_back(pTest);
}

void TestManager::PrintModuleName(const std::string& moduleName) const
{
	std::cout << "[TESTS]\t\tExecuting \"" << moduleName << "\" tests...\n";
}

void TestManager::PrintModuleStatus() const
{
	std::cout << "[TESTS]\t\t" << "Module status : ";
}

void TestManager::PrintTestInfo(const std::string& testName) const
{
	std::cout << "[TESTS]\t\t  > " << std::left << std::setw(35) << testName << ": ";
}

void TestManager::PrintSuccess() const
{
#ifdef BP_PLATFORM_WINDOWS
	WORD defaultColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	WORD successColor = FOREGROUND_GREEN | FOREGROUND_INTENSITY;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, successColor);
#endif

	std::cout << std::right << "succeeded\n";

#ifdef BP_PLATFORM_WINDOWS
	SetConsoleTextAttribute(hConsole, defaultColor);
#endif
}

void TestManager::PrintFailure() const
{
#ifdef BP_PLATFORM_WINDOWS
	WORD defaultColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	WORD failureColor = FOREGROUND_RED;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, failureColor);
#endif

	std::cout << std::right << "failure\n";

#ifdef BP_PLATFORM_WINDOWS
	SetConsoleTextAttribute(hConsole, defaultColor);
#endif
}
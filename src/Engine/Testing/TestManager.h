// Folder: Testing

#pragma once

#include "TestRegisterer.h"

class TestManager
{
public:
	static TestManager* GetInstance();

	bool ExecuteTests();

	void AddTest(const std::string& moduleName, TestData* pTest);

private:
	void PrintModuleName(const std::string& moduleName) const;
	void PrintModuleStatus() const;
	void PrintTestInfo(const std::string& testName) const;
	void PrintSuccess() const;
	void PrintFailure() const;

private:
	std::unordered_map<std::string, std::vector<TestData*>> m_Tests;
};

#define RUN_TESTS() TestManager::GetInstance()->ExecuteTests()

#define TEST_FUNCTION_BEGIN_MODULE(moduleName, funcName)												\
bool funcName()																							\
{																										\
	bool _testStatus = true;

#define TEST_FUNCTION_END_MODULE(moduleName, funcName)													\
	return _testStatus;																					\
}																										\
static TestRegisterer _##funcName##Registerer(#moduleName, #funcName, funcName);

#define TEST_FUNCTION_BEGIN(funcName) TEST_FUNCTION_BEGIN_MODULE(default, funcName)
#define TEST_FUNCTION_END(funcName) TEST_FUNCTION_END_MODULE(default, funcName)

#define TEST_VALIDATE(result, expected) _testStatus &= ((result) == (expected));
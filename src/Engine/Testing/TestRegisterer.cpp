// Folder: Testing

#include "EnginePCH.h"
#include "TestRegisterer.h"

#include "TestManager.h"

TestRegisterer::TestRegisterer(const std::string& moduleName, const std::string& funcName, TestData::TestFunction func)
	: m_Data{ funcName, func }
{
	TestManager::GetInstance()->AddTest(moduleName, &m_Data);
}
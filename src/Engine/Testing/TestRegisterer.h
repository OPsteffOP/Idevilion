// Folder: Testing

#pragma once

struct TestData
{
	typedef bool(*TestFunction)();

	std::string m_FuncName;
	TestFunction m_Func;
};

class TestRegisterer
{
public:
	TestRegisterer(const std::string& moduleName, const std::string& funcName, TestData::TestFunction func);

private:
	TestData m_Data;
};
// Folder: Utils

#pragma once

template<typename T>
class RAIIMod
{
public:
	RAIIMod(T& variable, T newValue);
	~RAIIMod();

private:
	T* m_pPtr;
	T m_OldValue;
};

template<typename T>
RAIIMod<T>::RAIIMod(T& variable, T newValue)
	: m_pPtr(&variable)
	, m_OldValue(variable)
{
	variable = newValue;
}

template<typename T>
RAIIMod<T>::~RAIIMod()
{
	*m_pPtr = m_OldValue;
}
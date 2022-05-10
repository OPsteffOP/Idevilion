// Folder: Graphics/Core

#pragma once
#include "BufferEnums.h"

class Buffer
{
public:
	Buffer(uint elementSize, uint elementCount, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags);

	virtual ~Buffer() = default;

	virtual void UpdateData(const void* pData) = 0;
	virtual void UpdateData(const void* pData, uint size) = 0;

protected:
	const uint m_ElementSize;
	const uint m_ElementCount;
	const BufferUsage m_BufferUsage;
	const ShaderAccessFlags m_ShaderAccess;
	const CPUAccessFlags m_CPUAccess;
	const BindFlags m_BindFlags;
};
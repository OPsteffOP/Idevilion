// Folder: Graphics/Core

#include "EnginePCH.h"
#include "Buffer.h"

Buffer::Buffer(uint elementSize, uint elementCount, BufferUsage usage, ShaderAccessFlags shaderAccess, CPUAccessFlags cpuAccess, BindFlags bindFlags)
	: m_ElementSize(elementSize)
	, m_ElementCount(elementCount)
	, m_BufferUsage(usage)
	, m_ShaderAccess(shaderAccess)
	, m_CPUAccess(cpuAccess)
	, m_BindFlags(bindFlags)
{
	// TODO: VALIDATE ALL PROPERTIES
}
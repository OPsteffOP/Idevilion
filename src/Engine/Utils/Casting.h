// Folder: Utils
#pragma once

namespace Utils
{
	template<typename T, typename U>
	T CheckCast(U* pOriginal)
	{
		DEBUG_ASSERT(dynamic_cast<T>(pOriginal) != nullptr, "CheckCast resulted in an unsafe cast");
		return static_cast<T>(pOriginal);
	}
}
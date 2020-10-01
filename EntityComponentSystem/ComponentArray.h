#pragma once

namespace ecs {
/**
 * \brief ���Ԃ�ێ����Ȃ��z��
 * \tparam T �^
 */
template <class T>
class ComponentArray
{
public:
	ComponentArray(T* pBegin, const std::size_t size)
	{
		mpBegin = pBegin;
		mSize = size;
	}

	T& operator[](const int index)
	{
		return mpBegin[index];
	}

	T* begin()
	{
		return mpBegin;
	}
	T* end()
	{
		return mpBegin + mSize;
	}
	
private:
	T* mpBegin = nullptr;
	std::size_t mSize = 0;
};
}
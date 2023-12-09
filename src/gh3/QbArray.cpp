#include "QbArray.h"

namespace GH3
{

	/*QbArray::QbArray()
	{
		static const void *function = (void *)0x0055DE70;
		__asm jmp function;
	}*/
	/*__declspec(naked) int* __thiscall QbArray::Initialize(int size, int type)
	{
		static const void *function = (void *)0x0055DF00;
		__asm jmp function;
	}*/

	int* __thiscall QbArray::Initialize(int size, int type) {
		static const void* function = (void*)0x0055DF00;
		__asm pop ecx; // stupid
		__asm pop ebp; // stupid
		__asm jmp function;
	}

	void QbArray::Clear()
	{
		SetType(QbValueType::TypeUndefined);
		arr = nullptr;
		size = 0;
		unk1 = 0;
	}

	uint32_t QbArray::operator[](int index) const
	{
		if (size != 1)
			return arr[index];
		else
			return reinterpret_cast<uint32_t>(arr);
	}

	uint32_t & QbArray::operator[](int index)
	{
		if (Length() == 1)
			return reinterpret_cast<uint32_t &>(arr);
		else
			return arr[index];
	}

}
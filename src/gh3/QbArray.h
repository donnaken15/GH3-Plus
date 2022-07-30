#pragma once

#include "core\GH3Plus.h"

#include "QbValueType.h"
#include <stdint.h>

namespace GH3
{

	struct GH3P_API QbArray final
	{

	private:
		uint8_t unk0;
		uint8_t unk1;

	public:
		QbValueType type;
	private:
		uint8_t unk3;

	public:
		uint32_t size;
		uint32_t* arr;

		int * __thiscall Initialize(int size, int type);

		void Clear();

		uint32_t operator [](int index) const;
		uint32_t & operator [](int index);

		inline void Set(int index, uint32_t value) { (*this)[index] = value; }
		inline uint32_t Get(int index) const { return (*this)[index]; }

		inline void SetType(QbValueType _type) { type = _type; }
		inline QbValueType Type() const { return type;}
		inline uint32_t Length() const { return size;}
	};

}
#pragma once

typedef void* NodeDataPtr;
#ifdef _DEBUG
#include <assert.h>
#endif // DEBUG

namespace NodeData
{
	template <class Type>
	Type Get(void* someData)
	{
#ifdef _DEBUG
		assert(someData);
#endif // DEBUG
		return *(static_cast<Type*>(someData));
	}
}

#define COL32_R_SHIFT    0
#define COL32_G_SHIFT    8
#define COL32_B_SHIFT    16
#define COL32_A_SHIFT    24
#define COL32_A_MASK     0xFF000000
#define COL32(R,G,B,A)    (((unsigned int)(A)<<COL32_A_SHIFT) | ((unsigned int)(B)<<COL32_B_SHIFT) | ((unsigned int)(G)<<COL32_G_SHIFT) | ((unsigned int)(R)<<COL32_R_SHIFT))
#define COL32_WHITE       COL32(255,255,255,255)
#define COL32_BLACK       COL32(0,0,0,255)
#define COL32_BLACK_TRANS COL32(0,0,0,0)
#pragma once

#include "BasicTypes.h"

template<typename TYPE, p_int MAX_POINTS> struct GameStack
{
	p_int Count;
	TYPE Stack[MAX_POINTS];

	inline GameStack()
	{
		Clear();
	}

	inline void Clear()
	{
		Count = 0;
	}

	inline void Push(TYPE x)
	{
		Stack[Count++] = x;
	}

	inline TYPE Pop()
	{
		Count--;
		return Stack[Count];
	}

	inline void Copy(const GameStack &Orig)
	{
		Count = Orig.Count;
		for (p_int i = 0; i < Orig.Count; i++)
			Stack[i] = Orig.Stack[i];
	}

	inline const p_int Find(const TYPE x)
	{
		for (p_int i = 0; i < Count; i++)
			if (Stack[i] == x)
				return i;
		return -1;
	}

	inline void Intersect(GameStack &Orig)
	{
		GameStack TempStack;
		TempStack.Copy(*this);

		this->Clear();
		for (p_int i = 0; i < TempStack.Count; i++)
			if (Orig.Find(TempStack.Stack[i]) != -1)
				this->Push(TempStack.Stack[i]);
	}

	inline void Union(GameStack &Orig)
	{
		for (p_int i = 0; i < Orig.Count; i++)
			if (Find(Orig.Stack[i]) == -1)
				Push(Orig.Stack[i]);
	}
};
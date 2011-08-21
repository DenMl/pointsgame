#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "static_vector.h"
#include "Zobrist.h"

class Trajectory : private static_vector<pos, MAX_TRAJECTORY_LENGTH>
{
private:
	size_t _hash;

public:
	bool Excluded;

	inline Trajectory() : static_vector()
	{
		_hash = 0;
		Excluded = false;
	}
	using static_vector<pos, MAX_TRAJECTORY_LENGTH>::size;
	using static_vector<pos, MAX_TRAJECTORY_LENGTH>::begin;
	using static_vector<pos, MAX_TRAJECTORY_LENGTH>::end;
	inline void clear()
	{
		static_vector::clear();
		_hash = 0;
		Excluded = false;
	}
	inline void push_back(pos Pos)
	{
		static_vector::push_back(Pos);
		_hash ^= GetZobristHash(Pos);
	}

	inline void Copy(const Trajectory &Orig)
	{
		assign(Orig.begin(), Orig.end());
		_hash = Orig._hash;
		Excluded = Orig.Excluded;
	}
	inline void Copy(const Trajectory &Orig, const pos Pos)
	{
		clear();
		for (auto i = Orig.begin(); i < Orig.end(); i++)
			if (*i != Pos)
				push_back(*i);
		_hash = Orig._hash ^ GetZobristHash(Pos);
		Excluded = Orig.Excluded;
	}
	inline size_t get_hash()
	{
		return _hash;
	}
};
#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "static_vector.h"
#include "Zobrist.h"

class Trajectory : private static_vector<pos, MAX_TRAJECTORY_LENGTH>
{
private:
	size_t _hash;
	bool _excluded;

public:
	inline Trajectory() : static_vector()
	{
		_hash = 0;
		_excluded = false;
	}
	using static_vector<pos, MAX_TRAJECTORY_LENGTH>::size;
	using static_vector<pos, MAX_TRAJECTORY_LENGTH>::begin;
	using static_vector<pos, MAX_TRAJECTORY_LENGTH>::end;
	inline void clear()
	{
		static_vector::clear();
		_hash = 0;
		_excluded = false;
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
		_excluded = Orig.excluded();
	}
	inline void Copy(const Trajectory &Orig, const pos Pos)
	{
		clear();
		for (auto i = Orig.begin(); i < Orig.end(); i++)
			if (*i != Pos)
				push_back(*i);
		_hash = Orig.hash() ^ GetZobristHash(Pos);
		_excluded = Orig.excluded();
	}
	inline size_t hash() const
	{
		return _hash;
	}
	inline void exclude()
	{
		_excluded = true;
	}
	inline void include()
	{
		_excluded = false;
	}
	inline bool excluded() const
	{
		return _excluded;
	}
};
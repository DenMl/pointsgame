#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "Zobrist.h"
#include <list>

class trajectory
{
private:
	list<pos> _points;
	size_t _hash;
	bool _excluded;

public:
	inline trajectory()
	{
		_hash = 0;
		_excluded = false;
	}
	size_t size() const
	{
		return _points.size();
	}
	list<pos>::iterator begin()
	{
		return _points.begin();
	}
	list<pos>::iterator end()
	{
		return _points.end();
	}
	list<pos>::const_iterator begin() const
	{
		return _points.begin();
	}
	list<pos>::const_iterator end() const
	{
		return _points.end();
	}
	inline void clear()
	{
		_points.clear();
		_hash = 0;
		_excluded = false;
	}
	inline void push_back(pos Pos)
	{
		_points.push_back(Pos);
		_hash ^= GetZobristHash(Pos);
	}

	inline void Copy(const trajectory &Orig)
	{
		_points.assign(Orig.begin(), Orig.end());
		_hash = Orig._hash;
		_excluded = Orig.excluded();
	}
	inline void Copy(const trajectory &Orig, const pos Pos)
	{
		clear();
		for (auto i = Orig.begin(); i != Orig.end(); i++)
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
#pragma once

#include "config.h"
#include "basic_types.h"
#include "Random.h"

using namespace std;

class zobrist
{
private:
	size_t _size;
	size_t* _hashes;

public:
	inline zobrist(size_t size)
	{
		_size = size;
		_hashes = new size_t[size];
		Randomize();
		fill_n(_hashes, size, Random());
	}
	inline zobrist(const zobrist &other)
	{
		_size = other._size;
		_hashes = new size_t[other._size];
		copy_n(other._hashes, other._size, _hashes);
	}
	inline ~zobrist()
	{
		delete _hashes;
	}
	inline size_t get_hash(size_t num)
	{
		return _hashes[num];
	}
	zobrist& operator =(const zobrist &other)
	{
		_size = other._size;
		_hashes = new size_t[other._size];
		copy_n(other._hashes, other._size, _hashes);
		return *this;
	}
};
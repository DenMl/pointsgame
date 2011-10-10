#pragma once

#include "config.h"
#include "basic_types.h"
#include "Random.h"

using namespace std;

// Хеш Зобриста.
class zobrist
{
private:
	// Размер таблицы хешей.
	size_t _size;
	// Хеши.
	hash_t *_hashes;

public:
	// Конструктор.
	inline zobrist(size_t size)
	{
		_size = size;
		_hashes = new hash_t[size];
		Randomize();
		for (size_t i = 0; i < size; i++)
			_hashes[i] = Random();
	}
	// Конструктор копирования.
	inline zobrist(const zobrist &other)
	{
		_size = other._size;
		_hashes = new hash_t[other._size];
		copy_n(other._hashes, other._size, _hashes);
	}
	// Деструктор.
	inline ~zobrist()
	{
		delete _hashes;
	}
	// Возвращает хеш Зобриста элемента num.
	inline hash_t get_hash(size_t num)
	{
		return _hashes[num];
	}
};
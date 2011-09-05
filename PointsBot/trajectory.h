#pragma once

#include "Config.h"
#include "basic_types.h"
#include "Zobrist.h"
#include <list>

class trajectory
{
private:
	list<pos> _points;
	size_t _hash;
	bool _excluded;

public:
	inline trajectory() { _hash = 0; _excluded = false; }
	inline trajectory(const trajectory &other) { _points = other._points; _hash = other._hash; _excluded = other._excluded; }
	inline size_t size() const { return _points.size(); }
	inline bool empty() const { return _points.empty(); }
	inline void push_back(pos Pos) { _points.push_back(Pos); _hash ^= GetZobristHash(Pos); }
	inline void clear() { _points.clear(); _hash = 0; _excluded = false; }
	inline const trajectory& operator =(const trajectory &other) { _points = other._points; _hash = other._hash; _excluded = other._excluded; return *this; }
	inline void swap(trajectory &other) { trajectory tmp(*this); *this = other; other = tmp; }
	list<pos>::iterator begin() { return _points.begin(); }
	list<pos>::const_iterator begin() const { return _points.begin(); }
	list<pos>::iterator end() { return _points.end(); }
	list<pos>::const_iterator end() const { return _points.end(); }
	list<pos>::reverse_iterator rbegin() { return _points.rbegin(); }
	list<pos>::reverse_iterator rend() { return _points.rend(); }
	list<pos>::const_reverse_iterator rbegin() const { return _points.rbegin(); }
	list<pos>::const_reverse_iterator rend() const { return _points.rend(); }

	inline size_t hash() const { return _hash; }
	inline void exclude() { _excluded = true; }
	inline void include() { _excluded = false; }
	inline bool excluded() const { return _excluded; }

	// Проверяет, во все ли точки траектории можно сделать ход.
	inline bool is_valid(field &cur_field) const
	{
		for (auto i = _points.begin(); i != _points.end(); i++)
			if (!cur_field.putting_allow(*i))
				return false;
		return true;
	}
	// Проверяет, во все ли точки траектории можно сделать ход, кроме, возможно, точки Pos.
	inline bool is_valid(field &cur_field, pos cur_pos) const
	{
		for (auto i = _points.begin(); i != _points.end(); i++)
			if (*i != cur_pos && !cur_field.putting_allow(*i))
				return false;
		return true;
	}
};
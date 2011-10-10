#pragma once

#include "config.h"
#include "basic_types.h"
#include "Zobrist.h"
#include <list>

class trajectory
{
private:
	list<pos> _points;
	zobrist *_zobrist;
	size_t _hash;
	bool _excluded;

public:
	inline trajectory(zobrist &cur_zobrist) { _hash = 0; _excluded = false; _zobrist = &cur_zobrist; }
	template<typename _InIt> inline trajectory(_InIt first, _InIt last, zobrist &cur_zobrist) { _hash = 0; _excluded = false; _zobrist = &cur_zobrist; assign(first, last); }
	template<typename _InIt> inline trajectory(_InIt first, _InIt last, zobrist &cur_zobrist, size_t hash) { _hash = 0; _excluded = false; _zobrist = &cur_zobrist; assign(first, last, hash); }
	inline trajectory(const trajectory &other) { _points = other._points; _hash = other._hash; _excluded = other._excluded; _zobrist = other._zobrist; }
	inline size_t size() const { return _points.size(); }
	inline bool empty() const { return _points.empty(); }
	inline void push_back(pos cur_pos) { _points.push_back(cur_pos); _hash ^= _zobrist->get_hash(cur_pos); }
	inline void clear() { _points.clear(); _hash = 0; _excluded = false; }
	inline const trajectory& operator =(const trajectory &other) { _points = other._points; _hash = other._hash; _excluded = other._excluded; _zobrist = other._zobrist; return *this; }
	inline void swap(trajectory &other) { trajectory tmp(*this); *this = other; other = tmp; }
	inline list<pos>::iterator begin() { return _points.begin(); }
	inline list<pos>::const_iterator begin() const { return _points.begin(); }
	inline list<pos>::iterator end() { return _points.end(); }
	inline list<pos>::const_iterator end() const { return _points.end(); }
	inline list<pos>::reverse_iterator rbegin() { return _points.rbegin(); }
	inline list<pos>::reverse_iterator rend() { return _points.rend(); }
	inline list<pos>::const_reverse_iterator rbegin() const { return _points.rbegin(); }
	inline list<pos>::const_reverse_iterator rend() const { return _points.rend(); }

	template<typename _InIt>
	inline void assign(_InIt first, _InIt last)
	{
		for (auto i = first; i != last; i++)
			push_back(*i, _zobrist->get_hash(*i));
	}
	template<typename _InIt>
	inline void assign(_InIt first, _InIt last, size_t hash)
	{
		_points.assign(first, last);
		_hash = hash;
	}

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
	// Проверяет, во все ли точки траектории можно сделать ход, кроме, возможно, точки cur_pos.
	inline bool is_valid(field &cur_field, pos cur_pos) const
	{
		for (auto i = _points.begin(); i != _points.end(); i++)
			if (*i != cur_pos && !cur_field.putting_allow(*i))
				return false;
		return true;
	}
};
#pragma once

#include "Config.h"
#include <iterator>

template<typename value_type, typename difference_type, typename pointer, typename reference>
class static_vector_iterator : public iterator<random_access_iterator_tag, value_type, difference_type, pointer, reference>
{
private:
	value_type *p;
public:

	typedef size_t size_type;

	static_vector_iterator()
	{
		p = 0;
	}

	static_vector_iterator(const value_type* other)
	{
		p = const_cast<value_type*>(other);
	}

	static_vector_iterator(const static_vector_iterator& other)
	{
		p = other.p;
	}

	~static_vector_iterator()
	{
	}

	static_vector_iterator& operator =(const static_vector_iterator& other)
	{
		p = other.p;
		return *this;
	}

	pointer operator ->() const
	{
		return &**p;
	}

	reference operator *() const
	{
		return *p;
	}

	static_vector_iterator& operator ++()
	{
		++p;
		return *this;
	}

	static_vector_iterator operator ++(int)
	{
		static_vector_iterator tmp(*this);
		p++;
		return tmp;
	}

	static_vector_iterator& operator --()
	{
		--p;
		return *this;
	}

	static_vector_iterator operator --(int)
	{
		static_vector_iterator tmp(*this);
		p--;
		return tmp;
	}

	static_vector_iterator& operator +=(const difference_type& idx)
	{
		p += idx;
		return *this;
	}

	static_vector_iterator& operator -=(const difference_type& idx)
	{
		p -= idx;
		return *this;
	}

	static_vector_iterator operator +(const size_type& idx) const
	{
		static_vector_iterator tmp(*this);
		return tmp += idx;
	}

	static_vector_iterator operator -(const size_type& idx) const
	{
		static_vector_iterator tmp(*this);
		return tmp -= idx;
	}

	reference operator [](const size_type& idx) const
	{
		return *(p + idx);
	}

	bool operator <(const static_vector_iterator& other) const
	{
		return p < other.p;
	}

	bool operator >(const static_vector_iterator& other) const
	{
		return p > other.p;
	}

	bool operator ==(const static_vector_iterator& other) const
	{
		return p == other.p;
	}

	bool operator !=(const static_vector_iterator& other) const
	{
		return p != other.p;
	}

	bool operator <=(const static_vector_iterator& other) const
	{
		return p <= other.p;
	}

	bool operator >=(const static_vector_iterator& other) const
	{
		return p >= other.p;
	}

	difference_type operator -(const static_vector_iterator& other) const
	{
		return p - other.p;
	}

};

template<class _Tp, size_t _S> class static_vector
{
public:
	typedef _Tp value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef _Tp* pointer;
	typedef const _Tp* const_pointer;
	typedef _Tp& reference;
	typedef const _Tp& const_reference;
	typedef static_vector_iterator<value_type, difference_type, pointer, reference> iterator;
	typedef static_vector_iterator<value_type, difference_type, const_pointer, const_reference> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

private:
	value_type _array[_S];
	size_type _size;

public:
	static_vector()
	{
		_size = 0;
	}
	static_vector(const size_type size, const value_type& val = value_type())
	{
		_size = size;
		for (size_type i = 0; i < size; i++)
			_array[i] = val;
	}
	static_vector(const static_vector &other)
	{
		_size = other._size;
		for (size_type i = 0; i < _size; i++)
			_array[i] = other._array[i];
	}
	template<typename _InputIterator>
	static_vector(_InputIterator first, _InputIterator last)
	{
		assign(first, last);
	}
	~static_vector()
	{
	}
	inline size_type size() const
	{
		return _size;
	}
	inline bool empty() const
	{
		return _size == 0;
	}
	inline void fast_resize(size_type size)
	{
		_size = size;
	}
	inline void resize(size_type size, const value_type &val = value_type())
	{
		for (size_type i = _size; i < size; i++)
			_array[i] = val;
		_size = size;
	}
	inline reference operator [](const size_type idx)
	{
		return _array[idx];
	}
	inline const_reference operator [](const size_type idx) const
	{
		return _array[idx];
	}
	inline void push_back(const value_type &val)
	{
		_array[_size] = val;
		_size++;
	}
	inline void clear()
	{
		_size = 0;
	}
	inline const static_vector& operator =(const static_vector &other)
	{
		if (this == &other)
			return *this;
		_size = other._size;
		for (size_type i = 0; i < _size; i++)
			_array[i] = other._array[i];
	}
	inline void swap(static_vector &other)
	{
		static_vector<value_type> tmp(*this);
		*this = other;
		other = tmp;
	}
	inline iterator begin()
	{
		return iterator(_array);
	}
	inline const_iterator begin() const
	{
		return const_iterator(_array);
	}
	inline iterator end()
	{
		return iterator(_array + _size);
	}
	inline const_iterator end() const
	{
		return const_iterator(_array + _size);
	}
	inline reverse_iterator rbegin()
	{
		return reverse_iterator(_array + _size);
	}
	inline reverse_iterator rend()
	{
		return reverse_iterator(_array);
	}
	inline const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(_array + _size);
	}
	inline const_reverse_iterator rend() const
	{
		return const_reverse_iterator(_array);
	}
	inline reference back()
	{
		return *(_array + _size - 1);
	}
	inline const_reference back() const
	{
		return *(_array + _size - 1);
	}
	inline reference front()
	{
		return *_array;
	}
	inline const_reference front() const
	{
		return *_array;
	}
	inline void pop_back()
	{
		_size--;
	}
	inline reference at(const size_type idx)
	{
		return _array[idx];
	}
	inline const_reference at(const size_type idx) const
	{
		return _array[idx];
	}
	inline void assign(const size_type size, const value_type &val)
	{
		_size = size;
		for (size_type i = 0; i < size; i++)
			_array[i] = val;
	}
	template<typename _InputIterator>
	inline void assign(_InputIterator first, _InputIterator last)
	{
		_size = 0;
		for (_InputIterator i = first; i != last; i++)
			push_back(*i);
	}
	inline iterator erase(iterator position)
	{
		if (position == end())
			return position;
		while (++position != end())
			*(position - 1) = *position;
		_size--;
	}

// 	iterator erase(iterator first, iterator last)
// 	{
// 		if(first==last)
// 			return end();
// 		iterator ptr=stdext::unchecked_copy(last, end(), begin());
// 		last=ptr;
// 		Size-=last-first;
// 		return first;
// 	}
// 
// 	iterator insert(iterator Where, const value_type& elem)
// 	{
// 		if(Where>=end())
// 			bad_iter_range_end(Where);
// 		else if(Where<=begin())
// 			bad_iter_range_begin(Where);
// 		size_type offs=Where-begin();
// 		copy(1, offs, elem);
// 		return (begin()+offs);
// 	}
// 
// 	iterator insert(iterator Where, const value_type& elem, const size_type& count)
// 	{
// 		if(Where>=end())
// 			bad_iter_range_end(Where);
// 		else if(Where<=begin())
// 			bad_iter_range_begin(Where);
// 		size_t offs=Where-begin(Where);
// 		copy(count, offs, elem);
// 		return (begin()+offs);
// 	}
// 
// 	iterator insert(iterator Where, iterator first, iterator last)
// 	{
// 		if(first==last)
// 			return end();
// 		if(Where>=end())
// 			bad_iter_range_end(Where);
// 		else if(Where<=begin())
// 			bad_iter_range_begin(Where);
// 		size_t offs=Where-begin();
// 		difference_type sz=last-first;
// 		copy(sz, offs, 0, first, last);
// 		return (begin()+offs);
// 	}


// 	template<class Type, class AllocType>
// 	inline bool operator ==(const Vector<Type, AllocType>& First, 
// 		const Vector<Type, AllocType>& Second)
// 	{
// 		return (First.size()==Second.size()&&
// 			std::equal(First.begin(), First.end(), Second.begin()));
// 	}
// 
// 	template<class Type, class AllocType>
// 	inline bool operator !=(const Vector<Type, AllocType>& First,
// 		const Vector<Type, AllocType>& Second)
// 	{
// 		return (!(First==Second));
// 	}
// 
// 	template<class Type, class AllocType>
// 	inline bool operator <(const Vector<Type, AllocType>& First,
// 		const Vector<Type, AllocType>& Second)
// 	{
// 		return std::lexicographical_compare(First.begin(), First.end(), Second.begin(), Second.end());
// 	}
// 
// 	template<class Type, class AllocType>
// 	inline bool operator >(const Vector<Type, AllocType>& First,
// 		const Vector<Type, AllocType>& Second)
// 	{
// 		return Second<First;
// 	}
// 
// 	template<class Type, class AllocType>
// 	inline bool operator <=(const Vector<Type, AllocType>& First,
// 		const Vector<Type, AllocType>& Second)
// 	{
// 		return (!(Second<First));
// 	}
// 
// 	template<class Type, class AllocType>
// 	inline bool operator >=(const Vector<Type, AllocType>& First,
// 		const Vector<Type, AllocType>& Second)
// 	{
// 		return (!(First<Second));
// 	}
};
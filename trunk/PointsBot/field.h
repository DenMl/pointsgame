#pragma once

#include "Config.h"
#include "BasicTypes.h"
#include "BasicConstants.h"
#include "Zobrist.h"
#include "AuxFunc.h"
#include "Player.h"
#include <stack>
#include <list>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;
using std::tr1::function;

class field
{
#pragma region State bits and masks
private:

	// Бит, указывающий номер игрока.
	static const value player_bit = 0x1;
	// Бит, указывающий на наличие точки в поле.
	static const value put_bit = 0x2;
	// Бит, указывающий на захваченность точки.
	static const value sur_bit = 0x4;
	// Бит, указывающий на то, захватывает ли что-нибудь точка на поле.
	static const value bound_bit = 0x8;
	// Бит, указывающий на пустую базу.
	static const value empty_base_bit = 0x10;
	// Бит для временных пометок полей.
	static const value tag_bit = 0x20;
	// Бит, которым помечаются границы поля.
	static const value bad_bit = 0x40;

	// Маски для определения условий.
	static const value enable_mask = bad_bit | sur_bit | put_bit | player_bit;
	static const value bound_mask = bad_bit | bound_bit | sur_bit | put_bit | player_bit;
#pragma endregion

#pragma region Get and set state functions
public:
	// Get state functions.
	// Функции получения состояния.

	// Получить по координате игрока, чья точка там поставлена.
	inline player get_player(const pos cur_pos) const { return _points[cur_pos] & player_bit; }
	// Проверить по координате, поставлена ли там точка.
	inline bool is_putted(const pos cur_pos) const { return (_points[cur_pos] & put_bit) != 0; }
	// Прверить по координате, является ли точка окружающей базу.
	inline bool is_base_bound(const pos cur_pos) const { return (_points[cur_pos] & bound_bit) != 0; }
	// Проверить по координате, захвачено ли поле.
	inline bool is_captured(const pos cur_pos) const { return (_points[cur_pos] & sur_bit) != 0; }
	// Проверить по координате, лежит ли она в пустой базе.
	inline bool is_in_empty_base(const pos cur_pos) const { return (_points[cur_pos] & empty_base_bit) != 0; }
	// Проверить по координате, помечено ли поле.
	inline bool is_tagged(const pos cur_pos) const { return (_points[cur_pos] & tag_bit) != 0; }
	// Получить условие по координате.
	inline value get_enable_cond(const pos cur_pos) const { return _points[cur_pos] & enable_mask; }
	// Проверка незанятости поля по условию.
	inline bool is_enable(const pos cur_pos, const value enable_cond) const { return (_points[cur_pos] & enable_mask) == enable_cond; }
	// Проверка занятости поля по условию.
	inline bool is_not_enable(const pos cur_pos, const value enable_cond) const { return (_points[cur_pos] & enable_mask) != enable_cond; }
	// Проверка на то, захвачено ли поле.
	inline bool is_bound(const pos cur_pos, const value bound_cond) const { return (_points[cur_pos] & bound_mask) == bound_cond; }
	// Проверка на то, не захвачено ли поле.
	inline bool is_not_bound(const pos cur_pos, const value bound_cond) const { return (_points[cur_pos] & bound_mask) != bound_cond; }
	// Провека на то, возможно ли поставить точку в полке.
	inline bool putting_allow(const pos cur_pos) const { return !(_points[cur_pos] & (put_bit | sur_bit | bad_bit)); }

	// Set state functions.
	// Функции установки состояния.

	// Пометить поле по координате как содержащее точку.
	inline void set_putted(const pos cur_pos) { _points[cur_pos] |= put_bit; }
	// Убрать с поля по координате put_bit.
	inline void clear_put_bit(const pos cur_pos) { _points[cur_pos] &= ~put_bit; }
	// Пометить поле по координате как принадлежащее игроку.
	inline void set_player(const pos cur_pos, const player cur_player) { _points[cur_pos] = (_points[cur_pos] & ~player_bit) | cur_player; }
	// Пометить поле по координате как содержащее точку игрока.
	inline void set_player_putted(const pos cur_pos, const player cur_player) { _points[cur_pos] |= cur_player | put_bit; }
	// Пометить битом SurBit (захватить).
	inline void capture(const pos cur_pos) { _points[cur_pos] |= sur_bit; }
	// Убрать бит SurBit.
	inline void free(const pos cur_pos) { _points[cur_pos] &= ~sur_bit; }
	// Пометить точку как окружающую базу.
	inline void set_base_bound(const pos cur_pos) { _points[cur_pos] |= bound_bit; }
	// Пометить точку как не окружающую базу.
	inline void clear_base_bound(const pos cur_pos) { _points[cur_pos] &= ~bound_bit; }
	inline void set_empty_base(const pos cur_pos) { _points[cur_pos] |= empty_base_bit; }
	inline void clear_empty_base(const pos cur_pos) { _points[cur_pos] &= ~empty_base_bit; }
	// Установить бит TagBit.
	inline void set_tag(const pos cur_pos) { _points[cur_pos] |= tag_bit; }
	// Убрать бит TagBit.
	inline void clear_tag(const pos cur_pos) { _points[cur_pos] &= ~tag_bit; }
	inline void set_bad(const pos cur_pos) { _points[cur_pos] |= bad_bit; }
	inline void clear_bad(const pos Pos) { _points[Pos] &= ~bad_bit; }
#pragma endregion

#pragma region Main variables
private:
	vector<board_change> _changes;

	// Main points array (game board).
	// Основной массив точек (игровая доска).
	value* _points;

	// Real field width.
	// Действительная ширина поля.
	coord _width;
	// Real field height.
	// Действительная высота поля.
	coord _height;

	// Правила обработки пустых баз.
	// SurStandart = 0 - если PlayerRed ставит в пустую базу и ничего не обводит, то PlayerBlack обводит эту территорию.
	// SurAlways = 1 - обводить базу, даже если нет вражеских точек внутри.
	// SurAlwaysEnemy = 2 - обводит всегда PlayerBlack, если PlayerRed поставил точку в пустую базу.
	sur_cond _sur_cond;

	// Current player color.
	// Текущий цвет игроков.
	player _player;

	// Capture points count.
	// Количество захваченных точек.
	score capture_count[2];

	size_t _hash;

public:
	// History points sequance.
	// Последовательность поставленных точек.
	vector<pos> points_seq;
#pragma endregion

private:
	// Возвращает косое произведение векторов Pos1 и Pos2.
	inline static int square(const pos Pos1, const pos Pos2);
	//  * . .   x . *   . x x   . . .
	//  . o .   x o .   . o .   . o x
	//  x x .   . . .   . . *   * . x
	//  o - center pos
	//  x - pos
	//  * - result
	inline void GetFirstNextPos(const pos CenterPos, pos &Pos) const;
	//  . . .   * . .   x * .   . x *   . . x   . . .   . . .   . . .
	//  * o .   x o .   . o .   . o .   . o *   . o x   . o .   . o .
	//  x . .   . . .   . . .   . . .   . . .   . . *   . * x   * x .
	//  o - center pos
	//  x - pos
	//  * - result
	inline void GetNextPos(const pos CenterPos, pos &Pos) const;
	// Возвращает количество групп точек рядом с CenterPos.
	// InpChainPoints - возможные точки цикла, InpSurPoints - возможные окруженные точки.
	inline short GetInputPoints(const pos CenterPos, const value EnableCond, pos InpChainPoints[], pos InpSurPoints[]) const;
	// Поставить начальные точки.
	void place_begin_pattern(begin_pattern cur_pattern);
	// Изменение счета игроков.
	inline void AddSubCapturedFreed(const player Player, const score Captured, const score Freed);
	// Изменяет Captured/Free в зависимости от того, захвачена или окружена точка.
	inline void CheckCapturedAndFreed(const pos Pos, const player Player, score &Captured, score &Freed);
	// Захватывает/освобождает окруженную точку.
	inline void SetCaptureFreeState(const pos Pos, const player Player);
	inline void Wave(pos StartPos, function<bool(pos)> Condition);
	// Удаляет пометку пустой базы с поля точек, начиная с позиции StartPos.
	void remove_empty_base(const pos StartPos);
	bool BuildChain(const pos StartPos, const value EnableCond, const pos DirectionPos, list<pos> &Chain);
	void FindSurround(list<pos> &Chain, pos InsidePoint, player Player);
	inline void UpdateHash(player Player, short Surrounded, pos Pos);
	inline intersection_state get_intersection_state(const pos Pos, const pos NextPos) const;

public:
	// Конструктор.
	field(const coord FieldWidth, const coord FieldHeight, const sur_cond SurCond, const begin_pattern BeginPattern);
	// Конструктор копирования.
	field(const field &Orig);
	~field();

	inline score get_score(player Player) const { return capture_count[Player] - capture_count[next_player(Player)]; }
	inline score get_d_score(player cur_player) const { return _changes.size() > 0 ? capture_count[cur_player] - _changes.back().capture_count[cur_player] : capture_count[cur_player]; }
	inline score get_d_score() const { return get_d_score(next_player(_player)); }
	inline player get_player() const { return _player; }
	inline sur_cond get_sur_cond() const { return _sur_cond; }
	inline size_t get_hash() const { return _hash; }
	inline coord width() const { return _width; }
	inline coord height() const { return _height; }
	inline pos length() const { return (_width + 2) * (_height + 2); }
	inline pos min_pos() const { return to_pos(0, 0); }
	inline pos max_pos() const { return to_pos(_width - 1, _height - 1); }
	inline pos n(pos cur_pos) const { return cur_pos - (_width + 2); }
	inline pos s(pos cur_pos) const { return cur_pos + (_width + 2); }
	inline pos w(pos cur_pos) const { return cur_pos - 1; }
	inline pos e(pos cur_pos) const { return cur_pos + 1; }
	inline pos nw(pos cur_pos) const { return cur_pos - (_width + 2) - 1; }
	inline pos ne(pos cur_pos) const { return cur_pos - (_width + 2) + 1; }
	inline pos sw(pos cur_pos) const { return cur_pos + (_width + 2) - 1; }
	inline pos se(pos cur_pos) const { return cur_pos + (_width + 2) + 1; }
	inline pos to_pos(const coord X, const coord Y) const { return (Y + 1) * _width + X + 1; }
	inline coord to_x(const pos cur_pos) const { return static_cast<coord>(cur_pos % _width - 1); }
	inline coord to_y(const pos cur_pos) const { return static_cast<coord>(cur_pos / _width - 1); }
	// Конвертация из Pos в XY.
	inline void to_xy(const pos cur_pos, coord &x, coord &y) const { x = to_x(cur_pos); y = to_y(cur_pos); }
	// Поставить точку на поле следующего по очереди игрока.
	inline bool do_step(const pos Pos)
	{
		if (putting_allow(Pos))
		{
			do_unsafe_step(Pos);
			return true;
		}
		return false;
	}
	// Поставить точку на поле.
	inline bool do_step(const pos Pos, const player Player)
	{
		if (putting_allow(Pos))
		{
			do_unsafe_step(Pos, Player);
			return true;
		}
		return false;
	}
	// Поставить точку на поле максимально быстро (без дополнительных проверок).
	inline void do_unsafe_step(const pos Pos)
	{
		_changes.push_back(board_change());
		_changes.back().capture_count[0] = capture_count[0];
		_changes.back().capture_count[1] = capture_count[1];
		_changes.back().player = _player;

		// Добавляем в изменения поставленную точку.
		_changes.back().changes.push(pair<pos, value>(Pos, _points[Pos]));

		set_player(Pos, _player);
		set_putted(Pos);

		points_seq.push_back(Pos);

		CheckClosure(Pos, _player);

		set_next_player();
	}
	// Поставить точку на поле следующего по очереди игрока максимально быстро (без дополнительных проверок).
	inline void do_unsafe_step(const pos Pos, const player Player)
	{
		_changes.push_back(board_change());
		_changes.back().capture_count[0] = capture_count[0];
		_changes.back().capture_count[1] = capture_count[1];
		_changes.back().player = _player;

		// Добавляем в изменения поставленную точку.
		_changes.back().changes.push(pair<pos, value>(Pos, _points[Pos]));

		set_player(Pos, Player);
		set_putted(Pos);

		points_seq.push_back(Pos);

		CheckClosure(Pos, Player);
	}

	// Делает ход и проверяет на окруженность только точку CheckedPos.
	inline bool do_unsafe_step_and_check_point(const pos Pos, const player Player, const pos CheckedPos)
	{
		_changes.push_back(board_change());
		_changes.back().capture_count[0] = capture_count[0];
		_changes.back().capture_count[1] = capture_count[1];
		_changes.back().player = _player;

		// Добавляем в изменения поставленную точку.
		_changes.back().changes.push(pair<pos, value>(Pos, _points[Pos]));

		set_player(Pos, Player);
		set_putted(Pos);

		points_seq.push_back(Pos);
		
		return CheckClosure(Pos, CheckedPos);
	}

	// Откат хода.
	inline void undo_step()
	{
		points_seq.pop_back();
		while (!_changes.back().changes.empty())
		{
			_points[_changes.back().changes.top().first] = _changes.back().changes.top().second;
			_changes.back().changes.pop();
		}
		_player = _changes.back().player;
		capture_count[0] = _changes.back().capture_count[0];
		capture_count[1] = _changes.back().capture_count[1];
		_changes.pop_back();
	}

	// Установить следующего игрока как текущего.
	inline void set_next_player()
	{
		_player = next_player(_player);
	}

	inline void set_player(const player cur_player)
	{
		_player = cur_player;
	}

	// Проверяет, находятся ли две точки рядом.
	inline bool is_near(const pos pos1, const pos pos2) const
	{
		if (n(pos1) == pos2  ||
			s(pos1) == pos2  ||
			w(pos1) == pos2  ||
			e(pos1) == pos2  ||
			nw(pos1) == pos2 ||
			ne(pos1) == pos2 ||
			sw(pos1) == pos2 ||
			se(pos1) == pos2)
			return true;
		else
			return false;
	}
	// Проверяет, есть ли рядом с Pos точки цвета Player.
	inline bool is_near_points(const pos Pos, const player Player) const
	{
		if (is_enable(n(Pos), put_bit | Player)  ||
			is_enable(s(Pos), put_bit | Player)  ||
			is_enable(w(Pos), put_bit | Player)  ||
			is_enable(e(Pos), put_bit | Player)  ||
			is_enable(nw(Pos), put_bit | Player) ||
			is_enable(ne(Pos), put_bit | Player) ||
			is_enable(sw(Pos), put_bit | Player) ||
			is_enable(se(Pos), put_bit | Player))
			return true;
		else
			return false;
	}
	// Возвращает количество точек рядом с Pos цвета Player.
	inline short number_near_points(const pos Pos, const player Player) const
	{
		short result = 0;
		if (is_enable(n(Pos), put_bit | Player))
			result++;
		if (is_enable(s(Pos), put_bit | Player))
			result++;
		if (is_enable(w(Pos), put_bit | Player))
			result++;
		if (is_enable(e(Pos), put_bit | Player))
			result++;
		if (is_enable(nw(Pos), put_bit | Player))
			result++;
		if (is_enable(ne(Pos), put_bit | Player))
			result++;
		if (is_enable(sw(Pos), put_bit | Player))
			result++;
		if (is_enable(se(Pos), put_bit | Player))
			result++;
		return result;
	}

	// Возвращает количество групп точек рядом с CenterPos.
	inline short number_near_groups(const pos CenterPos, const player Player) const
	{
		short result = 0;
		if (is_not_enable(w(CenterPos), Player | put_bit) && (is_enable(nw(CenterPos), Player | put_bit) || is_enable(n(CenterPos), Player | put_bit)))
			result++;
		if (is_not_enable(s(CenterPos), Player | put_bit) && (is_enable(sw(CenterPos), Player | put_bit) || is_enable(w(CenterPos), Player | put_bit)))
			result++;
		if (is_not_enable(e(CenterPos), Player | put_bit) && (is_enable(se(CenterPos), Player | put_bit) || is_enable(s(CenterPos), Player | put_bit)))
			result++;
		if (is_not_enable(n(CenterPos), Player | put_bit) && (is_enable(ne(CenterPos), Player | put_bit) || is_enable(e(CenterPos), Player | put_bit)))
			result++;
		return result;
	}

	bool is_point_inside_ring(const pos TestedPos, const list<pos> &Ring) const
	{
		uint intersections = 0;

		intersection_state state = IS_NONE;

		for (auto i = Ring.begin(); i != Ring.end(); i++)
		{
			switch (get_intersection_state(TestedPos, *i))
			{
			case (IS_NONE):
				state = IS_NONE;
				break;
			case (IS_UP):
				if (state == IS_DOWN)
					intersections++;
				state = IS_UP;
				break;
			case (IS_DOWN):
				if (state == IS_UP)
					intersections++;
				state = IS_DOWN;
				break;
			}
		}
		if (state == IS_UP || state == IS_DOWN)
		{
			auto i = Ring.begin();
			intersection_state TempState = get_intersection_state(TestedPos, *i);
			while (TempState == state || TempState == IS_TARGET)
			{
				i++;
				TempState = get_intersection_state(TestedPos, *i);
			}
			if (TempState != IS_NONE)
				intersections++;
		}

		return intersections % 2 == 1;
	}

	// Проверяет поставленную точку на наличие созданных ею окружений, и окружает, если они есть.
	void CheckClosure(const pos StartPos, player Player)
	{
		short InpPointsCount;
		pos InpChainPoints[4], InpSurPoints[4];

		list<pos> Chain;

		if (is_in_empty_base(StartPos)) // Если точка поставлена в пустую базу.
		{
			// Проверяем, в чью пустую базу поставлена точка.
			pos Pos = StartPos - 1;
			while (!is_putted(Pos))
				Pos--;

			if (get_player(Pos) == get_player(StartPos)) // Если поставили в свою пустую базу.
			{
				clear_empty_base(StartPos);
				return;
			}

#if SURROUND_CONDITIONS
			if (_sur_cond != SC_ALWAYS_ENEMY) // Если приоритет не всегда у врага.
#endif
			{
				InpPointsCount = GetInputPoints(StartPos, Player | put_bit, InpChainPoints, InpSurPoints);
				if (InpPointsCount > 1)
				{
					short ChainsCount = 0;
					for (short i = 0; i < InpPointsCount; i++)
						if (BuildChain(StartPos, get_player(StartPos) | put_bit, InpChainPoints[i], Chain))
						{
							FindSurround(Chain, InpSurPoints[i], Player);
							ChainsCount++;
							if (ChainsCount == InpPointsCount - 1)
								break;
						}
					if (is_base_bound(StartPos))
					{
						remove_empty_base(StartPos);
						return;
					}
				}
			}

			Pos++;
			do
			{
				Pos--;
				while (!is_enable(Pos, next_player(Player) | put_bit))
					Pos--;
				InpPointsCount = GetInputPoints(Pos, next_player(Player) | put_bit, InpChainPoints, InpSurPoints);
				for (short i = 0; i < InpPointsCount; i++)
					if (BuildChain(Pos, next_player(Player) | put_bit, InpChainPoints[i], Chain))
						if (is_point_inside_ring(StartPos, Chain))
						{
							FindSurround(Chain, InpSurPoints[i], get_player(Pos));
							break;
						}
			} while (!is_captured(StartPos));
		}
		else
		{
			InpPointsCount = GetInputPoints(StartPos, Player | put_bit, InpChainPoints, InpSurPoints);
			if (InpPointsCount > 1)
			{
				short ChainsCount = 0;
				for (short i = 0; i < InpPointsCount; i++)
					if (BuildChain(StartPos, get_player(StartPos) | put_bit, InpChainPoints[i], Chain))
					{
						FindSurround(Chain, InpSurPoints[i], Player);
						ChainsCount++;
						if (ChainsCount == InpPointsCount - 1)
							break;
					}
			}
		}
	}

	// Проверяет поставленную точку, не окружает ли она позицию CheckedPos.
	inline bool CheckClosure(const pos StartPos, const pos CheckedPos)
	{
		pos InpChainPoints[4], InpSurPoints[4];

		list<pos> Chain;

		// Цвет игрока, точка которого проверяется.
		short Player = get_player(StartPos);

		short InpPointsCount = GetInputPoints(StartPos, Player | put_bit, InpChainPoints, InpSurPoints);
		if (InpPointsCount > 1)
		{
			for (ushort i = 0; i < InpPointsCount; i++)
				if (BuildChain(StartPos, get_player(StartPos) | put_bit, InpChainPoints[i], Chain))
					if (is_point_inside_ring(CheckedPos, Chain))
					{
						for (auto j = Chain.begin(); j != Chain.end(); j++)
						{
							// Добавляем в список изменений точки цепочки.
							_changes.back().changes.push(pair<pos, value>(*j, _points[*j]));
							// Помечаем точки цепочки.
							set_base_bound(*j);
						}
						return true;
					}
		}

		return false;
	}
};


inline int field::square(const pos Pos1, const pos Pos2)
{
	return (Pos1 % FieldWidth2) * (Pos2 / FieldWidth2) - (Pos1 / FieldWidth2) * (Pos2 % FieldWidth2);
}
inline void field::GetFirstNextPos(const pos CenterPos, pos &Pos) const
{
	if (Pos < CenterPos)
	{
		if ((Pos == nw(CenterPos)) || (Pos == CenterPos - 1))
			Pos = ne(CenterPos); 
		else
			Pos = se(CenterPos);
	}
	else
	{
		if ((Pos == CenterPos + 1) || (Pos == se(CenterPos)))
			Pos = sw(CenterPos);
		else
			Pos = nw(CenterPos);
	}
}
inline void field::GetNextPos(const pos CenterPos, pos &Pos) const
{
	if (Pos < CenterPos)
	{
		if (Pos == nw(CenterPos))
			Pos = n(CenterPos);
		else if (Pos == n(CenterPos))
			Pos = ne(CenterPos);
		else if (Pos == ne(CenterPos))
			Pos = e(CenterPos);
		else
			Pos = nw(CenterPos);
	}
	else
	{
		if (Pos == e(CenterPos))
			Pos = se(CenterPos);
		else if (Pos == se(CenterPos))
			Pos = s(CenterPos);
		else if (Pos == s(CenterPos))
			Pos = sw(CenterPos);
		else
			Pos = w(CenterPos);
	}
}
inline short field::GetInputPoints(const pos CenterPos, const value EnableCond, pos InpChainPoints[], pos InpSurPoints[]) const
{
	short result = 0;

	if (is_not_enable(w(CenterPos), EnableCond))
	{
		if (is_enable(nw(CenterPos), EnableCond))
		{
			InpChainPoints[0] = nw(CenterPos);
			InpSurPoints[0] = w(CenterPos);
			result++;
		}
		else if (is_enable(n(CenterPos), EnableCond))
		{
			InpChainPoints[0] = n(CenterPos);
			InpSurPoints[0] = w(CenterPos);
			result++;
		}
	}

	if (is_not_enable(s(CenterPos), EnableCond))
	{
		if (is_enable(sw(CenterPos), EnableCond))
		{
			InpChainPoints[result] = sw(CenterPos);
			InpSurPoints[result] = s(CenterPos);
			result++;
		}
		else if (is_enable(w(CenterPos), EnableCond))
		{
			InpChainPoints[result] = w(CenterPos);
			InpSurPoints[result] = s(CenterPos);
			result++;
		}
	}

	if (is_not_enable(e(CenterPos), EnableCond))
	{
		if (is_enable(se(CenterPos), EnableCond))
		{
			InpChainPoints[result] = se(CenterPos);
			InpSurPoints[result] = e(CenterPos);
			result++;
		}
		else if (is_enable(s(CenterPos), EnableCond)) 
		{
			InpChainPoints[result] = s(CenterPos);
			InpSurPoints[result] = e(CenterPos);
			result++;
		}
	}

	if (is_not_enable(n(CenterPos), EnableCond))
	{
		if (is_enable(ne(CenterPos), EnableCond))
		{
			InpChainPoints[result] = ne(CenterPos);
			InpSurPoints[result] = n(CenterPos);
			result++;
		}
		else if (is_enable(e(CenterPos), EnableCond))
		{
			InpChainPoints[result] = e(CenterPos);
			InpSurPoints[result] = n(CenterPos);
			result++;
		}
	}

	return result;
}
inline void field::AddSubCapturedFreed(const player Player, const score Captured, const score Freed)
{
	if (Captured == -1)
	{
		capture_count[next_player(Player)]++;
	}
	else
	{
		capture_count[Player] += Captured;
		capture_count[next_player(Player)] -= Freed;
	}
}
inline void field::CheckCapturedAndFreed(const pos Pos, const player Player, score &Captured, score &Freed)
{
	if (is_putted(Pos))
	{
		if (get_player(Pos) != Player)
			Captured++;
		else if (is_captured(Pos))
			Freed++;
	}
}
inline void field::SetCaptureFreeState(const pos Pos, const player Player)
{
	if (is_putted(Pos))
	{
		if (get_player(Pos) != Player)
			capture(Pos);
		else
			free(Pos);
	}
	else
		capture(Pos);
}
inline void field::Wave(pos StartPos, function<bool(pos)> Condition)
{
	// Очередь для волнового алгоритма (обхода в ширину).
	list<pos> q;

	q.push_back(StartPos);
	set_tag(StartPos);
	auto it = q.begin();
	while (it != q.end())
	{
		if (!is_tagged(w(*it)) && Condition(w(*it)))
		{
			q.push_back(w(*it));
			set_tag(w(*it));
		}
		if (!is_tagged(n(*it)) && Condition(n(*it)))
		{
			q.push_back(n(*it));
			set_tag(n(*it));
		}
		if (!is_tagged(e(*it)) && Condition(e(*it)))
		{
			q.push_back(e(*it));
			set_tag(e(*it));
		}
		if (!is_tagged(s(*it)) && Condition(s(*it)))
		{
			q.push_back(s(*it));
			set_tag(s(*it));
		}
		it++;
	}

	for (it = q.begin(); it != q.end(); it++)
		clear_tag(*it);
}
inline void field::UpdateHash(player Player, short Surrounded, pos Pos)
{
	_hash ^= GetZobristHash(Player, Surrounded, Pos);
}
inline intersection_state field::get_intersection_state(const pos Pos, const pos NextPos) const
{
	point a, b;
	to_xy(Pos, a.x, a.y);
	to_xy(NextPos, b.x, b.y);

	if (b.x <= a.x)
		switch (b.y - a.y)
	{
		case (1):
			return IS_UP;
		case (0):
			return IS_TARGET;
		case (-1):
			return IS_DOWN;
		default:
			return IS_NONE;
	}
	else
		return IS_NONE;
}
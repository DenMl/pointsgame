#include "Config.h"
#include "Field.h"

using namespace std;

void field::place_begin_pattern(begin_pattern cur_pattern)
{
	ushort X, Y;
	switch (cur_pattern)
	{
	case (BP_CROSSWIRE):
		X = _width / 2 - 1;
		Y = _height / 2 - 1;
		do_step(to_pos(X, Y));
		X++;
		do_step(to_pos(X, Y));
		Y++;
		do_step(to_pos(X, Y));
		X--;
		do_step(to_pos(X, Y));
		break;
	case (BP_SQUARE):
		X = _width / 2 - 1;
		Y = _height / 2 - 1;
		do_step(to_pos(X, Y));
		X++;
		do_step(to_pos(X, Y));
		X--;
		Y++;
		do_step(to_pos(X, Y));
		X++;
		do_step(to_pos(X, Y));
		break;
	}
}

void field::remove_empty_base(const pos start_pos)
{
	Wave(	start_pos,
			[&](pos Pos)->bool
			{
				if (is_in_empty_base(Pos))
				{
					_changes.back().changes.push(pair<pos, value>(Pos, _points[Pos] & !tag_bit));
					clear_empty_base(Pos);
					return true;
				}
				else
				{
					return false;
				}
			});
}

bool field::BuildChain(const pos StartPos, const value EnableCond, const pos DirectionPos, list<pos> &chain)
{
	chain.clear();
	chain.push_back(StartPos);
	pos Pos = DirectionPos;
	pos CenterPos = StartPos;
	// Площадь базы.
	int TempSquare = square(CenterPos, Pos);
	do
	{
		if (is_tagged(Pos))
		{
			while (chain.back() != Pos)
			{
				clear_tag(chain.back());
				chain.pop_back();
			}
		}
		else
		{
			set_tag(Pos);
			chain.push_back(Pos);
		}
		swap(Pos, CenterPos);
		GetFirstNextPos(CenterPos, Pos);
		while (is_not_enable(Pos, EnableCond))
			GetNextPos(CenterPos, Pos);
		TempSquare += square(CenterPos, Pos);
	}
	while (Pos != StartPos);

	for (auto i = chain.begin(); i != chain.end(); i++)
		clear_tag(*i);

	return (TempSquare < 0 && chain.size() > 2);
}

void field::FindSurround(list<pos> &chain, pos InsidePoint, player Player)
{
	// Количество захваченных точек.
	int CurCaptureCount = 0;
	// Количество захваченных пустых полей.
	int CurFreedCount = 0;

	list<pos> SurPoints;

	// Помечаем точки цепочки.
	for (auto i = chain.begin(); i != chain.end(); i++)
		set_tag(*i);

	Wave(	InsidePoint,
			[&, Player](pos Pos)->bool
			{
				if (is_not_bound(Pos, Player | put_bit | bound_bit))
				{
					CheckCapturedAndFreed(Pos, Player, CurCaptureCount, CurFreedCount);
					SurPoints.push_back(Pos);
					return true;
				}
				else
				{
					return false;
				}
	});
	// Изменение счета игроков.
	AddSubCapturedFreed(Player, CurCaptureCount, CurFreedCount);

#if SURROUND_CONDITIONS
	if ((CurCaptureCount != 0) || (_sur_cond == SC_ALWAYS)) // Если захватили точки, или стоит опция захватывать всегда.
#else
	if (CurCaptureCount != 0) // Если захватили точки.
#endif
	{
		for (auto i = chain.begin(); i != chain.end(); i++)
		{
			clear_tag(*i);
			// Добавляем в список изменений точки цепочки.
			_changes.back().changes.push(pair<pos, value>(*i, _points[*i]));
			// Помечаем точки цепочки.
			set_base_bound(*i);
		}

		for (auto i = SurPoints.begin(); i != SurPoints.end(); i++)
		{
			_changes.back().changes.push(pair<pos, value>(*i, _points[*i]));

			SetCaptureFreeState(*i, Player);
		}
	}
	else // Если ничего не захватили.
	{
		for (auto i = chain.begin(); i != chain.end(); i++)
			clear_tag(*i);

		for (auto i = SurPoints.begin(); i != SurPoints.end(); i++)
		{
			_changes.back().changes.push(pair<pos, value>(*i, _points[*i]));

			if (!is_putted(*i))
				set_empty_base(*i);
		}
	}
}

field::field(const coord FieldWidth, const coord FieldHeight, const sur_cond SurCond, const begin_pattern BeginPattern)
{
	_width = FieldWidth;
	_height = FieldHeight;
#if SURROUND_CONDITIONS
	_sur_cond = SurCond;
#endif
	_player = player_red;
	capture_count[player_red] = 0;
	capture_count[player_black] = 0;

	_points = new value[length()];
	fill_n(_points, length(), 0);

	for (coord x = -1; x <= FieldWidth; x++)
	{
		set_bad(to_pos(x, -1));
		set_bad(to_pos(x, FieldHeight));
	}
	for (coord y = -1; y <= FieldHeight; y++)
	{
		set_bad(to_pos(-1, y));
		set_bad(to_pos(FieldWidth, y));
	}

	_changes.reserve(length());
	points_seq.reserve(length());

	_hash = 0;

	place_begin_pattern(BeginPattern);
}

field::field(const field &orig)
{
	_width = orig._width;
	_height = orig._height;
#if SURROUND_CONDITIONS
	_sur_cond = orig._sur_cond;
#endif
	_player = orig._player;
	capture_count[player_red] = orig.capture_count[player_red];
	capture_count[player_black] = orig.capture_count[player_black];

	_points = new value[length()];
	copy_n(orig._points, length(), _points);

	_changes.reserve(length());
	points_seq.reserve(length());

	_changes.assign(orig._changes.begin(), orig._changes.end());
	points_seq.assign(orig.points_seq.begin(), orig.points_seq.end());

	_hash = orig._hash;
}

field::~field()
{
	delete _points;
}
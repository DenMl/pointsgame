#include "config.h"
#include "Random.h"
#include "field.h"
#include "basic_types.h"
#include "bot_engine.h"
#include <iostream>
#include <string>
#include <map>

field *main_field;

const uint min_minimax_depth = 0;
const uint max_minimax_depth = 8;
const ulong min_uct_iterations = 0;
const ulong max_uct_iterations = 100000;
const ulong min_p = 0;
const ulong max_p = 100;

inline uint get_minimax_depth(ulong p)
{
	return (p - min_p) * (max_minimax_depth - min_minimax_depth) / (max_p - min_p) + min_minimax_depth;
}

inline ulong get_uct_iterations(ulong p)
{
	return (p - min_p) * (max_uct_iterations - min_uct_iterations) / (max_p - min_p) + min_uct_iterations;
}

void boardsize(size_t id)
{
	coord x, y;

	cin >> x >> y;
	// Если существовало поле - удаляем его.
	if (main_field != NULL)
		delete main_field;
	main_field = new field(x, y, SC_STANDART, BP_CLEAN);
	cout << "=" << " " << id << " " << "boardsize" << endl;
}

void genmove(size_t id)
{
	player cur_player;
	pos cur_pos;

	cin >> cur_player;
	if (main_field == NULL)
	{
		cout << "?" << " " << id << " " << "genmove" << endl;
	}
	else
	{
		main_field->set_player(cur_player);
		cur_pos = minimax_uct_best_move(*main_field, 8, 100000);
		if (cur_pos == -1)
		{
			cout << "?" << " " << id << " " << "genmove" << endl;
			return;
		}
		main_field->do_step(cur_pos);
		cout << "=" << " " << id << " " << "genmove" << " " << main_field->to_x(cur_pos) << " " << main_field->to_y(cur_pos) << " " << cur_player << endl;
	}
}

void list_commands(size_t id)
{
	cout << "=" << " " << id << " " << "list_commands" << " " << "boardsize genmove list_commands name play quit reg_genmove reg_genmove_with_complexity reg_genmove_with_time undo version" << endl;
}

void name(size_t id)
{
	cout << "=" << " " << id << " " << "name" << " " << "kkai" << endl;
}

void play(size_t id)
{
	coord x, y;
	player cur_player;

	cin >> x >> y >> cur_player;
	if (main_field == NULL || !main_field->do_step(main_field->to_pos(x, y), cur_player))
		cout << "?" << " " << id << " " << "play" << endl;
	else
		cout << "=" << " " << id << " " << "play" << " " << x << " " << y << " " << cur_player << endl;
}

void quit(size_t id)
{
	cout << "=" << " " << id << " " << "quit" << endl;
	exit(0);
}

void reg_genmove(size_t id)
{
	player cur_player;
	pos cur_pos;

	cin >> cur_player;
	if (main_field == NULL)
	{
		cout << "?" << " " << id << " " << "reg_genmove" << endl;
	}
	else
	{
		main_field->set_player(cur_player);
		cur_pos = minimax_uct_best_move(*main_field, 8, 100000);
		if (cur_pos == -1)
		{
			cout << "?" << " " << id << " " << "reg_genmove" << endl;
			return;
		}
		cout << "=" << " " << id << " " << "reg_genmove" << " " << main_field->to_x(cur_pos) << " " << main_field->to_y(cur_pos) << " " << cur_player << endl;
	}
}

void reg_genmove_with_complexity(size_t id)
{
	player cur_player;
	pos cur_pos;
	size_t p;

	cin >> cur_pos >> p;
	if (main_field == NULL)
	{
		cout << "?" << " " << id << " " << "reg_genmove_with_complexity" << endl;
	}
	else
	{
		main_field->set_player(cur_player);
		cur_pos = minimax_uct_best_move(*main_field, get_minimax_depth(p), get_uct_iterations(p));
		if (cur_pos == -1)
		{
			cout << "?" << " " << id << " " << "reg_genmove_with_complexity" << endl;
			return;
		}
		cout << "=" << " " << id << " " << "reg_genmove_with_complexity" << " " << main_field->to_x(cur_pos) << " " << main_field->to_y(cur_pos) << " " << cur_player << endl;
	}
}

void reg_genmove_with_time(size_t id)
{
	player cur_player;
	pos cur_pos;
	size_t time;

	cin >> cur_player >> time;
	if (main_field == NULL)
	{
		cout << "?" << " " << id << " " << "reg_genmove_with_time" << endl;
	}
	else
	{
		main_field->set_player(cur_player);
		cur_pos = uct_with_time_best_move(*main_field, time);
		if (cur_pos == -1)
		{
			cout << "?" << " " << id << " " << "reg_genmove_with_time" << endl;
			return;
		}
		cout << "=" << " " << id << " " << "reg_genmove_with_time" << " " << main_field->to_x(cur_pos) << " " << main_field->to_y(cur_pos) << " " << cur_player << endl;
	}
}

void undo(size_t id)
{
	if (main_field == NULL || main_field->points_seq.size() == 0)
	{
		cout << "?" << " " << id << " " << "undo" << endl;
	}
	else
	{
		main_field->undo_step();
		cout << "=" << " " << id << " " << "undo" << endl;
	}
}

void version(size_t id)
{
	cout << "=" << " " << id << " " << "version" << " " << "1.9.9.9" << endl;
}

inline void fill_codes(map<string, void(*)(size_t)> &codes)
{
	codes["boardsize"] = boardsize;
	codes["genmove"] = genmove;
	codes["list_commands"] = list_commands;
	codes["name"] = name;
	codes["play"] = play;
	codes["quit"] = quit;
	codes["reg_genmove"] = reg_genmove;
	codes["reg_genmove_with_complexity"] = reg_genmove_with_complexity;
	codes["reg_genmove_with_time"] = reg_genmove_with_time;
	codes["undo"] = undo;
	codes["version"] = version;
}

int main()
{
	string s;
	size_t id;
	map<string, void(*)(size_t)> codes;

	main_field = NULL;
	fill_codes(codes);
	Randomize();
	
	while (true)
	{
		cin >> id >> s;
		auto i = codes.find(s);
		if (i != codes.end())
			i->second(id);
		else
			cout << "?" << " " << id << " " << s << endl;
		// Очищаем cin до конца строки.
		while (cin.get() != 10) {}
	}
}
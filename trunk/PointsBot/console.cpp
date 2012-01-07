#include "config.h"
#include "field.h"
#include "basic_types.h"
#include "bot.h"
#include <iostream>
#include <string>
#include <map>

bot *main_bot;

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

void gen_move(size_t id)
{
	player cur_player;
	coord x, y;

	cin >> cur_player;
	if (main_bot == NULL)
	{
		cout << "?" << " " << id << " " << "gen_move" << endl;
	}
	else
	{
		main_bot->set_player(cur_player);
		main_bot->minimax_uct_best_move(x, y, 8, 100000);
		if (x == -1 || y == -1)
			cout << "?" << " " << id << " " << "gen_move" << endl;
		else
			cout << "=" << " " << id << " " << "gen_move" << " " << x << " " << y << " " << cur_player << endl;
	}
}

void gen_move_with_complexity(size_t id)
{
	player cur_player;
	coord x, y;
	size_t p;

	cin >> cur_player >> p;
	if (main_bot == NULL)
	{
		cout << "?" << " " << id << " " << "gen_move_with_complexity" << endl;
	}
	else
	{
		main_bot->set_player(cur_player);
		main_bot->minimax_uct_best_move(x, y, get_minimax_depth(p), get_uct_iterations(p));
		if (x == -1 || y == -1)
			cout << "?" << " " << id << " " << "gen_move_with_complexity" << endl;
		else
			cout << "=" << " " << id << " " << "gen_move_with_complexity" << " " << x << " " << y << " " << cur_player << endl;
	}
}

void gen_move_with_time(size_t id)
{
	player cur_player;
	coord x, y;
	size_t time;

	cin >> cur_player >> time;
	if (main_bot == NULL)
	{
		cout << "?" << " " << id << " " << "gen_move_with_time" << endl;
	}
	else
	{
		main_bot->set_player(cur_player);
		main_bot->uct_with_time_best_move(x, y, time);
		if (x == -1 || y == -1)
			cout << "?" << " " << id << " " << "gen_move_with_time" << endl;
		else
			cout << "=" << " " << id << " " << "gen_move_with_time" << " " << x << " " << y << " " << cur_player << endl;
	}
}

void init(size_t id)
{
	coord x, y;
	ptrdiff_t seed;

	cin >> x >> y >> seed;
	// Если существовало поле - удаляем его.
	if (main_bot != NULL)
		delete main_bot;
	main_bot = new bot(x, y, SUR_COND_STANDART, BEGIN_PATTERN_CLEAN, seed);
	cout << "=" << " " << id << " " << "init" << endl;
}

void list_commands(size_t id)
{
	cout << "=" << " " << id << " " << "list_commands" << " " << "gen_move gen_move_with_complexity gen_move_with_time init list_commands name play quit undo version" << endl;
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
	if (main_bot == NULL || !main_bot->do_step(x, y, cur_player))
		cout << "?" << " " << id << " " << "play" << endl;
	else
		cout << "=" << " " << id << " " << "play" << " " << x << " " << y << " " << cur_player << endl;
}

void quit(size_t id)
{
	if (main_bot != NULL)
		delete main_bot;
	cout << "=" << " " << id << " " << "quit" << endl;
	exit(0);
}

void undo(size_t id)
{
	if (main_bot == NULL || !main_bot->undo_step())
		cout << "?" << " " << id << " " << "undo" << endl;
	else
		cout << "=" << " " << id << " " << "undo" << endl;
}

void version(size_t id)
{
	cout << "=" << " " << id << " " << "version" << " " << "2.0.0.0" << endl;
}

inline void fill_codes(map<string, void(*)(size_t)> &codes)
{
	codes["gen_move"] = gen_move;
	codes["gen_move_with_complexity"] = gen_move_with_complexity;
	codes["gen_move_with_time"] = gen_move_with_time;
	codes["init"] = init;
	codes["list_commands"] = list_commands;
	codes["name"] = name;
	codes["play"] = play;
	codes["quit"] = quit;
	codes["undo"] = undo;
	codes["version"] = version;
}

int main()
{
	string s;
	size_t id;
	map<string, void(*)(size_t)> codes;

	main_bot = NULL;
	fill_codes(codes);
	
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
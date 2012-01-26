#pragma once

// Правила обработки пустых баз.
// STANDART = 0 - если PlayerRed ставит в пустую базу и ничего не обводит, то PlayerBlack обводит эту территорию.
// ALWAYS = 1 - обводить базу, даже если нет вражеских точек внутри.
// ALWAYS_ENEMY = 2 - обводит всегда PlayerBlack, если PlayerRed поставил точку в пустую базу.
#define SUR_COND 0

// Включает сортировку по вероятностям для улучшения альфабета-отсечения.
#define ALPHABETA_SORT 1

// Larger values give uniform search.
// Smaller values give very selective search.
#define UCTK 1

// Радиус, внутри которого происходит анализ UCT.
#define UCT_RADIUS 2

// Количество итераций uct перед проверкой времени.
#define UCT_ITERATIONS_BEFORE_CHECK_TIME 100

// 1, если компиляция происходит для Windows.
#define WINDOWS 1

// 1, если компиляция происходит для Linux.
#define LINUX 0

#define X64 0
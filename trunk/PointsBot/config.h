#pragma once

// Следует выключать, когда не нужны дополнительные условия окружения (только классический тип) для увеличения производительности.
#define SURROUND_CONDITIONS 1

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
#pragma once
#include "BasicTypes.h"

// Minimum supported board width.
// Минимальная поддерживаемая ширина доски.
static const p_int MinFieldWidth = 1;
// Maximum supported board width.
// Максимальная поддерживаемая ширина доски.
static const p_int MaxFieldWidth = 39;
// Minimum supported board height.
// Минимальная поддерживаемая высота доски.
static const p_int MinFieldHeight = 1;
// Maximum supported board height.
// Максимальная поддерживаемая высота доски.
static const p_int MaxFieldHeight = 32;

// Фактические размеры поля. Сделано для оптимизации операций.
static const p_int FieldWidth2 = 64;
static const p_int FieldHeight2 = 64;
static const p_int PointsLength22 = FieldWidth2 * FieldHeight2;

// Максимальное количество точек на данном поле, которые могут образовывать цепочку.
static const p_int MAX_CHAIN_POINTS = MaxFieldWidth * MaxFieldHeight;
// Максимальное количество точек на данном поле, которые могут находиться в окружении.
static const p_int MAX_SUR_POINTS = (MaxFieldWidth - 2) * (MaxFieldHeight - 2);
#pragma once
#include "BasicTypes.h"

// Minimum supported board width.
// Минимальная поддерживаемая ширина доски.
static const _int MinFieldWidth = 1;
// Maximum supported board width.
// Максимальная поддерживаемая ширина доски.
static const _int MaxFieldWidth = 39;
// Minimum supported board height.
// Минимальная поддерживаемая высота доски.
static const _int MinFieldHeight = 1;
// Maximum supported board height.
// Максимальная поддерживаемая высота доски.
static const _int MaxFieldHeight = 32;

// Фактические размеры поля. Сделано для оптимизации операций.
static const _int FieldWidth2 = 64;
static const _int FieldHeight2 = 64;
static const _int PointsLength22 = FieldWidth2 * FieldHeight2;

// Максимальное количество точек на данном поле, которые могут образовывать цепочку.
static const _int MAX_CHAIN_POINTS = MaxFieldWidth * MaxFieldHeight;
// Максимальное количество точек на данном поле, которые могут находиться в окружении.
static const _int MAX_SUR_POINTS = (MaxFieldWidth - 2) * (MaxFieldHeight - 2);
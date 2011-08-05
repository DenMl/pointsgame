#pragma once

#include "Config.h"
#include "BasicTypes.h"

// Minimum supported board width.
// Минимальная поддерживаемая ширина доски.
static const ushort MinFieldWidth = 1;
// Maximum supported board width.
// Максимальная поддерживаемая ширина доски.
static const ushort MaxFieldWidth = 39;
// Minimum supported board height.
// Минимальная поддерживаемая высота доски.
static const ushort MinFieldHeight = 1;
// Maximum supported board height.
// Максимальная поддерживаемая высота доски.
static const ushort MaxFieldHeight = 32;

// Фактические размеры поля. Сделано для оптимизации операций.
static const ushort FieldWidth2 = 64;
static const ushort FieldHeight2 = 64;
static const uint PointsLength22 = FieldWidth2 * FieldHeight2;

// Максимальное количество точек на данном поле, которые могут образовывать цепочку.
static const uint MAX_CHAIN_POINTS = MaxFieldWidth * MaxFieldHeight;
// Максимальное количество точек на данном поле, которые могут находиться в окружении.
static const uint MAX_SUR_POINTS = (MaxFieldWidth - 2) * (MaxFieldHeight - 2);
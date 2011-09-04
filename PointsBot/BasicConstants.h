#pragma once

#include "config.h"
#include "basic_types.h"

// Minimum supported board width.
// Минимальная поддерживаемая ширина доски.
const ushort MinFieldWidth = 1;
// Maximum supported board width.
// Максимальная поддерживаемая ширина доски.
const ushort MaxFieldWidth = 39;
// Minimum supported board height.
// Минимальная поддерживаемая высота доски.
const ushort MinFieldHeight = 1;
// Maximum supported board height.
// Максимальная поддерживаемая высота доски.
const ushort MaxFieldHeight = 32;

// Фактические размеры поля. Сделано для оптимизации операций.
const ushort FieldWidth2 = 64;
const ushort FieldHeight2 = 64;
const uint PointsLength22 = FieldWidth2 * FieldHeight2;

// Максимальное количество точек на данном поле, которые могут образовывать цепочку.
const uint MAX_CHAIN_POINTS = MaxFieldWidth * MaxFieldHeight;
// Максимальное количество точек на данном поле, которые могут находиться в окружении.
const uint MAX_SUR_POINTS = (MaxFieldWidth - 2) * (MaxFieldHeight - 2);
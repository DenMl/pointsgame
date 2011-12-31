using System;

namespace PointsShell.Enums
{
	// Возможные типы игры:
	// Standart - классический тип.
	// Always - всегда захватывает тот игрок, в чье пустое окружение поставлена точка.
	// AlwaysEnemy - захватывать даже пустые области.
	[Serializable]
	public enum SurroundCond
	{
		Standart,
		Always,
		AlwaysEnemy
	}
}

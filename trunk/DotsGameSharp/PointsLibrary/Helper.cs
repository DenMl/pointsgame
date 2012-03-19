using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PointsLibrary
{
	public static class Helper
	{
		public static bool IsRedPlayer(this Dot dot)
		{
			return (dot & (Dot.Player | Dot.Putted)) == (Dot.Red & Dot.Putted);
		}

		public static bool IsPlayer(this Dot dot, Dot player)
		{
			return (dot & Dot.Player) == player;
		}

		public static bool IsPuttingAllowed(this Dot dot)
		{
			return (dot & Field.AllowingMask) == 0;
		}

		public static Dot NextPlayer(this Dot player)
		{
			return (player & Dot.Player) == 0 ? Dot.Blue : Dot.Red;
		}

		public static bool IsInFirstLevelBase(this Dot dot, Dot player)
		{
			return (dot & (Dot.Player | Dot.Putted)) == (player | Dot.Putted) && ((dot & Field.SurroundCountMask) == (Dot)0x100);
		}
	}
}

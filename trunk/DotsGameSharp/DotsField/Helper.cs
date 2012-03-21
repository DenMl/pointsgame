using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Dots.Library
{
	public static class Helper
	{
		public static bool IsPutted(this Dot dot)
		{
			return (dot & Dot.Putted) == Dot.Putted;
		}

		public static bool IsPuttingAllowed(this Dot dot)
		{
			return (dot & Dot.AllowingMask) == 0;
		}

		public static Dot NextPlayer(this Dot player)
		{
			return (player & Dot.Player) == Dot.RedPlayer ? Dot.BluePlayer : Dot.RedPlayer;
		}

		public static bool IsPlayerPutted(this Dot dot, Dot player)
		{
			return (dot & Dot.EnableMask) == (Dot.Putted | player);
		}

		public static bool IsRedPutted(this Dot dot)
		{
			return (dot & Dot.EnableMask) == (Dot.Putted | Dot.RedPlayer);
		}

		public static bool IsBluePutted(this Dot dot)
		{
			return (dot & Dot.EnableMask) == (Dot.Putted | Dot.BluePlayer);
		}

		public static bool IsZeroSurroundLevel(this Dot dot)
		{
			return (dot & Dot.SurroundCountMask) == (Dot)0;
		}

		public static bool IsOneSurroundLevel(this Dot dot)
		{
			return (dot & Dot.SurroundCountMask) == Dot.FirstSurroundLevel;
		}

		public static bool IsMoreThanOneSurroundLevel(this Dot dot)
		{
			return (dot & Dot.SurroundCountMask) > Dot.FirstSurroundLevel;
		}

		public static bool IsRealPutted(this Dot dot)
		{
			return (dot & Dot.RealPutted) == Dot.RealPutted;
		}

		public static bool IsRealRedPlayer(this Dot dot)
		{
			return (dot & Dot.RealPlayer) == Dot.RedRealPlayer;
		}

		public static bool IsRealBluePlayer(this Dot dot)
		{
			return (dot & Dot.RealPlayer) == Dot.BlueRealPlayer;
		}
	}
}

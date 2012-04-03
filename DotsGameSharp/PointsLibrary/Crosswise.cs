﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Dots.Library
{
	public class Crosswise
	{
		#region Readonly & Fields

		public readonly Dot Player;
		public readonly int Position;
		public readonly enmCrosswiseOrientation Orientation;
		public readonly int Number;

		#endregion

		#region Constructors

		public Crosswise(Dot player, int pos, enmCrosswiseOrientation orientation, int number)
		{
			Player = player;
			Position = pos;
			Orientation = orientation;
			Number = number;
		}

		#endregion

		#region Public Methods

		

		#endregion
	}
}

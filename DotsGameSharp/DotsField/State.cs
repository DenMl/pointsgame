using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Dots.Library
{
	/// <remarks>
	/// Struct for field changes on each move
	/// if surround on Move then Base != null
	/// </remarks>
	public class State
	{
		#region Properties

		public Base Base
		{
			get;
			set;
		}

		public DotPosition Move
		{
			get;
			set;
		}

		#endregion

		#region Constructors

		public State()
		{
		}

		public State(DotPosition dotPosition, Base Base)
		{
			this.Base = Base;
			Move = dotPosition;
		}

		#endregion

		public bool HasBase()
		{
			return Base != null;
		}

		public override string ToString()
		{
			return Move.ToString() + "; " + Base.ToString();
		}
	}
}

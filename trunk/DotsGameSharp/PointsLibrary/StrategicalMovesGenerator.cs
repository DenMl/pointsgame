using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Dots.Library;

namespace Dots.AI
{
	class StrategicalMovesGenerator
	{
		#region Fields

		private Field Field_;
		private Dictionary<int, LinkedGroup> RedGroups;

		#endregion

		#region Constructors

		public StrategicalMovesGenerator(Field field)
		{
			Field_ = field;
		}

		#endregion

		#region Public Methods


		#endregion

		#region Helpers

		private void GenerateGroups()
		{
			var dots = new int[Field_.Width, Field_.Height];

			/*for (int i = 0; i < Field_.Width; i++)
				for (int j = 0; j < Field_.Height; j++)
				{
					if (Field_[(j + 1) * Field.RealWidth + i + 1].)
				}
			*/
		}

		#endregion
	}
}

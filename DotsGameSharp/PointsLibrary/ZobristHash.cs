using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Security.Cryptography;
using Dots.Library;

namespace Dots.AI
{
	/// <summary>
	/// TODO: take in calculation Current Red and Blue Capture Count.
	/// </summary>
	class ZobristHashField
	{
		#region Fields

		private Field Field_;
		private ulong[] HashTable_;
		private ulong Key_;

		#endregion

		#region Constructors

		public ZobristHashField(Field field, int? seed = null,
			enmRandomGenerateMethod randomGenerateMethod = enmRandomGenerateMethod.Standart)
		{
			Field_ = field;
			Seed = seed;
			RandomGenerateMethod = randomGenerateMethod;

			HashTable_ = new ulong[field.RealDotsCount * 2];
			FillWithRandomValues();
			Key_ = 0;
		}

		#endregion

		#region Public Methods
	
		public void UpdateHash()
		{
			if (Field_.LastMoveState == enmMoveState.Add)
			{
				int pos = Field_.LastPosition;
				if (Field_.CurrentPlayer == Dot.RedPlayer)
					pos *= 2;
				Key_ ^= HashTable_[pos];
				
				if (Field_.LastMoveCaptureCount != 0)
					UpdateLastBaseHash();
			}
			else
			{
				if (Field_.LastMoveCaptureCount != 0)
					UpdateLastBaseHash();

				int pos = Field_.LastPosition;
				if (Field_.CurrentPlayer == Dot.BluePlayer)
					pos *= 2;
				Key_ ^= HashTable_[pos];
			}
		}

		#endregion

		#region Helpers

		private void FillWithRandomValues()
		{
			if (RandomGenerateMethod == enmRandomGenerateMethod.Standart)
			{
				var random = Seed.HasValue ? new Random((int)Seed) : new Random();
				var buffer = new byte[sizeof(ulong)];
				for (int i = 0; i < HashTable_.Length; i++)
				{
					random.NextBytes(buffer);
					HashTable_[i] = BitConverter.ToUInt64(buffer, 0);
				}
			}
			else
				if (RandomGenerateMethod == enmRandomGenerateMethod.Crypto)
				{
					// TODO: Understand how to generate sequence with define seed.
					using (var generator = new RNGCryptoServiceProvider())
					{
						var bytes = new byte[sizeof(ulong)];
						for (int i = 0; i < HashTable_.Length; i++)
						{
							generator.GetBytes(bytes);
							HashTable_[i] = BitConverter.ToUInt64(bytes, 0);
						}
					}
				}
		}

		/// <summary>
		/// TODO: Try to optimize it.
		/// </summary>
		private void UpdateLastBaseHash()
		{
			bool isRed = !((Field_.CurrentPlayer == Dot.RedPlayer && Field_.LastMoveState == enmMoveState.Add)
				|| (Field_.CurrentPlayer == Dot.BluePlayer && Field_.LastMoveState == enmMoveState.Remove)) ? true : false;
			if (Field_.LastMoveCaptureCount < 0)
				isRed = !isRed;
			if (isRed)
				foreach (var surroundPos in Field.SurroundPositions)
				{
					if ((Field[surroundPos].IsOneSurroundLevel() && Field_.LastMoveState == enmMoveState.Add) ||
						 Field[surroundPos].IsZeroSurroundLevel() && Field_.LastMoveState == enmMoveState.Remove)
					{
						if (Field[surroundPos].IsRealPutted())
						{
							if (Field[surroundPos].IsRealBluePlayer())
							{
								Key_ ^= HashTable_[surroundPos * 2];
								Key_ ^= HashTable_[surroundPos];
							}
						}
						else
						{
							if (Field_.LastMoveCaptureCount < 0 && Field_.LastMoveState == enmMoveState.Remove &&
								Field_.LastPosition == surroundPos)
									Key_ ^= HashTable_[surroundPos * 2];
							Key_ ^= HashTable_[surroundPos];
						}
					}
					else
						if ((Field[surroundPos].IsMoreThanOneSurroundLevel() && Field_.LastMoveState == enmMoveState.Add) ||
							(Field[surroundPos].IsOneSurroundLevel() && Field_.LastMoveState == enmMoveState.Remove))
						{
							Key_ ^= HashTable_[surroundPos * 2];
							Key_ ^= HashTable_[surroundPos];
						}
				}
			else
				foreach (var surroundPos in Field.SurroundPositions)
				{
					if ((Field[surroundPos].IsOneSurroundLevel() && Field_.LastMoveState == enmMoveState.Add) ||
						(Field[surroundPos].IsZeroSurroundLevel() && Field_.LastMoveState == enmMoveState.Remove))
					{
						if (Field[surroundPos].IsRealPutted())
						{
							if (Field[surroundPos].IsRealRedPlayer())
							{
								Key_ ^= HashTable_[surroundPos];
								Key_ ^= HashTable_[surroundPos * 2];
							}
						}
						else
						{
							if (Field_.LastMoveCaptureCount < 0 && Field_.LastMoveState == enmMoveState.Remove &&
								   Field_.LastPosition == surroundPos)
							{
								Key_ ^= HashTable_[surroundPos];
							}
							Key_ ^= HashTable_[surroundPos * 2];
						}
					}
					else
						if ((Field[surroundPos].IsMoreThanOneSurroundLevel() && Field_.LastMoveState == enmMoveState.Add) ||
							(Field[surroundPos].IsOneSurroundLevel() && Field_.LastMoveState == enmMoveState.Remove))
						{
							Key_ ^= HashTable_[surroundPos];
							Key_ ^= HashTable_[surroundPos * 2];
						}
				}
		}

		#endregion

		#region Properties

		public Field Field
		{
			get
			{
				return Field_;
			}
		}

		public enmRandomGenerateMethod RandomGenerateMethod
		{
			get;
			private set;
		}

		public int? Seed
		{
			get;
			private set;
		}

		public ulong Key
		{
			get
			{
				return Key_;
			}
		}

		#endregion
	}
}

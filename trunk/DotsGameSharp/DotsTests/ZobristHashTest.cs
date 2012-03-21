using Dots.Library;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;
using System.Collections.Generic;
using Dots.AI;

namespace DotsTests
{
	/// <summary>
	///This is a test class for ZobristHashTest and is intended
	///to contain all ZobristHashTest Unit Tests
	///</summary>
	[TestClass()]
	public class ZobristHashTest
	{
		private TestContext testContextInstance;
		private int StartX_ = 16;
		private int StartY_ = 16;

		/// <summary>
		///Gets or sets the test context which provides
		///information about and functionality for the current test run.
		///</summary>
		public TestContext TestContext
		{
			get
			{
				return testContextInstance;
			}
			set
			{
				testContextInstance = value;
			}
		}

		#region Additional test attributes

		[TestInitialize()]
		public void MyTestInitialize()
		{
			
		}

		// 
		//You can use the following additional attributes as you write your tests:
		//
		//Use ClassInitialize to run code before running the first test in the class
		//[ClassInitialize()]
		//public static void MyClassInitialize(TestContext testContext)
		//{
		//}
		//
		//Use ClassCleanup to run code after all tests in a class have run
		//[ClassCleanup()]
		//public static void MyClassCleanup()
		//{
		//}
		//
		//Use TestInitialize to run code before running each test
		//[TestInitialize()]
		//public void MyTestInitialize()
		//{
		//}
		//
		//Use TestCleanup to run code after each test has run
		//[TestCleanup()]
		//public void MyTestCleanup()
		//{
		//}
		//
		#endregion

		[TestMethod()]
		public void SimpleSequenceTest()
		{
			var field = new Field();
			ZobristHashField hash = new ZobristHashField(field, 0);

			ulong initKey = hash.Key;

			field.MakeMove(StartX_, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ + 1);
			hash.UpdateHash();
			
			field.MakeMove(StartX_, StartY_ + 1);
			hash.UpdateHash();

			while (field.DotsSequanceStates.Count() != 0)
			{
				field.UnmakeMove();
				hash.UpdateHash();
			}

			Assert.AreEqual(initKey, hash.Key);
		}

		[TestMethod()]
		public void SimpleBaseTest()
		{
			var field = new Field();
			ZobristHashField hash = new ZobristHashField(field, 0);

			ulong initKey = hash.Key;

			field.MakeMove(StartX_, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 1, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ + 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 2, StartY_ + 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ - 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 2, StartY_ - 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 2, StartY_);
			hash.UpdateHash();

			while (field.DotsSequanceStates.Count() != 0)
			{
				field.UnmakeMove();
				hash.UpdateHash();
			}

			Assert.AreEqual(initKey, hash.Key);
		}

		[TestMethod()]
		public void DifferentBaseCreationOrderTest()
		{
			var field = new Field();
			ZobristHashField hash = new ZobristHashField(field, 0);

			ulong initKey = hash.Key;

			field.MakeMove(StartX_, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 1, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ + 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 2, StartY_ + 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ - 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 2, StartY_ - 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 2, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 3, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_ - 2, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ - 2, StartY_ - 1);
			hash.UpdateHash();

			ulong firstHash = hash.Key;

			while (field.DotsSequanceStates.Count() != 0)
			{
				field.UnmakeMove();
				hash.UpdateHash();
			}

			Assert.AreEqual(initKey, hash.Key);


			field.MakeMove(StartX_ - 2, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 1, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 2, StartY_ + 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ + 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 2, StartY_ - 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ - 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 3, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 2, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ - 2, StartY_ - 1);
			hash.UpdateHash();

			ulong secondHash = hash.Key;

			Assert.AreEqual(firstHash, secondHash);

			while (field.DotsSequanceStates.Count() != 0)
			{
				field.UnmakeMove();
				hash.UpdateHash();
			}

			Assert.AreEqual(initKey, hash.Key);
		}

		[TestMethod()]
		public void BaseInBaseTest()
		{
			StartX_ = 5;
			StartY_ = 0;

			var field = new Field();
			ZobristHashField hash = new ZobristHashField(field, 0);

			ulong initKey = hash.Key;

			// center.
			field.MakeMove(StartX_, StartY_ + 3);
			hash.UpdateHash();
			field.MakeMove(StartX_, StartY_ + 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 10, StartY_ + 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 1, StartY_ + 3);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ + 2);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 10, StartY_ + 2);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 2, StartY_ + 3);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 10, StartY_ + 3);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ + 4);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 10, StartY_ + 4);
			hash.UpdateHash();

			PutBigBase(field, hash);

			ulong firstHash = hash.Key;

			while (field.DotsSequanceStates.Count() != 0)
			{
				field.UnmakeMove();
				hash.UpdateHash();
			}

			Assert.AreEqual(initKey, hash.Key);

			// center.
			field.MakeMove(StartX_ + 1, StartY_ + 3);
			hash.UpdateHash();
			field.MakeMove(StartX_, StartY_ + 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 10, StartY_ + 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 2, StartY_ + 3);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 2, StartY_ + 2);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 10, StartY_ + 2);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 3, StartY_ + 3);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 10, StartY_ + 3);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 4, StartY_ + 4);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 10, StartY_ + 4);
			hash.UpdateHash();

			PutBigBase(field, hash);

			ulong secondHash = hash.Key;

			Assert.AreEqual(firstHash, secondHash);

			while (field.DotsSequanceStates.Count() != 0)
			{
				field.UnmakeMove();
				hash.UpdateHash();
			}

			Assert.AreEqual(initKey, hash.Key);

			StartX_ = 16;
			StartY_ = 16;
		}

		[TestMethod()]
		public void EnemyBaseInBaseTest()
		{
			StartX_ = 5;
			StartY_ = 0;

			var field = new Field();
			ZobristHashField hash = new ZobristHashField(field, 0);

			ulong initKey = hash.Key;

			// center.
			field.MakeMove(StartX_ + 1, StartY_ + 3);
			hash.UpdateHash();
			field.MakeMove(StartX_, StartY_ + 3);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 10, StartY_ + 2);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 1, StartY_ + 2);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 10, StartY_ + 3);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 2, StartY_ + 2);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 10, StartY_ + 4);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 3, StartY_ + 3);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 10, StartY_ + 5);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 2, StartY_ + 4);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 10, StartY_ + 6);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 1, StartY_ + 4);
			hash.UpdateHash();

			PutBigBase(field, hash);

			ulong firstHash = hash.Key;

			while (field.DotsSequanceStates.Count() != 0)
			{
				field.UnmakeMove();
				hash.UpdateHash();
			}

			Assert.AreEqual(initKey, hash.Key);

			StartX_ = 16;
			StartY_ = 16;
		}

		private void PutBigBase(Field field, ZobristHashField hash)
		{
			// top.
			field.MakeMove(StartX_, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 11, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 11, StartY_ + 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 2, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 11, StartY_ + 2);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 3, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 11, StartY_ + 3);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 4, StartY_ + 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 11, StartY_ + 4);
			hash.UpdateHash();

			// right.
			field.MakeMove(StartX_ + 5, StartY_ + 2);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 12, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 5, StartY_ + 3);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 12, StartY_ + 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 5, StartY_ + 4);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 12, StartY_ + 2);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 4, StartY_ + 5);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 12, StartY_ + 3);
			hash.UpdateHash();

			// bottom
			field.MakeMove(StartX_ + 3, StartY_ + 6);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 13, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 2, StartY_ + 6);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 13, StartY_ + 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ + 6);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 13, StartY_ + 2);
			hash.UpdateHash();

			field.MakeMove(StartX_, StartY_ + 6);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 13, StartY_ + 3);
			hash.UpdateHash();

			field.MakeMove(StartX_ - 1, StartY_ + 5);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 13, StartY_ + 4);
			hash.UpdateHash();

			// left.
			field.MakeMove(StartX_ - 2, StartY_ + 4);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 14, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_ - 2, StartY_ + 3);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 14, StartY_ + 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ - 2, StartY_ + 2);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 14, StartY_ + 2);
			hash.UpdateHash();

			field.MakeMove(StartX_ - 1, StartY_ + 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 14, StartY_ + 3);
			hash.UpdateHash();
		}

		[TestMethod()]
		public void EmptyBaseTest()
		{
			var field = new Field();
			ZobristHashField hash = new ZobristHashField(field, 0);

			ulong initKey = hash.Key;

			field.MakeMove(StartX_, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 10, StartY_);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ + 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 10, StartY_ + 1);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 2, StartY_ + 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 10, StartY_ + 2);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 3, StartY_);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 10, StartY_ + 3);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 2, StartY_ - 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 10, StartY_ + 4);
			hash.UpdateHash();

			field.MakeMove(StartX_ + 1, StartY_ - 1);
			hash.UpdateHash();
			field.MakeMove(StartX_ + 1, StartY_);
			hash.UpdateHash();

			while (field.DotsSequanceStates.Count() != 0)
			{
				field.UnmakeMove();
				hash.UpdateHash();
			}

			Assert.AreEqual(initKey, hash.Key);
		}

		[TestMethod()]
		public void VeryLongGameTest()
		{
			var field = new Field();
			ZobristHashField hash = new ZobristHashField(field, 0);

			ulong initKey = hash.Key;

			var buffer = DotsTests.Properties.Resources.VeryLongGame;

			for (var i = 58; i < buffer.Length; i += 13)
			{
				Assert.AreEqual(field.MakeMove(buffer[i], buffer[i + 1]), true);
				hash.UpdateHash();
			}

			while (field.DotsSequanceStates.Count() != 0)
			{
				field.UnmakeMove();
				hash.UpdateHash();
			}

			Assert.AreEqual(initKey, hash.Key);
		}
	}
}

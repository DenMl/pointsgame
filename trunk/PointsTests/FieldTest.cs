using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;
using PointsShell;

namespace PointsTests
{
	/// <summary>
	///This is a test class for FieldTest and is intended
	///to contain all FieldTest Unit Tests
	///</summary>
	[TestClass()]
	public class FieldTest
	{
		private int StartX_ = 16;
		private int StartY_ = 16;

		private TestContext testContextInstance;

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

		/// <summary>
		/// xo
		/// ox
		/// </summary>
		[TestMethod()]
		public void SimpleSequenceTest()
		{
			Field field = new Field();
			field.PutPoint(StartX_, StartY_);
			field.PutPoint(StartX_ + 1, StartY_);
			field.PutPoint(StartX_ + 1, StartY_ + 1);
			field.PutPoint(StartX_, StartY_ + 1);
			field.BackAllMoves();
			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.CaptureCountRed + field.CaptureCountBlack, 0);
		}

		/// <summary>
		/// </summary>
		[TestMethod()]
		public void BlueSurroundFirst()
		{
			Field field = new Field();

			field.PutPoint(StartX_, StartY_);
			field.PutPoint(StartX_ - 1, StartY_);

			field.PutPoint(StartX_ + 1, StartY_ + 1);
			field.PutPoint(StartX_, StartY_ + 1);

			field.PutPoint(StartX_ + 1, StartY_ - 1);
			field.PutPoint(StartX_, StartY_ - 1);

			field.PutPoint(StartX_ - 2, StartY_);

			field.PutPoint(StartX_ + 1, StartY_);

			Assert.AreEqual(field.CaptureCountRed, 0);
			Assert.AreEqual(field.CaptureCountBlack, 1);

			field.BackAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.CaptureCountRed + field.CaptureCountBlack, 0);
		}

		[TestMethod()]
		public void OneBaseTest()
		{
			Field field = new Field();

			field.PutPoint(StartX_, StartY_);
			field.PutPoint(StartX_ + 1, StartY_);
			field.PutPoint(StartX_ + 1, StartY_ + 1);
			field.PutPoint(StartX_, StartY_ + 1);

			field.PutPoint(StartX_ + 2, StartY_);
			field.PutPoint(StartX_ - 1, StartY_);
			field.PutPoint(StartX_ + 1, StartY_ - 1);

			Assert.AreEqual(field.CaptureCountRed, 1);

			field.BackAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.CaptureCountRed + field.CaptureCountBlack, 0);
		}

		[TestMethod()]
		public void EmptyBaseTest()
		{
			Field field = new Field();

			field.PutPoint(StartX_, StartY_);
			field.PutPoint(StartX_ - 1, StartY_);

			field.PutPoint(StartX_ + 1, StartY_ + 1);
			field.PutPoint(StartX_, StartY_ + 1);

			field.PutPoint(StartX_ + 2, StartY_);
			field.PutPoint(StartX_ - 2, StartY_);

			field.PutPoint(StartX_ + 1, StartY_ - 1);

			field.PutPoint(StartX_ + 1, StartY_);

			Assert.AreEqual(field.CaptureCountRed, 1);
			Assert.AreEqual(field.CaptureCountBlack, 0);

			field.BackAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.CaptureCountRed + field.CaptureCountBlack, 0);
		}

		[TestMethod()]
		public void BlueSurroundFirstInEmptyBase()
		{
			Field field = new Field();

			field.PutPoint(StartX_, StartY_);
			field.PutPoint(StartX_ - 1, StartY_);

			field.PutPoint(StartX_ + 1, StartY_ + 1);
			field.PutPoint(StartX_, StartY_ + 1);

			field.PutPoint(StartX_ + 1, StartY_ - 1);
			field.PutPoint(StartX_, StartY_ - 1);

			field.PutPoint(StartX_ + 2, StartY_);
			field.PutPoint(StartX_ + 1, StartY_);

			Assert.AreEqual(field.CaptureCountRed, 0);
			Assert.AreEqual(field.CaptureCountBlack, 1);

			field.BackAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.CaptureCountRed + field.CaptureCountBlack, 0);
		}

		[TestMethod()]
		public void ThreeAdjacentBasesTest()
		{
			Field field = new Field();

			field.PutPoint(StartX_, StartY_);
			field.PutPoint(StartX_ + 1, StartY_);

			field.PutPoint(StartX_ + 1, StartY_ + 1);
			field.PutPoint(StartX_ + 2, StartY_ - 1);

			field.PutPoint(StartX_ + 2, StartY_);
			field.PutPoint(StartX_ + 1, StartY_ - 2);

			field.PutPoint(StartX_ + 3, StartY_ - 1);
			field.PutPoint(StartX_, StartY_ + 1);

			field.PutPoint(StartX_ + 2, StartY_ - 2);
			field.PutPoint(StartX_ + 2, StartY_ + 1);

			field.PutPoint(StartX_ + 1, StartY_ - 3);
			field.PutPoint(StartX_ + 3, StartY_ - 2);

			field.PutPoint(StartX_, StartY_ - 2);
			field.PutPoint(StartX_, StartY_ - 3);

			field.PutPoint(StartX_ + 1, StartY_ - 1);

			Assert.AreEqual(field.CaptureCountRed, 3);
			Assert.AreEqual(field.CaptureCountBlack, 0);

			field.BackAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.CaptureCountRed + field.CaptureCountBlack, 0);
		}

		[TestMethod()]
		public void BigEmptyBaseTest()
		{
			StartX_ = 10;
			StartY_ = 0;
			Field field = new Field();

			// top chain.
			field.PutPoint(StartX_, StartY_);
			field.PutPoint(StartX_ + 10, StartY_);

			field.PutPoint(StartX_ + 1, StartY_);
			field.PutPoint(StartX_ + 10, StartY_ + 1);

			field.PutPoint(StartX_ + 2, StartY_);
			field.PutPoint(StartX_ + 10, StartY_ + 2);

			field.PutPoint(StartX_ + 3, StartY_);
			field.PutPoint(StartX_ + 10, StartY_ + 3);

			field.PutPoint(StartX_ + 4, StartY_);
			field.PutPoint(StartX_ + 10, StartY_ + 4);

			// right chain.
			field.PutPoint(StartX_ + 5, StartY_ + 1);
			field.PutPoint(StartX_ + 10, StartY_ + 5);

			field.PutPoint(StartX_ + 5, StartY_ + 2);
			field.PutPoint(StartX_ + 10, StartY_ + 6);

			field.PutPoint(StartX_ + 5, StartY_ + 3);
			field.PutPoint(StartX_ + 10, StartY_ + 7);

			field.PutPoint(StartX_ + 5, StartY_ + 4);
			field.PutPoint(StartX_ + 10, StartY_ + 8);

			// bottom chain.
			field.PutPoint(StartX_ + 4, StartY_ + 5);
			field.PutPoint(StartX_ + 10, StartY_ + 9);

			field.PutPoint(StartX_ + 3, StartY_ + 5);
			field.PutPoint(StartX_ + 10, StartY_ + 10);

			field.PutPoint(StartX_ + 2, StartY_ + 5);
			field.PutPoint(StartX_ + 10, StartY_ + 11);

			field.PutPoint(StartX_ + 1, StartY_ + 5);
			field.PutPoint(StartX_ + 10, StartY_ + 12);

			field.PutPoint(StartX_, StartY_ + 5);
			field.PutPoint(StartX_ + 10, StartY_ + 13);

			field.PutPoint(StartX_ - 1, StartY_ + 5);
			field.PutPoint(StartX_ + 10, StartY_ + 14);

			// left chain.
			field.PutPoint(StartX_ - 2, StartY_ + 4);
			field.PutPoint(StartX_ + 10, StartY_ + 15);

			field.PutPoint(StartX_ - 2, StartY_ + 3);
			field.PutPoint(StartX_ + 10, StartY_ + 16);

			field.PutPoint(StartX_ - 2, StartY_ + 2);
			field.PutPoint(StartX_ + 10, StartY_ + 17);

			field.PutPoint(StartX_ - 1, StartY_ + 1);
			field.PutPoint(StartX_ + 10, StartY_ + 18);

			// center.
			Assert.AreEqual(field.PutPoint(StartX_, StartY_ + 3), true);
			field.PutPoint(StartX_ + 10, StartY_ + 19);

			Assert.AreEqual(field.PutPoint(StartX_ + 2, StartY_ + 2), true);
			field.PutPoint(StartX_ + 10, StartY_ + 20);

			Assert.AreEqual(field.PutPoint(StartX_ + 2, StartY_ + 3), true);
			field.PutPoint(StartX_ + 10, StartY_ + 21);

			Assert.AreEqual(field.PutPoint(StartX_ + 3, StartY_ + 3), true);

			// blue final.
			field.PutPoint(StartX_ + 3, StartY_ + 2);

			Assert.AreEqual(field.CaptureCountRed, 1);
			Assert.AreEqual(field.CaptureCountBlack, 0);

			field.BackAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.CaptureCountRed + field.CaptureCountBlack, 0);
		}

		[TestMethod()]
		public void VeryLongGameTest()
		{
			Field field = new Field();

			var buffer = PointsTests.Properties.Resources.VeryLongGame;

			for (var i = 58; i < buffer.Length; i += 13)
				Assert.AreEqual(field.PutPoint(buffer[i], buffer[i + 1]), true);

			Assert.AreEqual(field.CaptureCountRed, 179);
			Assert.AreEqual(field.CaptureCountBlack, 20);

			field.BackAllMoves();

			Assert.AreEqual(field.CaptureCountRed, 0);
			Assert.AreEqual(field.CaptureCountBlack, 0);
			Assert.AreEqual(field.IsEmpty, true);
		}
	}
}

using Dots.Library;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;

namespace DotsTests
{
	/// <summary>
	///This is a test class for FieldTest and is intended
	///to contain all FieldTest Unit Tests
	///</summary>
	[TestClass()]
	public class FieldFunctionsTest
	{
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
			int startX = 16;
			int startY = 16;

			Field field = new Field(39, 32);
			field.MakeMove(startX, startY);
			field.MakeMove(startX + 1, startY);
			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX, startY + 1);
			field.UnmakeAllMoves();
			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.RedCaptureCount + field.BlueCaptureCount, 0);
		}

		/// <summary>
		/// </summary>
		[TestMethod()]
		public void BlueSurroundFirst()
		{
			int startX = 16;
			int startY = 16;

			Field field = new Field(39, 32);

			field.MakeMove(startX, startY);
			field.MakeMove(startX - 1, startY);

			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX, startY + 1);

			field.MakeMove(startX + 1, startY - 1);
			field.MakeMove(startX, startY - 1);

			field.MakeMove(startX - 2, startY);

			field.MakeMove(startX + 1, startY);

			Assert.AreEqual(field.RedCaptureCount, 0);
			Assert.AreEqual(field.BlueCaptureCount, 1);

			field.UnmakeAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.RedCaptureCount + field.BlueCaptureCount, 0);
		}

		[TestMethod()]
		public void OneBaseTest()
		{
			int startX = 16;
			int startY = 16;

			Field field = new Field(39, 32);

			field.MakeMove(startX, startY);
			field.MakeMove(startX + 1, startY);
			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX, startY + 1);
			
			field.MakeMove(startX + 2, startY);
			field.MakeMove(startX - 1, startY);
			field.MakeMove(startX + 1, startY - 1);

			Assert.AreEqual(field.RedCaptureCount, 1);

			field.UnmakeAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.RedCaptureCount + field.BlueCaptureCount, 0);
		}

		[TestMethod()]
		public void BaseInBaseTest()
		{
			int startX = 16;
			int startY = 16;
			Field field = new Field(39, 32);

			field.MakeMove(startX, startY);
			field.MakeMove(startX + 1, startY);

			field.MakeMove(startX + 2, startY);
			field.MakeMove(startX + 2, startY + 1);
			
			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX + 3, startY);

			field.MakeMove(startX + 2, startY + 2);
			field.MakeMove(startX + 2, startY - 1);

			Assert.AreEqual(field.RedCaptureCount, 0);
			Assert.AreEqual(field.BlueCaptureCount, 1);

			field.MakeMove(startX + 3, startY + 1);
			field.MakeMove(startX + 10, startY);

			field.MakeMove(startX + 4, startY);
			field.MakeMove(startX + 10, startY + 1);

			field.MakeMove(startX + 3, startY - 1);
			field.MakeMove(startX + 10, startY + 2);

			field.MakeMove(startX + 2, startY - 2);
			field.MakeMove(startX + 10, startY + 3);

			field.MakeMove(startX + 1, startY - 1);
			field.MakeMove(startX + 10, startY + 4);

			Assert.AreEqual(field.RedCaptureCount, 4);
			Assert.AreEqual(field.BlueCaptureCount, 0);

			field.UnmakeAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.RedCaptureCount + field.BlueCaptureCount, 0);
		}

		[TestMethod()]
		public void ComplexBaseInBaseTest()
		{
			Field field = new Field(39, 32);

			var buffer = DotsTests.Properties.Resources.DotFunctionsTest;

			for (var i = 58; i < buffer.Length; i += 13)
			{
				if (field.DotsSequenceCount == 24)
				{
					field.MakeMove(buffer[i], buffer[i + 1]);
					Assert.AreEqual(field.RedCaptureCount, 1);
					Assert.AreEqual(field.BlueCaptureCount, 0);
				}
				else
					field.MakeMove(buffer[i], buffer[i + 1]);
			}

			Assert.AreEqual(field.RedCaptureCount, 0);
			Assert.AreEqual(field.BlueCaptureCount, 16);

			field.UnmakeAllMoves();

			Assert.AreEqual(field.RedCaptureCount, 0);
			Assert.AreEqual(field.BlueCaptureCount, 0);
			Assert.AreEqual(field.IsEmpty, true);
		}

		[TestMethod()]
		public void EmptyBaseTest()
		{
			int startX = 16;
			int startY = 16;

			Field field = new Field(39, 32);

			field.MakeMove(startX, startY);
			field.MakeMove(startX - 1, startY);

			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX, startY + 1);

			field.MakeMove(startX + 2, startY);
			field.MakeMove(startX - 2, startY);

			// Create empty base.
			field.MakeMove(startX + 1, startY - 1);

			// Red Surround.
			field.MakeMove(startX + 1, startY);

			Assert.AreEqual(field.RedCaptureCount, 1);
			Assert.AreEqual(field.BlueCaptureCount, 0);

			field.UnmakeAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.RedCaptureCount + field.BlueCaptureCount, 0);
		}

		[TestMethod()]
		public void BlueSurroundFirstInEmptyBase()
		{
			int startX = 16;
			int startY = 16;

			Field field = new Field(39, 32);

			field.MakeMove(startX, startY);
			field.MakeMove(startX - 1, startY);

			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX, startY + 1);

			field.MakeMove(startX + 1, startY - 1);
			field.MakeMove(startX, startY - 1);

			field.MakeMove(startX + 2, startY);
			field.MakeMove(startX + 1, startY);

			Assert.AreEqual(field.RedCaptureCount, 0);
			Assert.AreEqual(field.BlueCaptureCount, 1);

			field.UnmakeAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.RedCaptureCount + field.BlueCaptureCount, 0);
		}

		[TestMethod()]
		public void ThreeAdjacentBasesTest()
		{
			int startX = 16;
			int startY = 16;

			Field field = new Field(39, 32);

			field.MakeMove(startX, startY);
			field.MakeMove(startX + 1, startY);

			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX + 2, startY - 1);

			field.MakeMove(startX + 2, startY);
			field.MakeMove(startX + 1, startY - 2);

			field.MakeMove(startX + 3, startY - 1);
			field.MakeMove(startX, startY + 1);

			field.MakeMove(startX + 2, startY - 2);
			field.MakeMove(startX + 2, startY + 1);

			field.MakeMove(startX + 1, startY - 3);
			field.MakeMove(startX + 3, startY - 2);

			field.MakeMove(startX, startY - 2);
			field.MakeMove(startX, startY - 3);

			field.MakeMove(startX + 1, startY - 1);

			Assert.AreEqual(field.RedCaptureCount, 3);
			Assert.AreEqual(field.BlueCaptureCount, 0);

			field.UnmakeAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.RedCaptureCount + field.BlueCaptureCount, 0);
		}

		[TestMethod()]
		public void BigEmptyBaseTest()
		{
			int startX = 12;
			int startY = 2;
			Field field = new Field(39, 32);

			// top chain.
			field.MakeMove(startX, startY);
			field.MakeMove(startX + 10, startY);

			field.MakeMove(startX + 1, startY);
			field.MakeMove(startX + 10, startY + 1);

			field.MakeMove(startX + 2, startY);
			field.MakeMove(startX + 10, startY + 2);

			field.MakeMove(startX + 3, startY);
			field.MakeMove(startX + 10, startY + 3);

			field.MakeMove(startX + 4, startY);
			field.MakeMove(startX + 10, startY + 4);

			// right chain.
			field.MakeMove(startX + 5, startY + 1);
			field.MakeMove(startX + 10, startY + 5);

			field.MakeMove(startX + 5, startY + 2);
			field.MakeMove(startX + 10, startY + 6);

			field.MakeMove(startX + 5, startY + 3);
			field.MakeMove(startX + 10, startY + 7);

			field.MakeMove(startX + 5, startY + 4);
			field.MakeMove(startX + 10, startY + 8);

			// bottom chain.
			field.MakeMove(startX + 4, startY + 5);
			field.MakeMove(startX + 10, startY + 9);

			field.MakeMove(startX + 3, startY + 5);
			field.MakeMove(startX + 10, startY + 10);

			field.MakeMove(startX + 2, startY + 5);
			field.MakeMove(startX + 10, startY + 11);

			field.MakeMove(startX + 1, startY + 5);
			field.MakeMove(startX + 10, startY + 12);

			field.MakeMove(startX, startY + 5);
			field.MakeMove(startX + 10, startY + 13);

			field.MakeMove(startX - 1, startY + 5);
			field.MakeMove(startX + 10, startY + 14);

			// left chain.
			field.MakeMove(startX - 2, startY + 4);
			field.MakeMove(startX + 10, startY + 15);

			field.MakeMove(startX - 2, startY + 3);
			field.MakeMove(startX + 10, startY + 16);

			field.MakeMove(startX - 2, startY + 2);
			field.MakeMove(startX + 10, startY + 17);

			field.MakeMove(startX - 1, startY + 1);
			field.MakeMove(startX + 10, startY + 18);

			// center.
			Assert.AreEqual(field.MakeMove(startX, startY + 3), true);
			field.MakeMove(startX + 10, startY + 19);

			Assert.AreEqual(field.MakeMove(startX + 2, startY + 2), true);
			field.MakeMove(startX + 10, startY + 20);

			Assert.AreEqual(field.MakeMove(startX + 2, startY + 3), true);
			field.MakeMove(startX + 10, startY + 21);

			Assert.AreEqual(field.MakeMove(startX + 3, startY + 3), true);

			// blue final.
			field.MakeMove(startX + 3, startY + 2);

			Assert.AreEqual(field.RedCaptureCount, 1);
			Assert.AreEqual(field.BlueCaptureCount, 0);

			field.UnmakeAllMoves();

			Assert.AreEqual(field.IsEmpty, true);
			Assert.AreEqual(field.RedCaptureCount + field.BlueCaptureCount, 0);
		}

		[TestMethod()]
		public void VeryLongGameTest()
		{
			Field field = new Field(39, 32);

			var buffer = DotsTests.Properties.Resources.VeryLongGame;
			for (var i = 58; i < buffer.Length; i += 13)
				Assert.AreEqual(field.MakeMove(buffer[i] + 1, buffer[i + 1] + 1), true);
			
			Assert.AreEqual(field.RedCaptureCount, 179);
			Assert.AreEqual(field.BlueCaptureCount, 20);

			field.UnmakeAllMoves();

			Assert.AreEqual(field.RedCaptureCount, 0);
			Assert.AreEqual(field.BlueCaptureCount, 0);
			Assert.AreEqual(field.IsEmpty, true);
		}
	}
}

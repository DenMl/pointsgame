using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Dots.Library;

namespace DotsTests
{
	/// <summary>
	/// Summary description for FieldFunctionsTest
	/// </summary>
	[TestClass]
	public class DotFunctionsTest
	{
		public DotFunctionsTest()
		{
			//
			// TODO: Add constructor logic here
			//
		}

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
		// You can use the following additional attributes as you write your tests:
		//
		// Use ClassInitialize to run code before running the first test in the class
		// [ClassInitialize()]
		// public static void MyClassInitialize(TestContext testContext) { }
		//
		// Use ClassCleanup to run code after all tests in a class have run
		// [ClassCleanup()]
		// public static void MyClassCleanup() { }
		//
		// Use TestInitialize to run code before running each test 
		// [TestInitialize()]
		// public void MyTestInitialize() { }
		//
		// Use TestCleanup to run code after each test has run
		// [TestCleanup()]
		// public void MyTestCleanup() { }
		//
		#endregion

		[TestMethod()]
		public void IsRedDotTest()
		{
			Field field = new Field(39, 32);

			var buffer = DotsTests.Properties.Resources.DotFunctionsTest;

			for (var i = 58; i < buffer.Length; i += 13)
			{
				field.MakeMove(buffer[i] + 1, buffer[i + 1] + 1);
				if (field.DotsSequenceCount == 25)
				{
					int pos = Field.GetPosition(13, 13);

					Assert.IsTrue(field[pos].IsRedPutted());
					Assert.IsTrue(field[pos + 1].IsRedPutted());
					Assert.IsTrue(field[pos + 2].IsRedPutted());
					Assert.IsTrue(field[pos + 3].IsRedPutted());
				}
				else if (field.DotsSequenceCount == 58)
				{
					int pos = Field.GetPosition(13, 13);

					Assert.IsTrue(field[pos].IsBluePutted());
					Assert.IsTrue(field[pos + 1].IsBluePutted());
					Assert.IsTrue(field[pos + 2].IsBluePutted());
					Assert.IsTrue(field[pos + 3].IsBluePutted());
					Assert.IsTrue(field[Field.GetPosition(13 - 3, 13 - 1)].IsBluePutted());
					Assert.IsTrue(field[Field.GetPosition(13 + 5, 13)].IsBluePutted());

					Assert.IsFalse(field[pos].IsRedPutted());
					Assert.IsFalse(field[pos + 1].IsRedPutted());
					Assert.IsFalse(field[pos + 2].IsRedPutted());
					Assert.IsFalse(field[pos + 3].IsRedPutted());
				}
			}

			Assert.AreEqual(field.RedCaptureCount, 0);
			Assert.AreEqual(field.BlueCaptureCount, 16);

			field.UnmakeAllMoves();

			Assert.AreEqual(field.RedCaptureCount, 0);
			Assert.AreEqual(field.BlueCaptureCount, 0);
			Assert.AreEqual(field.IsEmpty, true);
		}
	}
}

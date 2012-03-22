using Dots.AI;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Dots.Library;
using System.Collections.Generic;
using System.Linq;

namespace DotsTests
{
	/// <summary>
	///This is a test class for LinkedGroupTest and is intended
	///to contain all LinkedGroupTest Unit Tests
	///</summary>
	[TestClass()]
	public class LinkedGroupTest
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

		[TestMethod()]
		public void EnvelopeOneDotTest()
		{
			int startX = 16;
			int startY = 16;
			Field field = new Field();

			field.MakeMove(startX, startY, Dot.RedPlayer);

			LinkedGroup linkedGroup = new LinkedGroup(Dot.RedPlayer, field.DotsSequancePositions.ToList());
			linkedGroup.BuildEnvelope();

			Assert.AreEqual(linkedGroup.EnvelopePositions.Count(), 1);

			var envelopePositions = linkedGroup.EnvelopePositions;
			Assert.IsTrue(envelopePositions.Contains(Field.GetPosition(startX, startY)));
		}

		[TestMethod()]
		public void EnvelopeTwoDotsTest()
		{
			int startX = 16;
			int startY = 16;
			Field field = new Field();

			field.MakeMove(startX, startY, Dot.RedPlayer);
			field.MakeMove(startX, startY + 3, Dot.RedPlayer);

			LinkedGroup linkedGroup = new LinkedGroup(Dot.RedPlayer, field.DotsSequancePositions.ToList());
			linkedGroup.BuildEnvelope();

			Assert.AreEqual(linkedGroup.EnvelopePositions.Count(), 2);
		}

		[TestMethod()]
		public void EnvelopeThreeDotsTest()
		{
			int startX = 16;
			int startY = 16;
			Field field = new Field();

			field.MakeMove(startX, startY, Dot.RedPlayer);
			field.MakeMove(startX + 3, startY + 3, Dot.RedPlayer);
			field.MakeMove(startX, startY + 6, Dot.RedPlayer);

			LinkedGroup linkedGroup = new LinkedGroup(Dot.RedPlayer, field.DotsSequancePositions.ToList());
			linkedGroup.BuildEnvelope();

			Assert.AreEqual(linkedGroup.EnvelopePositions.Count(), 3);
		}

		/// <summary>
		///A test for BuildEnvelope
		///</summary>
		[TestMethod()]
		public void EnvelopeManyDotsTest()
		{
			int startX = 16;
			int startY = 16;
			Field field = new Field();

			field.MakeMove(startX, startY, Dot.RedPlayer);
			field.MakeMove(startX, startY + 1, Dot.RedPlayer);

			field.MakeMove(startX + 2, startY - 2, Dot.RedPlayer);
			field.MakeMove(startX + 2, startY, Dot.RedPlayer);
			field.MakeMove(startX + 2, startY + 1, Dot.RedPlayer);

			field.MakeMove(startX + 4, startY - 2, Dot.RedPlayer);
			field.MakeMove(startX + 4, startY, Dot.RedPlayer);

			field.MakeMove(startX + 5, startY - 1, Dot.RedPlayer);
			field.MakeMove(startX + 5, startY + 2, Dot.RedPlayer);

			field.MakeMove(startX + 6, startY - 4, Dot.RedPlayer);
			field.MakeMove(startX + 6, startY, Dot.RedPlayer);

			field.MakeMove(startX + 7, startY - 2, Dot.RedPlayer);

			LinkedGroup linkedGroup = new LinkedGroup(Dot.RedPlayer, field.DotsSequancePositions.ToList());
			linkedGroup.BuildEnvelope();

			Assert.AreEqual(linkedGroup.EnvelopePositions.Count(), 6);

			var envelopePositions = linkedGroup.EnvelopePositions;
			Assert.IsTrue(envelopePositions.Contains(Field.GetPosition(startX, startY)));
			Assert.IsTrue(envelopePositions.Contains(Field.GetPosition(startX, startY + 1)));
			Assert.IsTrue(envelopePositions.Contains(Field.GetPosition(startX + 2, startY - 2)));
			Assert.IsTrue(envelopePositions.Contains(Field.GetPosition(startX + 5, startY + 2)));
			Assert.IsTrue(envelopePositions.Contains(Field.GetPosition(startX + 6, startY - 4)));
			Assert.IsTrue(envelopePositions.Contains(Field.GetPosition(startX + 7, startY - 2)));
		}
	}
}

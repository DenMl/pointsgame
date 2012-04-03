using Dots.AI;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Dots.Library;
using System.Linq;

namespace DotsTests
{
	/// <summary>
	///This is a test class for StrategicalMovesGeneratorTest and is intended
	///to contain all StrategicalMovesGeneratorTest Unit Tests
	///</summary>
	[TestClass()]
	public class StrategicalMovesGeneratorTest
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
		///A test for GenerateGroups
		///</summary>
		[TestMethod()]
		public void GenerateGroupsTest()
		{
			var field = new Field();
			var buffer = DotsTests.Properties.Resources.VeryLongGame;
			for (var i = 58; i < buffer.Length; i += 13)
				field.MakeMove(buffer[i] + 1, buffer[i + 1] + 1);

			var analyzer = new StrategicMovesAnalyzer(field);
			analyzer.GenerateGroups();

			//Assert.AreEqual(analyzer.RedGroups.Count(), 5);
			//Assert.AreEqual(analyzer.BlueGroups.Count(), 16);
		}
	}
}

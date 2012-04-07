using Dots.Library;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Dots.AI;

namespace DotsTests
{
	/// <summary>
	///This is a test class for AlphaBetaPruningAlgoritmTest and is intended
	///to contain all AlphaBetaPruningAlgoritmTest Unit Tests
	///</summary>
	[TestClass()]
	public class AlphaBetaAlgoritmTest
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
		public void SimpleAttackTest()
		{
			int startX = 16;
			int startY = 16;
			var field = new Field(39, 32);

			field.MakeMove(startX, startY);
			field.MakeMove(startX + 1, startY);

			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX, startY + 1);

			field.MakeMove(startX + 1, startY - 1);
			field.MakeMove(startX, startY - 1);

			int realBestMove = Field.GetPosition(startX + 2, startY);

			var alphaBetaAlgoritm = new AlphaBetaAlgoritm(field);
			int alphaBetaBestMove = alphaBetaAlgoritm.SearchBestMove(1, Dot.RedPlayer,
				-AiSettings.InfinityScore, AiSettings.InfinityScore);

			Assert.AreEqual(alphaBetaBestMove, realBestMove);

			alphaBetaAlgoritm = new AlphaBetaAlgoritm(field);
			alphaBetaBestMove = alphaBetaAlgoritm.SearchBestMove(2, Dot.RedPlayer,
				-AiSettings.InfinityScore, AiSettings.InfinityScore);

			Assert.AreEqual(alphaBetaBestMove, realBestMove);
		}

		[TestMethod()]
		public void SimpleDefenseTest()
		{
			int startX = 16;
			int startY = 16;
			var field = new Field(39, 32);

			field.MakeMove(startX, startY);
			field.MakeMove(startX + 1, startY);

			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX, startY + 1);

			field.MakeMove(startX + 10, startY - 1);
			field.MakeMove(startX, startY - 1);

			int realBestMove = Field.GetPosition(startX - 1, startY);

			var alphaBetaAlgoritm = new AlphaBetaAlgoritm(field);
			int alphaBetaBestMove = alphaBetaAlgoritm.SearchBestMove(2, Dot.RedPlayer,
				-AiSettings.InfinityScore, AiSettings.InfinityScore);

			Assert.AreEqual(alphaBetaBestMove, realBestMove);

			alphaBetaAlgoritm = new AlphaBetaAlgoritm(field);
			alphaBetaBestMove = alphaBetaAlgoritm.SearchBestMove(3, Dot.RedPlayer,
				-AiSettings.InfinityScore, AiSettings.InfinityScore);

			Assert.AreEqual(alphaBetaBestMove, realBestMove);
		}

		[TestMethod()]
		public void SquareTest()
		{
			int startX = 16;
			int startY = 16;
			var field = new Field(39, 32);

			field.MakeMove(startX, startY);
			field.MakeMove(startX + 1, startY);

			field.MakeMove(startX + 1, startY - 1);
			field.MakeMove(startX + 10, startY + 1);

			field.MakeMove(startX + 2, startY - 1);
			field.MakeMove(startX + 10, startY + 2);

			field.MakeMove(startX + 3, startY - 1);
			field.MakeMove(startX + 10, startY + 3);

			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX + 11, startY + 1);

			field.MakeMove(startX + 2, startY + 1);
			field.MakeMove(startX + 11, startY + 2);

			field.MakeMove(startX + 3, startY + 1);
			field.MakeMove(startX + 11, startY + 3);

			int realBestMove = Field.GetPosition(startX + 4, startY);

			var alphaBetaAlgoritm = new AlphaBetaAlgoritm(field);
			int alphaBetaBestMove = alphaBetaAlgoritm.SearchBestMove(1, Dot.RedPlayer,
				-AiSettings.InfinityScore, AiSettings.InfinityScore);

			Assert.AreEqual(alphaBetaBestMove, realBestMove);

			alphaBetaAlgoritm = new AlphaBetaAlgoritm(field);
			alphaBetaBestMove = alphaBetaAlgoritm.SearchBestMove(2, Dot.RedPlayer,
				-AiSettings.InfinityScore, AiSettings.InfinityScore);

			Assert.AreEqual(alphaBetaBestMove, realBestMove);
		}

		[TestMethod()]
		public void SimpleBlueAttackTest()
		{
			int startX = 16;
			int startY = 16;
			var field = new Field(39, 32);

			field.MakeMove(startX, startY);
			field.MakeMove(startX + 1, startY);

			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX, startY + 1);

			field.MakeMove(startX + 1, startY - 1);
			field.MakeMove(startX, startY - 1);

			field.MakeMove(startX + 10, startY);

			int realBestMove = Field.GetPosition(startX - 1, startY);

			var alphaBetaAlgoritm = new AlphaBetaAlgoritm(field);
			int alphaBetaBestMove = alphaBetaAlgoritm.SearchBestMove(1, Dot.BluePlayer,
				-AiSettings.InfinityScore, AiSettings.InfinityScore);

			Assert.AreEqual(alphaBetaBestMove, realBestMove);

			alphaBetaAlgoritm = new AlphaBetaAlgoritm(field);
			alphaBetaBestMove = alphaBetaAlgoritm.SearchBestMove(2, Dot.BluePlayer,
				-AiSettings.InfinityScore, AiSettings.InfinityScore);

			Assert.AreEqual(alphaBetaBestMove, realBestMove);
		}

		[TestMethod()]
		public void SimpleBlueDefenseTest()
		{
			int startX = 16;
			int startY = 16;
			var field = new Field(39, 32);

			field.MakeMove(startX, startY);
			field.MakeMove(startX + 1, startY);

			field.MakeMove(startX + 1, startY + 1);
			field.MakeMove(startX, startY + 1);

			field.MakeMove(startX + 1, startY - 1);
			field.MakeMove(startX + 10, startY - 1);

			field.MakeMove(startX - 2, startY);

			int realBestMove = Field.GetPosition(startX + 2, startY);

			var alphaBetaAlgoritm = new AlphaBetaAlgoritm(field);
			int alphaBetaBestMove = alphaBetaAlgoritm.SearchBestMove(2, Dot.BluePlayer,
				-AiSettings.InfinityScore, AiSettings.InfinityScore);

			Assert.AreEqual(alphaBetaBestMove, realBestMove);

			alphaBetaAlgoritm = new AlphaBetaAlgoritm(field);
			alphaBetaBestMove = alphaBetaAlgoritm.SearchBestMove(3, Dot.BluePlayer,
				-AiSettings.InfinityScore, AiSettings.InfinityScore);

			Assert.AreEqual(alphaBetaBestMove, realBestMove);
		}
	}
}

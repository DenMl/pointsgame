using Dots.Library;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Diagnostics;
using Dots.AI;

namespace DotsTests
{
	/// <summary>
	///This is a test class for AlphaBetaHashAlgoritmTest and is intended
	///to contain all AlphaBetaHashAlgoritmTest Unit Tests
	///</summary>
	[TestClass()]
	public class AlphaBetaHashAlgoritmTest
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
		public void AlphaBetaUsualAndHashPerformanceTest()
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

			var stopwatch = new Stopwatch();
			byte depth = 6;

			var alphaBetaAlgoritm = new AlphaBetaAlgoritm(field);
			stopwatch.Start();
			int alphaBetaBestMove = alphaBetaAlgoritm.SearchBestMove(depth, Dot.RedPlayer, -AiSettings.InfinityScore, AiSettings.InfinityScore);
			stopwatch.Stop();
			TimeSpan alphaBetaElapsed = stopwatch.Elapsed;
			stopwatch.Reset();
			
			var alphaBetaHashAlgoritm = new AlphaBetaHashAlgoritm(field);
			stopwatch.Start();
			int alphaBetaHashBestMove = alphaBetaHashAlgoritm.SearchBestMove((byte)depth, Dot.RedPlayer, -AiSettings.InfinityScore, AiSettings.InfinityScore);
			stopwatch.Stop();
			TimeSpan alphaBetaHashElapsed = stopwatch.Elapsed;

			Assert.AreEqual(alphaBetaBestMove, alphaBetaHashBestMove);
			//if (depth > 2)
			//	Assert.IsTrue(alphaBetaHashElapsed < alphaBetaElapsed);

			#if DEBUG
				Console.WriteLine("Configuration: Debug");
			#else
				Console.WriteLine("Configuration: Release");
			#endif
			Console.WriteLine("Depth: {0}", depth);
			Console.WriteLine("Usual AlphaBeta time elapsed: {0}", alphaBetaElapsed);
			Console.WriteLine("Hash AlphaBeta time elapsed: {0}", alphaBetaHashElapsed);
			Console.WriteLine("Ratio: {0}", (double)alphaBetaHashElapsed.Ticks / (double)alphaBetaElapsed.Ticks);
		}
	}
}

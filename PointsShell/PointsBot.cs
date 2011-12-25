using System.Runtime.InteropServices;

namespace PointsShell
{
#if x32
	using size_t = System.Int32;
#elif x64
	using size_t = System.Int64;
#endif

	class PointsBot
	{
#if DEBUG
		const string DllName = "../../../../PointsBot/Debug/PointsBot.dll";
#else
		const string DllName = "PointsBot.dll";
#endif

		private readonly size_t _handle;

		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern int InitField(size_t width, size_t height, size_t surCond, size_t beginPattern);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void FinalField(size_t field);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void PutPoint(size_t field, size_t x, size_t y);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void PutPlayersPoint(size_t field, size_t x, size_t y, size_t player);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void RemoveLastPoint(size_t field);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void GetBotMove(size_t field, size_t minMaxDepth, size_t UCTIterations, ref short x, ref short y);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void SetCurrentPlayer(size_t field, size_t player);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern size_t GetCurrentPlayer(size_t field);
		[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
		private static extern void SetNextPlayer(size_t field);

		public PointsBot(int width, int height, SurroundCond surCond, BeginPattern beginPattern)
		{
			_handle = InitField((size_t)width, (size_t)height, (size_t)surCond, (size_t)beginPattern);
		}

		~PointsBot()
		{
			FinalField(_handle);
		}

		public void PutPoint(Pos pos)
		{
			PutPoint(_handle, (size_t)(pos.X - 1), (size_t)(pos.Y - 1));
		}

		public void PutPoint(Pos pos, PlayerColor player)
		{
			PutPlayersPoint(_handle, (size_t)(pos.X - 1), (size_t)(pos.Y - 1), (size_t)player);
		}

		public void RemoveLastPoint()
		{
			RemoveLastPoint(_handle);
		}

		public Pos GetBotMovie(int minMaxDepth, int UCTIterations)
		{
			short x = 0, y = 0;
			GetBotMove(_handle, (size_t)minMaxDepth, (size_t)UCTIterations, ref x, ref y);
			x++;
			y++;
			return new Pos(x, y);
		}

		public void SetCurrentPlayer(PlayerColor player)
		{
			SetCurrentPlayer(_handle, (size_t)player);
		}

		public PlayerColor GetCurrentPlayer()
		{
			return (PlayerColor)GetCurrentPlayer(_handle);
		}

		public void SetNextPlayer()
		{
			SetNextPlayer(_handle);
		}
	}
}
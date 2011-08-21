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

        private readonly size_t Handle;

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int InitField(size_t Width, size_t Height, size_t SurCond, size_t BeginPattern);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void FinalField(size_t Field);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void PutPoint(size_t Field, size_t X, size_t Y);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void PutPlayersPoint(size_t Field, size_t X, size_t Y, size_t Player);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void RemoveLastPoint(size_t Field);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void GetBotMove(size_t Field, size_t MinMaxDepth, size_t UCTIterations, ref short X, ref short Y);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetCurrentPlayer(size_t Field, size_t Player);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern size_t GetCurrentPlayer(size_t Field);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetNextPlayer(size_t Field);

        public PointsBot(int Width, int Height, SurroundCond SurCond, BeginPattern BeginPattern)
        {
            Handle = InitField((size_t)Width, (size_t)Height, (size_t)SurCond, (size_t)BeginPattern);
        }

        ~PointsBot()
        {
            FinalField(Handle);
        }

        public void PutPoint(Pos pos)
        {
            PutPoint(Handle, (size_t)(pos.X - 1), (size_t)(pos.Y - 1));
        }

        public void PutPoint(Pos pos, PlayerColor Player)
        {
            PutPlayersPoint(Handle, (size_t)(pos.X - 1), (size_t)(pos.Y - 1), (size_t)Player);
        }

        public void RemoveLastPoint()
        {
            RemoveLastPoint(Handle);
        }

        public Pos GetBotMovie(int MinMaxDepth, int UCTIterations)
        {
            short X = 0, Y = 0;
            GetBotMove(Handle, (size_t)MinMaxDepth, (size_t)UCTIterations, ref X, ref Y);
            X++;
            Y++;
            return new Pos(X, Y);
        }

        public void SetCurrentPlayer(PlayerColor Player)
        {
            SetCurrentPlayer(Handle, (size_t)Player);
        }

        public PlayerColor GetCurrentPlayer()
        {
            return (PlayerColor)GetCurrentPlayer(Handle);
        }

        public void SetNextPlayer()
        {
            SetNextPlayer(Handle);
        }
    }
}
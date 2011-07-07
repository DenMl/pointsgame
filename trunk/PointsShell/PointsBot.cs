using System.Runtime.InteropServices;

namespace PointsShell
{
    class PointsBot
    {
        #if DEBUG
            const string DllName = "../../../../PointsBot/Debug/PointsBot.dll";
        #else
            const string DllName = "PointsBot.dll";
        #endif

        private readonly int Handle;

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int InitField(int Width, int Height, int SurCond, int BeginPattern);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void FinalField(int Field);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void PutPoint(int Field, int X, int Y);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void PutPlayersPoint(int Field, int X, int Y, PlayerColor Player);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void RemoveLastPoint(int Field);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void GetBotMove(int Field, int MinMaxDepth, int UCTIterations, ref int X, ref int Y);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetCurrentPlayer(int Field, int Player);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetCurrentPlayer(int Field);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetNextPlayer(int Field);

        public PointsBot(int Width, int Height, SurroundCond SurCond, BeginPattern BeginPattern)
        {
            Handle = InitField(Width, Height, (int)SurCond, (int)BeginPattern);
        }

        ~PointsBot()
        {
            FinalField(Handle);
        }

        public void PutPoint(Pos pos)
        {
            PutPoint(Handle, pos.X, pos.Y);
        }

        public void PutPoint(Pos pos, PlayerColor Player)
        {
            PutPlayersPoint(Handle, pos.X, pos.Y, Player);
        }

        public void RemoveLastPoint()
        {
            RemoveLastPoint(Handle);
        }

        public Pos GetBotMovie(int MinMaxDepth, int UCTIterations)
        {
            var Result = new Pos();
            GetBotMove(Handle, MinMaxDepth, UCTIterations, ref Result.X, ref Result.Y);
            return Result;
        }

        public void SetCurrentPlayer(PlayerColor Player)
        {
            SetCurrentPlayer(Handle, (int)Player);
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
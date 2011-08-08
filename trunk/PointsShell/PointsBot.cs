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
        private static extern int InitField(ushort Width, ushort Height, int SurCond, int BeginPattern);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void FinalField(int Field);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void PutPoint(int Field, ushort X, ushort Y);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void PutPlayersPoint(int Field, ushort X, ushort Y, PlayerColor Player);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void RemoveLastPoint(int Field);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void GetBotMove(int Field, uint MinMaxDepth, uint UCTIterations, ref ushort X, ref ushort Y);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetCurrentPlayer(int Field, PlayerColor Player);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern short GetCurrentPlayer(int Field);
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetNextPlayer(int Field);

        public PointsBot(int Width, int Height, SurroundCond SurCond, BeginPattern BeginPattern)
        {
            Handle = InitField((ushort)Width, (ushort)Height, (int)SurCond, (int)BeginPattern);
        }

        ~PointsBot()
        {
            FinalField(Handle);
        }

        public void PutPoint(Pos pos)
        {
            PutPoint(Handle, (ushort)(pos.X - 1), (ushort)(pos.Y - 1));
        }

        public void PutPoint(Pos pos, PlayerColor Player)
        {
            PutPlayersPoint(Handle, (ushort)(pos.X - 1), (ushort)(pos.Y - 1), Player);
        }

        public void RemoveLastPoint()
        {
            RemoveLastPoint(Handle);
        }

        public Pos GetBotMovie(int MinMaxDepth, int UCTIterations)
        {
            ushort X = 0, Y = 0;
            GetBotMove(Handle, (uint)MinMaxDepth, (uint)UCTIterations, ref X, ref Y);
            X++;
            Y++;
            return new Pos(X, Y);
        }

        public void SetCurrentPlayer(PlayerColor Player)
        {
            SetCurrentPlayer(Handle, Player);
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
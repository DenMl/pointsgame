using System.Collections.Generic;

namespace PointsShell
{
    // Возможные цвета игроков.
    public enum PlayerColor
    {
        Red,
        Black
    }

    // Возможные типы игры:
    // Standart - классический тип.
    // Always - всегда захватывает тот игрок, в чье пустое окружение поставлена точка.
    // AlwaysEnemy - захватывать даже пустые области.
    public enum SurroundCond
    {
        Standart,
        Always,
        AlwaysEnemy
    }

    public enum BeginPattern
    {
        CleanPattern,
        CrosswisePattern,
        SquarePattern
    }

    public class Field
    {
        // Ширина поля.
        public int Width { get; protected set; }
        // Высота поля.
        public int Height { get; protected set; }
        public SurroundCond SurCond { get; protected set; }

        public int CaptureCountRed;
        public int CaptureCountBlack;
        public PlayerColor CurPlayer;
        public PlayerColor EnemyPlayer;

        protected int DCaptureCount;
        protected int DFreedCount;

        // Глобальный массив точек.
        public GamePoint[,] Points { get; protected set; }
        // Последовательность поставленных точек.
        public List<Pos> PointsSeq { get; protected set; }
        protected readonly StateChange MainState;

        // Количество поставленных точек.
        public int PointsCount
        {
            get { return PointsSeq.Count; }
        }

        public List<List<Pos>> LastChains;

        public Field(int width, int height, SurroundCond surCond)
        {
            Width = width;
            Height = height;
            SurCond = surCond;
            CaptureCountRed = 0;
            CaptureCountBlack = 0;
            CurPlayer = PlayerColor.Red;
            EnemyPlayer = PlayerColor.Black;

            Points = new GamePoint[width + 2, height + 2];
            PointsSeq = new List<Pos>();
            MainState = new StateChange();

            // Помечаем граничные точки как невалидные.
            for (var i = 0; i < width + 2; i++)
            {
                Points[i, 0].Bad = true;
                Points[i, height + 1].Bad = true;
            }
            for (var i = 0; i < height + 2; i++)
            {
                Points[0, i].Bad = true;
                Points[width + 1, i].Bad = true;
            }
        }

        public static PlayerColor NextPlayer(PlayerColor Player)
        {
            return Player == PlayerColor.Red ? PlayerColor.Black : PlayerColor.Red;
        }

        public void SetNextPlayer()
        {
            CurPlayer = NextPlayer(CurPlayer);
            EnemyPlayer = NextPlayer(EnemyPlayer);
        }

        public void PlaceBeginPattern(BeginPattern BeginPattern)
        {
            Pos pos;
            switch (BeginPattern)
            {
                case (BeginPattern.CrosswisePattern):
                    pos = new Pos(Width / 2 - 1, Height / 2 - 1);
                    PutPoint(pos);
                    pos.X++;
                    PutPoint(pos);
                    pos.Y++;
                    PutPoint(pos);
                    pos.X--;
                    PutPoint(pos);
                    break;
                case (BeginPattern.SquarePattern):
                    pos = new Pos(Width / 2 - 1, Height / 2 - 1);
                    PutPoint(pos);
                    pos.X++;
                    PutPoint(pos);
                    pos.Y++;
                    pos.X--;
                    PutPoint(pos);
                    pos.X++;
                    PutPoint(pos);
                    break;
            }
        }

        // Направленная площадь параллелограмма с вершинами в (0; 0) и (Pos1, Pos2).
        static int Square(Pos Pos1, Pos Pos2)
        {
            return Pos1.X * Pos2.Y - Pos1.Y * Pos2.X;
        }

        // Возвращает количество групп точек рядом с CenterPos.
        // InpChainPoints - возможные точки цикла, InpSurPoints - возможные окруженные точки.
        int GetInputPoints(Pos CenterPos, PlayerColor Player, out List<Pos> InpChainPoints, out List<Pos> InpSurPoints)
        {
            InpChainPoints = new List<Pos>(4);
            InpSurPoints = new List<Pos>(4);

            if (!Points[CenterPos.X - 1, CenterPos.Y].Enabled(Player))
            {
                if (Points[CenterPos.X - 1, CenterPos.Y - 1].Enabled(Player))
                {
                    InpChainPoints.Add(new Pos(CenterPos.X - 1, CenterPos.Y - 1));
                    InpSurPoints.Add(new Pos(CenterPos.X - 1, CenterPos.Y));
                }
                else if (Points[CenterPos.X, CenterPos.Y - 1].Enabled(Player))
                {
                    InpChainPoints.Add(new Pos(CenterPos.X, CenterPos.Y - 1));
                    InpSurPoints.Add(new Pos(CenterPos.X - 1, CenterPos.Y));
                }
            }

            if (!Points[CenterPos.X, CenterPos.Y + 1].Enabled(Player))
            {
                if (Points[CenterPos.X - 1, CenterPos.Y + 1].Enabled(Player))
                {
                    InpChainPoints.Add(new Pos(CenterPos.X - 1, CenterPos.Y + 1));
                    InpSurPoints.Add(new Pos(CenterPos.X, CenterPos.Y + 1));
                }
                else if (Points[CenterPos.X - 1, CenterPos.Y].Enabled(Player))
                {
                    InpChainPoints.Add(new Pos(CenterPos.X - 1, CenterPos.Y));
                    InpSurPoints.Add(new Pos(CenterPos.X, CenterPos.Y + 1));
                }
            }

            if (!Points[CenterPos.X + 1, CenterPos.Y].Enabled(Player))
            {
                if (Points[CenterPos.X + 1, CenterPos.Y + 1].Enabled(Player))
                {
                    InpChainPoints.Add(new Pos(CenterPos.X + 1, CenterPos.Y + 1));
                    InpSurPoints.Add(new Pos(CenterPos.X + 1, CenterPos.Y));
                }
                else if (Points[CenterPos.X, CenterPos.Y + 1].Enabled(Player))
                {
                    InpChainPoints.Add(new Pos(CenterPos.X, CenterPos.Y + 1));
                    InpSurPoints.Add(new Pos(CenterPos.X + 1, CenterPos.Y));
                }
            }

            if (!Points[CenterPos.X, CenterPos.Y - 1].Enabled(Player))
            {
                if (Points[CenterPos.X + 1, CenterPos.Y - 1].Enabled(Player))
                {
                    InpChainPoints.Add(new Pos(CenterPos.X + 1, CenterPos.Y - 1));
                    InpSurPoints.Add(new Pos(CenterPos.X, CenterPos.Y - 1));
                }
                else if (Points[CenterPos.X + 1, CenterPos.Y].Enabled(Player))
                {
                    InpChainPoints.Add(new Pos(CenterPos.X + 1, CenterPos.Y));
                    InpSurPoints.Add(new Pos(CenterPos.X, CenterPos.Y - 1));
                }
            }

            return InpChainPoints.Count;
        }

        //  * . .   x . *   . x x   . . .
        //  . o .   x o .   . o .   . o x
        //  x x .   . . .   . . *   * . x
        //  o - center pos
        //  x - pos
        //  * - result
        static void GetFirstNextPos(Pos CenterPos, ref Pos Pos)
        {
            var Dx = Pos.X - CenterPos.X;
            var Dy = Pos.Y - CenterPos.Y;
            switch (Dy)
            {
                case -1:
                    if (Dx >= 0)
                    {
                        Pos.X = CenterPos.X + 1;
                        Pos.Y = CenterPos.Y + 1;
                    }
                    else
                    {
                        Pos.X = CenterPos.X + 1;
                        Pos.Y = CenterPos.Y - 1;
                    }
                    break;
                case 1:
                    if (Dx <= 0)
                    {
                        Pos.X = CenterPos.X - 1;
                        Pos.Y = CenterPos.Y - 1;
                    }
                    else
                    {
                        Pos.X = CenterPos.X - 1;
                        Pos.Y = CenterPos.Y + 1;
                    }
                    break;
                default:
                    if (Dx == 1)
                    {
                        Pos.X = CenterPos.X - 1;
                        Pos.Y = CenterPos.Y + 1;
                    }
                    else
                    {
                        Pos.X = CenterPos.X + 1;
                        Pos.Y = CenterPos.Y - 1;
                    }
                    break;
            }
        }

        //  . . .   * . .   x * .   . x *   . . x   . . .   . . .   . . .
        //  * o .   x o .   . o .   . o .   . o *   . o x   . o .   . o .
        //  x . .   . . .   . . .   . . .   . . .   . . *   . * x   * x .
        //  o - center pos
        //  x - pos
        //  * - result
        static void GetNextPos(Pos CenterPos, ref Pos Pos)
        {
            var Dx = Pos.X - CenterPos.X;
            var Dy = Pos.Y - CenterPos.Y;
            switch (Dy)
            {
                case -1:
                    if (Dx <= 0)
                        Pos.X++;
                    else
                        Pos.Y++;
                    break;
                case 1:
                    if (Dx >= 0)
                        Pos.X--;
                    else
                        Pos.Y--;
                    break;
                default:
                    if (Dx == 1)
                        Pos.Y++;
                    else
                        Pos.Y--;
                    break;
            }
        }

        protected void CheckCapturedAndFreed(Pos Pos, PlayerColor Player, ref int Captured, ref int Free)
        {
            if (!Points[Pos.X, Pos.Y].Putted)
                return;
            if (Points[Pos.X, Pos.Y].Color != Player)
                Captured++;
            else
                if (Points[Pos.X, Pos.Y].Surrounded)
                    Free++;
        }

        // Волновой алгоритм для определения захваченной зоны.
        // Начинает работать со StartPos.
        protected void CapturedAndFreedCount(Pos StartPos, PlayerColor Player, out int Captured, out int Freed, out List<Pos> SurPoints)
        {
            SurPoints = new List<Pos>();
            Captured = 0;
            Freed = 0;
            var Pos = StartPos;
            var TempStack = new Stack<Pos>();
            TempStack.Push(StartPos);
            Points[Pos.X, Pos.Y].Tagged = true;

            while (TempStack.Count != 0)
            {
                Pos = TempStack.Pop();
                CheckCapturedAndFreed(Pos, Player, ref Captured, ref Freed);
                SurPoints.Add(new Pos(Pos));

                if (!Points[Pos.X - 1, Pos.Y].IsBound(Player) && !Points[Pos.X - 1, Pos.Y].Tagged)
                {
                    TempStack.Push(new Pos(Pos.X - 1, Pos.Y));
                    Points[Pos.X - 1, Pos.Y].Tagged = true;
                }

                if (!Points[Pos.X, Pos.Y - 1].IsBound(Player) && !Points[Pos.X, Pos.Y - 1].Tagged)
                {
                    TempStack.Push(new Pos(Pos.X, Pos.Y - 1));
                    Points[Pos.X, Pos.Y - 1].Tagged = true;
                }

                if (!Points[Pos.X + 1, Pos.Y].IsBound(Player) && !Points[Pos.X + 1, Pos.Y].Tagged)
                {
                    TempStack.Push(new Pos(Pos.X + 1, Pos.Y));
                    Points[Pos.X + 1, Pos.Y].Tagged = true;
                }

                if (!Points[Pos.X, Pos.Y + 1].IsBound(Player) && !Points[Pos.X, Pos.Y + 1].Tagged)
                {
                    TempStack.Push(new Pos(Pos.X, Pos.Y + 1));
                    Points[Pos.X, Pos.Y + 1].Tagged = true;
                }
            }
        }

        // Удалить пустую базу, начиная с точки StartPos.
        // StartPos не попадет в список изменений доски.
        private void RemoveEmptyBase(Pos StartPos)
        {
            var TempStack = new Stack<Pos>();
            TempStack.Push(StartPos);
            Points[StartPos.X, StartPos.Y].EmptyBase = false;

            while (TempStack.Count != 0)
            {
                var Pos = TempStack.Pop();

                if (Points[Pos.X - 1, Pos.Y].EmptyBase)
                {
                    TempStack.Push(new Pos(Pos.X - 1, Pos.Y));
                    MainState.AddPosPoint(Pos, Points[Pos.X - 1, Pos.Y]);
                    Points[Pos.X - 1, Pos.Y].EmptyBase = false;
                }

                if (Points[Pos.X, Pos.Y - 1].EmptyBase)
                {
                    TempStack.Push(new Pos(Pos.X, Pos.Y - 1));
                    MainState.AddPosPoint(Pos, Points[Pos.X, Pos.Y - 1]);
                    Points[Pos.X, Pos.Y - 1].EmptyBase = false;
                }

                if (Points[Pos.X + 1, Pos.Y].EmptyBase)
                {
                    TempStack.Push(new Pos(Pos.X + 1, Pos.Y));
                    MainState.AddPosPoint(Pos, Points[Pos.X + 1, Pos.Y]);
                    Points[Pos.X + 1, Pos.Y].EmptyBase = false;
                }

                if (Points[Pos.X, Pos.Y + 1].EmptyBase)
                {
                    TempStack.Push(new Pos(Pos.X, Pos.Y + 1));
                    MainState.AddPosPoint(Pos, Points[Pos.X, Pos.Y + 1]);
                    Points[Pos.X, Pos.Y + 1].EmptyBase = false;
                }
            }
        }

        // Добавляет к счету игроков результат хода.
        protected void AddSubCapturedFreed(PlayerColor Player, int Captured, int Freed)
        {
            if (Captured == -1)
            {
                if (Player == 0)
                    CaptureCountRed++;
                else
                    CaptureCountBlack++;
            }
            else
            {
                if (Player == 0)
                {
                    CaptureCountRed += Captured;
                    CaptureCountBlack -= Freed;
                }
                else
                {
                    CaptureCountBlack += Captured;
                    CaptureCountRed -= Freed;
                }
            }
        }

        protected void SetCaptureFreeState(Pos Pos, PlayerColor Player)
        {
            if (Points[Pos.X, Pos.Y].Putted)
                Points[Pos.X, Pos.Y].Surrounded = Points[Pos.X, Pos.Y].Color != Player;
            else
                Points[Pos.X, Pos.Y].Surrounded = true;
        }

        private bool BuildChain(Pos StartPos, Pos InpChainPoint, out List<Pos> Chain)
        {
            var Player = Points[StartPos.X, StartPos.Y].Color;

            Chain = new List<Pos> { StartPos };
            var Pos = InpChainPoint;
            var CenterPos = StartPos;
            // Площадь базы.
            var TempSquare = Square(CenterPos, Pos);
            do
            {
                if (Points[Pos.X, Pos.Y].Tagged)
                {
                    while (Chain[Chain.Count - 1] != Pos)
                    {
                        Points[Chain[Chain.Count - 1].X, Chain[Chain.Count - 1].Y].Tagged = false;
                        Chain.RemoveAt(Chain.Count - 1);
                    }
                }
                else
                {
                    Points[Pos.X, Pos.Y].Tagged = true;
                    Chain.Add(Pos);
                }
                Pos.Swap(ref CenterPos);
                GetFirstNextPos(CenterPos, ref Pos);
                while (!Points[Pos.X, Pos.Y].Enabled(Player))
                    GetNextPos(CenterPos, ref Pos);
                TempSquare += Square(CenterPos, Pos);
            }
            while (Pos != StartPos);

            for (var j = 0; j < Chain.Count; j++)
                Points[Chain[j].X, Chain[j].Y].Tagged = false;

            return (TempSquare < 0) && (Chain.Count > 2);
        }

        private int BuildChains(Pos StartPos, IList<Pos> InpChainPoints, IList<Pos> InpSurPoints, int InpPointsCount, out List<Pos>[] Chains, out Pos[] InsidePoints)
        {
            Chains = new List<Pos>[4];
            InsidePoints = new Pos[4];

            var Count = 0;
            for (var i = 0; i < InpPointsCount; i++)
                if (BuildChain(StartPos, InpChainPoints[i], out Chains[Count]))
                {
                    InsidePoints[Count] = InpSurPoints[i];
                    Count++;
                }

            return Count;
        }

        private int BuildChainsFast(Pos StartPos, IList<Pos> InpChainPoints, IList<Pos> InpSurPoints, int InpPointsCount, out List<Pos>[] Chains, out Pos[] InsidePoints)
        {
            Chains = new List<Pos>[4];
            InsidePoints = new Pos[4];

            var Count = 0;
            for (var i = 0; i < InpPointsCount; i++)
                if (BuildChain(StartPos, InpChainPoints[i], out Chains[Count]))
                {
                    InsidePoints[Count] = InpSurPoints[i];
                    Count++;

                    if (Count == InpPointsCount - 1)
                        break;
                }

            return Count;
        }

        protected virtual void FindSurround(List<Pos> Chain, Pos InsidePoint, PlayerColor Player)
        {
            int CurCaptureCount, CurFreedCount;
            List<Pos> SurPoints;

            // Помечаем точки цепочки.
            for (var i = 0; i < Chain.Count; i++)
                Points[Chain[i].X, Chain[i].Y].Tagged = true;

            // Заливка захваченной области.
            CapturedAndFreedCount(InsidePoint, Player, out CurCaptureCount, out CurFreedCount, out SurPoints);
            // Изменение счета игроков.
            AddSubCapturedFreed(Player, CurCaptureCount, CurFreedCount);
            DCaptureCount += CurCaptureCount;
            DFreedCount += CurFreedCount;

            if ((CurCaptureCount != 0) || (SurCond == SurroundCond.Always)) // Если захватили точки, или стоит опция захватывать всегда.
            {
                for (var i = 0; i < Chain.Count; i++)
                {
                    Points[Chain[i].X, Chain[i].Y].Tagged = false;
                    // Добавляем в список изменений точки цепочки.
                    MainState.AddPosPoint(Chain[i], Points[Chain[i].X, Chain[i].Y]);
                    // Помечаем точки цепочки.
                    Points[Chain[i].X, Chain[i].Y].Bound = true;
                }

                for (var j = 0; j < SurPoints.Count; j++)
                {
                    Points[SurPoints[j].X, SurPoints[j].Y].Tagged = false;

                    MainState.AddPosPoint(SurPoints[j], Points[SurPoints[j].X, SurPoints[j].Y]);

                    SetCaptureFreeState(SurPoints[j], Player);
                }

                LastChains.Add(Chain);
            }
            else // Если ничего не захватили.
            {
                for (var i = 0; i < Chain.Count; i++)
                    Points[Chain[i].X, Chain[i].Y].Tagged = false;

                for (var i = 0; i < SurPoints.Count; i++)
                {
                    Points[SurPoints[i].X, SurPoints[i].Y].Tagged = false;

                    MainState.AddPosPoint(SurPoints[i], Points[SurPoints[i].X, SurPoints[i].Y]);

                    if (!Points[SurPoints[i].X, SurPoints[i].Y].Putted)
                        Points[SurPoints[i].X, SurPoints[i].Y].EmptyBase = true;
                }
            }
        }

        protected void FindSurrounds(List<Pos>[] Chains, Pos[] InsidePoints, int ChainsCount, PlayerColor Player)
        {
            for (var i = 0; i < ChainsCount; i++)
                FindSurround(Chains[i], InsidePoints[i], Player);
        }

        private enum IntersectionState
        {
            None,
            Up,
            Down,
            Target
        }
        private static IntersectionState GetIntersectionState(Pos Pos, Pos NextPos)
        {
            if (NextPos.X <= Pos.X)
                switch (NextPos.Y - Pos.Y)
                {
                    case (1):
                        return IntersectionState.Up;
                    case (0):
                        return IntersectionState.Target;
                    case (-1):
                        return IntersectionState.Down;
                    default:
                        return IntersectionState.None;
                }
            return IntersectionState.None;
        }

        protected bool PointInsideRing(Pos TestedPos, List<Pos> Ring)
        {
            var Intersections = 0;

            var State = IntersectionState.None;

            foreach (var pos in Ring)
            {
                switch (GetIntersectionState(TestedPos, pos))
                {
                    case (IntersectionState.None):
                        State = IntersectionState.None;
                        break;
                    case (IntersectionState.Up):
                        if (State == IntersectionState.Down)
                            Intersections++;
                        State = IntersectionState.Up;
                        break;
                    case (IntersectionState.Down):
                        if (State == IntersectionState.Up)
                            Intersections++;
                        State = IntersectionState.Down;
                        break;
                }
            }
            if (State == IntersectionState.Up || State == IntersectionState.Down)
            {
                var TempState = GetIntersectionState(TestedPos, Ring[0]);
                var i = 0;
                while (TempState == State || TempState == IntersectionState.Target)
                {
                    i++;
                    TempState = GetIntersectionState(TestedPos, Ring[i]);
                }
                if (TempState != IntersectionState.None)
                    Intersections++;
            }

            return Intersections % 2 == 1;
        }

        protected void CheckClosure(Pos StartPos)
        {
            int InpPointsCount;
            List<Pos> InpChainPoints, InpSurPoints;

            List<Pos>[] Chains;
            Pos[] InsidePoints;
            int ChainsCount;

            DCaptureCount = 0;
            DFreedCount = 0;

            LastChains = new List<List<Pos>>();

            // Цвет игрока, сделавшего последний ход.
            var OutPlayer = Points[StartPos.X, StartPos.Y].Color;

            if (Points[StartPos.X, StartPos.Y].EmptyBase) // Если точка поставлена в пустую базу.
            {
                // Проверяем, в чью пустую базу поставлена точка.
                var Pos = new Pos(StartPos.X - 1, StartPos.Y);
                while (!Points[Pos.X, Pos.Y].Putted)
                    Pos.X--;

                if (Points[Pos.X, Pos.Y].Color == Points[StartPos.X, StartPos.Y].Color) // Если поставили в свою пустую базу.
                {
                    Points[StartPos.X, StartPos.Y].EmptyBase = false;
                    return;
                }

                if (SurCond != SurroundCond.AlwaysEnemy) // Если приоритет не всегда у врага.
                {
                    InpPointsCount = GetInputPoints(StartPos, OutPlayer, out InpChainPoints, out InpSurPoints);
                    if (InpPointsCount > 1)
                    {
                        ChainsCount = BuildChainsFast(StartPos, InpChainPoints, InpSurPoints, InpPointsCount, out Chains, out InsidePoints);
                        FindSurrounds(Chains, InsidePoints, ChainsCount, OutPlayer);
                        if (Points[StartPos.X, StartPos.Y].Bound)
                        {
                            RemoveEmptyBase(StartPos);
                            return;
                        }
                    }
                }

                Pos.X++;
                do
                {
                    do
                    {
                        Pos.X--;
                        while (!Points[Pos.X, Pos.Y].Enabled(NextPlayer(OutPlayer)))
                            Pos.X--;
                        InpPointsCount = GetInputPoints(Pos, Points[Pos.X, Pos.Y].Color, out InpChainPoints, out InpSurPoints);
                        ChainsCount = BuildChains(Pos, InpChainPoints, InpSurPoints, InpPointsCount, out Chains, out InsidePoints);
                    } while (ChainsCount == 0);

                    for (var i = 0; i < ChainsCount; i++)
                        if (PointInsideRing(StartPos, Chains[i]))
                        {
                            FindSurround(Chains[i], InsidePoints[i], Points[Pos.X, Pos.Y].Color);
                            break;
                        }
                } while (!Points[StartPos.X, StartPos.Y].Surrounded);

                DCaptureCount = -1;
                return;
            }

            InpPointsCount = GetInputPoints(StartPos, OutPlayer, out InpChainPoints, out InpSurPoints);
            if (InpPointsCount > 1)
            {
                ChainsCount = BuildChainsFast(StartPos, InpChainPoints, InpSurPoints, InpPointsCount, out Chains, out InsidePoints);
                FindSurrounds(Chains, InsidePoints, ChainsCount, OutPlayer);
            }
        }

        public virtual void BackMove()
        {
            CaptureCountRed = MainState.States[MainState.States.Count - 1].CaptureCountsRed;
            CaptureCountBlack = MainState.States[MainState.States.Count - 1].CaptureCountsBlack;
            for (var i = MainState.States[MainState.States.Count - 1].PointPoses.Count - 1; i >= 0; i--) // Делаем отмену обязательно в обратном порядке, так как возможны дублирования позции (в частности, когда происходит окружение, точка, приведшая к окружению будет в изменениях 2 раза).
                // Стоит подумать над откатом поставленной точки отдельно. Но проблема - посталенная точка в свою пустую базу.
                Points[MainState.States[MainState.States.Count - 1].PointPoses[i].Pos.X, MainState.States[MainState.States.Count - 1].PointPoses[i].Pos.Y] = MainState.States[MainState.States.Count - 1].PointPoses[i].Point;
            CurPlayer = MainState.States[MainState.States.Count - 1].Player;
            EnemyPlayer = NextPlayer(CurPlayer);
            MainState.DeleteLastState();
            PointsSeq.RemoveAt(PointsCount - 1);
        }

        public bool PutPoint(Pos Point)
        {
            if (PutPoint(Point, CurPlayer))
            {
                SetNextPlayer();
                return true;
            }
            return false;
        }

        public virtual bool PutPoint(Pos Point, PlayerColor Player)
        {
            if (Point.X < 1 || Point.X > Width || Point.Y < 1 || Point.Y > Height)
                return false;

            if (!Points[Point.X, Point.Y].PuttingAllow())
                return false;

            // Добавляем новый список изменений поля.
            MainState.AddNewBase();
            // Запоминаем захваченные точки и поля игроков.
            MainState.AddCaptureCount(CaptureCountRed, CaptureCountBlack);
            // Запоминаем игрока, чей ход сейчас должен быть.
            MainState.AddPlayer(CurPlayer);

            MainState.AddPosPoint(Point, Points[Point.X, Point.Y]);

            Points[Point.X, Point.Y].Putted = true;
            Points[Point.X, Point.Y].Color = Player;
            PointsSeq.Add(Point);

            CheckClosure(Point);

            return true;
        }
    }
}
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Media;

namespace PointsShell
{
    public enum GameFormat
    {
        PointsXT,
        SGF,
        VKontacte,
        Unknown
    }

    public partial class Game
    {
        private const int CellSize = 18;

        private GamePreferences _preferences;
        public GamePreferences Preferences
        {
            get { return new GamePreferences(_preferences); }
            set
            {
                _preferences.AI = value.AI;
                _preferences.MinMaxDepth = value.MinMaxDepth;
                _preferences.UCTIterations = value.UCTIterations;
                _preferences.RedName = value.RedName;
                _preferences.BlackName = value.BlackName;
                _preferences.BackgroundColor = value.BackgroundColor;
                canvas.Background = new SolidColorBrush(Preferences.BackgroundColor);

                if (_preferences.RedColor != value.RedColor || _preferences.BlackColor != value.BlackColor || _preferences.FillingAlpha != value.FillingAlpha)
                {
                    _preferences.Sounds = false;

                    _preferences.RedColor = value.RedColor;
                    _preferences.BlackColor = value.BlackColor;
                    _preferences.FillingAlpha = value.FillingAlpha;

                    canvas.Children.Clear();
                    CanvasChildrenCount.Clear();
                    DrawField(_preferences.Width, _preferences.Height);
                    var LastField = Field;
                    Field = new Field(_preferences.Width, _preferences.Height, _preferences.SurCond);

                    foreach (var pos in LastField.PointsSeq)
                        PutPoint(new Pos(pos.X - 1, pos.Y - 1), LastField.Points[pos.X, pos.Y].Color);
                }
                _preferences.Sounds = value.Sounds;

                UpdateTextInfo();
            }
        }

        public Field Field { get; private set; }
        private PointsBot Bot;

        // Переменная, показывающая, выполняются ли в данный момент вычисления для хода ИИ.
        private bool Thinking;
        // Вспомогательный список, нужный для отката ходов.
        private readonly List<int> CanvasChildrenCount = new List<int>();

        private Game()
        {
            InitializeComponent();
        }

        public Game(GamePreferences preferences)
        {
            InitializeComponent();
            _preferences = preferences;
            Field = new Field(preferences.Width, preferences.Height, preferences.SurCond);
            Bot = new PointsBot(preferences.Width, preferences.Height, preferences.SurCond, BeginPattern.CleanPattern);
            DrawField(_preferences.Width, _preferences.Height);
            PlaceBeginPattern(preferences.BeginPattern);
            UpdateTextInfo();
        }

        public Game(GamePreferences preferences, GameLanguage language)
        {
            InitializeComponent();
            _preferences = preferences;
            Field = new Field(preferences.Width, preferences.Height, preferences.SurCond);
            Bot = new PointsBot(preferences.Width, preferences.Height, preferences.SurCond, BeginPattern.CleanPattern);
            DrawField(_preferences.Width, _preferences.Height);
            PlaceBeginPattern(preferences.BeginPattern);
            SetLanguage(language);
            UpdateTextInfo();
        }

        public static Game Load(string FileName, GamePreferences preferences)
        {
            var Format = GetFormat(FileName);
            switch (Format)
            {
                case (GameFormat.PointsXT):
                    var result = new Game {_preferences = preferences};
                    result.LoadXT(FileName);
                    return result;
                default:
                    return null;
            }
        }

        public static Game Load(string FileName, GamePreferences preferences, GameLanguage language)
        {
            var result = Load(FileName, preferences);
            if (result != null)
                result.SetLanguage(language);
            return result;
        }

        // Проверяет файл FileName на формат PointsXT и его валидность.
        public static bool IsXT(string FileName)
        {
            var Stream = new StreamReader(FileName);
            var buffer = new byte[Stream.BaseStream.Length];
            var Count = (int)Stream.BaseStream.Length;
            Stream.BaseStream.Read(buffer, 0, Count);
            Stream.Close();

            if (Count < 71) // Размер должен быть таким, чтобы в файле содержался как минимум 1 ход.
                return false;

            if ((Count - 58) % 13 != 0)
                return false;

            for (var i = 58; i < Count; i += 13)
                if (buffer[i] > 38 || buffer[i + 1] > 31 || (buffer[i + 3] != 0x00 && buffer[i + 3] != 0xFF))
                    return false;

            return true;
        }

        public static bool IsSGF(string FileName)
        {
            var s = "";

            using (var Stream = new StreamReader(FileName))
                while (!Stream.EndOfStream)
                    s += Stream.ReadLine();

            var re = new Regex(@"\s*\(\s*;(\s*[a-zA-Z]*\[.*\])*\s*SZ\[([a-zA-Z][a-zA-Z]|\d+[-:]\d+)\]");
            // \s* - любое число пробельных символов.
            // \( - открывающаяся скобка.
            // \s* - любое число пробельных символов.
            // ; - точка с запятой.
            // (\s*[a-zA-Z]*\[.*\])* - любое число выражений вида CA[UTF-8] AP[vpoints122] etc.
            // \s* - любое число пробельных символов.
            // SZ\[([a-zA-Z][a-zA-Z]|\d+[-:]\d+) - выражение SZ[NG]

            return re.IsMatch(s);
        }

        // Определяет формат сохранения для файла.
        public static GameFormat GetFormat(string FileName)
        {
            if (IsXT(FileName))
                return GameFormat.PointsXT;
            else if (IsSGF(FileName))
                return GameFormat.SGF;
            else
                return GameFormat.Unknown;
        }

        public void SetLanguage(GameLanguage language)
        {
            RedTextBlock.Text = language.Red;
            BlackTextBlock.Text = language.Black;
        }

        private static Pos ConvertToPos(Point Point)
        {
            return new Pos((int)Math.Round(Point.X / CellSize - 0.5), (int)Math.Round(Point.Y / CellSize - 0.5));
        }

        private void DrawField(int width, int height)
        {
            canvas.Width = width * CellSize;
            canvas.Height = height * CellSize;
            canvas.Background = new SolidColorBrush(Preferences.BackgroundColor);

            for (var i = 0; i < width; i++)
                canvas.Children.Add(new Line
                                        {
                                            X1 = CellSize * i + CellSize / 2,
                                            X2 = CellSize * i + CellSize / 2,
                                            Y1 = 0,
                                            Y2 = CellSize * height,
                                            Stroke = Brushes.Black,
                                            StrokeThickness = 0.5
                                        });

            for (var i = 0; i < height; i++)
                canvas.Children.Add(new Line
                                        {
                                            Y1 = CellSize * i + CellSize / 2,
                                            Y2 = CellSize * i + CellSize / 2,
                                            X1 = 0,
                                            X2 = CellSize * width,
                                            Stroke = Brushes.Black,
                                            StrokeThickness = 0.5
                                        });
        }

        private void UpdateTextInfo()
        {
            RedName.Text = _preferences.RedName;
            BlackName.Text = _preferences.BlackName;
            RedCount.Text = Field.CaptureCountRed.ToString();
            BlackCount.Text = Field.CaptureCountBlack.ToString();
            StepCount.Text = Field.PointsCount.ToString();
            if (Field.CurPlayer == PlayerColor.Red)
            {
                RedTextBlock.TextDecorations = TextDecorations.Underline;
                BlackTextBlock.TextDecorations = null;
            }
            else
            {
                RedTextBlock.TextDecorations = null;
                BlackTextBlock.TextDecorations = TextDecorations.Underline;
            }
        }

        private void PlaceBeginPattern(BeginPattern BeginPattern)
        {
            // Отключаем звуки.
            var Sounds = _preferences.Sounds;
            _preferences.Sounds = false;

            Pos pos;
            switch (BeginPattern)
            {
                case (BeginPattern.CrosswisePattern):
                    pos = new Pos(Preferences.Width / 2 - 1, Preferences.Height / 2 - 1);
                    PutPoint(pos);
                    pos.X++;
                    PutPoint(pos);
                    pos.Y++;
                    PutPoint(pos);
                    pos.X--;
                    PutPoint(pos);
                    break;
                case (BeginPattern.SquarePattern):
                    pos = new Pos(Preferences.Width / 2 - 1, Preferences.Height / 2 - 1);
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

            _preferences.Sounds = Sounds;
        }

        public bool PutPoint(Pos Point, PlayerColor Player)
        {
            if (!Field.PutPoint(Point, Player))
                return false;
            Bot.PutPoint(Point, Player);

            // Если стоит опция - воспроизводим звук.
            if (Preferences.Sounds)
                new SoundPlayer(Properties.Resources.Step).Play();
            
            if (Field.PointsCount != 1)
                canvas.Children.RemoveAt(canvas.Children.Count - 1);

            // Запомиаем количество обьектов на доске для отката ходов.
            CanvasChildrenCount.Add(canvas.Children.Count);

            // Рисуем поставленную точку.
            var E = new Ellipse
            {
                Fill = Player == PlayerColor.Red ? new SolidColorBrush(Preferences.RedColor) : new SolidColorBrush(Preferences.BlackColor),
                Width = 8,
                Height = 8
            };
            Canvas.SetLeft(E, Point.X * CellSize + CellSize / 2 - 4);
            Canvas.SetTop(E, Point.Y * CellSize + CellSize / 2 - 4);
            canvas.Children.Add(E);

            // Рисуем заливку.
            foreach (var chain in Field.LastChains)
            {
                var GraphicsPoints = new PointCollection(chain.Count);
                for (var i = 0; i < chain.Count; i++)
                    GraphicsPoints.Add(new Point((chain[i].X - 1) * CellSize + CellSize / 2, (chain[i].Y - 1) * CellSize + CellSize / 2));

                var Poly = new Polygon { Points = GraphicsPoints };
                if (Field.Points[chain[0].X, chain[0].Y].Color == PlayerColor.Red)
                {
                    Poly.Fill = new SolidColorBrush(Color.FromArgb(Preferences.FillingAlpha, Preferences.RedColor.R, Preferences.RedColor.G, Preferences.RedColor.B));
                    Poly.Stroke = new SolidColorBrush(Preferences.RedColor);
                }
                else
                {
                    Poly.Fill = new SolidColorBrush(Color.FromArgb(Preferences.FillingAlpha, Preferences.BlackColor.R, Preferences.BlackColor.G, Preferences.BlackColor.B));
                    Poly.Stroke = new SolidColorBrush(Preferences.BlackColor);
                }
                canvas.Children.Add(Poly);
            }

            // Обводим последнюю поставленную точку.
            E = new Ellipse
            {
                Stroke = Player == PlayerColor.Red ? new SolidColorBrush(Preferences.RedColor) : new SolidColorBrush(Preferences.BlackColor),
                Width = 12,
                Height = 12
            };
            Canvas.SetLeft(E, Point.X * CellSize + CellSize / 2 - 6);
            Canvas.SetTop(E, Point.Y * CellSize + CellSize / 2 - 6);
            canvas.Children.Add(E);

            UpdateTextInfo();

            return true;
        }

        public bool PutPoint(Pos Point)
        {
            if (PutPoint(Point, Field.CurPlayer))
            {
                SetNextPlayer();
                return true;
            }
            return false;
        }

        public void DoBotStep()
        {
            if (Thinking)
                return;

            Pos BotMovie;
            Thinking = true;
            try
            {
                BotMovie = Bot.GetBotMovie(Preferences.MinMaxDepth, Preferences.UCTIterations);
            }
            catch
            {
                SaveXT("bug.sav");
                MessageBox.Show("Error in PointsBot.dll");
                return;
            }
            Dispatcher.BeginInvoke(DispatcherPriority.Normal, (Action)(() => PutPoint(BotMovie)));
            Thinking = false;
        }

        public void BackMove()
        {
            if (Thinking || Field.PointsCount == 0)
                return;

            Field.BackMove();
            Bot.RemoveLastPoint();

            canvas.Children.RemoveRange(CanvasChildrenCount[CanvasChildrenCount.Count - 1], canvas.Children.Count - CanvasChildrenCount[CanvasChildrenCount.Count - 1]);
            CanvasChildrenCount.RemoveAt(CanvasChildrenCount.Count - 1);

            UpdateTextInfo();

            // Обводим последнюю поставленную точку, если такая есть.
            if (Field.PointsCount != 0)
            {
                var E = new Ellipse
                {
                    Stroke = Field.Points[Field.PointsSeq[Field.PointsCount - 1].X, Field.PointsSeq[Field.PointsCount - 1].Y].Color == PlayerColor.Red ? new SolidColorBrush(Preferences.RedColor) : new SolidColorBrush(Preferences.BlackColor),
                    Width = 12,
                    Height = 12
                };
                Canvas.SetLeft(E, (Field.PointsSeq[Field.PointsCount - 1].X - 1) * CellSize + CellSize / 2 - 6);
                Canvas.SetTop(E, (Field.PointsSeq[Field.PointsCount - 1].Y - 1) * CellSize + CellSize / 2 - 6);
                canvas.Children.Add(E);
            }
        }

        public void SetNextPlayer()
        {
            Field.SetNextPlayer();
            Bot.SetNextPlayer();
            UpdateTextInfo();
        }

        private void canvasMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (Thinking)
                return;

            var Movie = ConvertToPos(e.GetPosition(canvas));
            if (!PutPoint(Movie))
                return;
            Bot.PutPoint(Movie);
            if (Preferences.AI)
                (new Thread(DoBotStep)).Start();
        }

        private void canvasMouseMove(object sender, MouseEventArgs e)
        {
            var pos = ConvertToPos(e.GetPosition(canvas));
            MouseCoord.Text = pos.X + ":" + pos.Y;
        }

        public bool Save(string FileName, GameFormat Format)
        {
            switch (Format)
            {
                case (GameFormat.PointsXT):
                    return SaveXT(FileName);
                default:
                    return false;
            }
        }

        public bool SaveXT(string PointsXTFileName)
        {
            if (Preferences.Width != 39 || Preferences.Height != 32 || Preferences.SurCond != SurroundCond.Standart || Field.PointsCount == 0)
                return false;

            var Stream = new BinaryWriter(new FileStream(PointsXTFileName, FileMode.Create));
            // Первый байт - версия клиента.
            Stream.Write((byte)121);
            // Следующие 2 байта - количество поставленных точек - 1.
            Stream.Write((ushort)(Field.PointsCount - 1));
            // Далее 2 байта, указывающие на цвет последнего игрока, сделавшего ход.
            if (Field.Points[Field.PointsSeq[Field.PointsCount - 1].X, Field.PointsSeq[Field.PointsCount - 1].Y].Color == PlayerColor.Red)
                Stream.Write((ushort)0xFFFF);
            else
                Stream.Write((ushort)0x0000);
            // ???
            Stream.Write((ushort)0x0000);
            Stream.Write((ushort)0x0000);
            Stream.Write((ushort)0x0000);
            // Далее идут имена двух игроков по 9 байт.
            Stream.Write(Encoding.GetEncoding(1251).GetBytes(Preferences.RedName != null ? Preferences.RedName.PadRight(9).Substring(0, 9) : "         "));
            Stream.Write(Encoding.GetEncoding(1251).GetBytes(Preferences.BlackName != null ? Preferences.BlackName.PadRight(9).Substring(0, 9) : "         "));
            // Видимо, здесь в первых четырех байтах идет время сохранения партии или ее продолжительность, дальше нули.
            for (var i = 0; i < 29; i++)
                Stream.Write((byte)0x00);
            for (var i = 0; i < Field.PointsCount; i++)
            {
                // Далее координаты хода - X, Y.
                Stream.Write((byte)(Field.PointsSeq[i].X - 1));
                Stream.Write((byte)(Field.PointsSeq[i].Y - 1));
                // В этом байте помечается последовательность точек, от которых следует пускать волну для проверки окружений (которые были в процессе игры захвачены). Не страшно, если будут помечены все.
                Stream.Write((byte)1);
                // Затем цвет игрока, поставившего точку.
                if (Field.Points[Field.PointsSeq[i].X, Field.PointsSeq[i].Y].Color == PlayerColor.Red)
                    Stream.Write((ushort)0xFFFF);
                else
                    Stream.Write((ushort)0x0000);
                // ???
                for (var j = 0; j < 8; j++)
                    Stream.Write((byte)0x00);
            }
            Stream.Close();

            return true;
        }

        // Загрузка игры из формата PointsXT. Полное описание формата можно посмотреть в SaveXT.
        private void LoadXT(string PointsXTFileName)
        {
            Field = new Field(39, 32, SurroundCond.Standart);
            Bot = new PointsBot(39, 32, SurroundCond.Standart, BeginPattern.CleanPattern);
            DrawField(39, 32);

            var Stream = new StreamReader(PointsXTFileName);
            var buffer = new byte[Stream.BaseStream.Length];
            var Count = (int)Stream.BaseStream.Length;
            Stream.BaseStream.Read(buffer, 0, Count);
            Stream.Close();

            Preferences.RedName = Encoding.GetEncoding(1251).GetString(buffer, 11, 9);
            Preferences.BlackName = Encoding.GetEncoding(1251).GetString(buffer, 20, 9);

            // Отключаем звуки.
            var Sounds = _preferences.Sounds;
            _preferences.Sounds = false;

            for (var i = 58; i < Count; i += 13)
                PutPoint(new Pos(buffer[i], buffer[i + 1]), buffer[i + 3] == 0x00 ? PlayerColor.Black : PlayerColor.Red);
            Field.CurPlayer = buffer[Count - 10] == 0x00 ? PlayerColor.Red : PlayerColor.Black;

            _preferences.Sounds = Sounds;

            UpdateTextInfo();
        }
    }
}
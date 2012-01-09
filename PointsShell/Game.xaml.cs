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
using PointsShell.Bots;
using PointsShell.Enums;

namespace PointsShell
{
	public partial class Game
	{
		private GamePreferences _preferences;
		public GamePreferences Preferences
		{
			get { return new GamePreferences(_preferences); }
			set
			{
				_preferences.AI = value.AI;
				_preferences.Depth = value.Depth;
				_preferences.Iterations = value.Iterations;
				_preferences.RedName = value.RedName;
				_preferences.BlackName = value.BlackName;
				_preferences.BackgroundColor = value.BackgroundColor;
				canvas.Background = new SolidColorBrush(Preferences.BackgroundColor);

				if (_preferences.RedColor != value.RedColor || _preferences.BlackColor != value.BlackColor || _preferences.FillingAlpha != value.FillingAlpha || _preferences.CellSize != value.CellSize || _preferences.FullFill != value.FullFill)
				{
					_preferences.Sounds = false;

					_preferences.RedColor = value.RedColor;
					_preferences.BlackColor = value.BlackColor;
					_preferences.FillingAlpha = value.FillingAlpha;
					_preferences.CellSize = value.CellSize;
					_preferences.FullFill = value.FullFill;

					ReDraw();
				}
				_preferences.Sounds = value.Sounds;

				UpdateTextInfo();
			}
		}

		public Field Field { get; private set; }
		private DllBot _bot;

		// Переменная, показывающая, выполняются ли в данный момент вычисления для хода ИИ.
		private bool _thinking;
		// Вспомогательный список, нужный для отката ходов.
		private readonly List<int> _canvasChildrenCount = new List<int>();

		private Game()
		{
			InitializeComponent();
		}

		public Game(GamePreferences preferences)
		{
			InitializeComponent();
			_preferences = preferences;
			Field = new Field(preferences.Width, preferences.Height, preferences.SurCond);
			_bot = new DllBot(preferences.Width, preferences.Height, preferences.SurCond, BeginPattern.CleanPattern);
			DrawField(_preferences.Width, _preferences.Height);
			PlaceBeginPattern(preferences.BeginPattern);
			UpdateTextInfo();
		}

		public Game(GamePreferences preferences, GameLanguage language)
		{
			InitializeComponent();
			_preferences = preferences;
			Field = new Field(preferences.Width, preferences.Height, preferences.SurCond);
			_bot = new DllBot(preferences.Width, preferences.Height, preferences.SurCond, BeginPattern.CleanPattern);
			DrawField(_preferences.Width, _preferences.Height);
			PlaceBeginPattern(preferences.BeginPattern);
			SetLanguage(language);
			UpdateTextInfo();
		}

		public static Game Load(string fileName, GamePreferences preferences)
		{
			var format = GetFormat(fileName);
			switch (format)
			{
				case (GameFormat.PointsXT):
					var result = new Game {_preferences = preferences};
					result.LoadXT(fileName);
					return result;
				default:
					return null;
			}
		}

		public static Game Load(string fileName, GamePreferences preferences, GameLanguage language)
		{
			var result = Load(fileName, preferences);
			if (result != null)
				result.SetLanguage(language);
			return result;
		}

		// Проверяет файл FileName на формат PointsXT и его валидность.
		public static bool IsXT(string fileName)
		{
			var stream = new StreamReader(fileName);
			var buffer = new byte[stream.BaseStream.Length];
			var count = (int)stream.BaseStream.Length;
			stream.BaseStream.Read(buffer, 0, count);
			stream.Close();

			if (count < 71) // Размер должен быть таким, чтобы в файле содержался как минимум 1 ход.
				return false;

			if ((count - 58) % 13 != 0)
				return false;

			for (var i = 58; i < count; i += 13)
				if (buffer[i] > 38 || buffer[i + 1] > 31 || (buffer[i + 3] != 0x00 && buffer[i + 3] != 0xFF))
					return false;

			return true;
		}

		public static bool IsSGF(string fileName)
		{
			var s = "";

			using (var stream = new StreamReader(fileName))
				while (!stream.EndOfStream)
					s += stream.ReadLine();

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
		public static GameFormat GetFormat(string fileName)
		{
			if (IsXT(fileName))
				return GameFormat.PointsXT;
			if (IsSGF(fileName))
				return GameFormat.SGF;
			return GameFormat.Unknown;
		}

		public void SetLanguage(GameLanguage language)
		{
			RedTextBlock.Text = language.Red;
			BlackTextBlock.Text = language.Black;
		}

		private Pos ConvertToPos(Point point)
		{
			return new Pos((int)Math.Round(point.X / Preferences.CellSize - 0.5) + 1, (int)Math.Round(point.Y / Preferences.CellSize - 0.5) + 1);
		}
		private Point ConvertToGraphicsPoint(Pos pos)
		{
			return new Point((pos.X - 1) * Preferences.CellSize + Preferences.CellSize / 2, (pos.Y - 1) * Preferences.CellSize + Preferences.CellSize / 2);
		}

		private void DrawField(int width, int height)
		{
			canvas.Width = width * Preferences.CellSize;
			canvas.Height = height * Preferences.CellSize;
			canvas.Background = new SolidColorBrush(Preferences.BackgroundColor);

			for (var i = 0; i < width; i++)
				canvas.Children.Add(new Line
										{
											X1 = Preferences.CellSize * i + Preferences.CellSize / 2,
											X2 = Preferences.CellSize * i + Preferences.CellSize / 2,
											Y1 = 0,
											Y2 = Preferences.CellSize * height,
											Stroke = Brushes.Black,
											StrokeThickness = 0.5
										});

			for (var i = 0; i < height; i++)
				canvas.Children.Add(new Line
										{
											Y1 = Preferences.CellSize * i + Preferences.CellSize / 2,
											Y2 = Preferences.CellSize * i + Preferences.CellSize / 2,
											X1 = 0,
											X2 = Preferences.CellSize * width,
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

		private void PlaceBeginPattern(BeginPattern beginPattern)
		{
			// Отключаем звуки.
			var sounds = _preferences.Sounds;
			_preferences.Sounds = false;

			Pos pos;
			switch (beginPattern)
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

			_preferences.Sounds = sounds;
		}

		public void ReDraw()
		{
			canvas.Children.Clear();
			_canvasChildrenCount.Clear();
			DrawField(_preferences.Width, _preferences.Height);
			var LastField = Field;
			Field = new Field(_preferences.Width, _preferences.Height, _preferences.SurCond);

			foreach (var pos in LastField.PointsSeq)
				PutPoint(new Pos(pos.X - 1, pos.Y - 1), LastField.Points[pos.X, pos.Y].Color);
		}

		private void FillTriangle(Pos pos1, Pos pos2, Pos pos3, PlayerColor player)
		{
			canvas.Children.Add(new Polygon
									{
										Points =
											new PointCollection
												{
													ConvertToGraphicsPoint(pos1),
													ConvertToGraphicsPoint(pos2),
													ConvertToGraphicsPoint(pos3)
												},
										Fill =
											player == PlayerColor.Red
												? new SolidColorBrush(Color.FromArgb(Preferences.FillingAlpha,
																					 Preferences.RedColor.R,
																					 Preferences.RedColor.G,
																					 Preferences.RedColor.B))
												: new SolidColorBrush(Color.FromArgb(Preferences.FillingAlpha,
																					 Preferences.BlackColor.R,
																					 Preferences.BlackColor.G,
																					 Preferences.BlackColor.B))
									});
		}

		private void UpdateFullFill(Pos pos, PlayerColor player)
		{
			// Более компактная проверка, нужная дальше.
			Func<int, int, bool> test = (x, y) => Field.Points[x, y].Enabled(player);

			if (test(pos.X, pos.Y - 1) && test(pos.X + 1, pos.Y))
			{
				FillTriangle(pos, new Pos(pos.X, pos.Y - 1), new Pos(pos.X + 1, pos.Y), player);
			}
			else
			{
				if (test(pos.X, pos.Y - 1) && test(pos.X + 1, pos.Y - 1))
					FillTriangle(pos, new Pos(pos.X, pos.Y - 1), new Pos(pos.X + 1, pos.Y - 1), player);
				if (test(pos.X + 1, pos.Y) && test(pos.X + 1, pos.Y - 1))
					FillTriangle(pos, new Pos(pos.X + 1, pos.Y), new Pos(pos.X + 1, pos.Y - 1), player);
			}

			if (test(pos.X + 1, pos.Y) && test(pos.X, pos.Y + 1))
			{
				FillTriangle(pos, new Pos(pos.X + 1, pos.Y), new Pos(pos.X, pos.Y + 1), player);
			}
			else
			{
				if (test(pos.X + 1, pos.Y) && test(pos.X + 1, pos.Y + 1))
					FillTriangle(pos, new Pos(pos.X + 1, pos.Y), new Pos(pos.X + 1, pos.Y + 1), player);
				if (test(pos.X, pos.Y + 1) && test(pos.X + 1, pos.Y + 1))
					FillTriangle(pos, new Pos(pos.X, pos.Y + 1), new Pos(pos.X + 1, pos.Y + 1), player);
			}

			if (test(pos.X, pos.Y + 1) && test(pos.X - 1, pos.Y))
			{
				FillTriangle(pos, new Pos(pos.X, pos.Y + 1), new Pos(pos.X - 1, pos.Y), player);
			}
			else
			{
				if (test(pos.X, pos.Y + 1) && test(pos.X - 1, pos.Y + 1))
					FillTriangle(pos, new Pos(pos.X, pos.Y + 1), new Pos(pos.X - 1, pos.Y + 1), player);
				if (test(pos.X - 1, pos.Y) && test(pos.X - 1, pos.Y + 1))
					FillTriangle(pos, new Pos(pos.X - 1, pos.Y), new Pos(pos.X - 1, pos.Y + 1), player);
			}

			if (test(pos.X - 1, pos.Y) && test(pos.X, pos.Y - 1))
			{
				FillTriangle(pos, new Pos(pos.X - 1, pos.Y), new Pos(pos.X, pos.Y - 1), player);
			}
			else
			{
				if (test(pos.X - 1, pos.Y) && test(pos.X - 1, pos.Y - 1))
					FillTriangle(pos, new Pos(pos.X - 1, pos.Y), new Pos(pos.X - 1, pos.Y - 1), player);
				if (test(pos.X, pos.Y - 1) && test(pos.X - 1, pos.Y - 1))
					FillTriangle(pos, new Pos(pos.X, pos.Y - 1), new Pos(pos.X - 1, pos.Y - 1), player);
			}
		}

		public bool PutPoint(Pos point, PlayerColor player)
		{
			if (!Field.PutPoint(point, player))
				return false;
			_bot.PutPoint(point, player);

			// Если стоит опция - воспроизводим звук.
			if (Preferences.Sounds)
				new SoundPlayer(Properties.Resources.Step).Play();
			
			if (Field.PointsCount != 1)
				canvas.Children.RemoveAt(canvas.Children.Count - 1);

			// Запомиаем количество обьектов на доске для отката ходов.
			_canvasChildrenCount.Add(canvas.Children.Count);

			// Рисуем поставленную точку.
			var e = new Ellipse
			{
				Fill = player == PlayerColor.Red ? new SolidColorBrush(Preferences.RedColor) : new SolidColorBrush(Preferences.BlackColor),
				Width = 8,
				Height = 8
			};
			Canvas.SetLeft(e, (point.X - 1) * Preferences.CellSize + Preferences.CellSize / 2 - 4);
			Canvas.SetTop(e, (point.Y - 1) * Preferences.CellSize + Preferences.CellSize / 2 - 4);
			canvas.Children.Add(e);

			// Рисуем заливку.
			foreach (var chain in Field.LastChains)
			{
				var graphicsPoints = new PointCollection(chain.Count);
				foreach (var pos in chain)
					graphicsPoints.Add(ConvertToGraphicsPoint(pos));

				var poly = new Polygon { Points = graphicsPoints };
				if (Field.Points[chain[0].X, chain[0].Y].Color == PlayerColor.Red)
				{
					poly.Fill = new SolidColorBrush(Color.FromArgb(Preferences.FillingAlpha, Preferences.RedColor.R, Preferences.RedColor.G, Preferences.RedColor.B));
					poly.Stroke = new SolidColorBrush(Preferences.RedColor);
				}
				else
				{
					poly.Fill = new SolidColorBrush(Color.FromArgb(Preferences.FillingAlpha, Preferences.BlackColor.R, Preferences.BlackColor.G, Preferences.BlackColor.B));
					poly.Stroke = new SolidColorBrush(Preferences.BlackColor);
				}
				canvas.Children.Add(poly);
			}

			if (Preferences.FullFill)
				UpdateFullFill(point, player);

			// Обводим последнюю поставленную точку.
			e = new Ellipse
			{
				Stroke = player == PlayerColor.Red ? new SolidColorBrush(Preferences.RedColor) : new SolidColorBrush(Preferences.BlackColor),
				Width = 12,
				Height = 12
			};
			Canvas.SetLeft(e, (point.X - 1) * Preferences.CellSize + Preferences.CellSize / 2 - 6);
			Canvas.SetTop(e, (point.Y - 1) * Preferences.CellSize + Preferences.CellSize / 2 - 6);
			canvas.Children.Add(e);

			UpdateTextInfo();

			return true;
		}

		public bool PutPoint(Pos point)
		{
			if (PutPoint(point, Field.CurPlayer))
			{
				SetNextPlayer();
				return true;
			}
			return false;
		}

		public void DoBotStep()
		{
			if (_thinking)
				return;

			_thinking = true;
			var botMovie = _bot.GetMove(Field.CurPlayer);
			Dispatcher.BeginInvoke(DispatcherPriority.Normal, (Action)(() => PutPoint(botMovie)));
			_thinking = false;
		}

		public void BackMove()
		{
			if (_thinking || Field.PointsCount == 0)
				return;

			Field.BackMove();
			_bot.RemoveLastPoint();

			canvas.Children.RemoveRange(_canvasChildrenCount[_canvasChildrenCount.Count - 1], canvas.Children.Count - _canvasChildrenCount[_canvasChildrenCount.Count - 1]);
			_canvasChildrenCount.RemoveAt(_canvasChildrenCount.Count - 1);

			UpdateTextInfo();

			// Обводим последнюю поставленную точку, если такая есть.
			if (Field.PointsCount != 0)
			{
				var e = new Ellipse
				{
					Stroke = Field.Points[Field.PointsSeq[Field.PointsCount - 1].X, Field.PointsSeq[Field.PointsCount - 1].Y].Color == PlayerColor.Red ? new SolidColorBrush(Preferences.RedColor) : new SolidColorBrush(Preferences.BlackColor),
					Width = 12,
					Height = 12
				};
				Canvas.SetLeft(e, (Field.PointsSeq[Field.PointsCount - 1].X - 1) * Preferences.CellSize + Preferences.CellSize / 2 - 6);
				Canvas.SetTop(e, (Field.PointsSeq[Field.PointsCount - 1].Y - 1) * Preferences.CellSize + Preferences.CellSize / 2 - 6);
				canvas.Children.Add(e);
			}
		}

		public void SetNextPlayer()
		{
			Field.SetNextPlayer();
			UpdateTextInfo();
		}

		private void CanvasMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			if (_thinking)
				return;

			var movie = ConvertToPos(e.GetPosition(canvas));
			if (!PutPoint(movie))
				return;
			_bot.PutPoint(movie, Field.EnemyPlayer);
			if (Preferences.AI)
				(new Thread(DoBotStep)).Start();
		}

		private void CanvasMouseMove(object sender, MouseEventArgs e)
		{
			var pos = ConvertToPos(e.GetPosition(canvas));
			MouseCoord.Text = pos.X + ":" + pos.Y;
		}

		public bool Save(string fileName, GameFormat format)
		{
			switch (format)
			{
				case (GameFormat.PointsXT):
					return SaveXT(fileName);
				default:
					return false;
			}
		}

		public bool SaveXT(string pointsXTFileName)
		{
			if (Preferences.Width != 39 || Preferences.Height != 32 || Preferences.SurCond != SurroundCond.Standart || Field.PointsCount == 0)
				return false;

			var stream = new BinaryWriter(new FileStream(pointsXTFileName, FileMode.Create));
			// Первый байт - версия клиента.
			stream.Write((byte)121);
			// Следующие 2 байта - количество поставленных точек - 1.
			stream.Write((ushort)(Field.PointsCount - 1));
			// Далее 2 байта, указывающие на цвет последнего игрока, сделавшего ход.
			if (Field.Points[Field.PointsSeq[Field.PointsCount - 1].X, Field.PointsSeq[Field.PointsCount - 1].Y].Color == PlayerColor.Red)
				stream.Write((ushort)0xFFFF);
			else
				stream.Write((ushort)0x0000);
			// ???
			stream.Write((ushort)0x0000);
			stream.Write((ushort)0x0000);
			stream.Write((ushort)0x0000);
			// Далее идут имена двух игроков по 9 байт.
			stream.Write(Encoding.GetEncoding(1251).GetBytes(Preferences.RedName != null ? Preferences.RedName.PadRight(9).Substring(0, 9) : "         "));
			stream.Write(Encoding.GetEncoding(1251).GetBytes(Preferences.BlackName != null ? Preferences.BlackName.PadRight(9).Substring(0, 9) : "         "));
			// Видимо, здесь в первых четырех байтах идет время сохранения партии или ее продолжительность, дальше нули.
			for (var i = 0; i < 29; i++)
				stream.Write((byte)0x00);
			for (var i = 0; i < Field.PointsCount; i++)
			{
				// Далее координаты хода - X, Y.
				stream.Write((byte)(Field.PointsSeq[i].X - 1));
				stream.Write((byte)(Field.PointsSeq[i].Y - 1));
				// В этом байте помечается последовательность точек, от которых следует пускать волну для проверки окружений (которые были в процессе игры захвачены). Не страшно, если будут помечены все.
				stream.Write((byte)1);
				// Затем цвет игрока, поставившего точку.
				if (Field.Points[Field.PointsSeq[i].X, Field.PointsSeq[i].Y].Color == PlayerColor.Red)
					stream.Write((ushort)0xFFFF);
				else
					stream.Write((ushort)0x0000);
				// ???
				for (var j = 0; j < 8; j++)
					stream.Write((byte)0x00);
			}
			stream.Close();

			return true;
		}

		// Загрузка игры из формата PointsXT. Полное описание формата можно посмотреть в SaveXT.
		private void LoadXT(string pointsXTFileName)
		{
			Field = new Field(39, 32, SurroundCond.Standart);
			_bot = new DllBot(39, 32, SurroundCond.Standart, BeginPattern.CleanPattern);
			DrawField(39, 32);

			var stream = new StreamReader(pointsXTFileName);
			var buffer = new byte[stream.BaseStream.Length];
			var count = (int)stream.BaseStream.Length;
			stream.BaseStream.Read(buffer, 0, count);
			stream.Close();

			Preferences.RedName = Encoding.GetEncoding(1251).GetString(buffer, 11, 9);
			Preferences.BlackName = Encoding.GetEncoding(1251).GetString(buffer, 20, 9);

			// Отключаем звуки.
			var sounds = _preferences.Sounds;
			_preferences.Sounds = false;

			for (var i = 58; i < count; i += 13)
				PutPoint(new Pos(buffer[i] + 1, buffer[i + 1] + 1), buffer[i + 3] == 0x00 ? PlayerColor.Black : PlayerColor.Red);
			Field.CurPlayer = buffer[count - 10] == 0x00 ? PlayerColor.Red : PlayerColor.Black;

			_preferences.Sounds = sounds;

			UpdateTextInfo();
		}
	}
}
using System;
using System.Collections.Generic;
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
		private SafeBot _bot;

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
			_bot = new SafeBot(new DllBot());
			_bot.Init(preferences.Width, preferences.Height, preferences.SurCond, preferences.BeginPattern);
			DrawField(_preferences.Width, _preferences.Height);
			PlaceBeginPattern(preferences.BeginPattern);
			UpdateTextInfo();
		}

		public void SetLanguage(GameLanguage language)
		{
			RedTextBlock.Text = language.Red;
			BlackTextBlock.Text = language.Black;
		}

		// Конвертация из координаты на canvas в pos.
		private Pos ConvertToPos(Point point)
		{
			return new Pos((int)Math.Round(point.X / Preferences.CellSize - 0.5) + 1, (int)Math.Round(point.Y / Preferences.CellSize - 0.5) + 1);
		}

		// Конвертация из pos в координату на canvas.
		private Point ConvertToPoint(Pos pos)
		{
			return new Point((pos.X - 1) * Preferences.CellSize + Preferences.CellSize / 2, (pos.Y - 1) * Preferences.CellSize + Preferences.CellSize / 2);
		}

		// Отрисовывает сетку.
		private void DrawField(int width, int height)
		{
			canvas.Children.Clear();
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

		// Обновляет текст на контроле.
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

		private void ReDraw()
		{
			canvas.Children.Clear();
			_canvasChildrenCount.Clear();
			DrawField(_preferences.Width, _preferences.Height);
			var lastField = Field;
			Field = new Field(_preferences.Width, _preferences.Height, _preferences.SurCond);

			foreach (var pos in lastField.PointsSeq)
				PutPoint(new Pos(pos.X - 1, pos.Y - 1), lastField.Points[pos.X, pos.Y].Color);
		}

		// Залить треугольник.
		private void FillTriangle(Pos pos1, Pos pos2, Pos pos3, PlayerColor player)
		{
			canvas.Children.Add(new Polygon
									{
										Points =
											new PointCollection
												{
													ConvertToPoint(pos1),
													ConvertToPoint(pos2),
													ConvertToPoint(pos3)
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

		// Обновить заливку после поставленной точки pos.
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

		private bool PutPoint(Pos point, PlayerColor player)
		{
			if (!Field.PutPoint(point, player))
				return false;

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
					graphicsPoints.Add(ConvertToPoint(pos));

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

		private bool PutPoint(Pos point)
		{
			if (PutPoint(point, Field.CurPlayer))
			{
				SetNextPlayer();
				return true;
			}
			return false;
		}

		private void UndoMove()
		{
			if (Field.PointsCount == 0)
				return;
			Field.BackMove();
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

		private void SetNextPlayer()
		{
			Field.SetNextPlayer();
			UpdateTextInfo();
		}

		private void CanvasMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			if (_thinking)
				return;
			var pos = ConvertToPos(e.GetPosition(canvas));
			gPutPoint(pos);
			if (Preferences.AI)
				gDoBotStep();
		}

		private void CanvasMouseMove(object sender, MouseEventArgs e)
		{
			var pos = ConvertToPos(e.GetPosition(canvas));
			MouseCoord.Text = pos.X + ":" + pos.Y;
		}

		public void gDoBotStep()
		{
			if (_thinking)
				return;
			_thinking = true;
			_bot.GetMove(Field.CurPlayer, pos =>
											{
												_bot.PutPoint(pos, Field.CurPlayer);
												Dispatcher.BeginInvoke(DispatcherPriority.Normal, (Action)(() => PutPoint(pos)));
												_thinking = false;
											});
		}

		public void gPutPoint(Pos pos)
		{
			if (_thinking)
				return;
			if (!PutPoint(pos))
				return;
			_bot.PutPoint(pos, Field.EnemyPlayer);
		}

		public void gPutPoint(Pos pos, PlayerColor player)
		{
			if (_thinking)
				return;
			if (!PutPoint(pos, player))
				return;
			_bot.PutPoint(pos, player);
		}

		public void gUndoMove()
		{
			if (_thinking)
				return;
			UndoMove();
			_bot.RemoveLastPoint();
		}

		public void gSetNextPlayer()
		{
			if (_thinking)
				return;
			SetNextPlayer();
		}
	}
}
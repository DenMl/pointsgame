using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using PointsLibrary;
using System.IO;

namespace DotsShell
{
	/// <summary>
	/// Interaction logic for DotsGameControl.xaml
	/// </summary>
	public partial class DotsGameControl : UserControl
	{
		double CellSize;
		double CellSizeDiv2;
		double Radius;
		double LineWidth;

		Brush Player1Fill = Brushes.Red;
		Brush Player2Fill = Brushes.Blue;
		Brush Player1Stroke = Brushes.Red;
		Brush Player2Stroke = Brushes.Blue;
		Brush Background = Brushes.White;
		Brush Line = Brushes.Black;
		double PointCellRatio = 0.23;
		double LineCellRatio = 0.14;

		int FieldWidth, FieldHeight;
		GameField GameField;
		Field Field;
		GameBot Bot;

		private void DrawState(State State)
		{
			int x, y;
			Field.GetPosition(State.Move.Position, out x, out y);

			Ellipse E = new Ellipse
			{
				Fill = (Field[State.Move.Position] & Dot.Player) == Dot.Red ? Player1Stroke : Player2Stroke,
				Width = Radius * 2,
				Height = Radius * 2,
				Stretch = Stretch.Uniform
			};
			Canvas.SetLeft(E, x * CellSize + CellSizeDiv2 - Radius);
			Canvas.SetTop(E, y * CellSize + CellSizeDiv2 - Radius);
			canvasField.Children.Add(E);

			if (State.Base != null && Field.LastMoveCaptureCount != 0)
			{
				var chainPositions = State.Base.ChainDotPositions;
				PointCollection GraphicsPoints = new PointCollection(chainPositions.Count);
				foreach (var PointPos in chainPositions)
				{
					Field.GetPosition(PointPos.Position, out x, out y);
					GraphicsPoints.Add(new Point(x * CellSize + CellSizeDiv2,
												 y * CellSize + CellSizeDiv2));
				}

				Polygon Poly = new Polygon { Points = GraphicsPoints, Stretch = Stretch.None };
				if ((Field[chainPositions.Peek().Position] & Dot.Player) == Dot.Red)
				{
					Poly.Fill = Player1Fill;
					Poly.Stroke = Player1Stroke;
				}
				else
				{
					Poly.Fill = Player2Fill;
					Poly.Stroke = Player2Stroke;
				}

				canvasField.Children.Add(Poly);
			}
		}

		public DotsGameControl(int FieldWidth, int FieldHeight)
		{
			InitializeComponent();

			this.FieldWidth = FieldWidth;
			this.FieldHeight = FieldHeight;

			Field = new Field(FieldWidth, FieldHeight, enmSurroundCondition.Standart);
			Bot = new GameBot(Field);
			GameField = new GameField(Field, enmBeginPattern.Clean, Bot);

			GameField.Move += OnMakeMove;

			CellSize = 647 / Field.Width; // временно
			CellSizeDiv2 = CellSize / 2;

			Player1Fill = new SolidColorBrush(Color.FromArgb(127, 255, 0, 0));
			Player2Fill = new SolidColorBrush(Color.FromArgb(127, 0, 0, 255));

			//RedrawField();
		}

		private void OnMakeMove(object sender, MoveEventArgs e)
		{
			if (e.Action == MoveAction.Add)
			{
				DrawState(Field.DotsSequanceStates.Last());

				//sliderMain.Maximum = GameField.DotsSequanceStates.Count;
			}
			else
				if (e.Action == MoveAction.Remove)
				{
					// removing base polygon if exists
					if (Field.IsBaseAddedAtLastMove)
						canvasField.Children.RemoveAt(canvasField.Children.Count - 1);
					canvasField.Children.RemoveAt(canvasField.Children.Count - 1);
				}
			tbRedCaptureCount.Text = Field.RedCaptureCount.ToString();
			tbBlueCaptureCount.Text = Field.BlueCaptureCount.ToString();
		}

		private void canvasField_MouseMove(object sender, MouseEventArgs e)
		{

		}

		private void canvasField_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			Point Pos = e.GetPosition(canvasField);
			int X = (int)Math.Round((Pos.X - CellSizeDiv2) / CellSize);
			int Y = (int)Math.Round((Pos.Y - CellSizeDiv2) / CellSize);
			GameField.MakeMove(X, Y);
		}

		public void RedrawField()
		{
			if (canvasField.RenderSize.Width == 0)
				return;

			CellSize = canvasField.RenderSize.Width / Field.Width;
			CellSizeDiv2 = CellSize / 2.0;
			Radius = CellSize * PointCellRatio;

			canvasField.Height = CellSize * Field.Height;

			double a = CellSizeDiv2;
			canvasField.Background = Background;
			canvasField.Children.Clear();

			double LineLength = CellSize * (Field.Height + 1);
			for (int i = 0; i < Field.Width; i++)
			{
				Line L = new Line();
				L.X1 = L.X2 = a;
				L.Y1 = 0;
				L.Y2 = LineLength;
				L.Stroke = Line;
				L.StrokeThickness = 0.25;
				canvasField.Children.Add(L);
				a += CellSize;
			}

			LineLength = CellSize * (Field.Width + 1);
			a = CellSizeDiv2;
			for (int i = 0; i < Field.Height; i++)
			{
				Line L = new Line();
				L.Y1 = L.Y2 = a;
				L.X1 = 0;
				L.X2 = LineLength;
				L.Stroke = Line;
				L.StrokeThickness = 0.25;
				canvasField.Children.Add(L);
				a += CellSize;
			}

			foreach (var P in Field.DotsSequanceStates)
				DrawState(P);
		}

		private void sliderMain_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			if (GameField != null)
				GameField.SetMoveNumber((int)Math.Round(e.NewValue));
		}

		private void button1_Click(object sender, RoutedEventArgs e)
		{
			GameField.SetMoveNumber(Field.DotsSequanceStates.Count() - 1);
			//sliderMain.Value--;
		}

		private void button2_Click(object sender, RoutedEventArgs e)
		{
			sliderMain.Value++;
		}

		private void button3_Click(object sender, RoutedEventArgs e)
		{
			Line L = new Line();
			L.X1 = L.X2 = 0;
			L.Y1 = 50;
			L.Y2 = 50;
			L.Stroke = new SolidColorBrush(Colors.Red);
			L.StrokeThickness = 3.0;
			L.Stretch = Stretch.Uniform;

			Ellipse E = new Ellipse
			{
				Fill = Brushes.Cyan,
				Width = 60,
				Height = 60,
				Stretch = Stretch.Uniform
			};
			LineGeometry LG = new LineGeometry(new Point(0, 0), new Point(300, 150));

			E.Stretch = Stretch.Uniform;
			Canvas.SetLeft(E, 10);
			Canvas.SetTop(E, 10);
			//grid2.Children.Add(E);
		}

		private void UserControl_Loaded(object sender, RoutedEventArgs e)
		{
			RedrawField();
		}

		private void btnUnmakeMove_Click(object sender, RoutedEventArgs e)
		{
			GameField.UnmakeMove();
		}

		private void btnLoadGame_Click(object sender, RoutedEventArgs e)
		{
			using (var ofd = new System.Windows.Forms.OpenFileDialog() { Filter = "pointsxt files|*.sav" })
			{
				if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
				{
					Field = new Field(39, 32, enmSurroundCondition.Standart);
					Bot = new GameBot(Field);
					GameField = new GameField(Field, enmBeginPattern.Crosswise, Bot);

					RedrawField();
					GameField.Move += OnMakeMove;
					using (var Stream = new StreamReader(ofd.FileName))
					{
						var buffer = new byte[Stream.BaseStream.Length];
						var Count = (int)Stream.BaseStream.Length;
						Stream.BaseStream.Read(buffer, 0, Count);
						Stream.Close();
						//GameField.PlayerNameRed = Encoding.GetEncoding(1251).GetString(buffer, 11, 9);
						//GameField.PlayerNameBlack = Encoding.GetEncoding(1251).GetString(buffer, 20, 9);
						GameField.PointSeq2 = new List<int>();
						for (var i = 58; i < Count; i += 13)
						{
							GameField.MakeMove(buffer[i], buffer[i + 1]);
							GameField.PointSeq2.Add((buffer[i + 1] + 1) * Field.RealWidth + (buffer[i] + 1));
						}
						sliderMain.ValueChanged -= sliderMain_ValueChanged;
						sliderMain.SmallChange = 1;
						sliderMain.Minimum = 1;
						sliderMain.Maximum = Field.DotsSequanceStates.Count();
						sliderMain.Value = sliderMain.Maximum;
						sliderMain.ValueChanged += sliderMain_ValueChanged;
					}
				}
			}
		}
	}
}

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
using System.IO;
using Dots.AI;
using Dots.Library;

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

		Brush Player1Fill = new SolidColorBrush(Color.FromArgb(127, 255, 0, 0));
		Brush Player2Fill = new SolidColorBrush(Color.FromArgb(127, 0, 0, 255));
		Brush Player1Stroke = Brushes.Red;
		Brush Player2Stroke = Brushes.Blue;
		Brush Background_ = Brushes.White;
		Brush Line = Brushes.Black;
		Brush CrosswiseStroke = Brushes.SpringGreen;
		double PointCellRatio = 0.23;
		double LineCellRatio = 0.14;

		int FieldWidth, FieldHeight;
		GameField GameField_;
		Field Field_;

		private List<Polygon> GroupPolygons = new List<Polygon>();
		private List<Polygon> CrosswisePolygons = new List<Polygon>();

		#region Constructors

		public DotsGameControl(int FieldWidth, int FieldHeight)
		{
			InitializeComponent();

			this.FieldWidth = FieldWidth;
			this.FieldHeight = FieldHeight;

			Field_ = new Field(FieldWidth, FieldHeight, enmSurroundCondition.Standart);
			GameField_ = new GameField(Field_, enmBeginPattern.Clean);
			GameField_.Move += OnMakeMove;

			CellSize = 647 / Field_.Width;
			CellSizeDiv2 = CellSize / 2;
		}

		#endregion

		#region Event Handlers

		private void UserControl_Loaded(object sender, RoutedEventArgs e)
		{
			RedrawField();
		}

		private void OnMakeMove(object sender, MoveEventArgs e)
		{
			if (e.Action == enmMoveState.Add)
			{
				DrawState(Field_.DotsSequanceStates.Last());
			}
			else
				if (e.Action == enmMoveState.Remove)
				{
					// removing base polygon if exists
					if (Field_.IsBaseAddedAtLastMove)
						canvasField.Children.RemoveAt(canvasField.Children.Count - 1);
					canvasField.Children.RemoveAt(canvasField.Children.Count - 1);
				}
			lblRedCaptureCount.Content = Field_.RedCaptureCount.ToString();
			lblBlueCaptureCount.Content = Field_.BlueCaptureCount.ToString();
		}

		private void canvasField_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			Point Pos = e.GetPosition(canvasField);
			int X = (int)Math.Round((Pos.X - 1 - CellSizeDiv2) / CellSize);
			int Y = (int)Math.Round((Pos.Y - 1 - CellSizeDiv2) / CellSize);
			GameField_.MakeMove(X, Y);
		}

		private void sliderMain_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			if (GameField_ != null)
				GameField_.SetMoveNumber((int)Math.Round(e.NewValue));
		}

		private void btnUnmakeMove_Click(object sender, RoutedEventArgs e)
		{
			GameField_.UnmakeMove();
		}

		private void btnLoadGame_Click(object sender, RoutedEventArgs e)
		{
			using (var ofd = new System.Windows.Forms.OpenFileDialog() { Filter = "pointsxt files|*.sav" })
			{
				if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
				{
					Field_ = new Field(39, 32, enmSurroundCondition.Standart);
					GameField_ = new GameField(Field_, enmBeginPattern.Crosswise);

					RedrawField();
					GameField_.Move += OnMakeMove;
					using (var Stream = new StreamReader(ofd.FileName))
					{
						var buffer = new byte[Stream.BaseStream.Length];
						var Count = (int)Stream.BaseStream.Length;
						Stream.BaseStream.Read(buffer, 0, Count);
						Stream.Close();

						for (var i = 58; i < Count; i += 13)
						{
							GameField_.MakeMove(buffer[i] + 1, buffer[i + 1] + 1);
						}
						sliderMain.ValueChanged -= sliderMain_ValueChanged;
						sliderMain.SmallChange = 1;
						sliderMain.Minimum = 1;
						sliderMain.Maximum = Field_.DotsSequanceStates.Count();
						sliderMain.Value = sliderMain.Maximum;
						sliderMain.ValueChanged += sliderMain_ValueChanged;
					}
				}
			}
		}

		private void btnClear_Click(object sender, RoutedEventArgs e)
		{
			Field_ = new Field(FieldWidth, FieldHeight, enmSurroundCondition.Standart);
			GameField_ = new GameField(Field_, enmBeginPattern.Clean);
			GameField_.Move += OnMakeMove;
			RedrawField();
		}

		private void cbConvexGroups_Checked(object sender, RoutedEventArgs e)
		{
			var analyzer = new StrategicMovesAnalyzer(Field_);
			analyzer.GenerateGroups();

			foreach (var group in analyzer.Groups)
			{
				var polygon = new Polygon { Stretch = Stretch.None };
				polygon.Stroke = group.Player == Dot.RedPlayer ? Player1Stroke : Player2Stroke;
				polygon.Fill = group.Player == Dot.RedPlayer ? Player1Fill : Player2Fill;
				foreach (var pos in group.EnvelopePositions)
				{
					int x, y;
					Field.GetPosition(pos, out x, out y);
					polygon.Points.Add(new Point(x * CellSize + CellSizeDiv2, y * CellSize + CellSizeDiv2));
				}
				GroupPolygons.Add(polygon);
				canvasField.Children.Add(polygon);
			}
		}

		private void cbConvexGroups_Unchecked(object sender, RoutedEventArgs e)
		{
			foreach (var polygon in GroupPolygons)
				canvasField.Children.Remove(polygon);
			GroupPolygons.Clear();
		}

		private void cbCrosswises_Checked(object sender, RoutedEventArgs e)
		{
			var analyzer = new StrategicMovesAnalyzer(Field_);
			analyzer.GenerateGroups();
			analyzer.GenerateCrosswises();

			foreach (var crosswise in analyzer.Crosswises)
			{
				var polygon = new Polygon { Stretch = Stretch.None };
				polygon.Stroke = CrosswiseStroke; ;
				polygon.Fill = null;
				int x, y;
				Field.GetPosition(crosswise.Value.Position, out x, out y);

				if (crosswise.Value.Orientation == enmCrosswiseOrientation.BottomRight)
				{
					polygon.Points.Add(GetGraphicaPoint(x, y));
					polygon.Points.Add(GetGraphicaPoint(x + 1, y));
					polygon.Points.Add(GetGraphicaPoint(x + 1, y + 1));
					polygon.Points.Add(GetGraphicaPoint(x, y + 1));
				}
				else
					if (crosswise.Value.Orientation == enmCrosswiseOrientation.BottomLeft)
					{
						polygon.Points.Add(GetGraphicaPoint(x, y));
						polygon.Points.Add(GetGraphicaPoint(x - 1, y));
						polygon.Points.Add(GetGraphicaPoint(x - 1, y + 1));
						polygon.Points.Add(GetGraphicaPoint(x, y + 1));
					}

				CrosswisePolygons.Add(polygon);
				canvasField.Children.Add(polygon);
			}
		}

		private void cbCrosswises_Unchecked(object sender, RoutedEventArgs e)
		{
			foreach (var polygon in CrosswisePolygons)
				canvasField.Children.Remove(polygon);
			CrosswisePolygons.Clear();
		}

		#endregion
		
		#region Helpers

		private void DrawState(State State)
		{
			int x, y;
			Field.GetPosition(State.Move.Position, out x, out y);

			Ellipse E = new Ellipse
			{
				Fill = (Field_[State.Move.Position] & Dot.Player) == Dot.RedPlayer ? Player1Stroke : Player2Stroke,
				Width = Radius * 2,
				Height = Radius * 2,
				Stretch = Stretch.Uniform
			};
			Canvas.SetLeft(E, x * CellSize + CellSizeDiv2 - Radius);
			Canvas.SetTop(E, y * CellSize + CellSizeDiv2 - Radius);
			canvasField.Children.Add(E);

			if (State.Base != null && Field_.LastMoveCaptureCount != 0)
			{
				var chainPositions = State.Base.ChainDotPositions;
				PointCollection GraphicsPoints = new PointCollection(chainPositions.Count);
				foreach (var PointPos in chainPositions)
					GraphicsPoints.Add(GetGraphicaPoint(PointPos.Position));

				Polygon Poly = new Polygon { Points = GraphicsPoints, Stretch = Stretch.None };
				if (Field_[chainPositions.Peek().Position].IsRedPutted())
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

		public void RedrawField()
		{
			if (canvasField.RenderSize.Width == 0)
				return;

			CellSize = canvasField.RenderSize.Width / Field_.Width;
			CellSizeDiv2 = CellSize / 2.0;
			Radius = CellSize * PointCellRatio;

			canvasField.Height = CellSize * Field_.Height;

			double a = CellSizeDiv2;
			canvasField.Background = Background_;
			canvasField.Children.Clear();

			double LineLength = CellSize * (Field_.Height + 1);
			for (int i = 0; i < Field_.Width; i++)
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

			LineLength = CellSize * (Field_.Width + 1);
			a = CellSizeDiv2;
			for (int i = 0; i < Field_.Height; i++)
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

			foreach (var P in Field_.DotsSequanceStates)
				DrawState(P);
		}

		private Point GetGraphicaPoint(int pos)
		{
			int x, y;
			Field.GetPosition(pos, out x, out y);
			return GetGraphicaPoint(x, y);
		}

		private Point GetGraphicaPoint(int x, int y)
		{
			return new Point(x * CellSize + CellSizeDiv2, y * CellSize + CellSizeDiv2);
		}

		#endregion
	}
}

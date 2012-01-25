using System;
using System.Windows.Media;
using System.Xml.Serialization;
using System.IO;
using PointsShell.Enums;

namespace PointsShell
{
	[Serializable]
	public class GamePreferences
	{
		private int _width;
		private int _height;
		private int _complexity;
		private int _time;
		private string _redName;
		private string _blackName;

		public int Width
		{
			get { return _width; }
			set
			{
				if (value <= 0)
					throw new ApplicationException("Width must be over 0");
				_width = value;
			}
		}
		public int Height
		{
			get { return _height; }
			set
			{
				if (value <= 0)
					throw new ApplicationException("Height must be over 0");
				_height = value;
			}
		}
		public SurroundCond SurCond { get; set; }
		public BeginPattern BeginPattern { get; set; }
		public bool AI { get; set; }
		public int Complexity
		{
			get { return _complexity; }
			set
			{
				if (value < 0 || value > 100)
					throw new ApplicationException("Complexity must be in [0..100]");
				_complexity = value;
			}
		}
		public int Time
		{
			get { return _time; }
			set
			{
				if (value <= 0)
					throw new ApplicationException("Time must be over 0");
				_time = value;
			}
		}
		public string RedName
		{
			get { return _redName; }
			set { _redName = value.Trim(); }
		}
		public string BlackName
		{
			get { return _blackName; }
			set { _blackName = value.Trim(); }
		}
		public Color RedColor { get; set; }
		public Color BlackColor { get; set; }
		public byte FillingAlpha { get; set; }
		public Color BackgroundColor { get; set; }
		public bool Sounds { get; set; }
		public bool FullFill { get; set; }
		public int CellSize { get; set; }
		public BotType BotType { get; set; }
		public GetMoveType GetMoveType { get; set; }
		public string TabName { get; set; }

		public GamePreferences()
		{
			Width = 39;
			Height = 32;
			SurCond = SurroundCond.Standart;
			BeginPattern = BeginPattern.CleanPattern;
			AI = true;
			Complexity = 100;
			Time = 10000;
			RedName = string.Empty;
			BlackName = string.Empty;
			RedColor = Colors.Red;
			BlackColor = Colors.Black;
			FillingAlpha = 127;
			BackgroundColor = Colors.White;
			Sounds = true;
			FullFill = true;
			CellSize = 18;
			BotType = BotType.Dll;
			GetMoveType = GetMoveType.GetMove;
			TabName = Properties.Resources.GameHeader;
		}

		public GamePreferences(GamePreferences preferences)
		{
			Width = preferences.Width;
			Height = preferences.Height;
			SurCond = preferences.SurCond;
			BeginPattern = preferences.BeginPattern;
			AI = preferences.AI;
			Complexity = preferences.Complexity;
			Time = preferences.Time;
			RedName = preferences.RedName;
			BlackName = preferences.BlackName;
			RedColor = preferences.RedColor;
			BlackColor = preferences.BlackColor;
			FillingAlpha = preferences.FillingAlpha;
			BackgroundColor = preferences.BackgroundColor;
			Sounds = preferences.Sounds;
			FullFill = preferences.FullFill;
			CellSize = preferences.CellSize;
			BotType = preferences.BotType;
			GetMoveType = preferences.GetMoveType;
			TabName = preferences.TabName;
		}

		public static GamePreferences Load(string file)
		{
			try
			{
				var serializer = new XmlSerializer(typeof(GamePreferences));
				using (Stream stream = File.OpenRead(file))
				{
					return (GamePreferences)serializer.Deserialize(stream);
				}
			}
			catch
			{
				return new GamePreferences();
			}
		}
	}
}
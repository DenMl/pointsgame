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
		public int Width { get; set; }
		public int Height { get; set; }
		public SurroundCond SurCond { get; set; }
		public BeginPattern BeginPattern { get; set; }
		public bool AI { get; set; }
		public int Complexity { get; set; }
		public int Time { get; set; }
		private string _redName;
		private string _blackName;
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
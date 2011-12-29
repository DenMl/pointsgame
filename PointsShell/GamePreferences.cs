using System.Windows.Media;
using System.Xml.Serialization;
using System.IO;
using PointsShell.Enums;

namespace PointsShell
{
	public class GamePreferences
	{
		public int Width = 39;
		public int Height = 32;
		public SurroundCond SurCond = SurroundCond.Standart;
		public BeginPattern BeginPattern = BeginPattern.CleanPattern;
		public bool AI = true;
		public int MinMaxDepth = 8;
		public int UCTIterations = 100000;
		private string _redName = "";
		private string _blackName = "";
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
		public Color RedColor = Colors.Red;
		public Color BlackColor = Colors.Black;
		public byte FillingAlpha = 127;
		public Color BackgroundColor = Colors.White;
		public bool Sounds = true;
		public string Language = "English.xml";
		public bool FullFill = true;
		public int CellSize = 18;

		public string Header
		{
			get
			{
				var header = "";
				if (!string.IsNullOrEmpty(RedName))
					header += RedName;
				if (!string.IsNullOrEmpty(RedName) && !string.IsNullOrEmpty(BlackName))
					header += " - ";
				if (!string.IsNullOrEmpty(BlackName))
					header += BlackName;
				return header;
			}
		}

		public GamePreferences() {}

		public GamePreferences(GamePreferences preferences)
		{
			Width = preferences.Width;
			Height = preferences.Height;
			SurCond = preferences.SurCond;
			BeginPattern = preferences.BeginPattern;
			AI = preferences.AI;
			MinMaxDepth = preferences.MinMaxDepth;
			UCTIterations = preferences.UCTIterations;
			RedName = preferences.RedName;
			BlackName = preferences.BlackName;
			RedColor = preferences.RedColor;
			BlackColor = preferences.BlackColor;
			FillingAlpha = preferences.FillingAlpha;
			BackgroundColor = preferences.BackgroundColor;
			Sounds = preferences.Sounds;
			FullFill = preferences.FullFill;
			CellSize = preferences.CellSize;
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
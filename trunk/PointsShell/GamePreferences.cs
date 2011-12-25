using System.Windows.Media;
using System.Xml.Serialization;
using System.IO;

namespace PointsShell
{
	public class GamePreferences
	{
		private int _width = 39;
		private int _height = 32;
		public int Width
		{
			get { return _width; }
			set { _width = value <= 39 ? value : 39; }
		}
		public int Height
		{
			get { return _height; }
			set { _height = value <= 32 ? value : 32; }
		}
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

		public GamePreferences(GamePreferences Preferences)
		{
			Width = Preferences.Width;
			Height = Preferences.Height;
			SurCond = Preferences.SurCond;
			BeginPattern = Preferences.BeginPattern;
			AI = Preferences.AI;
			MinMaxDepth = Preferences.MinMaxDepth;
			UCTIterations = Preferences.UCTIterations;
			RedName = Preferences.RedName;
			BlackName = Preferences.BlackName;
			RedColor = Preferences.RedColor;
			BlackColor = Preferences.BlackColor;
			FillingAlpha = Preferences.FillingAlpha;
			BackgroundColor = Preferences.BackgroundColor;
			Sounds = Preferences.Sounds;
			FullFill = Preferences.FullFill;
			CellSize = Preferences.CellSize;
		}

		public static GamePreferences Load(string File)
		{
			try
			{
				var serializer = new XmlSerializer(typeof(GamePreferences));
				using (Stream stream = System.IO.File.OpenRead(File))
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
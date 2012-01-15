using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using PointsShell.Bots;
using PointsShell.Enums;

namespace PointsShell
{
	public partial class Game
	{
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
			string s;
			using (var stream = new StreamReader(fileName))
				s = stream.ReadToEnd();

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

		// Загрузка игры из формата PointsXT. Полное описание формата можно посмотреть в SaveXT.
		private void LoadXT(string pointsXTFileName)
		{
			Field = new Field(39, 32, SurroundCond.Standart);
			_bot = new SafeBot(new DllBot());
			_bot.Init(39, 32, SurroundCond.Standart, BeginPattern.CleanPattern);
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
				gPutPoint(new Pos(buffer[i] + 1, buffer[i + 1] + 1), buffer[i + 3] == 0x00 ? PlayerColor.Black : PlayerColor.Red);
			Field.CurPlayer = buffer[count - 10] == 0x00 ? PlayerColor.Red : PlayerColor.Black;

			_preferences.Sounds = sounds;

			UpdateTextInfo();
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

		public static Game Load(string fileName, GamePreferences preferences)
		{
			var format = GetFormat(fileName);
			switch (format)
			{
				case (GameFormat.PointsXT):
					var result = new Game { _preferences = preferences };
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
	}
}

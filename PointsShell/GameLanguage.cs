using System.Xml.Serialization;
using System.IO;

namespace PointsShell
{
    public class GameLanguage
    {
        public string File = "_File";
        public string Game = "_Game";
        public string Options = "_Options";
        public string Help = "_Help";
        public string New = "_New";
        public string Close = "_Close";
        public string Save = "_Save";
        public string Load = "_Load";
        public string Back = "_Back";
        public string DoStep = "_Do step";
        public string NextPlayer = "_Next player";
        public string GlobalPreferences = "_Global preferences";
        public string LocalPreferences = "_Local preferences";
        public string Language = "_Language";
        public string About = "_About";
        public string Red = "Red";
        public string Black = "Black";
        public string OK = "_OK";
        public string Cancel = "_Cancel";
        public string GamePreferences = "Game preferences";
        public string AIPreferences = "AI preferences";
        public string PlayersPreferences = "Players preferences";
        public string OtherPrefernces = "Other preferences";
        public string Width = "Width";
        public string Height = "Height";
        public string SurroundCondition = "Surround condition";
        public string Standart = "Standart";
        public string Always = "Always";
        public string AlwaysEnemy = "Always enemy";
        public string BeginPattern = "Begin pattern";
        public string Clean = "Clean";
        public string Crosswire = "Crosswire";
        public string Square = "Square";
        public string AI = "AI";
        public string MinMaxDepth = "MinMax depth";
        public string UCTIterations = "UCT iterations";
        public string RedName = "Red name";
        public string BlackName = "Black name";
        public string RedColor = "Red color";
        public string BlackColor = "Black color";
        public string FillingAlpha = "Filling alpha";
        public string BackgroundColor = "Background color";
        public string FullFill = "Full fill";
        public string Sounds = "Sounds";
        public string NewGame = "New game";
        public string preferences = "preferences";

        public static bool IsFileValid(string File)
        {
            try
            {
                var serializer = new XmlSerializer(typeof(GameLanguage));
                using (Stream stream = System.IO.File.OpenRead(File))
                {
                    serializer.Deserialize(stream);
                    return true;
                }
            }
            catch
            {
                return false;
            }
        }

        public static GameLanguage LoadLanguage(string File)
        {
            try
            {
                var serializer = new XmlSerializer(typeof(GameLanguage));
                using (Stream stream = System.IO.File.OpenRead(File))
                {
                    return (GameLanguage)serializer.Deserialize(stream);
                }
            }
            catch
            {
                return new GameLanguage();
            }
        }
    }
}

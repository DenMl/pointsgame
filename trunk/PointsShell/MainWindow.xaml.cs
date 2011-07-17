using System.IO;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Xml.Serialization;
using Microsoft.Win32;
using System.Linq;

namespace PointsShell
{
    public partial class MainWindow
    {
        #if DEBUG
            private const string LanguagesDirectory = @"..\..\..\Languages\";
        #else
            private const string LanguagesDirectory = @"Languages\";
        #endif

        private GamePreferences GlobalPreferences;

        private GameLanguage _GlobalLanguage;
        private GameLanguage GlobalLanguage
        {
            get { return _GlobalLanguage; }
            set
            {
                _GlobalLanguage = value;
                SetLanguage(value);
                foreach (var item in MainTabControl.Items)
                {
                    if (((TabItem)item).Content is Game)
                        (((TabItem)item).Content as Game).SetLanguage(value);
                    else if (((TabItem)item).Content is GamePreferencesDialog)
                        (((TabItem)item).Content as GamePreferencesDialog).SetLanguage(value);
                }
            }
        }

        public MainWindow()
        {
            InitializeComponent();

            GlobalPreferences = GamePreferences.Load("Preferences.xml");

            var LDirectory = new DirectoryInfo(LanguagesDirectory);
            if (LDirectory.Exists)
                foreach (var fileInfo in LDirectory.GetFiles("*.xml", SearchOption.TopDirectoryOnly).Where(fileInfo => GameLanguage.IsFileValid(LanguagesDirectory + fileInfo.Name)))
                {
                    var MI = new MenuItem
                                 {
                                     Header = fileInfo.Name.Substring(0, fileInfo.Name.Length - 4),
                                     Icon = new RadioButton
                                                {
                                                    HorizontalAlignment = HorizontalAlignment.Center,
                                                    GroupName = "Languages",
                                                    IsHitTestVisible = false,
                                                    IsChecked = GlobalPreferences.Language == fileInfo.Name
                                                }
                                 };
                    MI.Click += (sender, e) =>
                                    {
                                        ((RadioButton) ((MenuItem) sender).Icon).IsChecked = true;
                                        GlobalPreferences.Language = (string)((MenuItem)sender).Header + ".xml";
                                        GlobalLanguage = GameLanguage.LoadLanguage(LanguagesDirectory + GlobalPreferences.Language);
                                    };
                    LanguageMenuItem.Items.Add(MI);
                }

            if (LanguageMenuItem.Items.Count == 0)
                LanguageMenuItem.Visibility = Visibility.Collapsed;

            GlobalLanguage = GameLanguage.LoadLanguage(LanguagesDirectory + GlobalPreferences.Language);

            MainTabControl.Items.Add(new TabItem { Header = GlobalLanguage.Game, Content = new Game(new GamePreferences(GlobalPreferences), GlobalLanguage) });
        }

        ~MainWindow()
        {
            var serializer = new XmlSerializer(typeof(GamePreferences));
            using (Stream stream = File.Create("Preferences.xml"))
                serializer.Serialize(stream, GlobalPreferences);
        }

        public void SetLanguage(GameLanguage language)
        {
            FileMenuItem.Header = language.File;
            GameMenuItem.Header = language.Game;
            OptionsMenuItem.Header = language.Options;
            HelpMenuItem.Header = language.Help;
            NewMenuItem.Header = language.New;
            CloseMenuItem.Header = language.Close;
            SaveMenuItem.Header = language.Save;
            LoadMenuItem.Header = language.Load;
            BackMenuItem.Header = language.Back;
            DoStepMenuItem.Header = language.DoStep;
            NextPlayerMenuItem.Header = language.NextPlayer;
            GlobalPreferencesMenuItem.Header = language.GlobalPreferences;
            LocalPreferencesMenuItem.Header = language.LocalPreferences;
            LanguageMenuItem.Header = language.Language;
            AboutMenuItem.Header = language.About;
        }

        private void NewClick(object sender, RoutedEventArgs e)
        {
            var content = new GamePreferencesDialog(new GamePreferences(GlobalPreferences), GlobalLanguage);
            var preferencestab = new TabItem {Header = GlobalLanguage.NewGame, Content = content};

            content.OKClicked +=
                preferences =>
                    {
                        MainTabControl.Items.Remove(preferencestab);
                        MainTabControl.Items.Add(new TabItem { Header = preferences.Header != "" ? preferences.Header : GlobalLanguage.Game, Content = new Game(preferences, GlobalLanguage) });
                        MainTabControl.SelectedIndex = MainTabControl.Items.Count - 1;
                    };
            content.CancelClicked += () => MainTabControl.Items.Remove(preferencestab);

            MainTabControl.Items.Add(preferencestab);
            MainTabControl.SelectedIndex = MainTabControl.Items.Count - 1;
        }

        private void CloseClick(object sender, RoutedEventArgs e)
        {
            if (MainTabControl.Items.Count > 0)
                MainTabControl.Items.RemoveAt(MainTabControl.SelectedIndex);
        }

        private void SaveClick(object sender, RoutedEventArgs e)
        {
            if (!(MainTabControl.SelectedContent is Game))
                return;

            var SFD = new SaveFileDialog { Filter = "PointsXT|*.sav" };
            if (SFD.ShowDialog() != true)
                return;

            if (!(MainTabControl.SelectedContent as Game).Save(SFD.FileName, GameFormat.PointsXT))
                MessageBox.Show("Invalid game preferences for this format!");
        }

        private void LoadClick(object sender, RoutedEventArgs e)
        {
            var OFD = new OpenFileDialog { Filter = "PointsXT|*.sav|All files|*" };

            if (OFD.ShowDialog() != true)
                return;

            var game = Game.Load(OFD.FileName, new GamePreferences(GlobalPreferences), GlobalLanguage);

            if (game == null)
            {
                MessageBox.Show("Unknown format!", "Error!");
                return;
            }

            MainTabControl.Items.Add(new TabItem { Content = game, Header = GlobalPreferences.Header != "" ? GlobalPreferences.Header : GlobalLanguage.Game });
            MainTabControl.SelectedIndex = MainTabControl.Items.Count - 1;
        }

        private void BackClick(object sender, RoutedEventArgs e)
        {
            if (!(MainTabControl.SelectedContent is Game))
                return;

            (MainTabControl.SelectedContent as Game).BackMove();
        }

        private void DoStepClick(object sender, RoutedEventArgs e)
        {
            if (!(MainTabControl.SelectedContent is Game))
                return;

            (new Thread((MainTabControl.SelectedContent as Game).DoBotStep)).Start();
        }

        private void NextPlayerClick(object sender, RoutedEventArgs e)
        {
            if (!(MainTabControl.SelectedContent is Game))
                return;

            (MainTabControl.SelectedContent as Game).SetNextPlayer();
        }

        private void LocalPreferencesClick(object sender, RoutedEventArgs e)
        {
            if (!(MainTabControl.SelectedContent is Game))
                return;

            var game = MainTabControl.SelectedContent as Game;
            var gametab = MainTabControl.SelectedItem as TabItem;
            var content = new GamePreferencesDialog(game.Preferences, GlobalLanguage);
            var preferncestab = new TabItem {Header = "'" + gametab.Header + "' " + GlobalLanguage.preferences, Content = content};

            content.LockGlobal();
            content.OKClicked +=
                preferences =>
                    {
                        MainTabControl.Items.Remove(preferncestab);
                        game.Preferences = preferences;
                        gametab.Header = preferences.Header != "" ? preferences.Header : GlobalLanguage.Game;
                    };
            content.CancelClicked += () => MainTabControl.Items.Remove(preferncestab);

            MainTabControl.Items.Add(preferncestab);
            MainTabControl.SelectedIndex = MainTabControl.Items.Count - 1;
        }

        private void GlobalPreferencesClick(object sender, RoutedEventArgs e)
        {
            var content = new GamePreferencesDialog(new GamePreferences(GlobalPreferences), GlobalLanguage);
            var preferncestab = new TabItem { Header = GlobalLanguage.GlobalPreferences, Content = content };

            content.OKClicked +=
                preferences =>
                {
                    MainTabControl.Items.Remove(preferncestab);
                    GlobalPreferences = preferences;
                };
            content.CancelClicked += () => MainTabControl.Items.Remove(preferncestab);

            MainTabControl.Items.Add(preferncestab);
            MainTabControl.SelectedIndex = MainTabControl.Items.Count - 1;
        }

        private void AboutClick(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("PointsGame 1.4.0.0\nAuthors: Keij, Kvanttt\nContacts:\nKeij: ICQ - 366-369-317; mail, jabber - kurnevsky@gmail.com", "PointsGame");
        }
    }
}
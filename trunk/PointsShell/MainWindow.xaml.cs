using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Xml.Serialization;
using Microsoft.Win32;
using PointsShell.Enums;

namespace PointsShell
{
	public partial class MainWindow
	{
		private GamePreferences _globalPreferences;

		public MainWindow()
		{
			InitializeComponent();

			_globalPreferences = GamePreferences.Load("Preferences.xml");

			MainTabControl.Items.Add(new TabItem { Header = _globalPreferences.TabName, Content = new Game(new GamePreferences(_globalPreferences)) });
		}

		~MainWindow()
		{
			var serializer = new XmlSerializer(typeof(GamePreferences));
			using (Stream stream = File.Create("Preferences.xml"))
				serializer.Serialize(stream, _globalPreferences);
		}

		private void NewClick(object sender, RoutedEventArgs e)
		{
			var content = new GamePreferencesDialog(new GamePreferences(_globalPreferences));
			var preferencestab = new TabItem {Header = Properties.Resources.LocalPreferencesHeader, Content = content};

			content.OkClicked +=
				preferences =>
					{
						MainTabControl.Items.Remove(preferencestab);
						MainTabControl.Items.Add(new TabItem { Header = preferences.TabName, Content = new Game(preferences) });
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

			var dialog = new SaveFileDialog { Filter = "PointsXT|*.sav" };
			if (dialog.ShowDialog() != true)
				return;

			if (!(MainTabControl.SelectedContent as Game).Save(dialog.FileName, GameFormat.PointsXT))
				MessageBox.Show("Invalid game preferences for this format!");
		}

		private void LoadClick(object sender, RoutedEventArgs e)
		{
			var dialog = new OpenFileDialog { Filter = "PointsXT|*.sav|All files|*" };
			if (dialog.ShowDialog() != true)
				return;
			var game = Game.Load(dialog.FileName, new GamePreferences(_globalPreferences));

			if (game == null)
			{
				MessageBox.Show("Unknown format!");
				return;
			}

			MainTabControl.Items.Add(new TabItem { Content = game, Header = _globalPreferences.TabName });
			MainTabControl.SelectedIndex = MainTabControl.Items.Count - 1;
		}

		private void BackClick(object sender, RoutedEventArgs e)
		{
			if (!(MainTabControl.SelectedContent is Game))
				return;
			(MainTabControl.SelectedContent as Game).gUndoMove();
		}

		private void DoStepClick(object sender, RoutedEventArgs e)
		{
			if (!(MainTabControl.SelectedContent is Game))
				return;
			(MainTabControl.SelectedContent as Game).gDoBotStep();
		}

		private void NextPlayerClick(object sender, RoutedEventArgs e)
		{
			if (!(MainTabControl.SelectedContent is Game))
				return;
			(MainTabControl.SelectedContent as Game).gSetNextPlayer();
		}

		private void LocalPreferencesClick(object sender, RoutedEventArgs e)
		{
			if (!(MainTabControl.SelectedContent is Game))
				return;

			var game = MainTabControl.SelectedContent as Game;
			var gametab = MainTabControl.SelectedItem as TabItem;
			var content = new GamePreferencesDialog(game.Preferences);
			var preferncestab = new TabItem { Header = string.Format("{0} \"{1}\"", Properties.Resources.LocalPreferencesHeader, gametab.Header), Content = content };

			content.LockGlobal();
			content.OkClicked +=
				preferences =>
					{
						MainTabControl.Items.Remove(preferncestab);
						game.Preferences = preferences;
						gametab.Header = preferences.TabName;
					};
			content.CancelClicked += () => MainTabControl.Items.Remove(preferncestab);

			MainTabControl.Items.Add(preferncestab);
			MainTabControl.SelectedIndex = MainTabControl.Items.Count - 1;
		}

		private void GlobalPreferencesClick(object sender, RoutedEventArgs e)
		{
			var content = new GamePreferencesDialog(new GamePreferences(_globalPreferences));
			var preferncestab = new TabItem { Header = Properties.Resources.GlobalPreferencesHeader, Content = content };

			content.OkClicked +=
				preferences =>
				{
					MainTabControl.Items.Remove(preferncestab);
					_globalPreferences = preferences;
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
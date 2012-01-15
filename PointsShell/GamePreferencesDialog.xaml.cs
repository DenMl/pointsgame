using System;
using System.Windows;
using PointsShell.Enums;

namespace PointsShell
{
	public partial class GamePreferencesDialog
	{
		public event Action<GamePreferences> OkClicked;
		public event Action CancelClicked;

		public GamePreferencesDialog()
		{
			InitializeComponent();

			SetPreferences(new GamePreferences());
		}

		public GamePreferencesDialog(GamePreferences preferences)
		{
			InitializeComponent();

			SetPreferences(preferences);
		}

		public GamePreferencesDialog(GamePreferences preferences, GameLanguage language)
		{
			InitializeComponent();

			SetPreferences(preferences);
			SetLanguage(language);
		}

		public void SetPreferences(GamePreferences preferences)
		{
			WidthBox.Text = preferences.Width.ToString();
			HeightBox.Text = preferences.Height.ToString();
			AICheckBox.IsChecked = preferences.AI;
			MinMaxDepthBox.Text = preferences.Depth.ToString();
			UCTIterationsBox.Text = preferences.Iterations.ToString();
			RedNameBox.Text = preferences.RedName;
			BlackNameBox.Text = preferences.BlackName;
			RedColorPicker.SelectedColor = preferences.RedColor;
			BlackColorPicker.SelectedColor = preferences.BlackColor;
			FillingAlphaBox.Text = preferences.FillingAlpha.ToString();
			BackgroundColorPicker.SelectedColor = preferences.BackgroundColor;
			FullFillCheckBox.IsChecked = preferences.FullFill;
			SoundsCheckBox.IsChecked = preferences.Sounds;
			switch (preferences.SurCond)
			{
				case (SurroundCond.Standart):
					StandartRadioButton.IsChecked = true;
					break;
				case (SurroundCond.Always):
					AlwaysRadioButton.IsChecked = true;
					break;
				case (SurroundCond.AlwaysEnemy):
					AlwaysEnemyRadioButton.IsChecked = true;
					break;
			}
			switch (preferences.BeginPattern)
			{
				case (BeginPattern.CleanPattern):
					CleanRadioButton.IsChecked = true;
					break;
				case (BeginPattern.CrosswisePattern):
					CrosswireRadioButton.IsChecked = true;
					break;
				case (BeginPattern.SquarePattern):
					SquareRadioButton.IsChecked = true;
					break;
			}
		}

		public void SetLanguage(GameLanguage language)
		{
			OKButton.Content = language.OK;
			CancelButton.Content = language.Cancel;
			GamePreferencesGroupBox.Header = language.GamePreferences;
			AIPreferencesGroupBox.Header = language.AIPreferences;
			PlayersPreferencesGroupBox.Header = language.PlayersPreferences;
			OtherPreferencesGroupBox.Header = language.OtherPrefernces;
			WidthTextBlock.Text = language.Width;
			HeightTextBlock.Text = language.Height;
			SurroundConditionGroupBox.Header = language.SurroundCondition;
			StandartRadioButton.Content = language.Standart;
			AlwaysRadioButton.Content = language.Always;
			AlwaysEnemyRadioButton.Content = language.AlwaysEnemy;
			BeginPatternGroupBox.Header = language.BeginPattern;
			CleanRadioButton.Content = language.Clean;
			CrosswireRadioButton.Content = language.Crosswire;
			SquareRadioButton.Content = language.Square;
			AICheckBox.Content = language.AI;
			MinMaxDepthTextBlock.Text = language.MinMaxDepth;
			UCTIterationsTextBlock.Text = language.UCTIterations;
			RedNameTextBlock.Text = language.RedName;
			BlackNameTextBlock.Text = language.BlackName;
			RedColorTextBlock.Text = language.RedColor;
			BlackColorTextBlock.Text = language.BlackColor;
			FillingAlphaTextBlock.Text = language.FillingAlpha;
			BackgroundColorTextBlock.Text = language.BackgroundColor;
			FullFillCheckBox.Content = language.FullFill;
			SoundsCheckBox.Content = language.Sounds;
		}

		private void OKClick(object sender, RoutedEventArgs routedEventArgs)
		{
			var preferences = new GamePreferences();

			try
			{
				preferences.Width = int.Parse(WidthBox.Text);
				preferences.Height = int.Parse(HeightBox.Text);
				preferences.AI = AICheckBox.IsChecked.Value;
				preferences.Depth = int.Parse(MinMaxDepthBox.Text);
				preferences.Iterations = int.Parse(UCTIterationsBox.Text);
				preferences.RedName = RedNameBox.Text;
				preferences.BlackName = BlackNameBox.Text;
				preferences.RedColor = RedColorPicker.SelectedColor;
				preferences.BlackColor = BlackColorPicker.SelectedColor;
				preferences.FillingAlpha = byte.Parse(FillingAlphaBox.Text);
				preferences.BackgroundColor = BackgroundColorPicker.SelectedColor;
				preferences.FullFill = FullFillCheckBox.IsChecked.Value;
				preferences.Sounds = SoundsCheckBox.IsChecked.Value;

				if (StandartRadioButton.IsChecked == true)
					preferences.SurCond = SurroundCond.Standart;
				else if (AlwaysRadioButton.IsChecked == true)
					preferences.SurCond = SurroundCond.Always;
				else if (AlwaysEnemyRadioButton.IsChecked == true)
					preferences.SurCond = SurroundCond.AlwaysEnemy;
				if (CleanRadioButton.IsChecked == true)
					preferences.BeginPattern = BeginPattern.CleanPattern;
				else if (CrosswireRadioButton.IsChecked == true)
					preferences.BeginPattern = BeginPattern.CrosswisePattern;
				else if (SquareRadioButton.IsChecked == true)
					preferences.BeginPattern = BeginPattern.SquarePattern;
			}
			catch
			{
				return;
			}

			if (OkClicked != null)
				OkClicked(preferences);
		}

		private void CancelClick(object sender, RoutedEventArgs routedEventArgs)
		{
			if (CancelClicked != null)
				CancelClicked();
		}

		public void LockGlobal()
		{
			WidthBox.IsEnabled = false;
			HeightBox.IsEnabled = false;
			StandartRadioButton.IsEnabled = false;
			AlwaysRadioButton.IsEnabled = false;
			AlwaysEnemyRadioButton.IsEnabled = false;
			CleanRadioButton.IsEnabled = false;
			CrosswireRadioButton.IsEnabled = false;
			SquareRadioButton.IsEnabled = false;
		}
	}
}
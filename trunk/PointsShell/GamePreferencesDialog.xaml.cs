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

		public void SetPreferences(GamePreferences preferences)
		{
			WidthBox.Text = preferences.Width.ToString();
			HeightBox.Text = preferences.Height.ToString();
			AICheckBox.IsChecked = preferences.AI;
			ComplexityBox.Text = preferences.Complexity.ToString();
			TimeBox.Text = preferences.Time.ToString();
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

		private void OKClick(object sender, RoutedEventArgs routedEventArgs)
		{
			var preferences = new GamePreferences();

			try
			{
				preferences.Width = int.Parse(WidthBox.Text);
				preferences.Height = int.Parse(HeightBox.Text);
				preferences.AI = AICheckBox.IsChecked.Value;
				preferences.Complexity = int.Parse(ComplexityBox.Text);
				preferences.Time = int.Parse(TimeBox.Text);
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
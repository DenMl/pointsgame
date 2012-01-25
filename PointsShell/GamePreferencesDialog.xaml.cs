using System;
using System.Windows;
using PointsShell.Enums;

namespace PointsShell
{
	public partial class GamePreferencesDialog
	{
		private readonly GamePreferences _preferences;

		public event Action<GamePreferences> OkClicked;
		public event Action CancelClicked;

		public GamePreferencesDialog(GamePreferences preferences = null)
		{
			InitializeComponent();
			_preferences = preferences ?? new GamePreferences();
			DataContext = _preferences;
			SetPreferences(_preferences);
		}

		public void SetPreferences(GamePreferences preferences)
		{
			RedColorPicker.SelectedColor = preferences.RedColor;
			BlackColorPicker.SelectedColor = preferences.BlackColor;
			BackgroundColorPicker.SelectedColor = preferences.BackgroundColor;
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
			try
			{
				_preferences.RedColor = RedColorPicker.SelectedColor;
				_preferences.BlackColor = BlackColorPicker.SelectedColor;
				_preferences.BackgroundColor = BackgroundColorPicker.SelectedColor;

				if (StandartRadioButton.IsChecked == true)
					_preferences.SurCond = SurroundCond.Standart;
				else if (AlwaysRadioButton.IsChecked == true)
					_preferences.SurCond = SurroundCond.Always;
				else if (AlwaysEnemyRadioButton.IsChecked == true)
					_preferences.SurCond = SurroundCond.AlwaysEnemy;
				if (CleanRadioButton.IsChecked == true)
					_preferences.BeginPattern = BeginPattern.CleanPattern;
				else if (CrosswireRadioButton.IsChecked == true)
					_preferences.BeginPattern = BeginPattern.CrosswisePattern;
				else if (SquareRadioButton.IsChecked == true)
					_preferences.BeginPattern = BeginPattern.SquarePattern;
			}
			catch
			{
				return;
			}

			if (OkClicked != null)
				OkClicked(_preferences);
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
namespace LicenseDetectionUI.ViewModels
{
    public class MainViewModel : ViewModelBase
    {
		private string _testTxt;
		public string TestTxt
		{
			get
			{
				return _testTxt;
			}
			set
			{
				_testTxt = value;
				OnPropertyChanged(nameof(TestTxt));
			}
		}

        public MainViewModel()
        {
			TestTxt = "test";
        }
    }
}

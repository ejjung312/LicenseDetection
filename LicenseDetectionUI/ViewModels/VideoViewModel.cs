using LicenseDetectionUI.Commands;
using System.Windows.Input;

namespace LicenseDetectionUI.ViewModels
{
    public class VideoViewModel : ViewModelBase
    {
        //private string _test;
        //public string Test
        //{
        //    get
        //    {
        //        return _test;
        //    }
        //    set
        //    {
        //        _test = value;
        //        OnPropertyChanged(nameof(Test));
        //    }
        //}

        //public ICommand FrameProcessCommand { get; }

        public VideoViewModel()
        {
            //_test = "test!";

            //FrameProcessCommand = new FrameProcessCommand(this);
        }

        public override void Dispose()
        {
            base.Dispose();
        }
    }
}

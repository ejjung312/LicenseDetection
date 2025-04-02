using LicenseDetectionUI.ViewModels;
using System.Windows;

namespace LicenseDetectionUI.Commands
{
    public class FrameProcessCommand : AsyncCommandBase
    {
        private readonly VideoViewModel _viewoViewModel;

        public FrameProcessCommand(VideoViewModel viewoViewModel)
        {
            _viewoViewModel = viewoViewModel;
        }

        public override async Task ExecuteAsync(object? parameter)
        {
            MessageBox.Show("Frame Processing!");
        }
    }
}

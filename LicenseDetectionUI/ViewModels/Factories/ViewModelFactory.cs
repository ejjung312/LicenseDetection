using LicenseDetectionUI.State.Navigators;

namespace LicenseDetectionUI.ViewModels.Factories
{
    public class ViewModelFactory : IViewModelFactory
    {
        private readonly CreateViewModel<VideoViewModel> _createVideoViewModel;
        private readonly CreateViewModel<LoggingViewModel> _createLoggingViewModel;

        public ViewModelFactory(CreateViewModel<VideoViewModel> createVideoViewModel, CreateViewModel<LoggingViewModel> createLoggingViewModel)
        {
            _createVideoViewModel = createVideoViewModel;
            _createLoggingViewModel = createLoggingViewModel;
        }

        public ViewModelBase CreateViewModel(ViewType viewType)
        {
            switch (viewType)
            {
                case ViewType.Video:
                    return _createVideoViewModel();
                case ViewType.Logging:
                    return _createLoggingViewModel();
                default:
                    throw new ArgumentException("The ViewType does not have a ViewModel.");
            }
        }
    }
}

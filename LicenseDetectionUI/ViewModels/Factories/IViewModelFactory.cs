using LicenseDetectionUI.State.Navigators;

namespace LicenseDetectionUI.ViewModels.Factories
{
    public interface IViewModelFactory
    {
        ViewModelBase CreateViewModel(ViewType viewType);
    }
}

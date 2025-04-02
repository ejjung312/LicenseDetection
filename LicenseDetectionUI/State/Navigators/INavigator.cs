using LicenseDetectionUI.ViewModels;

namespace LicenseDetectionUI.State.Navigators
{
    public enum ViewType
    {
        Video,
        Logging
    }

    public interface INavigator
    {
        ViewModelBase CurrentViewModel { get; set; }
        event Action StateChanged;
    }
}

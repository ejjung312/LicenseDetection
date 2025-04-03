using LicenseDetectionUI.Services;
using LicenseDetectionUI.State.Navigators;
using LicenseDetectionUI.ViewModels;
using LicenseDetectionUI.ViewModels.Factories;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

namespace LicenseDetectionUI.HostBuilders
{
    public static class AddViewModelsHostBuilderExtensions
    {
        public static IHostBuilder AddViewModels(this IHostBuilder host)
        {
            host.ConfigureServices(services =>
            {
                services.AddTransient<MainViewModel>();
                services.AddTransient<LoggingViewModel>();
                services.AddTransient(CreateVideoViewModel);

                services.AddSingleton<CreateViewModel<VideoViewModel>>(services => () => services.GetRequiredService<VideoViewModel>());
                services.AddSingleton<CreateViewModel<LoggingViewModel>>(services => () => services.GetRequiredService<LoggingViewModel>());

                services.AddSingleton<IViewModelFactory, ViewModelFactory>();

                services.AddSingleton<ViewModelDelegateRenavigator<VideoViewModel>>();
                services.AddSingleton<ViewModelDelegateRenavigator<LoggingViewModel>>();
            });

            return host;
        }

        private static VideoViewModel CreateVideoViewModel(IServiceProvider services)
        {
            return new VideoViewModel(services.GetRequiredService<IVideoService>());
        }
    }
}

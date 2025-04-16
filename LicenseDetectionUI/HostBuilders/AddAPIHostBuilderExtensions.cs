using LicenseDetection.API;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

namespace LicenseDetectionUI.HostBuilders
{
    public static class AddAPIHostBuilderExtensions
    {
        public static IHostBuilder AddAPI(this IHostBuilder host)
        {
            host.ConfigureServices((context, services) =>
            {
                services.AddHttpClient<PrepHttpClient>(c =>
                {
                    c.BaseAddress = new Uri("http://127.0.0.1:8000");
                });
            });

            return host;
        }
    }
}

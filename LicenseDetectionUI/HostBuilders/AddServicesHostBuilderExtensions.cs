﻿using LicenseDetection.API.Services;
using LicenseDetectionUI.Services;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

namespace LicenseDetectionUI.HostBuilders
{
    public static class AddServicesHostBuilderExtensions
    {
        public static IHostBuilder AddServices(this IHostBuilder host)
        {
            host.ConfigureServices(services =>
            {
                services.AddSingleton<IVideoService, VideoService>();

                services.AddSingleton<ILicensePlateRecognition, LicensePlateRecognition>();
            });

            return host;
        }
    }
}

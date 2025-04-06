using LicenseDetectionUI.HostBuilders;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using System.Runtime.InteropServices;
using System.Windows;

namespace LicenseDetectionUI
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private readonly IHost _host;

        [DllImport("LicenseDetection.dll")]
        public static extern void InitLicenseModel(string modelPath, string classNames);

        [DllImport("LicenseDetection.dll")]
        public static extern void ReleaseLicenseModel();

        [DllImport("kernel32.dll")]
        public static extern bool AllocConsole();

        public App()
        {
            _host = CreateHostBuilder().Build();
        }

        private static IHostBuilder CreateHostBuilder(string[] args=null)
        {
            return Host.CreateDefaultBuilder(args)
                .AddServices()
                .AddStores()
                .AddViewModels()
                .AddViews();
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            _host.Start();

            Window window = _host.Services.GetRequiredService<MainWindow>();
            window.Show();

            base.OnStartup(e);

            // 콘솔 창 열기
            AllocConsole();

            // 모델 로드
            loadDetectModel();
        }

        protected override async void OnExit(ExitEventArgs e)
        {
            await _host.StopAsync();
            _host.Dispose();

            base.OnExit(e);

            // 모델 메모리 해제
            ReleaseLicenseModel();
        }

        private void loadDetectModel()
        {
            try
            {
                string modelPath = "Onnx/license_detect_model.onnx";
                string classPath = "Onnx/license.names";

                InitLicenseModel(modelPath, classPath);
            }
            catch (Exception)
            {
                MessageBox.Show("Model Load failed.");
            }
        }
    }
}

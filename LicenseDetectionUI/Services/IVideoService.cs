using System.Windows.Media.Imaging;

namespace LicenseDetectionUI.Services
{
    public interface IVideoService
    {
        event Action<BitmapSource> FrameProcessed;
        event Action<BitmapSource, string> LicensePlateProcessed;

        Task VideoPlayAsync(string videoPath, CancellationToken cancellationToken);
    }
}

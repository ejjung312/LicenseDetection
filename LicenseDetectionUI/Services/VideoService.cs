using OpenCvSharp;
using OpenCvSharp.WpfExtensions;
using System.IO;
using System.Windows.Media.Imaging;

namespace LicenseDetectionUI.Services
{
    public class VideoService : IVideoService
    {
        public event Action<BitmapSource, BitmapSource, string> FrameProcessed;

        public async Task VideoPlayAsync(string videoPath, CancellationToken cancellationToken)
        {
            await Task.Run(() =>
            {
                string video = Path.Combine(Directory.GetCurrentDirectory(), videoPath);

                using var cap = new VideoCapture(video);
                if (!cap.IsOpened()) return;

                Mat frame = new Mat();
                while (!cancellationToken.IsCancellationRequested)
                {
                    if (!cap.Read(frame) || frame.Empty()) break;

                    BitmapSource bitmapSource = frame.ToBitmapSource();
                    bitmapSource.Freeze();

                    FrameProcessed?.Invoke(bitmapSource, bitmapSource, null);

                    Thread.Sleep(30);
                }
            }, cancellationToken);
        }
    }
}

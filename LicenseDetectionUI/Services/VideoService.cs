using OpenCvSharp;
using OpenCvSharp.WpfExtensions;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Media.Imaging;

namespace LicenseDetectionUI.Services
{
    public class VideoService : IVideoService
    {
        public event Action<BitmapSource, BitmapSource, string> FrameProcessed;

        [DllImport("LicenseDetection.dll")]
        public static extern void LicenseDetection(string modelPath, string classPath, [In] byte[] data, int width, int height, int stride);

        public async Task VideoPlayAsync(string videoPath, string modelPath, string classPath, CancellationToken cancellationToken)
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

                    LicenseDetection(modelPath, classPath, frame.ToBytes(), frame.Width, frame.Height, frame.Width * frame.Channels());

                    BitmapSource bitmapSource = frame.ToBitmapSource();
                    bitmapSource.Freeze();

                    FrameProcessed?.Invoke(bitmapSource, bitmapSource, null);

                    Thread.Sleep(30);
                }
            }, cancellationToken);
        }
    }
}

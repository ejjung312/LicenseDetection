using LicenseDetectionUI.Interop;
using LicenseDetectionUI.Models;
using OpenCvSharp;
using OpenCvSharp.WpfExtensions;
using System.Collections.Concurrent;
using System.IO;
using System.Windows.Media.Imaging;

namespace LicenseDetectionUI.Services
{
    public class VideoService : IVideoService
    {
        public event Action<BitmapSource, BitmapSource, string> FrameProcessed;
        private ConcurrentQueue<Mat> _frameQueue = new ConcurrentQueue<Mat>();

        public async Task VideoPlayAsync(string videoPath, CancellationToken cancellationToken)
        {
            string video = Path.Combine(Directory.GetCurrentDirectory(), videoPath);

            using var cap = new VideoCapture(video);
            if (!cap.IsOpened()) return;

            int delayPerFrame = (int)(1000.0 / cap.Fps);

            // 입력 스레드
            var readingTask =  Task.Run(() =>
            {
                Mat frame = new Mat();
                while (!cancellationToken.IsCancellationRequested)
                {
                    if (!cap.Read(frame) || frame.Empty()) break;
                    _frameQueue.Enqueue(frame.Clone());

                    Thread.Sleep(delayPerFrame); // 프레임 간 딜레이 맞추기
                }
            }, cancellationToken);

            // 추론 스레드
            var detectionTask = Task.Run(() =>
            {
                while (!cancellationToken.IsCancellationRequested)
                {
                    if (_frameQueue.TryDequeue(out var frame))
                    {
                        // 큐에 프레임이 여러개 쌓여 있으면 중간 프레임 건너뛰기
                        while (_frameQueue.Count > 10)
                        {
                            _frameQueue.TryDequeue(out frame);
                        }

                        unsafe
                        {
                            DetectionResult[] results = new DetectionResult[10]; // 최대 10개 검출 예상
                            int resultCount = 0;

                            //using var frameClone = frame.Clone();
                            IntPtr dataPtr = frame.Data; // Mat 내부 메모리 포인터
                            NativeMethods.LicenseDetection(dataPtr, frame.Width, frame.Height, (int)frame.Step(), results, ref resultCount);

                            BitmapSource bitmapSource = frame.ToBitmapSource();
                            bitmapSource.Freeze();

                            FrameProcessed?.Invoke(bitmapSource, bitmapSource, null);
                        }
                    }

                    Thread.Sleep(10);
                }
            }, cancellationToken);

            await Task.WhenAll(readingTask, detectionTask);
        }
    }
}

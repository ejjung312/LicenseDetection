using LicenseDetection.API.Services;
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
        private ILicensePlateRecognition _licensePlateRecognition;
        public event Action<BitmapSource> FrameProcessed;
        public event Action<BitmapSource, string> LicensePlateProcessed;
        private ConcurrentQueue<Mat> _frameQueue = new ConcurrentQueue<Mat>();

        public VideoService(ILicensePlateRecognition licensePlateRecognition)
        {
            _licensePlateRecognition = licensePlateRecognition;
        }

        public async Task VideoPlayAsync(string videoPath, CancellationToken cancellationToken)
        {
            string video = Path.Combine(Directory.GetCurrentDirectory(), videoPath);

            using var cap = new VideoCapture(video);
            if (!cap.IsOpened()) return;

            int delayPerFrame = (int)(1000.0 / cap.Fps);

            // 입력 스레드
            var readingTask = Task.Run(() =>
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
            var detectionTask = Task.Run(async () =>
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

                        
                        DetectionResult[] results = new DetectionResult[10]; // 최대 10개 검출 예상
                        int resultCount = 0;

                        //using var frameClone = frame.Clone();
                        IntPtr dataPtr = frame.Data; // Mat 내부 메모리 포인터
                        NativeMethods.LicenseDetection(dataPtr, frame.Width, frame.Height, (int)frame.Step(), results, ref resultCount);

                        for (int i = 0; i < resultCount; i++)
                        {
                            DetectionResult temp = results[i];

                            // 자동차번호판 자르기
                            Rect roi = new Rect(temp.X1, temp.Y1, temp.X2-temp.X1, temp.Y2-temp.Y1);
                            Mat licenseImage = new Mat(frame, roi);

                            // TODO 임시 테스트
                            string jsonData = await _licensePlateRecognition.getLicensePlateInfo(licenseImage.ToBytes());

                            // ViewModel에 전달
                            BitmapSource licenseImageSource = licenseImage.ToBitmapSource();
                            licenseImageSource.Freeze();

                            LicensePlateProcessed?.Invoke(licenseImageSource, null);

                            // 자동차번호판 영역표시
                            Cv2.Rectangle(frame, new Point(temp.X1, temp.Y1), new Point(temp.X2, temp.Y2), new Scalar(0, 255, 0));
                        }

                        BitmapSource bitmapSource = frame.ToBitmapSource();
                        bitmapSource.Freeze();

                        FrameProcessed?.Invoke(bitmapSource);
                    }

                    Thread.Sleep(10);
                }
            }, cancellationToken);

            await Task.WhenAll(readingTask, detectionTask);
        }
    }
}

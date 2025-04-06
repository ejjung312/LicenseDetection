using LicenseDetectionUI.Commands;
using LicenseDetectionUI.Models;
using LicenseDetectionUI.Services;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace LicenseDetectionUI.ViewModels
{
    public class VideoViewModel : ViewModelBase
    {
        private readonly IVideoService _videoService;

        private ImageSource _videoFrameImage;
        public ImageSource VideoFrameImage
        {
            get
            {
                return _videoFrameImage;
            }
            set
            {
                _videoFrameImage = value;
                OnPropertyChanged(nameof(VideoFrameImage));
            }
        }

        private ObservableCollection<LicensePlateItem> _detectedList;
        public ObservableCollection<LicensePlateItem> DetectedList
        {
            get => _detectedList;
            set
            {
                _detectedList = value;
                OnPropertyChanged(nameof(DetectedList));
            }
        }

        private string _video = "Video/car3.mp4";
        public string Video => _video;

        private CancellationTokenSource _token;

        public ICommand FrameProcessCommand { get; }

        public VideoViewModel(IVideoService videoService)
        {
            _videoService = videoService;

            DetectedList = new ObservableCollection<LicensePlateItem>();

            _videoService.FrameProcessed += VideoService_FrameProcessed;

            _token = new CancellationTokenSource();
            FrameProcessCommand = new FrameProcessCommand(this, _videoService, _token.Token);
        }

        private void VideoService_FrameProcessed(BitmapSource processedImg, BitmapSource licensePlateImage, string licensePlateText)
        {
            // UI 스레드에서 호출
            var dispatcher = Application.Current?.Dispatcher;
            if (dispatcher != null && !dispatcher.HasShutdownStarted)
            {
                try
                {
                    // 비동기 UI 갱신
                    dispatcher.InvokeAsync(() =>
                    {
                        VideoFrameImage = processedImg;

                        DetectedList.Add(new LicensePlateItem { detectedImage = licensePlateImage, detectedText = licensePlateText });
                    });
                }
                catch (TaskCanceledException ex)
                {
                    
                }
            }
        }

        public override void Dispose()
        {
            _token.Cancel();

            base.Dispose();
        }
    }
}

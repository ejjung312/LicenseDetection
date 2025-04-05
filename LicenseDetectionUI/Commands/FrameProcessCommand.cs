using LicenseDetectionUI.Services;
using LicenseDetectionUI.ViewModels;
using System.Threading;

namespace LicenseDetectionUI.Commands
{
    public class FrameProcessCommand : AsyncCommandBase
    {
        private readonly VideoViewModel _videoViewModel;
        private readonly IVideoService _videoService;
        private readonly CancellationToken _cancellationToken;

        public FrameProcessCommand(VideoViewModel videoViewModel, IVideoService videoService, CancellationToken cancellationToken)
        {
            _videoViewModel = videoViewModel;
            _videoService = videoService;
            _cancellationToken = cancellationToken;
        }

        public override async Task ExecuteAsync(object? parameter)
        {
            await _videoService.VideoPlayAsync(_videoViewModel.Video, _cancellationToken);
        }
    }
}

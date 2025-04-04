﻿using System.Windows.Media.Imaging;

namespace LicenseDetectionUI.Services
{
    public interface IVideoService
    {
        event Action<BitmapSource, BitmapSource, string> FrameProcessed;

        Task VideoPlayAsync(string videoPath, CancellationToken cancellationToken);
    }
}

using System.Windows.Media.Imaging;

namespace LicenseDetectionUI.Models
{
    public class LicensePlateItem
    {
        public BitmapSource DetectedImage { get; set; }
        public string DetectedText { get; set; }
    }
}

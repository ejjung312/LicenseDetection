using System.Windows.Media.Imaging;

namespace LicenseDetectionUI.Models
{
    public class LicensePlateItem
    {
        public BitmapSource detectedImage { get; set; }
        public string detectedText { get; set; }
    }
}

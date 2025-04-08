using System.Runtime.InteropServices;

namespace LicenseDetectionUI.Models
{
    [StructLayout(LayoutKind.Sequential)]
    public struct DetectionResult
    {
        public int X;
        public int Y;
        public int Width;
        public int Height;
        public float Confidence;
        public int ClassId;
    }
}

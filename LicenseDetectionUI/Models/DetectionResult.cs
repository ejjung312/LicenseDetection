using System.Runtime.InteropServices;

namespace LicenseDetectionUI.Models
{
    [StructLayout(LayoutKind.Sequential)]
    public struct DetectionResult
    {
        public float Cx;
        public float Cy;
        public int X1;
        public int Y1;
        public int X2;
        public int Y2;
        public int Width;
        public int Height;
        public float Confidence;
        public int ClassId;
    }
}

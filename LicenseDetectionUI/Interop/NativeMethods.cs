using LicenseDetectionUI.Models;
using System.Runtime.InteropServices;

namespace LicenseDetectionUI.Interop
{
    internal static class NativeMethods
    {
        [DllImport("LicenseDetection.dll")]
        public static extern void InitLicenseModel(string modelPath, string classNames);

        [DllImport("LicenseDetection.dll")]
        public static extern void ReleaseLicenseModel();

        [DllImport("LicenseDetection.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void LicenseDetection(IntPtr data, int width, int height, int stride, [Out] DetectionResult[] results, ref int resultCount);
    }
}

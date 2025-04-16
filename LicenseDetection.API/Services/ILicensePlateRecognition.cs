using LicenseDetection.API.Results;

namespace LicenseDetection.API.Services
{
    public interface ILicensePlateRecognition
    {
        Task<string> getLicensePlateInfo(byte[] data);
    }
}

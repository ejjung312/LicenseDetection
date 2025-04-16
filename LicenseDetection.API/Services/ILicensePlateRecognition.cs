using LicenseDetection.API.Results;

namespace LicenseDetection.API.Services
{
    public interface ILicensePlateRecognition
    {
        Task<APIResponse> getLicensePlateInfo(byte[] data);
    }
}

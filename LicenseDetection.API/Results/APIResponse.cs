using System.Text.Json.Serialization;

namespace LicenseDetection.API.Results
{
    public class APIResponse
    {
        [JsonPropertyName("license_plate_text")]
        public string LicensePlateText { get; set; }
    }
}

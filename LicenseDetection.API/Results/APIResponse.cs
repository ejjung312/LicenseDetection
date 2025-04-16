using System.Text.Json.Serialization;

namespace LicenseDetection.API.Results
{
    public class APIResponse
    {
        [JsonPropertyName("message")]
        public string Message { get; set; } // message -> Message

        [JsonPropertyName("license_plate_text")]
        public string LicensePlateText { get; set; } // license_plate_text -> LicensePlateText
    }
}

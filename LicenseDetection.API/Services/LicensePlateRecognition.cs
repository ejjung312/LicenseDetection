using System.Net.Http;
using System.Net.Http.Headers;
using System.Text.Json;
using LicenseDetection.API.Results;

namespace LicenseDetection.API.Services
{
    public class LicensePlateRecognition : ILicensePlateRecognition
    {
        private readonly PrepHttpClient _client;

        public LicensePlateRecognition(PrepHttpClient client)
        {
            _client = client;
        }

        public async Task<APIResponse> getLicensePlateInfo(byte[] data)
        {
            string uri = _client.BaseAddress + "api/ocr/plate";

            using var content = new MultipartFormDataContent();

            // 이미지 추가
            var imageContent = new ByteArrayContent(data);
            imageContent.Headers.ContentType = new MediaTypeHeaderValue("image/jpeg");
            content.Add(imageContent, "image", "license_plate.jpg");

            using var response = await _client.PostAsync(uri, content);
            response.EnsureSuccessStatusCode(); // 200번대 성공 코드가 아니면 예외 처리

            string responseJson = await response.Content.ReadAsStringAsync();

            // JSON 파싱
            var jsonData = JsonSerializer.Deserialize<APIResponse>(responseJson);

            return jsonData;
        }
    }
}

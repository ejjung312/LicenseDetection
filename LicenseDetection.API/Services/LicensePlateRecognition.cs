using System.Net.Http;
using System.Net.Http.Headers;
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

        public async Task<string> getLicensePlateInfo(byte[] data)
        {
            string uri = _client.BaseAddress + "api/ocr/plate";

            using var content = new MultipartFormDataContent();

            // 이미지 추가
            var imageContent = new ByteArrayContent(data);
            imageContent.Headers.ContentType = new MediaTypeHeaderValue("image/jpeg");
            content.Add(imageContent, "image");

            using var response = await _client.PostAsync(uri, content);
            response.EnsureSuccessStatusCode(); // 200번대 성공 코드가 아니면 예외 처리

            return await response.Content.ReadAsStringAsync();
        }
    }
}

using System.Net.Http;
using Newtonsoft.Json;

namespace LicenseDetection.API
{
    public class PrepHttpClient : HttpClient
    {
        private readonly HttpClient _client;

        public PrepHttpClient(HttpClient client)
        {
            this.BaseAddress = new Uri("http://127.0.0.1:8000");
            _client = client;
        }

        public async Task<T> GetAsync<T>(string uri)
        {
            HttpResponseMessage response = await _client.GetAsync(uri);
            string jsonResponse = await response.Content.ReadAsStringAsync();

            return JsonConvert.DeserializeObject<T>(jsonResponse);
        }

        public async Task<HttpResponseMessage> PostAsync<T>(string uri, HttpContent content)
        {
            HttpResponseMessage response = await _client.PostAsync(uri, content);

            return response;
        }
    }
}

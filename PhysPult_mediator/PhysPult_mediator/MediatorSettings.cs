using PhysPult_mediator.Communication.SerialCommunicator;
using System.Text;
using System.Text.Json;

namespace PhysPult_mediator
{
    // Anti-pattern, but idgaf
    public class MediatorSettings
    {
        private static readonly JsonSerializerOptions jsonSerializerOptions = new()
        {
            WriteIndented = true,
            PropertyNameCaseInsensitive = true,
        };

        public SerialPortParameters SerialPortParameters { get; set; }
        public WebsocketParameters WebsocketParameters { get; set; }

        public void SaveToFile()
        {
            using var settingsFile = File.OpenWrite("MediatorSettings.json");

            var json = JsonSerializer.Serialize(this, jsonSerializerOptions);

            settingsFile.Write(Encoding.UTF8.GetBytes(json));
        }
        public static MediatorSettings LoadFromFile()
        {
            using var settingsFile = File.OpenRead("MediatorSettings.json");

            return JsonSerializer.Deserialize<MediatorSettings>(settingsFile, jsonSerializerOptions);
        }
    }
}

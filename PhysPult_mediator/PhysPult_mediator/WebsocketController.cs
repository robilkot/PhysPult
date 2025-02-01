using Microsoft.AspNetCore.Mvc;
using PhysPult_mediator.Communication.SerialCommunicator;
using PhysPult_mediator.Communication.SerialCommunicator.Messages;
using System.Net.WebSockets;
using System.Text;

namespace PhysPult_mediator
{
    public class WebsocketController : ControllerBase
    {
        private readonly SerialCommunicator _serialCommunicator;
        public WebsocketController(SerialCommunicator communicator)
        {
            _serialCommunicator = communicator;
        }

        [Route("/ws")]
        public async Task Get()
        {
            if (!HttpContext.WebSockets.IsWebSocketRequest)
            {
                HttpContext.Response.StatusCode = StatusCodes.Status400BadRequest;
                return;
            }
            if (!_serialCommunicator.Connected)
            {
                HttpContext.Response.StatusCode = StatusCodes.Status503ServiceUnavailable;
                return;
            }

            using var webSocket = await HttpContext.WebSockets.AcceptWebSocketAsync();

            await ProcessSession(webSocket);
        }

        // todo: handle force disconnect
        private async Task ProcessSession(WebSocket webSocket)
        {
            async void onSerialMessageReceived(object? sender, SerialCommunicatorMessage message)
            {
                var bytes = Encoding.UTF8.GetBytes(message.Content);
                await webSocket.SendAsync(bytes, WebSocketMessageType.Text, true, CancellationToken.None); // todo: what if message is longer than 1 segment? idgaf for now
            }

            void onSerialMessageCorrupted(object? sender, SerialCommunicatorMessage? msg)
            {
                Console.WriteLine($"MessageCorrupted ({msg.Crc}, {msg.SequenceNumber}, {msg.AckNumber}, {msg.ContentLength}, {msg.Content})");
            }

            // Subscribe
            // todo: MessageCorrupted
            _serialCommunicator.MessageReceived += onSerialMessageReceived;
            _serialCommunicator.MessageCorrupted += onSerialMessageCorrupted;

            // Accept messages
            var buffer = new byte[1024 * 4];
            var receiveResult = await webSocket.ReceiveAsync(
                new ArraySegment<byte>(buffer), CancellationToken.None);

            while (!receiveResult.CloseStatus.HasValue)
            {
                var message = Encoding.UTF8.GetString(buffer.Take(receiveResult.Count).ToArray());

                if(_serialCommunicator.Send(message) is (false, Exception ex))
                {
                    Console.WriteLine($"Serial send failure: {ex.Message}");
                }

                receiveResult = await webSocket.ReceiveAsync(
                    new ArraySegment<byte>(buffer), CancellationToken.None);
            }

            // Unsubscribe
            _serialCommunicator.MessageReceived -= onSerialMessageReceived;
            _serialCommunicator.MessageCorrupted -= onSerialMessageCorrupted;

            await webSocket.CloseAsync(
                receiveResult.CloseStatus.Value,
                receiveResult.CloseStatusDescription,
                CancellationToken.None);
        }
    }
}

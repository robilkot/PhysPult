using PhysPult_mediator;
using PhysPult_mediator.Communication.SerialCommunicator;
using PhysPult_mediator.Communication.SerialCommunicator.Messages;
using PhysPult_mediator.Communication.SerialCommunicator.Readers;
using PhysPult_mediator.Helpers;
using System.IO.Ports;


MediatorSettings? settings = null;
try
{
    settings = MediatorSettings.LoadFromFile();
}
catch (Exception ex_)
{
    Console.WriteLine($"Can't read settings from MediatorSettings.json: {ex_.Message}");
}

var reader = new SerialPultMessageReader();
var svc = new SerialCommunicator<SerialCommunicatorMessage>(reader);

var connected = svc.TryConnect(settings.SerialPortParameters);

while (connected is (false, Exception ex))
{
    Console.WriteLine(ex.Message);

    // Choose active port
    var ports = SerialPort.GetPortNames();

    Console.WriteLine("Available ports:");
    ports.Each((port, index) =>
    {
        Console.WriteLine($"{index}: {port}");
    });

    Console.WriteLine("Input port index:");
    var portIndex = int.Parse(Console.ReadLine()!);

    settings.SerialPortParameters.PortName = ports[portIndex];
    settings.SaveToFile();

    connected = svc.TryConnect(settings.SerialPortParameters);
}

// todo: handle disconnects
// Build app
var builder = WebApplication.CreateBuilder(args);

builder.Services.AddControllers();

builder.Services.AddSingleton(svc);

var app = builder.Build();

var webSocketOptions = new WebSocketOptions
{
    KeepAliveInterval = TimeSpan.FromMinutes(2)
};

app.UseWebSockets(webSocketOptions);

app.MapControllers();

app.Run();
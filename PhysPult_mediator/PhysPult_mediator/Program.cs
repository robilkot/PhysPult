using PhysPult_mediator.Communication.SerialCommunicator;
using PhysPult_mediator.Communication.SerialCommunicator.Messages;
using PhysPult_mediator.Communication.SerialCommunicator.Readers;
using PhysPult_mediator.Helpers;
using System.IO.Ports;

// Choose active port
var ports = SerialPort.GetPortNames();

Console.WriteLine("Available ports:");
ports.Each((port, index) =>
{
    Console.WriteLine($"{index}: {port}");
});

Console.WriteLine("Input port index:");
var portIndex = int.Parse(Console.ReadLine()!);

var parameters = new SerialPortParameters(ports[portIndex]);

// Create needed services
var reader = new SerialPultMessageReader();
var svc = new SerialCommunicator<SerialCommunicatorMessage>(reader);

// Subscribe to events
svc.MessageReceived += (sender, message) =>
{
    Console.WriteLine("Message received");
};

svc.MessageCorrupted += (sender, args) =>
{
    Console.WriteLine("Message corrupted");
};


// Start monitoring
svc.TryConnect(parameters);


while (true)
{
    var key = Console.ReadKey();

    if (key.Key == ConsoleKey.R)
    {
        var serialPultMsg = new SerialCommunicatorMessage("R;", 0, 0);

        svc.Send(serialPultMsg);
    }
    else if (key.Key == ConsoleKey.C)
    {
        var serialPultMsg = new SerialCommunicatorMessage("C;1/8;", 0, 0);

        svc.Send(serialPultMsg);
    }
    else
    {
        break;
    }
}

svc.TryDisconnect();
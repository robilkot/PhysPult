
using PhysPult_mediator;
using PhysPult_mediator.Communication;
using PhysPult_mediator.Messages;
using PhysPult_mediator.Messages.Readers;
using System.IO.Ports;


//var test = new SerialCommunicatorMessage("R;", 125, 111);
//var test2 = test.ToBytes().ToList();

// Choose active port
var ports = SerialPort.GetPortNames();

Console.WriteLine("Available ports:");
ports.Each((port, index) =>
{
    Console.WriteLine($"{index}: {port}");
});

Console.WriteLine("Input port index:");
var portIndex = int.Parse(Console.ReadLine()!);

var parameters = new ConnectionParameters(ports[portIndex]);

// Create needed services
var reader = new SerialPultMessageReader();
var svc = new CommunicationService<SerialCommunicatorMessage>(reader);

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


while(true)
{
    var key = Console.ReadKey();

    if(key.Key == ConsoleKey.R)
    {
        var serialPultMsg = new SerialCommunicatorMessage("R;", 125, 111);

        svc.Send(serialPultMsg);
    }
    else
    {
        break;
    }
}

svc.TryDisconnect();
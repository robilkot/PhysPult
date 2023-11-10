using PhysPult.Commands;
using System.IO.Ports;
using System.Management;

namespace PhysPult.Logic
{
    public class SerialList : IDisposable
    {
        public List<SerialPort> Ports = new();

        public SerialPort? ActivePort = null;
        public INotifier? Notifier = null;
        public ILogger? Logger = null;
        //public Invoker Invoker = new();

        public Action Refresh;
        public SerialList()
        {
            Refresh = RefreshComPortsList;

            StartDevicesEvenetsWatcher();
        }

        private void StartDevicesEvenetsWatcher()
        {
            var watcher = new ManagementEventWatcher();
            watcher.EventArrived += new EventArrivedEventHandler((object sender, EventArrivedEventArgs e) => Refresh());
            watcher.Query = new WqlEventQuery("SELECT * FROM Win32_DeviceChangeEvent WHERE EventType = 2 OR EventType = 3");
            watcher.Start();
        }
        public void SelectPort(string portName)
        {
            ActivePort = Ports.FirstOrDefault(p => p.PortName == portName);
        }
        public void Open()
        {
            TryExecute(ActivePort.Open);

            if (ActivePort.IsOpen)
            {
                Notifier?.Notify("Connection opened.");
            }
        }
        public void Close()
        {
            TryExecute(ActivePort.Close);

            if (!ActivePort.IsOpen)
            {
                Notifier?.Notify("Connection closed.");
            }
        }

        public void SetHue(byte hue)
        {
            TryExecute(() => ActivePort.Write(hue.ToString()));
        }

        private void TryExecute(Action action)
        {
            if (ActivePort == null)
            {
                Notifier?.Notify("No port is selected.");
                return;
            }

            try
            {
                action.Invoke();
            }
            catch (Exception ex)
            {
                Notifier?.Notify(ex.Message);
            }
        }
        private void RefreshComPortsList()
        {
            var newPorts = SerialPort.GetPortNames();

            Ports.RemoveAll(p => !newPorts.Contains(p.PortName));

            foreach (var portName in newPorts)
            {
                if (Ports.FirstOrDefault(p => p.PortName == portName) == null)
                {
                    AddPort(portName);
                }
            }

            Notifier?.Notify("Updated ports list.");

            if (ActivePort == null && Ports.Count > 0 || Ports.Count == 1)
            {
                ActivePort = Ports[0];
            }
        }

        private void AddPort(string portName)
        {
            SerialPort newPort = new()
            {
                BaudRate = 115200,
                ReadTimeout = 10,
                WriteTimeout = 10,
                PortName = portName
            };

            Ports.Add(newPort);
            newPort.DataReceived += new SerialDataReceivedEventHandler(ParseReceivedData);
        }

        private void ParseReceivedData(object sender, SerialDataReceivedEventArgs e)
        {
            var serialPort = sender as SerialPort;
            if (serialPort == null)
            {
                return;
            }

            // todo: split parser to another class?
            string receivedData = serialPort.ReadTo("\0");
            string[] tokens = receivedData.Split(';');

            Logger?.Log(tokens[1], (MessageTypes)tokens[0][0]);
        }

        public void Dispose()
        {
            foreach (var port in Ports)
            {
                port.Dispose();
            }
        }
    }
}

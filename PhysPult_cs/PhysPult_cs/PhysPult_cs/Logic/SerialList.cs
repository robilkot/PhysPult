using PhysPult.Commands;
using System.IO.Ports;
using System.Management;

namespace PhysPult.Logic
{
    public class SerialList
    {
        public List<SerialPort> Ports = new();

        private SerialPort? _activePort = null;
        public SerialPort? ActivePort {
            get
            {
                return _activePort;
            }
            set
            {
                _activePort = value;
                OnActivePortChanged();
            }
        }
        public INotifier? Notifier = null;
        public ILogger? Logger = null;

        public Action Refresh;
        public Action OnActivePortChanged;
        public SerialList()
        {
            Refresh = RefreshComPortsList;

            StartDevicesEventsWatcher();
        }

        private void StartDevicesEventsWatcher()
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

            OnActivePortChanged();
        }
        public void Close()
        {
            TryExecute(ActivePort.Close);

            if (!ActivePort.IsOpen)
            {
                Notifier?.Notify("Connection closed.");
            }

            OnActivePortChanged();
        }

        public void SetHue(byte hue)
        {
            TryExecute(() => ActivePort.Write($"h;{hue}"));
        }
        public void SetValue(byte value)
        {
            TryExecute(() => ActivePort.Write($"v;{value}"));
        }
        public void SetSensitity(float sensitivity)
        {
            TryExecute(() => ActivePort.Write($"s;{sensitivity}"));
        }
        public void RequestUpdate()
        {
            TryExecute(() => ActivePort.Write("r;"));
        }

        private void TryExecute(Action action)
        {
            if (ActivePort == null || action == null)
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
            if (sender is not SerialPort serialPort)
            {
                return;
            }

            try
            {
            // todo: split parser to another class?
            string receivedData = serialPort.ReadTo("\0");
            string[] tokens = receivedData.Split(';');

            Logger?.Log(tokens[1], (MessageTypes)tokens[0][0]);
            }
            catch(Exception ex)
            {
                Notifier?.Notify(ex.Message);
            }
        }
    }
}

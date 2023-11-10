using System.IO.Ports;
using System.Management;

namespace PhysPult.Logic
{
    public class SerialList : IDisposable
    {
        public List<SerialPort> Ports = new();

        public SerialPort? ActivePort = null;
        public INotifier? Notifier = null;

        public Action Update;
        public SerialList()
        {
            Update = UpdateComPortsList;

            StartComPortsWatcher();
            //Task.Run(StartComPortsWatcher);
        }

        private void StartComPortsWatcher()
        {
            var watcher = new ManagementEventWatcher();
            var query = new WqlEventQuery("SELECT * FROM Win32_DeviceChangeEvent WHERE EventType = 2 OR EventType = 3");
            watcher.EventArrived += new EventArrivedEventHandler((object sender, EventArrivedEventArgs e) => Update());
            watcher.Query = query;
            watcher.Start();
        }
        public void SelectPort(string portName)
        {
            ActivePort = Ports.FirstOrDefault(p => p.PortName == portName);
        }
        public void Open()
        {
            if (ActivePort == null)
            {
                Notifier?.Notify("No port is active");
                return;
            }

            try
            {
            ActivePort.Open();
            }
            catch (Exception ex)
            {
                Notifier?.Notify(ex.Message);
            }

            if (ActivePort.IsOpen)
            {
                Notifier?.Notify("Connection opened");
            }
        }
        public void Close()
        {
            if (ActivePort == null)
            {
                Notifier?.Notify("No port is active");
                return;
            }

            try
            {
                ActivePort.Close();
            }
            catch (Exception ex)
            {
                Notifier?.Notify(ex.Message);
            }

            if (!ActivePort.IsOpen)
            {
                Notifier?.Notify("Connection closed");
            }
        }
        private void UpdateComPortsList()
        {
            var newPorts = SerialPort.GetPortNames();

            Ports.RemoveAll(p => !newPorts.Contains(p.PortName));

            foreach (var portName in newPorts)
            {
                if (Ports.FirstOrDefault(p => p.PortName == portName) == null)
                {
                    Ports.Add(new()
                    {
                        BaudRate = 115200,
                        ReadTimeout = 500,
                        WriteTimeout = 500,
                        PortName = portName
                    });
                }
            }

            Notifier?.Notify("Updated COM ports list");

            if (ActivePort == null && Ports.Count > 0 || Ports.Count == 1)
            {
                ActivePort = Ports[0];
            }
        }

        public void Dispose()
        {
            foreach(var port in Ports)
            {
                port.Dispose();
            }
        }
    }
}

using System.IO.Ports;
using System.Management;

namespace PhysPult.Logic
{
    public class SerialList
    {
        public List<SerialPort> Ports = new();

        public SerialPort? ActivePort = null;
        public INotifier? Notifier = null;

        public Action Update;
        public SerialList()
        {
            Update = UpdateComPortsList;

            Task.Run(StartComPortsWatcher);
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
            ActivePort?.Open();
            Notifier?.Notify("Opened connection");
        }
        public void Close()
        {
            ActivePort?.Close();
            Notifier?.Notify("Closed connection");
        }
        public void UpdateComPortsList()
        {
            var newItems = SerialPort.GetPortNames();

            Ports.RemoveAll(p => !newItems.Contains(p.PortName));

            foreach (var item in newItems)
            {
                if (Ports.FirstOrDefault(p => p.PortName == item) == null)
                {
                    Ports.Add(new()
                    {
                        PortName = item,
                        BaudRate = 115200,
                        ReadTimeout = 5,
                        WriteTimeout = 5
                    });
                }
            }

            if (Ports.Count > 0)
            {
                if (ActivePort == null)
                {
                    ActivePort = Ports[0];
                }
            }
            else
            {
                ActivePort = null;
            }

            Notifier?.Notify("Updated COM ports list");
        }
    }
}

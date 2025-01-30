using PhysPult_mediator.Messages;
using PhysPult_mediator.Messages.Readers;
using System.IO.Ports;
using System.Management;

namespace PhysPult_mediator.Communication
{
    public class CommunicationService<T> where T : ISerialMessage
    {
        public event EventHandler<T>? MessageReceived = null;
        public event EventHandler? MessageCorrupted = null;

        private readonly ISerialReader<T> _reader;
        private SerialPort? _activePort = null;

        public CommunicationService(ISerialReader<T> reader)
        {
            _reader = reader;
            OnDeviceConfigurationChanged();
            StartDevicesEventsWatcher();
        }

        public (bool, Exception?) TryConnect(ConnectionParameters parameters)
        {
            SerialPort port = new()
            {
                PortName = parameters.PortName,
                BaudRate = parameters.BaudRate,
                DataBits = parameters.DataBits,
                StopBits = parameters.StopBits switch
                {
                    1 => StopBits.One,
                    1.5f => StopBits.OnePointFive,
                    2 => StopBits.Two,
                    _ => throw new NotImplementedException()
                },
            };

            try
            {
                port.Open();
            }
            catch (Exception ex)
            {
                return (false, ex);
            }

            _activePort = port;
            _activePort.DataReceived += OnReceiveData;

            _reader.MessageReceived += OnReaderMessageReceived!;
            _reader.MessageCorrupted += OnReaderMessageCorrupted!;

            return (true, null);
        }

        public (bool, IOException?) TryDisconnect()
        {
            if (_activePort is not null)
            {
                if (_activePort.IsOpen)
                {
                    try
                    {
                        _activePort.Close();
                    }
                    catch (IOException ex)
                    {
                        return (false, ex);
                    }
                    _activePort.Dispose();
                }

                _activePort.DataReceived -= OnReceiveData;
                _activePort = null;


                _reader.MessageReceived -= OnReaderMessageReceived!;
                _reader.MessageCorrupted -= OnReaderMessageCorrupted!;
            }

            return (true, null);
        }

        // Starts listening for device configuration changed events
        // https://learn.microsoft.com/en-us/windows/win32/cimwin32prov/win32-devicechangeevent
        private void StartDevicesEventsWatcher()
        {
#pragma warning disable CA1416 // Currently only running on windows
            var watcher = new ManagementEventWatcher();
            watcher.EventArrived += new EventArrivedEventHandler((sender, e) => OnDeviceConfigurationChanged());
            watcher.Query = new WqlEventQuery("SELECT * FROM Win32_DeviceChangeEvent WHERE EventType = 1");
            watcher.Start();
#pragma warning restore CA1416
        }

        private void OnDeviceConfigurationChanged()
        {
            if (_activePort is not null)
            {
                // If device was removed externally
                if (!_activePort.IsOpen)
                {
                    _activePort.DataReceived -= OnReceiveData;
                    _activePort = null;
                }
                return;
            }
        }

        private void OnReceiveData(object sender, SerialDataReceivedEventArgs e)
        {
            while (_activePort?.BytesToRead > 0)
            {
                var b = _activePort.ReadByte();

                // End of stream reached
                if (b == -1)
                    return;

                _reader.Next((byte)b);
            }
        }

        private void OnReaderMessageReceived(object sender, T message)
            => MessageReceived?.Invoke(sender, message);
        private void OnReaderMessageCorrupted(object sender, EventArgs args)
            => MessageCorrupted?.Invoke(sender, args);
    }
}

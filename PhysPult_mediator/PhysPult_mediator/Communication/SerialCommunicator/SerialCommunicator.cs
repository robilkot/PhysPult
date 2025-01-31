using PhysPult_mediator.Communication.SerialCommunicator.Messages;
using PhysPult_mediator.Communication.SerialCommunicator.Readers;
using System.IO.Ports;
using System.Management;

namespace PhysPult_mediator.Communication.SerialCommunicator
{
    public class SerialCommunicator<T> where T : ISerialMessage
    {
        public event EventHandler<T>? MessageReceived = null;
        public event EventHandler<T?>? MessageCorrupted = null;
        public bool Connected => _activePort?.IsOpen ?? false;

        private readonly ISerialReader<T> _reader;
        private SerialPort? _activePort = null;

        public SerialCommunicator(ISerialReader<T> reader)
        {
            _reader = reader;
            OnDeviceConfigurationChanged();
            StartDevicesEventsWatcher();
        }

        public (bool, Exception?) Send(T message)
        {
            if (_activePort is null)
            {
                // todo log
                return (false, null);
            }

            var bytes = message.ToBytes().ToArray();

            try
            {
                _activePort.Write(bytes, 0, bytes.Length);
                return (true, null);
            }
            catch (Exception ex)
            {
                return (false, ex);
            }
        }

        public (bool, Exception?) TryConnect(SerialPortParameters parameters)
        {
            SerialPort port = null!;
            try
            {
                port = new()
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

                port.Open();
            }
            catch (Exception ex)
            {
                return (false, ex);
            }

            _activePort = port;
            _activePort.ReceivedBytesThreshold = 1;
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

        // todo: reconnect handler
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
            if (!(_activePort?.IsOpen ?? false))
            {
                return;
            }

            var bytesToRead = _activePort.BytesToRead;
            byte[] buf = new byte[bytesToRead];
            _ = _activePort.Read(buf, 0, bytesToRead);
            foreach (var b in buf)
            {
                _reader.Next(b);
            }
        }

        private void OnReaderMessageReceived(object sender, T message)
            => MessageReceived?.Invoke(sender, message);
        private void OnReaderMessageCorrupted(object sender, T? args)
            => MessageCorrupted?.Invoke(sender, args);
    }
}

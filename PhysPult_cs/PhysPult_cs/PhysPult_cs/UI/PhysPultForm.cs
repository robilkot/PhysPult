using PhysPult.Logic;
using System.Reflection;
using System.Timers;

namespace PhysPult.UI
{
    public partial class PhysPultForm : Form
    {
        private readonly string _windowName = "Multicontrol";

        private StatusLabelNotifier _statusLabelHandler = new();
        private System.Timers.Timer _statusLabelUpdateTimer;

        private SerialList _ports = new();
        public PhysPultForm()
        {
            InitializeComponent();

            _statusLabelHandler.StatusLabel = statusLabel;

            _ports.Notifier = _statusLabelHandler;
            _ports.Logger = new DummyLogger();
            _ports.Logger.LogEvent += LogMessage;

            _statusLabelUpdateTimer = new()
            {
                Interval = 50,
                Enabled = true,
                AutoReset = true
            };
            _statusLabelUpdateTimer.Elapsed += new ElapsedEventHandler(
                (object? sender, ElapsedEventArgs e) => _statusLabelHandler.Update()
                );

            _ports.Refresh += UpdateComPortsComboBox;
            _ports.OnActivePortChanged += UpdateWindowTitle;

            UpdateComPortsList();
        }

        public void LogMessage(string message, MessageTypes messageTypes)
        {
            var logMessageFunc = () =>
            {
                comLogListBox.Items.Insert(0, $"{DateTime.Now.ToString("HH:mm:ss")}:\t {messageTypes} - {message}");
            };

            if (InvokeRequired)
            {
                Invoke(logMessageFunc);
            }
            else
            {
                logMessageFunc();
            }
        }

        private void UpdateWindowTitle()
        {
            var changeTextFunc = () =>
            {
                if (_ports.ActivePort == null)
                {
                    Text = _windowName;
                }
                else
                {
                    Text = String.Format("{0} ({1}) - {2}", _ports.ActivePort.PortName, _ports.ActivePort.IsOpen ? "Opened" : "Closed", _windowName);
                }
            };


            if (InvokeRequired)
            {
                Invoke(changeTextFunc);
            }
            else
            {
                changeTextFunc();
            }
        }
        public void UpdateComPortsComboBox()
        {
            var UpdateCOMPortsComboBoxFunc = () =>
            {
                var oldSelectedPort = comPortsComboBox.SelectedItem;

                comPortsComboBox.Items.Clear();
                foreach (var port in _ports.Ports)
                {
                    comPortsComboBox.Items.Add(port.PortName.ToString());
                }

                comPortsComboBox.SelectedItem = oldSelectedPort;

                if (oldSelectedPort == null && comPortsComboBox.Items.Count > 0 || comPortsComboBox.Items.Count == 1)
                {
                    comPortsComboBox.SelectedItem = comPortsComboBox.Items[0];
                }
            };

            if (InvokeRequired)
            {
                Invoke(UpdateCOMPortsComboBoxFunc);
            }
            else
            {
                UpdateCOMPortsComboBoxFunc();
            }
        }
        private void UpdateComPortsList()
        {
            _ports.Refresh();
        }
        private void connectButton_Click(object sender, EventArgs e)
        {
            _ports.Open();
        }

        private void disconnectButton_Click(object sender, EventArgs e)
        {
            _ports.Close();
        }

        private void comPortsComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            _ports.SelectPort((string)comPortsComboBox.SelectedItem);
        }

        private void setHueButton_Click(object sender, EventArgs e)
        {
            _ = byte.TryParse(hueTextBox.Text, out byte hue);
            _ports.SetHue(hue);
        }
        private void setHueButton_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                setHueButton_Click(this, new EventArgs());
                e.Handled = e.SuppressKeyPress = true;
            }
        }

        private void toggleDebugButton_Click(object sender, EventArgs e)
        {
            //_ports.RequestUpdate();
            _ports.ToggleDebug();
        }

        private void setValueButton_Click(object sender, EventArgs e)
        {
            _ = byte.TryParse(valueTextBox.Text, out byte value);
            _ports.SetValue(value);
        }

        private void valueTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                setValueButton_Click(this, new EventArgs());
                e.Handled = e.SuppressKeyPress = true;
            }
        }

        private void textBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                setSensitivityButton_Click(this, new EventArgs());
                e.Handled = e.SuppressKeyPress = true;
            }
        }

        private void setSensitivityButton_Click(object sender, EventArgs e)
        {
            _ = float.TryParse(sensitivityTextBox.Text, out float value);
            _ports.SetSensitity(value);
        }

        private void clearLogButton_Click(object sender, EventArgs e)
        {
            comLogListBox.Items.Clear();
        }
    }
}
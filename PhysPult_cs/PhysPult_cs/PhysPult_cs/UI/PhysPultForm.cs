using PhysPult.Logic;
using System.Timers;

namespace PhysPult.UI
{
    public partial class PhysPultForm : Form
    {
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
            _statusLabelUpdateTimer.Elapsed += new ElapsedEventHandler(_statusLabelHandler.Update);

            _ports.Refresh += UpdateComPortsComboBox;
        }

        public void LogMessage(string message, MessageTypes messageTypes)
        {
            logListBox.Items.Add($"{messageTypes} - {message}");
        }

        public void UpdateComPortsComboBox()
        {
            var oldSelectedPort = comPortsComboBox.SelectedItem; // Exception with threading?

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
        }
        private void UpdateComPortsList()
        {
            _ports.Refresh();
        }
        private void PhysPultForm_Load(object sender, EventArgs e)
        {
            UpdateComPortsList();
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
            byte hue = 0;
            _ = byte.TryParse(hueTextBox.Text, out hue);
            _ports.SetHue(hue);
        }
    }
}
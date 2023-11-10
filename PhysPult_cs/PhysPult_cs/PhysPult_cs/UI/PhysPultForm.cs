using PhysPult.Logic;
using System.Timers;

namespace PhysPult.UI
{
    public partial class PhysPultForm : Form
    {
        private StatusLabelHandler _statusLabelHandler = new();
        private System.Timers.Timer _statusLabelUpdateTimer;

        private SerialList _ports = new();
        public PhysPultForm()
        {
            InitializeComponent();

            _statusLabelHandler.StatusLabel = statusLabel;
            _ports.Notifier = _statusLabelHandler;

            _statusLabelUpdateTimer = new()
            {
                Interval = 50,
                Enabled = true,
                AutoReset = true
            };
            _statusLabelUpdateTimer.Elapsed += new ElapsedEventHandler(_statusLabelHandler.Update);

            _ports.Update += UpdateComPortsComboBox;
        }

        public void UpdateComPortsComboBox()
        {
            var oldSelectedPort = comPortsComboBox.SelectedItem; // Exception with threading

            comPortsComboBox.Items.Clear();
            foreach (var port in _ports.Ports)
            {
                comPortsComboBox.Items.Add(port.PortName.ToString());
            }

            comPortsComboBox.SelectedItem = oldSelectedPort;

            if(oldSelectedPort == null && comPortsComboBox.Items.Count > 0)
            {
                comPortsComboBox.SelectedItem = comPortsComboBox.Items[0];
            }
        }
        private void UpdateComPortsList()
        {
            _ports.Update();
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
            _ports.SelectPort(comPortsComboBox.SelectedText);
        }
    }
}
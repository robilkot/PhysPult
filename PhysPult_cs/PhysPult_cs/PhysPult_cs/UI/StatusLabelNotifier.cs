using System.Timers;

namespace PhysPult.UI
{
    public class StatusLabelNotifier : Logic.INotifier
    {
        private static readonly int s_messagesInterval = 1000;
        private static readonly string s_defaultStatus = "Waiting for actions...";

        private Queue<string> _messages = new();
        private ToolStripStatusLabel _statusLabel;
        public ToolStripStatusLabel StatusLabel { get => _statusLabel; set => _statusLabel = value; }
        private DateTime _lastUpdateTime = DateTime.Now;
        public void Notify(string message)
        {
            _messages.Enqueue(message);
        }
        public void Update(object? sender, ElapsedEventArgs e)
        {
            if ((DateTime.Now - _lastUpdateTime).TotalMilliseconds > s_messagesInterval)
            {
                if (_messages.Count > 0)
                {
                    _statusLabel.Text = _messages.Dequeue();
                    _lastUpdateTime = DateTime.Now;
                } else
                {
                    _statusLabel.Text = s_defaultStatus;
                }
            }
        }
    }
}

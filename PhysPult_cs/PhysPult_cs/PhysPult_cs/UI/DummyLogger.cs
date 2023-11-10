using PhysPult.Logic;

namespace PhysPult.UI
{
    public class DummyLogger : ILogger
    {
        public event Action<string, MessageTypes> LogEvent;
        //public ListBox? logListBox = null;
        //public DummyLogger() {
            //LogEvent += LogEventHandler;
        //}

        public void Log(string message, MessageTypes messageType = MessageTypes.Info)
        {
            LogEvent.Invoke(message, messageType);
        }
        //private void LogEventHandler(string message, MessageTypes messageType = MessageTypes.Info)
        //{
        //    if (logListBox == null)
        //    {
        //        return;
        //    }
        //    logListBox.Items.Add(message);
        //}
    }
}

namespace PhysPult.Logic
{
    public interface ILogger
    {
        public event Action<string, MessageTypes> LogEvent;
        public void Log(string message, MessageTypes messageType = MessageTypes.Info);
    }
}

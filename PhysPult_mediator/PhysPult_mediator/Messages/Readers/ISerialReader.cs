namespace PhysPult_mediator.Messages.Readers
{
    public interface ISerialReader<T> where T : ISerialMessage
    {
        void Next(byte Data);
        event EventHandler<T>? MessageReceived;
        event EventHandler? MessageCorrupted;
    }
}

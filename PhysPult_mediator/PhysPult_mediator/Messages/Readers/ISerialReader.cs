namespace PhysPult_mediator.Messages.Readers
{
    public interface ISerialReader<T> where T : ISerialMessage
    {
        void Next(byte read);
        event EventHandler<T>? MessageReceived;
        event EventHandler<T?>? MessageCorrupted;
    }
}

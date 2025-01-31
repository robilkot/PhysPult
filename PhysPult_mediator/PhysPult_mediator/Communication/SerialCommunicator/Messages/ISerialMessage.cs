namespace PhysPult_mediator.Communication.SerialCommunicator.Messages
{
    public interface ISerialMessage
    {
        IEnumerable<byte> ToBytes();
    }
}

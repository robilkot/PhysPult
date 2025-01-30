namespace PhysPult_mediator.Messages
{
    public interface ISerialMessage
    {
        IEnumerable<byte> ToBytes();
    }
}

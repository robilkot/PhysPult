namespace PhysPult_mediator.Communication
{
    public record ConnectionParameters(
        string PortName,
        int BaudRate = 2400,
        int DataBits = 8,
        float StopBits = 1f
        );
}

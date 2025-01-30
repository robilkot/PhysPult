namespace PhysPult_mediator.Communication
{
    public record ConnectionParameters(
        string PortName,
        int BaudRate = 115200,
        int DataBits = 8,
        float StopBits = 1f
        );
}

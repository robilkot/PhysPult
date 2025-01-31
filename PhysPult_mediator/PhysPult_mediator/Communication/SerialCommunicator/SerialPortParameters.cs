namespace PhysPult_mediator.Communication.SerialCommunicator
{
    // todo: configure
    public record SerialPortParameters(
        string PortName,
        int BaudRate = 115200,
        int DataBits = 8,
        float StopBits = 1f
        );
}

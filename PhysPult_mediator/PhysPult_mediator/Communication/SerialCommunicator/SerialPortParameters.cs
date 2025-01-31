namespace PhysPult_mediator.Communication.SerialCommunicator
{
    public class SerialPortParameters
    {
        public string PortName { get; set; } = string.Empty;
        public int BaudRate { get; set; } = 115200;
        public int DataBits { get; set; } = 8;
        public float StopBits { get; set; } = 1f;
    }
}

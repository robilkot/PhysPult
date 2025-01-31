namespace PhysPult_mediator
{
    public class WebsocketParameters
    {
        public TimeSpan Timeout { get; set; } = TimeSpan.FromMinutes(2);
        public int Port { get; set; } = 8080;
    } 
}

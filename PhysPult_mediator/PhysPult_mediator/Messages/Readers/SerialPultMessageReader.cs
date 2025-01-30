namespace PhysPult_mediator.Messages.Readers
{
    public class SerialPultMessageReader : ISerialReader<SerialPultMessage>
    {
        private List<byte> _inputBuffer = [];

        public event EventHandler<SerialPultMessage>? MessageReceived;
        public event EventHandler? MessageCorrupted;

        public void Next(byte data)
        {
            // todo
        }
        private void OnMessageCorrupted()
        {
            MessageCorrupted?.Invoke(this, EventArgs.Empty);
        }

        private void OnMessageCompleted()
        {
            SerialPultMessage msg = new(_inputBuffer);
            MessageReceived?.Invoke(this, msg);
        }
    }
}

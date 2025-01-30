namespace PhysPult_mediator.Messages.Readers
{
    public class SerialPultMessageReader : ISerialReader<SerialCommunicatorMessage>
    {
        private List<byte> _inputBuffer = [];
        private bool messageStarted;

        public event EventHandler<SerialCommunicatorMessage>? MessageReceived;
        public event EventHandler<SerialCommunicatorMessage?>? MessageCorrupted;

        public void Next(byte read)
        {
            char ch = (char)read;
            switch (read)
            {
                case SerialCommunicatorMessage.StartByte:
                    {
                        _inputBuffer.Add(read);
                        if (messageStarted)
                        {
                            // todo log ("second message start byte received before message end");
                            _inputBuffer.Clear();
                        }
                        messageStarted = true;
                        break;
                    }
                case SerialCommunicatorMessage.StopByte:
                    {
                        _inputBuffer.Add(read);
                        if (!messageStarted)
                        {
                            // todo log ("message end byte received before message start");
                            _inputBuffer.Clear();
                        }
                        messageStarted = false;

                        var msg = new SerialCommunicatorMessage(_inputBuffer);

                        DeincapsulateMessage(msg);
                        _inputBuffer.Clear();
                        break;
                    }
                default:
                    {
                        Console.Write(ch);
                        if (messageStarted)
                        {
                            _inputBuffer.Add(read);
                        }
                        break;
                    }
            }
        }
        private void DeincapsulateMessage(SerialCommunicatorMessage msg)
        {
            if(msg.Valid)
            {
                MessageReceived?.Invoke(this, msg);
            }
            else
            {
                MessageCorrupted?.Invoke(this, msg);
            }
        }
    }
}

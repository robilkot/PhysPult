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
            switch (read)
            {
                case SerialCommunicatorMessage.StartByte:
                    {
                        if (messageStarted)
                        {
                            Console.WriteLine("second message start byte received before message end");
                            // todo log ();
                            _inputBuffer.Clear();
                        }
                        _inputBuffer.Add(read);
                        messageStarted = true;
                        break;
                    }
                case SerialCommunicatorMessage.StopByte:
                    {
                        if(_inputBuffer.Count < 13)
                        {
                            _inputBuffer.Add(read); // 'stop byte' is part of crc or seq or ack
                            // todo: log this
                        }
                        else
                        {
                            if (!messageStarted)
                            {
                                Console.WriteLine("message end byte received before message start");
                                // todo log ("message end byte received before message start");
                                _inputBuffer.Clear();
                            }
                            _inputBuffer.Add(read);
                            messageStarted = false;

                            var msg = new SerialCommunicatorMessage(_inputBuffer);

                            DeincapsulateMessage(msg);
                            _inputBuffer.Clear();
                        }
                        break;
                    }
                default:
                    {
                        Console.Write((char)read);
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

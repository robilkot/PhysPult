using PhysPult_mediator.Communication.SerialCommunicator.Messages;

namespace PhysPult_mediator.Communication.SerialCommunicator.Readers
{
    public class SerialPultMessageReader : ISerialReader<SerialCommunicatorMessage>
    {
        private List<byte> _inputBuffer = [];

        public event EventHandler<SerialCommunicatorMessage>? MessageReceived;
        public event EventHandler<SerialCommunicatorMessage?>? MessageCorrupted;

        public void Next(byte read)
        {
            void resetInputBuffer()
            {
                _inputBuffer.Clear();
            }

            switch (read)
            {
                case SerialCommunicatorMessage.StartByte:
                    {
                        _inputBuffer.Add(read);
                        break;
                    }
                case SerialCommunicatorMessage.StopByte:
                    {
                        if (_inputBuffer.Count == 0)
                        {
                            resetInputBuffer();
                        }
                        else
                        {
                            _inputBuffer.Add(read);

                            var contentBytes = _inputBuffer.Count - 18;
                            if(contentBytes >= 0) // I do not expect stop byte in content
                            {
                                var msg = new SerialCommunicatorMessage(_inputBuffer);

                                DeincapsulateMessage(msg);
                                resetInputBuffer();
                            }
                        }
                        break;
                    }
                default:
                    {
                        if (_inputBuffer.Count > 0)
                        {
                            _inputBuffer.Add(read);
                        }
                        else
                        {
                            // Reading debug or other stuff from port
                            Console.Write((char)read);
                        }
                        break;
                    }
            }
        }
        private void DeincapsulateMessage(SerialCommunicatorMessage msg)
        {
            if (msg.Valid)
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

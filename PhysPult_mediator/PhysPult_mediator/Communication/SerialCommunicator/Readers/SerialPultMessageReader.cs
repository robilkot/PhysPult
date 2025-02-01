using PhysPult_mediator.Communication.SerialCommunicator.Messages;

namespace PhysPult_mediator.Communication.SerialCommunicator.Readers
{
    public class SerialPultMessageReader : ISerialReader<SerialCommunicatorMessage>
    {
        private List<byte> _inputBuffer = [];
        private UInt32? _expectedContentLength = null;

        public event EventHandler<SerialCommunicatorMessage>? MessageReceived;
        public event EventHandler<SerialCommunicatorMessage?>? MessageCorrupted;

        public void Next(byte read)
        {
            void resetInputBuffer()
            {
                _inputBuffer.Clear();
                _expectedContentLength = null;
            }
            
            // If content length is read
            if(_inputBuffer.Count == 17)
            {
                var readLength = BitConverter.ToUInt32(_inputBuffer.Skip(13).Take(4).ToArray());
                _expectedContentLength = readLength <= 255 ? readLength : null;
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
                            // If expected number was corrupter or we've reached length, reset
                            //if(_expectedContentLength is null || contentBytes >= _expectedContentLength)
                            {
                                var msg = new SerialCommunicatorMessage(_inputBuffer);

                                DeincapsulateMessage(msg);
                                resetInputBuffer();
                            } else
                            {
                                Console.WriteLine($"contentBytes: {contentBytes} < {_expectedContentLength}");
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

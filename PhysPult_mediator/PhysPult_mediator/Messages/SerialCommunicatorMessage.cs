using System.Text;
using System.IO.Hashing;
using System.Runtime.InteropServices;

namespace PhysPult_mediator.Messages
{
    // PDU format:
    // -------------------------------------------------------------------------------
    // |      1      |   4   |      4      |      4      |     ...     |      1      |
    // -------------------------------------------------------------------------------
    // | 0b01111110  | crc32 | seq. number | ack. number |   content   | 0b10000001  |
    // -------------------------------------------------------------------------------
    // CRC is calculated started from seq. number, finishing with content. Start and stop bits are not used for CRC.
    public class SerialCommunicatorMessage : ISerialMessage
    {
        private UInt32 crc;

        private UInt32 GetCrc32()
        {
            var bytes = ToBytes().Skip(5).SkipLast(1).ToList(); // todo: slice
            var crc32 = Crc32.HashToUInt32(CollectionsMarshal.AsSpan(bytes));
            return crc32;
        }

        public bool Valid { get; private set; }
        public UInt32 SequenceNumber { get; private set; }
        public UInt32 AckNumber { get; private set; }
        public string Content { get; private set; }

        public const byte StartByte = 0b01111110;
        public const byte StopByte = 0b10000001;

        public SerialCommunicatorMessage(IList<byte> bytes)
        {
            if (bytes.Count < 14)
            {
                throw new Exception("invalid serial message: less than 12 bytes");
            }

            StringBuilder sb = new(150);

            try
            {
                var crcBytes = CollectionsMarshal.AsSpan(bytes.Skip(1).Take(4).ToList());
                crc = BitConverter.ToUInt32(crcBytes);

                var seqBytes = CollectionsMarshal.AsSpan(bytes.Skip(5).Take(4).ToList());
                SequenceNumber = BitConverter.ToUInt32(seqBytes);

                var ackBytes = CollectionsMarshal.AsSpan(bytes.Skip(9).Take(4).ToList());
                AckNumber = BitConverter.ToUInt32(ackBytes);

                for (var i = 13; i < bytes.Count - 1; i++)
                {
                    sb.Append((char)bytes[i]);
                }

                Content = sb.ToString();
            }
            catch (Exception ex)
            {
                throw new Exception($"invalid serial message: {ex.Message}");
            }

            Valid = crc == GetCrc32();
        }
        public SerialCommunicatorMessage(string content, UInt32 sequence, UInt32 ack)
        {
            Content = content;
            SequenceNumber = sequence;
            AckNumber = ack;

            crc = GetCrc32();
            Valid = true;
        }

        public IEnumerable<byte> ToBytes()
        {
            yield return StartByte;

            var crcBytes = BitConverter.GetBytes(crc);
            for (int i = 0; i < 4; i++)
            {
                yield return crcBytes[i];
            }

            var seqBytes = BitConverter.GetBytes(SequenceNumber);
            for (int i = 0; i < 4; i++)
            {
                yield return seqBytes[i];
            }

            var ackBytes = BitConverter.GetBytes(AckNumber);
            for (int i = 0; i < 4; i++)
            {
                yield return ackBytes[i];
            }

            foreach (var c in Content)
            {
                yield return (byte)c;
            }

            yield return StopByte;
        }
    }
}
using System.Text;
using System.IO.Hashing;
using System.Runtime.InteropServices;

namespace PhysPult_mediator.Communication.SerialCommunicator.Messages
{
    // PDU format:
    // -----------------------------------------------------------------------------------------------------------------
    // |      1      |      4      |      4      |      4      |      3      |      1      |   0..255    |      1      |
    // -----------------------------------------------------------------------------------------------------------------
    // |  B00000010  |    crc32    | seq. number | ack. number |      0      |  c. length  |   content   |  B00000011  |
    // -----------------------------------------------------------------------------------------------------------------
    // CRC32 is calculated started for seq. number, ack. number, c. length and content
    public class SerialCommunicatorMessage
    {
        private uint crc;
        public uint Crc => crc;

        private uint GetCrc32()
        {
            var bytes = ToBytes().Skip(5).SkipLast(1).ToList(); // todo: slice
            var crc32 = Crc32.HashToUInt32(CollectionsMarshal.AsSpan(bytes));
            return crc32;
        }

        public bool Valid { get; private set; }
        public uint SequenceNumber { get; private set; }
        public uint AckNumber { get; private set; }
        public uint ContentLength { get; private set; }
        public string Content { get; private set; } = string.Empty;

        public const byte StartByte = 0b00000010;
        public const byte StopByte = 0b00000011;

        public SerialCommunicatorMessage(IList<byte> bytes)
        {
            if (bytes.Count < 18)
            {
                // todo log
                Console.WriteLine("invalid serial message: less than 18 bytes");
                return;
            }

            if (bytes[0] != StartByte)
            {
                Console.WriteLine("invalid serial message: invalid start byte");
                return;
            }
            if(bytes[bytes.Count - 1] != StopByte)
            {
                Console.WriteLine("invalid serial message: invalid stop byte");
                return;
            }

            var crcBytes = CollectionsMarshal.AsSpan(bytes.Skip(1).Take(4).ToList());
            crc = BitConverter.ToUInt32(crcBytes);

            var seqBytes = CollectionsMarshal.AsSpan(bytes.Skip(5).Take(4).ToList());
            SequenceNumber = BitConverter.ToUInt32(seqBytes);

            var ackBytes = CollectionsMarshal.AsSpan(bytes.Skip(9).Take(4).ToList());
            AckNumber = BitConverter.ToUInt32(ackBytes);

            var lenBytes = CollectionsMarshal.AsSpan(bytes.Skip(13).Take(4).ToList());
            ContentLength = BitConverter.ToUInt32(lenBytes);

            var contentBytes = bytes.Skip(17).SkipLast(1).ToArray();
            Content = Encoding.UTF8.GetString(contentBytes);

            Valid = crc == GetCrc32();
        }
        public SerialCommunicatorMessage(string content, uint sequence, uint ack)
        {
            Content = content;
            SequenceNumber = sequence;
            AckNumber = ack;
            ContentLength = (uint)content.Length;

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

            var lenBytes = BitConverter.GetBytes(ContentLength);
            for (int i = 0; i < 4; i++)
            {
                yield return lenBytes[i];
            }

            foreach (var c in Content)
            {
                yield return (byte)c;
            }

            yield return StopByte;
        }
    }
}
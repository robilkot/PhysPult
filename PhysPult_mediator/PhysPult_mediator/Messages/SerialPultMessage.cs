using System.Text;
using System.IO.Hashing;
using System.Runtime.InteropServices;

namespace PhysPult_mediator.Messages
{
    public class SerialPultMessage : ISerialMessage
    {
        private UInt32 crc;

        private UInt32 GetCrc32()
        {
            var bytes = ToBytes().Skip(4).ToList();
            var crc32 = Crc32.HashToUInt32(CollectionsMarshal.AsSpan(bytes));
            return crc32;
        }

        public bool Valid { get; private set; }
        public UInt32 SequenceNumber { get; private set; }
        public UInt32 AckNumber { get; private set; }
        public PultMessage PultMessage { get; private set; }

        public SerialPultMessage(IList<byte> bytes)
        {
            if (bytes.Count < 12)
            {
                throw new Exception("invalid serial message: less than 12 bytes");
            }

            StringBuilder sb = new(150);

            try
            {
                for (var i = 0; i < 4; i++)
                {
                    crc |= (UInt32) bytes[i] << (8 * (3 - i));
                }
                for (var i = 0; i < 4; i++)
                {
                    SequenceNumber |= (UInt32) bytes[i + 4] << (8 * (3 - i));
                }
                for (var i = 0; i < 4; i++)
                {
                    AckNumber |= (UInt32) bytes[i + 8] << (8 * (3 - i));
                }

                for (var i = 12; i < bytes.Count; i++)
                {
                    sb.Append((char)bytes[i]);
                }

                PultMessage = new PultMessage(sb.ToString());
            }
            catch (Exception ex)
            {
                throw new Exception($"invalid serial message: {ex.Message}");
            }

            Valid = crc == GetCrc32();
        }
        public SerialPultMessage(PultMessage pultMessage, UInt32 sequence, UInt32 ack)
        {
            PultMessage = pultMessage;
            SequenceNumber = sequence;
            AckNumber = ack;

            crc = GetCrc32();
            Valid = true;
        }

        public IEnumerable<byte> ToBytes()
        {
            for (var i = 3; i >= 0; i--)
            {
                yield return (byte)((crc >> (i * 8)) & 255);
            }
            for (var i = 3; i >= 0; i--)
            {
                yield return (byte)((SequenceNumber >> (i* 8)) & 255);
            }
            for (var i = 3; i >= 0; i--)
            {
                yield return (byte)((AckNumber >> (i* 8)) & 255);
            }
            foreach (var c in PultMessage.Message)
            {
                yield return (byte)c;
            }
        }
    }
}
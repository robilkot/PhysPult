﻿using PhysPult_mediator.Communication.SerialCommunicator.Messages;

namespace PhysPult_mediator.Communication.SerialCommunicator.Readers
{
    public interface ISerialReader<T>
    {
        void Next(byte read);
        event EventHandler<T>? MessageReceived;
        event EventHandler<T?>? MessageCorrupted;
    }
}

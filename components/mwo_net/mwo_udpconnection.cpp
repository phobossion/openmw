#include "mwo_udpconnection.hpp"
#include "mwo_corepackets.hpp"

#if MWO_UNIX
#include <sys/times.h>
#include <time.h>
#endif

namespace MWOnline
{
    // [TEMP] helper to return timeGetTime()
    unsigned int GetTime()
    {
#if MWO_UNIX
        tms t;
        clock_t time = times(&t);
        long ticksPerSec = sysconf(_SC_CLK_TCK);
        unsigned int fract = 1000 / ticksPerSec;
        unsigned int milis = time * fract;

        return milis;
#else
        return (int)timeGetTime();
#endif
    }

    //
    UDPConnection::UDPConnection(const Endpoint &local, const Endpoint& remote, const Packet* intro)
        : Connection(SocketType_UDP, local, remote), mLastInPacketTime(GetTime()), mLastOutPacketTime(GetTime()),
          mLocalSequenceNumber(1), mRemoteSequenceNumber(0), mRemoteSequenceBitfield(0), mLastPacketConfirmedToRemote(0),
          mLastPacketConfirmationTime(GetTime())
    {
        // 'intro' will be null on client!!!
        if (intro)
        {
            // Handle it as normal incoming packet
            ReceivePacketInternal((const Datagram*)intro, remote);
        }
    }

    //
    void UDPConnection::Start()
    {
        SetState(Connection_Connected);

        // Infinite look that continuously polls the local socket for incoming data and
        // sends queued packets to the remote host
        while (mSocket.IsOpen())
        {
            // Receive data from remote host
            ProcessIncomingData();

            // Process outbound queue
            ProcessOutgoingData();

            // Keep the connection alive
            KeepAlive();

            // Detect ending state
            if (mState == Connection_Disconnected)
            {
                break;
            }

            // Avoid stalling the OS
#if MWO_UNIX
            usleep(100 * 1000);
#else
            Sleep(100);
#endif

        }
    }

    //
    void UDPConnection::ProcessIncomingData()
    {
        while (mSocket.HasWaitingData())
        {
            char data[4196]; // [TODO] max packet size???

            Endpoint remoteHost;
            int ret = mSocket.ReceiveFrom(&data, sizeof(data), remoteHost);
            if (ret <= 0)
            {
                // All data received or an error was encountered
                break;
            }

            // Check if we have a standard MWO packet inbound
            // [TODO] we should also check if it came from the right host!!!
            Packet* p = (Packet*)data;
            if (Packet::IsValid(data))
            {
                MWO_TRACE("[UDP Connection] Received packet " << Packet::PacketCodeToName(p->Code) << " from " << remoteHost.ToString());

                // We are on UDP connection so it has to be a UDP datagram
                ReceivePacketInternal((const Datagram*)p, remoteHost);
            }
            else
            {
                // At least write out a warning
                MWO_TRACE("[UDP Connection] Received unknown data from " << remoteHost.ToString());
            }
        }
    }

    //
    void UDPConnection::ReceivePacketInternal(const Datagram* packet, const Endpoint& from)
    {
        using namespace Packets;

        // Update timestamp
        mLastInPacketTime = GetTime();

        // Handle reliability
        if (packet->Flags & Packet::Flag_Reliable)
        {
            boost::mutex::scoped_lock lock(mDataLock);

            const ReliablePacket* rp = (const ReliablePacket*)packet;

            // Take note of packet's sequence number - we'll have to send it back as confirmation
            if (rp->SequenceNumber > mRemoteSequenceNumber)
            {
                // This is the most recent packet received
                if (mRemoteSequenceNumber > 0)
                {
                    // Not the first packet received
                    unsigned int diff = rp->SequenceNumber - mRemoteSequenceNumber;
                    mRemoteSequenceBitfield <<= diff;
                    mRemoteSequenceBitfield |= (0x01 << (diff - 1));
                }
                mRemoteSequenceNumber = rp->SequenceNumber;
            }
            else if (rp->SequenceNumber < mRemoteSequenceNumber)
            {
                unsigned int diff = mRemoteSequenceNumber - rp->SequenceNumber;
                mRemoteSequenceBitfield |= (0x01 << (diff - 1));
            }

            // If we have many delivery confirmations pending, send back a response
            if (mRemoteSequenceNumber - mLastPacketConfirmedToRemote > 8 || // [TODO] Magical number, should be based on some heuristic
                GetTime() - mLastPacketConfirmationTime > 500)
            {
                POKE::ReliablePacket response;
                SendReliablePacket(&response, sizeof(response));
            }

            // Check if the incoming packet contains response IDs of any packets awaiting confirmation
            if (rp->Ack != 0)
            {
                for (int i = 0; i < 33; ++i)
                {
                    unsigned int ack = rp->Ack - i;
                    if (i == 0 || (rp->AckBitfield & (0x01 << (i - 1))))
                    {
                        std::map<unsigned int, PacketData>::iterator it = mUndeliveredPackets.find(ack);
                        if (it != mUndeliveredPackets.end())
                        {
                            // Packet was delivered!
                            MWO_TRACE("[UDP Connection] Remote host " << from.ToString() << " confirmed delivery of packet ID " << ack <<
                                      "(" << Packet::PacketCodeToName(((const Packet*)&it->second.Buffer)->Code) << ")");

                            mUndeliveredPackets.erase(it);
                        }
                    }

                    if (ack == 1)
                        break; // There cannot be more packets in the map anyway
                }
            }
        }

        // First check for special system packets
        if (INTR::ReliablePacket::IsValid(packet))
        {
            // Send back the ACPT packet
            ACPT::ReliablePacket response;
            SendReliablePacket(&response, sizeof(response));

            return;
        }
        else if (ACPT::ReliablePacket::IsValid(packet))
        {
            // Callback to the user (client connections hook onto this)
            ConnectionAccepted((const ReliablePacket*)packet, from);

            return;
        }

        // Unknown packet, give the user a chance to react
        if (!ReceivePacket(packet, from))
        {
            MWO_TRACE("[UDP Connection] Throwing away unhandled packet " << Packet::PacketCodeToName(packet->Code) << " from " << from.ToString());
        }
    }

    //
    void UDPConnection::ProcessOutgoingData()
    {
        boost::mutex::scoped_lock lock(mDataLock);

        if (!mOutboundQueue.empty())
        {
            // Update timestamp
            mLastOutPacketTime = GetTime();

            // Send all queued packets to the destination
            for (std::vector<PacketData>::iterator it = mOutboundQueue.begin(); it != mOutboundQueue.end(); ++it)
            {
                mSocket.Send(&it->Buffer, it->Size, mRemoteEndpoint);
            }

            mOutboundQueue.clear();
        }
    }

    //
    void UDPConnection::SendPacket(Datagram* packet, int packetSize)
    {
        boost::mutex::scoped_lock lock(mDataLock);

        // Just enqueue
        mOutboundQueue.push_back(PacketData());
        PacketData& data = mOutboundQueue.back();

        memcpy(&data.Buffer, packet, packetSize);
        data.Size = packetSize;
    }


    //
    void UDPConnection::SendReliablePacket(ReliablePacket* packet, int packetSize)
    {
        boost::mutex::scoped_lock lock(mDataLock);

        // Set local packet ID for confirmation
        packet->SequenceNumber = mLocalSequenceNumber++;

        // Confirm delivery of recent remote packets
        packet->Ack = mRemoteSequenceNumber;
        packet->AckBitfield = mRemoteSequenceBitfield;

        mLastPacketConfirmedToRemote = mRemoteSequenceNumber;
        mLastPacketConfirmationTime = GetTime();

        // Enqueue
        mOutboundQueue.push_back(PacketData());
        PacketData& data = mOutboundQueue.back();

        memcpy(&data.Buffer, packet, packetSize);
        data.Size = packetSize;

        // Store in cache for reliable packets
        mUndeliveredPackets[packet->SequenceNumber] = PacketData();
        PacketData& data2 = mUndeliveredPackets[packet->SequenceNumber];
        memcpy(&data2.Buffer, packet, packetSize);
        data2.Size = packetSize;
    }

    //
    void UDPConnection::KeepAlive()
    {
        unsigned int time = GetTime();

        // Check if the remote host is still communicating
        if (time - mLastInPacketTime > 10000)
        {
            SetState(Connection_Disconnected);
        }
        else if (time - mLastInPacketTime > 3000)
        {
            SetState(Connection_Stale);
        }
        else
        {
            // Remote host is on-line, we should make sure to respond!
            if (time - mLastOutPacketTime > 1000)
            {
                // [TODO] we will want to send any packet receipts here as well
                Packets::POKE::Datagram poke;
                SendPacket(&poke, sizeof(poke));
            }
        }
    }
}

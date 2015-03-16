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
        : Connection(SocketType_UDP, local, remote), mLastInPacketTime(GetTime()), mLastOutPacketTime(GetTime())
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
            // [TODO] set the connection state to 'connected'

            // Callback to the user (client connections hook onto this)
            ConnectionAccepted((const ReliablePacket*)packet, from);

            return;
        }

        // [TODO] handle reliability

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
    void UDPConnection::SendPacket(const Datagram* packet, int packetSize)
    {
        boost::mutex::scoped_lock lock(mDataLock);

        // Just enqueue
        mOutboundQueue.push_back(PacketData());
        PacketData& data = mOutboundQueue.back();

        memcpy(&data.Buffer, packet, packetSize);
        data.Size = packetSize;
    }


    //
    void UDPConnection::SendReliablePacket(const ReliablePacket* packet, int packetSize)
    {
        boost::mutex::scoped_lock lock(mDataLock);

        // [TODO] Add reliability fields

        // Enqueue
        mOutboundQueue.push_back(PacketData());
        PacketData& data = mOutboundQueue.back();

        memcpy(&data.Buffer, packet, packetSize);
        data.Size = packetSize;
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

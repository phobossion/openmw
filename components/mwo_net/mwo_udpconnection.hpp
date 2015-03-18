#ifndef MWO_NET_UDPCONNECTION
#define MWO_NET_UDPCONNECTION

#include "mwo_connection.hpp"

#include <boost/thread.hpp>

#include <vector>
#include <map>
#include <set>

namespace MWOnline
{
    struct Packet;
    struct Datagram;
    struct ReliablePacket;
    class Endpoint;

    /// Class that wraps a virtual connection over the UDP protocol
    class UDPConnection : public Connection
    {
    public:
        UDPConnection(const Endpoint& local, const Endpoint& remote, const Packet* intro);

        virtual void Start(); // final

        /// Send an unreliable UDP datagram
        void SendPacket(Datagram* packet, int packetSize);

        /// Send a reliable UDP packet
        /// The connection will ensure that the packet arrives to the destination at least once
        void SendReliablePacket(ReliablePacket* packet, int packetSize);

    protected:
        /// User code hook for incoming packets
        virtual bool ReceivePacket(const Datagram* packet, const Endpoint& from) = 0;

        /// Callback to user when his connection has been accepted by the remote host
        virtual void ConnectionAccepted(const ReliablePacket* /*acpt*/, const Endpoint& /*from*/) {}

    private:
        /// Receive all data waiting on the local socket and process it
        void ProcessIncomingData();
        /// Send all data wating in the outbound queue
        void ProcessOutgoingData();
        /// Keep the connection alive by sending an occasional POKE packet
        void KeepAlive();

        /// Packet received callback
        void ReceivePacketInternal(const Datagram* packet, const Endpoint& from);

        /// Data lock
        boost::recursive_mutex mDataLock;

        struct PacketData
        {
            char Buffer[4196]; // [TODO] max packet size???
            int Size;
            unsigned int Timestamp;
        };

        /// Data queued to send
        std::vector<PacketData> mOutboundQueue;

        // Timestamps
        unsigned int mLastInPacketTime;
        unsigned int mLastOutPacketTime;

        /// Local sequence number for packet reliability
        unsigned int mLocalSequenceNumber;

        /// Reliable packets that were not yet confirmed delivered
        std::map<unsigned int, PacketData> mUndeliveredPackets;

        /// Highest remote sequence number from received packets
        unsigned int mRemoteSequenceNumber;
        /// Bitfield used for packet confirmation
        unsigned int mRemoteSequenceBitfield;
        /// Last remote packet sequence number we have confirmed to the remote host
        unsigned int mLastPacketConfirmedToRemote;
        /// Time of last confirmation response
        unsigned int mLastPacketConfirmationTime;
    };
}

#endif

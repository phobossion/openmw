#include "mwo_udpclientconnection.hpp"
#include "mwo_corepackets.hpp"

namespace MWOnline
{
    //
    UDPClientConnection::UDPClientConnection(const Endpoint& local, const Endpoint& initialRemote)
        : UDPConnection(local, initialRemote, NULL)
    {
    }

    //
    void UDPClientConnection::Initiate()
    {
        using namespace Packets;

        // Send the intro packet
        INTR::ReliablePacket packet;
        SendReliablePacket(&packet, sizeof(packet));

        // And start the connection
        Start();

        // [TODO] once the response arrives, we'll need to change the remote endpoint to the sender!
    }

    //
    void UDPClientConnection::ConnectionAccepted(const ReliablePacket* /*acpt*/, const Endpoint& from)
    {
        // Set the remote host to the new address
        mRemoteEndpoint = from;
    }
}

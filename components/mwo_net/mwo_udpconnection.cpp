#include "mwo_udpconnection.hpp"

namespace MWOnline
{
    //
    UDPConnection::UDPConnection(const Endpoint &local, const Endpoint &remote, const Packet* /*intro*/)
        : Connection(SocketType_UDP, local, remote)
    {
    }

    //
    void UDPConnection::Start()
    {
        // ???
    }
}

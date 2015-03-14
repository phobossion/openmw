#ifndef MWO_NET_UDPCONNECTION
#define MWO_NET_UDPCONNECTION

#include "mwo_connection.hpp"

namespace MWOnline
{
    struct Packet;
    class Endpoint;

    /// Class that wraps a virtual connection over the UDP protocol
    class UDPConnection : public Connection
    {
    public:
        UDPConnection(const Endpoint& local, const Endpoint& remote, const Packet* intro);

        virtual void Start(); // final
    };
}

#endif

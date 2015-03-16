#ifndef MWO_NET_UDPCLIENTCONNECTION
#define MWO_NET_UDPCLIENTCONNECTION

#include "mwo_udpconnection.hpp"

namespace MWOnline
{
    /// Client-side UDP connection
    /// This just adds some supporting functionality to the basic UDP connection class
    class UDPClientConnection : public UDPConnection
    {
    public:
        UDPClientConnection(const Endpoint& local, const Endpoint& initialRemote);

        /// Initiate the connection by sending the INTR packet to the server
        void Initiate();

    protected:
        virtual void ConnectionAccepted(const ReliablePacket* acpt, const Endpoint& from); // override
    };
}

#endif

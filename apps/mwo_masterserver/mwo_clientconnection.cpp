#include "mwo_clientconnection.hpp"

namespace MWOnline
{
    namespace MasterServer
    {
        //
        ClientConnection::ClientConnection(const Endpoint &local, const Endpoint &remote, const Packet* intro)
            : UDPConnection(local, remote, intro)
        {
        }

        //
        boost::shared_ptr<UDPConnection> MasterServerConnectionFactory::CreateConnection(const Endpoint &local, const Endpoint &remote, const Packet* intro) const
        {
            return boost::shared_ptr<UDPConnection>(new ClientConnection(local, remote, intro));
        }
    }
}

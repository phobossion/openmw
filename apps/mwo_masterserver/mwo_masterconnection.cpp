#include "mwo_masterconnection.hpp"

namespace MWOnline
{
    namespace MasterServer
    {
        //
        MasterConnection::MasterConnection(const Endpoint &local, const Endpoint &remote, const Packet* intro)
            : UDPConnection(local, remote, intro)
        {
        }

        //
        bool MasterConnection::ReceivePacket(const Datagram* /*packet*/, const Endpoint& /*from*/)
        {
            return false;
        }

        //
        boost::shared_ptr<UDPConnection> MasterServerConnectionFactory::CreateConnection(const Endpoint &local, const Endpoint &remote, const Packet* intro) const
        {
            return boost::shared_ptr<UDPConnection>(new MasterConnection(local, remote, intro));
        }
    }
}

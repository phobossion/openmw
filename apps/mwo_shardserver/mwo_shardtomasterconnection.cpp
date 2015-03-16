#include "mwo_shardtomasterconnection.hpp"

namespace MWOnline
{
    namespace ShardServer
    {
        //
        ShardToMasterConnection::ShardToMasterConnection(const Endpoint &local, const Endpoint &initialRemote)
            : UDPClientConnection(local, initialRemote)
        {
        }

        //
        bool ShardToMasterConnection::ReceivePacket(const Datagram* packet, const Endpoint& from)
        {
            return false;
        }
    }
}

#ifndef MWO_SHARDSERVER_SHARDTOMASTERCONNECTION
#define MWO_SHARDSERVER_SHARDTOMASTERCONNECTION

#include <components/mwo_net/mwo_udpclientconnection.hpp>

namespace MWOnline
{
    namespace ShardServer
    {
        /// Connection between the shard and the master server
        class ShardToMasterConnection : public UDPClientConnection
        {
        public:
            ShardToMasterConnection(const Endpoint& local, const Endpoint& initialRemote);

        private:
            virtual bool ReceivePacket(const Datagram* packet, const Endpoint& from); // override
        };
    }
}

#endif

#ifndef MWO_SHARDSERVER_SHARDSERVER
#define MWO_SHARDSERVER_SHARDSERVER

#include <components/mwo_net/mwo_service.hpp>
#include <components/mwo_net/mwo_udpclient.hpp>

namespace MWOnline
{
    namespace ShardServer
    {
        /// Shard ("world") server implementation
        class ShardServer : public Service
        {
        public:
            ShardServer();

            /// Run the server
            /// This basically just loops infinitely until the master closes connection
            void Run();

            /// Connect the shard to the master server
            void ConnectToMaster();

        private:
            UDPClient mClient;
        };
    }
}

#endif

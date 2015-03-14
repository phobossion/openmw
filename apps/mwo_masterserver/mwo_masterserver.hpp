#ifndef MWO_MASTERSERVER_MASTERSERVER
#define MWO_MASTERSERVER_MASTERSERVER

#include <components/mwo_net/mwo_service.hpp>
#include <components/mwo_net/mwo_udpserver.hpp>

namespace MWOnline
{
    namespace MasterServer
    {
        /// MWO master server
        /// This server handles user authentication and shard registration
        /// It also serves as the introducer for subsequent connections within the network
        class MasterServer : public Service
        {
        public:
            MasterServer();

            /// Run the server's logic
            void Run();

            /// Stop the server
            void Stop();

        private:
            /// The UDP server listening for client connections
            UDPServer mServer;
        };
    }
}

#endif

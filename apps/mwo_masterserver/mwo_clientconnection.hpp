#ifndef MWO_MASTERSERVER_CLIENTCONNECTION
#define MWO_MASTERSERVER_CLIENTCONNECTION

#include <components/mwo_net/mwo_udpconnection.hpp>
#include <components/mwo_net/mwo_connectionfactory.hpp>

namespace MWOnline
{
    namespace MasterServer
    {
        /// The connection to client handling all of the logic provided by this server
        class ClientConnection : public UDPConnection
        {
        public:
            ClientConnection(const Endpoint& local, const Endpoint& remote, const Packet* intro);
        };

        /// The connection factory used by our master server
        class MasterServerConnectionFactory : public ConnectionFactory<UDPConnection>
        {
        public:
            /// Create a connection
            virtual boost::shared_ptr<UDPConnection> CreateConnection(const Endpoint& local, const Endpoint& remote, const Packet* intro) const; // override
        };
    }
}

#endif

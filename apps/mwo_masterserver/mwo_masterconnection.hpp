#ifndef MWO_MASTERSERVER_MASTERCONNECTION
#define MWO_MASTERSERVER_MASTERCONNECTION

#include <components/mwo_net/mwo_udpconnection.hpp>
#include <components/mwo_net/mwo_connectionfactory.hpp>

namespace MWOnline
{
    namespace MasterServer
    {
        /// The connection to client handling all of the logic provided by this server
        class MasterConnection : public UDPConnection
        {
        public:
            MasterConnection(const Endpoint& local, const Endpoint& remote, const Packet* intro);

        private:
            virtual bool ReceivePacket(const Datagram* packet, const Endpoint& from); // override
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

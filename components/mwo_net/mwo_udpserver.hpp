#ifndef MWO_NET_UDPSERVER
#define MWO_NET_UDPSERVER

#include "mwo_socket.hpp"
#include "mwo_connectionfactory.hpp"

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>

namespace MWOnline
{
    class UDPConnection;

    /// Basic UDP server implementation
    /// This server listens for connection on single UDP port. When a client tries
    /// to connect using the INTR packet, the server establishes a new UDPConnection
    /// on a new port - this connection is then used for further communication with
    /// the remote client
    class UDPServer
    {
    public:
        typedef ConnectionFactory<UDPConnection> CompatibleConnectionFactory;

        UDPServer(int port, int minPort, int maxPort, const CompatibleConnectionFactory* factory);

        /// Start the server's routine
        /// This is a blocking call that should be run on separate thread
        void Run();

        /// Stop the server
        void Stop();

        /// The connection function
        void RunConnection(boost::shared_ptr<UDPConnection> connection, int port);

    private:
        /// Start a new UDP connection with a remote host
        void StartConnection(const Endpoint& remoteHost, const Packet* intro);

        /// The socket used for inbound traffic monitoring
        Socket mInboundSocket;

        int mMinConnectionPort;
        int mMaxConnectionPort;

        /// Factory used to create new connections
        const CompatibleConnectionFactory* mConnectionFactory;

        struct ConnectionData
        {
            boost::shared_ptr<boost::thread> Thread;
            boost::shared_ptr<UDPConnection> Connection;
        };

        /// Array of live connections
        std::map<int, ConnectionData> mConnections;

        /// Pool of free connection threads
        std::vector<boost::shared_ptr<boost::thread> > mThreadPool;

        /// Lock for our shared data
        boost::recursive_mutex mDataLock;

        /// Exit flag
        bool mExit;
    };
}

#endif

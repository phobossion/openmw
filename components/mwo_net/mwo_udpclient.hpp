#ifndef MWO_NET_UDPCLIENT
#define MWO_NET_UDPCLIENT

#include <boost/shared_ptr.hpp>

namespace MWOnline
{
    class UDPClientConnection;

    /// Basic UDP client implementation
    /// This represents the counterpart of UDPServer and provides an interface
    /// for contacting such a server and communicating with it using the shared
    /// UDPConnection class
    class UDPClient
    {
    public:
        /// Connect to a remote server
        /// You need to pass in the connection object you want to handle this connection
        /// Once the connection is established, the Start() will be called on the connection
        /// on another thread. This is a non-blocking call
        void Connect(boost::shared_ptr<UDPClientConnection> connection);

        /// Internal function to start a connection
        void InitiateConnection(boost::shared_ptr<UDPClientConnection> connection);
    };
}

#endif

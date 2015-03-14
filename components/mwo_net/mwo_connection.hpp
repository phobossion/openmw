#ifndef MWO_NET_CONNECTION
#define MWO_NET_CONNECTION

#include "mwo_socket.hpp"
#include "mwo_endpoint.hpp"

namespace MWOnline
{
    /// Base class for network connections between two endpoints
    class Connection
    {
    public:
        Connection(SocketType type, const Endpoint& local, const Endpoint& remote);

        /// Connection's procedure
        /// After this method finishes, the connection is considered terminated
        virtual void Start() = 0;

        const Endpoint& GetLocalEndpoint() const { return mLocalEndpoint; }
        const Endpoint& GetRemoteEndpoint() const { return mRemoteEndpoint; }

    protected:
        Endpoint mLocalEndpoint;
        Endpoint mRemoteEndpoint;

        /// The local socket allocated for the connection
        Socket mSocket;
    };
}

#endif

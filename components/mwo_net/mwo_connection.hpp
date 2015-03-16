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

        /// End the connection prematurely
        void Stop();

        /// Connection states
        enum State
        {
            Connection_New,         //! The connection object was just created
            Connection_Connecting,  //! Attempting connection with remote host
            Connection_Connected,   //! Connection is established
            Connection_Stale,       //! The remote host stopped responding
            Connection_Disconnected //! Connection timed out and will be dropped
        };

        const Endpoint& GetLocalEndpoint() const { return mLocalEndpoint; }
        const Endpoint& GetRemoteEndpoint() const { return mRemoteEndpoint; }

    protected:
        /// Helper to set the connection state with some debug output
        void SetState(State state);

        Endpoint mLocalEndpoint;
        Endpoint mRemoteEndpoint;

        /// The local socket allocated for the connection
        Socket mSocket;

        /// Current state of the connection
        /// Note that you should consider using SetState() instead of writing this directly
        State mState;
    };
}

#endif

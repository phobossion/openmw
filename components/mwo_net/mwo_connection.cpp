#include "mwo_connection.hpp"

namespace MWOnline
{
    //
    Connection::Connection(SocketType type, const Endpoint &local, const Endpoint& remote)
        : mSocket(type, local), mLocalEndpoint(local), mRemoteEndpoint(remote)
    {
    }
}

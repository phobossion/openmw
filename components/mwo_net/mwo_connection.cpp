#include "mwo_connection.hpp"

namespace MWOnline
{
    //
    Connection::Connection(SocketType type, const Endpoint &local, const Endpoint& remote)
        : mSocket(type, local), mLocalEndpoint(local), mRemoteEndpoint(remote), mState(Connection_New)
    {
    }

    //
    void Connection::Stop()
    {
        mSocket.Close();
    }

    //
    void Connection::SetState(State state)
    {
        if (state != mState)
        {
            switch (state)
            {
            case Connection_Connecting:
                MWO_TRACE("Connection on socket " << mSocket.GetID() << " is CONNECTING...");
                break;
            case Connection_Connected:
                MWO_TRACE("Connection on socket " << mSocket.GetID() << " is now CONNECTED");
                break;
            case Connection_Stale:
                MWO_TRACE("Connection on socket " << mSocket.GetID() << " is STALE (remote host stopped responding)");
                break;
            case Connection_Disconnected:
                MWO_TRACE("Connection on socket " << mSocket.GetID() << " TIMED OUT and will be closed");
                break;
            default:
                // No need for any input here
                break;
            }

            mState = state;
        }
    }
}

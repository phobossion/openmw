#ifndef MWO_NET_SOCKET
#define MWO_NET_SOCKET

#include "mwo_common.hpp"

namespace MWOnline
{
    class Endpoint;

    /// Socket types
    enum SocketType
    {
        SocketType_TCP,
        SocketType_UDP
    };

    /// The native network socket wrapper
    /// This is a thin wrapper that does not provide any special additional functionality
    class Socket
    {
    public:
        explicit Socket(SocketType type);
        Socket(SocketType type, const Endpoint& endpoint);
        ~Socket();

        /// Close the socket
        void Close();

        /// Bind the socket to the specified local(!) endpoint
        bool Bind(const Endpoint& endpoint);

        /// Set the socket as non-blocking
        bool SetNonBlocking();

        // UDP API starts here

        /// Send data packet using this socket
        int Send(const void* data, int dataLen, const Endpoint& dest);

        /// Receive data packet using this socket
        int ReceiveFrom(void* data, int maxDataLen, Endpoint& from);

    private:
        void Create();

        /// The native socket handle
        int mHandle;

        /// Type of the socket
        SocketType mType;
    };
}

#endif

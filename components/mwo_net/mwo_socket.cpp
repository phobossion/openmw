#include "mwo_socket.hpp"
#include "mwo_endpoint.hpp"

#include <assert.h>
#include <errno.h>
#include <cstring>

namespace MWOnline
{
    //
    Socket::Socket(SocketType type)
        : mType(type)
    {
        // Create the socket
        Create();
    }

    //
    Socket::Socket(SocketType type, const Endpoint &endpoint)
        : mType(type)
    {
        // Create the socket
        Create();

        // Bind it to a local endpoint immediately
        Bind(endpoint);
//        if(!Bind(endpoint))
//        {
//            throw new std::runtime_error("Failed to bind new socket to local endpoint " + endpoint.ToString());
//        }
    }

    //
    void Socket::Create()
    {
        mHandle = socket(AF_INET,
                         mType == SocketType_TCP ? SOCK_STREAM : SOCK_DGRAM,
                         mType == SocketType_TCP ? IPPROTO_TCP : IPPROTO_UDP);

        if (mHandle <= 0)
        {
            throw new std::runtime_error("Failed to allocate new socket!");
        }

        MWO_TRACE("Allocated socket ID " << mHandle);
    }

    //
    Socket::~Socket()
    {
        Close();
    }

    //
    void Socket::Close()
    {
        if (mHandle > 0)
        {
            MWO_TRACE("Closing socket ID " << mHandle);

#if MWO_UNIX
            close(mHandle);
#else
            closesocket(mHandle);
#endif
            mHandle = 0;
        }
    }

    //
    bool Socket::Bind(const Endpoint& endpoint)
    {
        int result = bind(mHandle, (const sockaddr*)&endpoint.mAddress, sizeof(sockaddr_in));
        if (result < 0)
        {
            // Error occured
            std::string errorMessage(strerror(errno));
            throw new std::runtime_error("Failed to bind socket to endpoint " + endpoint.ToString() + ". Reason: " + errorMessage);
        }

        MWO_TRACE("Bound socket ID " << mHandle << " to endpoint " << endpoint.ToString());

        return true;
    }

    //
    bool Socket::SetNonBlocking()
    {
#if MWO_UNIX
        int nonBlocking = 1;
        return (fcntl(mHandle, F_SETFL, O_NONBLOCK, nonBlocking) >= 0);
#else
        DWORD nonBlocking = 1;
        return (ioctlsocket(mHandle, FIONBIO, &nonBlocking) == 0);
#endif
    }

    //
    int Socket::Send(const void* data, int dataLen, const Endpoint& dest)
    {
        assert(mType == SocketType_UDP);

        int sent = sendto(mHandle, data, (size_t)dataLen, 0, (const sockaddr*)&dest.mAddress, sizeof(sockaddr_in));

        if (sent != dataLen)
        {
            throw new std::runtime_error("Failed to send packet to " + dest.ToString());
        }

        MWO_TRACE("Socket ID " << mHandle << " sent " << sent << "B of data to " << dest.ToString());

        return sent;
    }

    //
    int Socket::ReceiveFrom(void* data, int maxDataLen, Endpoint& from)
    {
        assert(mType == SocketType_UDP);

#if !MWO_UNIX
        typedef int socklen_t;
#endif
        socklen_t addrLen = sizeof(sockaddr_in);
        int received = recvfrom(mHandle, data, (size_t)maxDataLen, 0, (sockaddr*)&from.mAddress, &addrLen);
        if (received >= 0)
        {
            MWO_TRACE("Socket ID " << mHandle << " received " << received << "B of data from " << from.ToString());

            return received;
        }
        else if (mHandle == 0)
        {
            // The socket was closed
            return 0;
        }
        else
        {
            // Error occured
            std::string errorMessage(strerror(errno));
            MWO_TRACE("Receive error " << errorMessage);
            throw new std::runtime_error("Failed to receive data on socket. Reason: " + errorMessage);
        }
    }
}

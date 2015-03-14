#ifndef MWO_NET_ENDPOINT
#define MWO_NET_ENDPOINT

#include "mwo_common.hpp"

#include <string>

namespace MWOnline
{
    class Socket;

    /// Simple abstraction of an IP endpoint
    /// [TODO] add IPv6 support!
    class Endpoint
    {
        friend class Socket;

    public:
        Endpoint();
        explicit Endpoint(int port);
        Endpoint(int a, int b, int c, int d, int port);

        /// Create a 'localhost' endpoint on the specified port
        static Endpoint Localhost(int port);

        /// Return string representation of the endpoint
        std::string ToString() const;

    private:
        /// The native IPv4 address
        sockaddr_in mAddress;
    };
}

#endif

#include "mwo_endpoint.hpp"

#include <cstring>
#include <sstream>

namespace MWOnline
{
    //
    Endpoint::Endpoint()
    {
        memset(&mAddress, 0, sizeof(sockaddr_in));
    }

    //
    Endpoint::Endpoint(int port)
    {
        mAddress.sin_family = AF_INET;
        mAddress.sin_addr.s_addr = INADDR_ANY;
        mAddress.sin_port = htons((unsigned short)port);
    }

    //
    Endpoint::Endpoint(int a, int b, int c, int d, int port)
    {
        mAddress.sin_family = AF_INET;
        mAddress.sin_addr.s_addr = htonl((a << 24) | (b << 16) | (c << 8) | d);
        mAddress.sin_port = htons((unsigned short)port);
    }

    //
    Endpoint Endpoint::Localhost(int port)
    {
        Endpoint localhost(port);
        localhost.mAddress.sin_addr.s_addr = INADDR_LOOPBACK;

        return localhost;
    }

    //
    std::string Endpoint::ToString() const
    {
        unsigned long addr = ntohl(mAddress.sin_addr.s_addr);
        int a = (addr & 0xff000000) >> 24;
        int b = (addr & 0x00ff0000) >> 16;
        int c = (addr & 0x0000ff00) >> 8;
        int d = (addr & 0x000000ff);

        std::ostringstream s;
        s << a << "." << b << "." << c << "." << d << ":" << ntohs(mAddress.sin_port);

        return s.str();
    }
}

#include "mwo_service.hpp"

namespace MWOnline
{
    //
    int Service::mActivationCounter = 0;

    //
    Service::Service()
    {
        // Initialize the network service if this is the first time
        if (mActivationCounter++ == 0)
        {
            MWO_TRACE("Starting the MWO network service");

#if !MWO_UNIX
            WSADATA winsockData;
            if (!WSAStartup(MAKEWORD(2, 2), &winsockData))
            {
                throw std::runtime_error("Failed to initialize WinSock!");
            }
#endif
        }
    }

    //
    Service::~Service()
    {
        if (--mActivationCounter == 0)
        {
            MWO_TRACE("Stopping the MWO network service");

#if !MWO_UNIX
            WSACleanup();
#endif
        }
    }
}

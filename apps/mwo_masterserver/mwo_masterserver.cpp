#include "mwo_masterserver.hpp"
#include "mwo_masterconnection.hpp"

namespace MWOnline
{
    namespace MasterServer
    {
        // Some configuration for the server
        static const int ServerPort = 6677;
        static const int ConnectionPortMin = 40000;
        static const int ConnectionPortMax = 44000;

        static MasterServerConnectionFactory Factory;

        //
        MasterServer::MasterServer()
            : Service(), mServer(ServerPort, ConnectionPortMin, ConnectionPortMax, &Factory)
        {
        }

        //
        void MasterServer::Run()
        {
            mServer.Run();
        }

        //
        void MasterServer::Stop()
        {
            mServer.Stop();
        }
    }
}

// The server instance used
MWOnline::MasterServer::MasterServer* serverPtr;

// Ctrl+C signal handler
#if defined(__linux__) || defined(__APPLE__)
#include <signal.h>

void SignalHandler(int /*signal*/)
{
    try
    {
        serverPtr->Stop();
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what();
    }
}

#endif

// Entry point
int main(int /*argc*/, char** /*argv*/)
{
#if defined(__linux__) || defined(__APPLE__)
    // Register terminal signal handler
    signal(SIGINT, SignalHandler);
#endif

    try
    {
        MWOnline::MasterServer::MasterServer server;
        serverPtr = &server;
        server.Run();
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what();
    }

    return 0;
}

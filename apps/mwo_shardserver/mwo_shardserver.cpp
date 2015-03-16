#include "mwo_shardserver.hpp"
#include "mwo_shardtomasterconnection.hpp"

namespace MWOnline
{
    namespace ShardServer
    {
        static const int LocalPort = 6678;
        static const int RemotePort = 6677;

        //
        ShardServer::ShardServer()
        {
        }

        //
        void ShardServer::Run()
        {
        }

        //
        void ShardServer::ConnectToMaster()
        {
            boost::shared_ptr<ShardToMasterConnection> connection(new ShardToMasterConnection(Endpoint(LocalPort), Endpoint(RemotePort)));
            mClient.InitiateConnection(connection);
        }
    }
}

// The client instance to use
MWOnline::ShardServer::ShardServer* serverPtr;

// Ctrl+C signal handler
#if defined(__linux__) || defined(__APPLE__)
#include <signal.h>

void SignalHandler(int /*signal*/)
{
    try
    {
        //serverPtr->Stop();
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
        MWOnline::ShardServer::ShardServer server;
        serverPtr = &server;

        // Connect to the master right away
        server.ConnectToMaster();

        // Await exit
        server.Run();
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what();
    }

    return 0;
}

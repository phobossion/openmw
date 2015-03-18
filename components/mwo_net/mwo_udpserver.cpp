#include "mwo_udpserver.hpp"
#include "mwo_udpconnection.hpp"
#include "mwo_endpoint.hpp"
#include "mwo_corepackets.hpp"

namespace MWOnline
{
    //
    UDPServer::UDPServer(int port, int minPort, int maxPort, const CompatibleConnectionFactory* factory)
        : mInboundSocket(SocketType_UDP, Endpoint(port)), mConnectionFactory(factory), mExit(false),
          mMinConnectionPort(minPort), mMaxConnectionPort(maxPort)
    {
        MWO_TRACE("[UDP Server] Created server on local UDP port " << port << " with assigned connection range [" << minPort << ":" << maxPort << "]");
    }

    //
    void UDPServer::Run()
    {
        using namespace Packets;

        MWO_TRACE("[UDP Server] Starting server");

        // This server only listens for one kind of packet
        INTR::ReliablePacket data;

        while (!mExit)
        {
            // Wait for a connection to arrive
            Endpoint remoteHost;
            int ret = mInboundSocket.ReceiveFrom(&data, sizeof(data), remoteHost);
            if (ret == 0)
            {
                // Standard socket shutdown
                break;
            }
            else if (ret == -1)
            {
                // Error occured
                throw new std::runtime_error("Error occured while waiting for inbound UDP connection!");
            }

            MWO_TRACE("[UDP Server] Received packet " << Packet::PacketCodeToName(data.Code) << " from " << remoteHost.ToString());

            // Make sure we've got the right packet
            if (ret == sizeof(data) && INTR::ReliablePacket::IsValid(&data))
            {
                // Start a new UDP connection
                StartConnection(remoteHost, (const Packet*)&data);
            }
        }
    }

    //
    void UDPServer::Stop()
    {
        MWO_TRACE("[UDP Server] Stopping server");

        mExit = true;
        mInboundSocket.Close();

        // [TODO] stop threads, ...
    }

    //
    void UDPServer::StartConnection(const Endpoint &remoteHost, const Packet* intro)
    {
        boost::recursive_mutex::scoped_lock lock(mDataLock);

        // Assign a free port
        int port = 0;
        for (port = mMinConnectionPort; port <= mMaxConnectionPort; ++port) // [TODO] Something less lame, perhaps? :)
        {
            std::map<int, ConnectionData>::iterator it = mConnections.find(port);
            if (it == mConnections.end())
            {
                break;
            }
        }

        if (port == 0)
            throw new std::runtime_error("Server ran out of allocated ports!"); // [TODO] This should probably be handled gracefully in the upper scope

        Endpoint localHost(port);

        // Initialize the connection
        boost::shared_ptr<UDPConnection> connection(mConnectionFactory->CreateConnection(localHost, remoteHost, intro));

        // Allocate a free thread for the connection
        //boost::shared_ptr<boost::thread> thread;
        //if (!mThreadPool.empty())
        //{
        //    // We have a free thread waiting for us
        //    thread = mThreadPool.back();
        //    mThreadPool.pop_back();
        //
        //    // Just start it
        //    // [TODO] how to pass in new parameters???
        //}

        // Just create a brand new thread for now
        boost::shared_ptr<boost::thread> thread(new boost::thread(&UDPServer::RunConnection, this, connection, port));

        // Store the connection in our cache
        ConnectionData c;
        c.Thread = thread;
        c.Connection = connection;
        mConnections[port] = c;
    }

    //
    void UDPServer::RunConnection(boost::shared_ptr<UDPConnection> connection, int port)
    {
        connection->Start();

        // Getting here means the connection has finished, we can now unregister it
        boost::recursive_mutex::scoped_lock lock(mDataLock);

        // [TODO] store the thread in the pool

        // Destroy the connection
        std::map<int, ConnectionData>::iterator it = mConnections.find(port);
        assert(it != mConnections.end());

        mConnections.erase(it);
    }
}

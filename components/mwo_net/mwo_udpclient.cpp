#include "mwo_udpclient.hpp"
#include "mwo_udpclientconnection.hpp"

#include <boost/thread.hpp>

namespace MWOnline
{
    //
    void UDPClient::Connect(boost::shared_ptr<UDPClientConnection> connection)
    {
        // Create new thread to handle the connection
        // [TODO] thread pooling!
        boost::shared_ptr<boost::thread> thread(new boost::thread(&UDPClient::InitiateConnection, this, connection));
    }

    //
    void UDPClient::InitiateConnection(boost::shared_ptr<UDPClientConnection> connection)
    {
        connection->Initiate();

        // Getting here means the connection has finished its job
    }
}

#ifndef MWO_NET_CONNECTIONFACTORY
#define MWO_NET_CONNECTIONFACTORY

#include <boost/shared_ptr.hpp>

namespace MWOnline
{
    struct Packet;
    class Endpoint;

    /// User connection factory
    /// This factory can be used to create different types of connections
    /// for different purposes on the same server
    template <class BaseConnectionClass>
    class ConnectionFactory
    {
    public:
        /// Create a compatible connection
        virtual boost::shared_ptr<BaseConnectionClass> CreateConnection(const Endpoint& local, const Endpoint& remote, const Packet* intro) const = 0;
    };
}

#endif

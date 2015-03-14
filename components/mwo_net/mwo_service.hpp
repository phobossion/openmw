#ifndef MWO_NET_SERVICE
#define MWO_NET_SERVICE

#include "mwo_common.hpp"

namespace MWOnline
{
    /// The master service class
    /// Every application that wants to use MWO netcode must create at least one
    /// instance of this class. It will be used to create all the sockets etc.
    class Service
    {
    public:
        Service();
        virtual ~Service();

    protected:

    private:
        /// Service activation counter
        static int mActivationCounter;
    };
}

#endif

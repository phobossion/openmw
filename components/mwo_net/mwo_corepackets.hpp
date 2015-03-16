#ifndef MWO_NET_COREPACKETS
#define MWO_NET_COREPACKETS

#include "mwo_packet.hpp"

namespace MWOnline
{
    // Here are the definitions of core data packets used in the MWO netcode

    // Introduction packet sent from one node to another as a request for connection
    BEGIN_PACKET(INTR)
        PACKET_DATA(int, Reason, 0)
    END_PACKET()

    // Connection accept packet sent back as a response to INTR
    BEGIN_PACKET(ACPT)
    END_PACKET()

    // Datagram used to keep the connection alive in case there is no other traffic
    BEGIN_PACKET(POKE)
    END_PACKET()
}

#endif

#ifndef MWO_NET_PACKET
#define MWO_NET_PACKET

#include "mwo_common.hpp"

#include <cstring>

namespace MWOnline
{
    /// A header that every data packet in MWO will carry
    static const unsigned int PacketHeader = 0xfee1f00d;

    /// Base class for a packet
    struct Packet
    {
        const unsigned int Header;
        const unsigned int Code;

        // Flag storage & defines
        // [TODO] we might consider creating a special set of flags for TCP and UDP respectively
        unsigned char Flags;
        enum Flag
        {
            Flag_Reliable = 1,      //< Reliable UDP packet
            Flag_DiscardOld = 2     //< In an UDP connection, when packets arrive out of order, setting this flag will effectively keep only the latest one
        };

        Packet(const char* code, unsigned char flags);

        /// Check if the given data contain a valid MWO packet
        static bool IsValid(const void* data);

        /// Convert the 4-character packet identifier to packet code
        static unsigned int PacketNameToCode(const char* name);
        /// Convert the packet code to packet name
        static std::string PacketCodeToName(unsigned int code);
    };

    /// Base class for an UDP datagram
    /// This adds the sequence number to the packet so that we can detect out-of-date datagrams
    struct Datagram : public Packet
    {
        unsigned int SequenceNumber;

        Datagram(const char* code, unsigned char flags);
    };

    /// Base class for a "reliable" UDP packet
    /// This adds the ack fields used to detect delivery of previous packets
    struct ReliablePacket : public Datagram
    {
        unsigned int Ack;
        unsigned int AckBitfield;

        ReliablePacket(const char* code, unsigned char flags);
    };

    // Here we define a macro used to declare data packet types
#define __DECLARE_PACKET(code, baseClass, flags) \
    struct baseClass : public MWOnline::baseClass \
    { \
        Data Payload; \
        \
        baseClass() : MWOnline::baseClass(code, flags) {} \
        \
        static unsigned int TypeCode() \
        { \
            static unsigned int c = PacketNameToCode(code); \
            return c; \
        } \
        \
        static std::string TypeName() \
        { \
            static std::string name(code); \
            return name; \
        } \
        \
        static bool IsValid(const void* packetData) \
        { \
            const baseClass* p = (const baseClass*)packetData; \
            return Packet::IsValid(packetData) && p->Code == TypeCode(); \
        } \
    };

    /// Helper template for auto-initialized data fields
    /// [TODO] once this projects gets full C++11 support, we can get rid of this
    template <typename T, T def>
    struct PacketDataStorage
    {
        T Data;

        PacketDataStorage()
            : Data(def)
        {}

        operator T() { return Data; }
    };

#define BEGIN_PACKET_FLAGS(code, flags) \
    namespace Packets { namespace code { \
        static const char* Name = #code; \
        static unsigned char UserFlags = flags; \
        struct Data { \

#define BEGIN_PACKET(code) \
    BEGIN_PACKET_FLAGS(code, 0)

#define END_PACKET() \
        }; \
        \
        __DECLARE_PACKET(Name, Packet, UserFlags)              /* Basic TCP packet */  \
        __DECLARE_PACKET(Name, Datagram, UserFlags)            /* Unreliable UDP packet */  \
        __DECLARE_PACKET(Name, ReliablePacket, UserFlags)      /* Reliable UDP packet */ \
    } }

#define PACKET_DATA(type, name, def) \
    PacketDataStorage<type, def> name;
}

#endif

#include "mwo_packet.hpp"

namespace MWOnline
{
    //
    Packet::Packet(const char* code, unsigned char flags)
        : Header(PacketHeader), Code(PacketNameToCode(code)), Flags(flags)
    {
    }

    //
    bool Packet::IsValid(const void* data)
    {
        const Packet* p = (const Packet*)data;
        return p->Header == PacketHeader;
    }

    //
    unsigned int Packet::PacketNameToCode(const char* name)
    {
        if (strlen(name) != 4)
            throw new std::runtime_error(std::string("Invalid packet code: ") + name);

        unsigned int code =
                ((unsigned int)name[0] << 24) |
                ((unsigned int)name[1] << 16) |
                ((unsigned int)name[2] << 8) |
                (unsigned int)name[3];

        return code;
    }

    //
    std::string Packet::PacketCodeToName(unsigned int code)
    {
        std::string s;
        s.reserve(4);
        s += (char)((code & 0xff000000) >> 24);
        s += (char)((code & 0x00ff0000) >> 16);
        s += (char)((code & 0x0000ff00) >> 8);
        s += (char)((code & 0x000000ff));

        return s;
    }

    //
    Datagram::Datagram(const char *code, unsigned char flags)
        : Packet(code, flags), Ack(0), AckBitfield(0)
    {
    }

    //
    ReliablePacket::ReliablePacket(const char *code, unsigned char flags)
        : Datagram(code, flags | Packet::Flag_Reliable), SequenceNumber(0)
    {
    }
}

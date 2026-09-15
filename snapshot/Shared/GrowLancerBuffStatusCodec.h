#pragma once
#include <cstddef>
#include <cstdint>

namespace rise { namespace growlancer {
// RISE-designed payload, NOT the S21 wire layout. No transport opcode assigned.
// Local owner only; legacy 0x2D and remote viewport packets remain unchanged.
// v1: version:u8, operation:u8, buff:u16LE, seconds:u32LE, auxiliary:u16LE.
// Auxiliary is preserved without assigning unverified gameplay meaning.
struct WrathStatusMessage
{
    bool remove;
    std::uint16_t buff;
    std::uint32_t durationSeconds;
    std::uint16_t auxiliary;
};
constexpr std::size_t kWrathStatusPayloadSize = 10;
constexpr bool IsWrathStatusId(std::uint16_t id) { return id==424 || id==425; }
constexpr bool DecodeWrathStatus(const unsigned char* bytes, std::size_t size,
    WrathStatusMessage& result)
{
    if(!bytes || size!=kWrathStatusPayloadSize || bytes[0]!=1 || bytes[1]>1)
        return false;
    const std::uint16_t id=static_cast<std::uint16_t>(bytes[2] | (bytes[3]<<8));
    if(!IsWrathStatusId(id)) return false;
    const std::uint32_t duration=std::uint32_t(bytes[4]) |
        (std::uint32_t(bytes[5])<<8) | (std::uint32_t(bytes[6])<<16) |
        (std::uint32_t(bytes[7])<<24);
    const std::uint16_t auxiliary=static_cast<std::uint16_t>(bytes[8] | (bytes[9]<<8));
    result={bytes[1]!=0,id,duration,auxiliary};
    return true;
}
constexpr bool EncodeWrathStatus(const WrathStatusMessage& message,
    unsigned char* bytes, std::size_t size)
{
    if(!bytes || size!=kWrathStatusPayloadSize || !IsWrathStatusId(message.buff))
        return false;
    bytes[0]=1; bytes[1]=message.remove ? 1 : 0;
    bytes[2]=static_cast<unsigned char>(message.buff);
    bytes[3]=static_cast<unsigned char>(message.buff>>8);
    for(unsigned i=0;i<4;++i)
        bytes[4+i]=static_cast<unsigned char>(message.durationSeconds>>(i*8));
    bytes[8]=static_cast<unsigned char>(message.auxiliary);
    bytes[9]=static_cast<unsigned char>(message.auxiliary>>8);
    return true;
}
} }

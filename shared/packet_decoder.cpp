#include <cstring>
#include <stdexcept>
#include "packet_decoder.h"

bool PacketDecoder::hasPacket() const{
    return !packets.empty();
}

Packet PacketDecoder::nextpacket(){
    Packet packet = packets.front();
    packets.pop();
    return packet;
}

//push complete ones to queue, incomplete to buffer till they are complete
//never discard partial data
void PacketDecoder::feed(const char* data, size_t bytes){
    buffer.insert(
        buffer.end(),
        data,
        data + bytes
    );

    //keep extracting
    while (true){
        if (buffer.size() < sizeof(PacketHeader)){
            return;
        }
        //read the header without removing it
        PacketHeader header;

        //not compatible on different architecture (endianness)
        memcpy(&header, buffer.data(), sizeof(PacketHeader));

        //keep an upper limit for payload size
        constexpr uint32_t MAX_PAYLOAD_SIZE = 1024 * 1024; //1MB
        if (header.payload_size > MAX_PAYLOAD_SIZE){
            throw runtime_error("Payload size too large");
        }


        //total bytes needed for this packet
        size_t packetSize = sizeof(PacketHeader) + header.payload_size;
    
        if (buffer.size() < packetSize){
            return;
        }

        //build the packet
        Packet packet;
        packet.header = header;

        packet.payload.assign(
            buffer.begin() + sizeof(PacketHeader),
            buffer.begin() + packetSize
        );

        //store the completed packet
        packets.push(packet);

        //remove the consumed bytes ie pushed packets
        //half packets remain in the buffer
        buffer.erase(
            buffer.begin(),
            buffer.begin() + packetSize
        );
    }
}  




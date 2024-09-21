#ifndef COMMUNICATIONS_HPP
#define COMMUNICATIONS_HPP

#include <cstdlib>

enum PacketType {
  StatusType = 0x01,
  PingType = 0x02,
  PingAckType = 0x03,
  Command = 0x04,
  CommandAck = 0x05
};

typedef struct {
  uint8_t type = PacketType::PingType;
  unsigned int pingId;
} Ping;

typedef struct {
  uint8_t type = PacketType::PingAckType;
  unsigned int pingId;
 } PingAck;













#endif //COMMUNICATIONS_HPP

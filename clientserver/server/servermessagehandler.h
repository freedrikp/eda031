#include <string>
#include "connection.h"

class ServerMessageHandler{
public:
  //WIP

private:
  int readBytes(const shared_ptr<Connection>& conn) {
    unsigned char byte1 = conn->read();
    unsigned char byte2 = conn->read();
    unsigned char byte3 = conn->read();
    unsigned char byte4 = conn->read();
    return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  }
};

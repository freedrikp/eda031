#include <string>
#include "connection.h"

class Client{
public:
  Client(Connection& conn);
  void interact();
private:
  const Connection& conn;
  enum Selection {LISTNG,CREATENG,DELETENG,LISTART,CREATEART,DELETEART,GETART,WRONG};

  unsigned char readCode();
  void writeCode(unsigned char value);
  int readNumber();
  void writeNumber(int value);
  std::string readString(int n);
  void writeString(const std::string& s);

  Selection queryUser();





};

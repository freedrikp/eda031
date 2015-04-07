#include "clientmessagehandler.h"
#include "connectionclosedexception.h"
#include <iostream>
#include "protocol.h"

ClientMessageHandler::ClientMessageHandler(Connection& conn) : conn(conn){}


unsigned char ClientMessageHandler::readCode() {
  unsigned char code = conn.read();
  std::cout << "readCode " << static_cast<int>(code) << std::endl;
  return code;
}

void ClientMessageHandler::writeCode(unsigned char value) {
  std::cout << "writeCode with parameter: " << (value&0xFF) << std::endl;
  conn.write(value);
}


int ClientMessageHandler::readNumber() {
  if (readCode() != Protocol::PAR_NUM){
    std::cerr << "Protocol does not match - Could not read number" << std::endl;
    return -1;
  }
  unsigned char byte1 = conn.read();
  unsigned char byte2 = conn.read();
  unsigned char byte3 = conn.read();
  unsigned char byte4 = conn.read();
  return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

void ClientMessageHandler::writeNumber(int value) {
  conn.write(Protocol::PAR_NUM);
  conn.write((value >> 24) & 0xFF);
  conn.write((value >> 16) & 0xFF);
  conn.write((value >> 8) & 0xFF);
  conn.write(value & 0xFF);
}

std::string ClientMessageHandler::readString() {
  if (readCode() != Protocol::PAR_STRING){
    std::cerr << "Protocol does not match - Could not read string" << std::endl;
    return std::string();
  }

  unsigned char byte1 = conn.read();
  unsigned char byte2 = conn.read();
  unsigned char byte3 = conn.read();
  unsigned char byte4 = conn.read();
  int n = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;

  std::string s;
  for(int i = 0; i < n; ++i){
    s += conn.read();
  }
  return s;
}


void ClientMessageHandler::writeString(const std::string& s) {
  conn.write(Protocol::PAR_STRING);
  size_t value = s.size();

  conn.write((value >> 24) & 0xFF);
  conn.write((value >> 16) & 0xFF);
  conn.write((value >> 8) & 0xFF);
  conn.write(value & 0xFF);

  for (char c : s) {
    conn.write(c);
  }
}

void ClientMessageHandler::listNewsgroups(){
  writeCode(Protocol::COM_LIST_NG);
  writeCode(Protocol::COM_END);
  if (readCode() != Protocol::ANS_LIST_NG){
    std::cerr << "Protocol does not match - ANS_LIST_NG" << std::endl;
    return;
  }
  int n = readNumber();
  if (n < 0){
    return;
  }
  for (int i = 0; i != n; ++i){
    int id = readNumber();
    if (id < 0){
      return;
    }
    std::string name = readString();
    if (name.empty()){
      return;
    }
    std::cout << id << " " << name << std::endl;
  }
  if (readCode() != Protocol::ANS_END){
    std::cerr << "Protocol does not match - ANS_END" << std::endl;
    return;
  }
}

void ClientMessageHandler::createNewsgroup(std::string& name){
  writeCode(Protocol::COM_CREATE_NG);
  writeString(name);
  writeCode(Protocol::COM_END);
  if (readCode() != Protocol::ANS_CREATE_NG){
    std::cerr << "Protocol does not match - ANS_CREATE_NG" << std::endl;
    return;
  }
  unsigned char code = readCode();
  if (code == Protocol::ANS_ACK){
    std::cout << "Newsgroup created" << std::endl;
  }else if (code == Protocol::ANS_NAK){
    if (readCode() != Protocol::ERR_NG_ALREADY_EXISTS){
      std::cerr << "Protocol does not match - ERR_NG_ALREADY_EXISTS" << std::endl;
      return;
    }
    std::cout << "Newsgroup already exists" << std::endl;
  }else{
    std::cerr << "Protocol does not match - ANS_ACK|ANS_NAK" << std::endl;
    return;
  }
  if (readCode() != Protocol::ANS_END){
    std::cerr << "Protocol does not match - ANS_END" << std::endl;
    return;
  }
}

void ClientMessageHandler::deleteNewsgroup(int groupID){
  writeCode(Protocol::COM_DELETE_NG);
  writeNumber(groupID);
  writeCode(Protocol::COM_END);
  if (readCode() != Protocol::ANS_DELETE_NG){
    std::cerr << "Protocol does not match - ANS_DELETE_NG" << std::endl;
    return;
  }
  unsigned char code = readCode();
  if (code == Protocol::ANS_ACK){
    std::cout << "Newsgroup deleted" << std::endl;
  }else if (code == Protocol::ANS_NAK){
    if (readCode() != Protocol::ERR_NG_DOES_NOT_EXIST){
      std::cerr << "Protocol does not match - ERR_NG_DOES_NOT_EXIST" << std::endl;
      return;
    }
    std::cout << "Newsgroup does not exist" << std::endl;
  }else{
    std::cerr << "Protocol does not match - ANS_ACK|ANS_NAK" << std::endl;
    return;
  }
  if (readCode() != Protocol::ANS_END){
    std::cerr << "Protocol does not match - ANS_END" << std::endl;
    return;
  }
}

void ClientMessageHandler::listArticles(int groupID){
  writeCode(Protocol::COM_LIST_ART);
  writeNumber(groupID);
  writeCode(Protocol::COM_END);
  if (readCode() != Protocol::ANS_LIST_ART){
    std::cerr << "Protocol does not match - ANS_LIST_ART" << std::endl;
    return;
  }
  unsigned char code = readCode();
  if (code == Protocol::ANS_ACK){
    int n = readNumber();
    if (n < 0){
      return;
    }
    for (int i = 0; i != n; ++i){
      int id = readNumber();
      if (id < 0){
        return;
      }
      std::string title = readString();
      if (title.empty()){
        return;
      }
      std::cout << id << " " << title << std::endl;
    }
  }else if (code == Protocol::ANS_NAK){
    if (readCode() != Protocol::ERR_NG_DOES_NOT_EXIST){
      std::cerr << "Protocol does not match - ERR_NG_DOES_NOT_EXIST" << std::endl;
      return;
    }
    std::cout << "Newsgroup does not exist" << std::endl;
  }else{
    std::cerr << "Protocol does not match - ANS_ACK|ANS_NAK" << std::endl;
    return;
  }
  if (readCode() != Protocol::ANS_END){
    std::cerr << "Protocol does not match - ANS_END" << std::endl;
    return;
  }
}

void ClientMessageHandler::createArticle(int groupID, std::string title, std::string author, std::string text){

  if (readCode() != Protocol::ANS_END){
    std::cerr << "Protocol does not match - ANS_END" << std::endl;
    return;
  }
}

void ClientMessageHandler::deleteArticle(int groupID, int articleID){

  if (readCode() != Protocol::ANS_END){
    std::cerr << "Protocol does not match - ANS_END" << std::endl;
    return;
  }
}

void ClientMessageHandler::getArticle(int groupID, int articleID){

  if (readCode() != Protocol::ANS_END){
    std::cerr << "Protocol does not match - ANS_END" << std::endl;
    return;
  }
}

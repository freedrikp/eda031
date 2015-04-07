#include "client.h"
#include "connectionclosedexception.h"
#include <iostream>
#include "protocol.h"

Client::Client(Connection& conn) : conn(conn){}


unsigned char Client::readCode() {
	return conn.read();
}

void Client::writeCode(unsigned char value) {
	std::cout << "writeCode with parameter: " << (value&0xFF) << std::endl;
	conn.write(value);
}


int Client::readNumber() {
	unsigned char byte1 = conn.read();
	unsigned char byte2 = conn.read();
	unsigned char byte3 = conn.read();
	unsigned char byte4 = conn.read();
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

void Client::writeNumber(int value) {
	conn.write(Protocol::PAR_NUM);
	conn.write((value >> 24) & 0xFF);
	conn.write((value >> 16) & 0xFF);
	conn.write((value >> 8) & 0xFF);
	conn.write(value & 0xFF);
}

std::string Client::readString(int n) {
	std::string s;
	for(int i = 0; i < n; ++i){
		s += conn.read();
	}
	return s;
}


void Client::writeString(const std::string& s) {
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

Client::Selection Client::queryUser(){
  std::cout << std::endl << "What is my bidding, master?" << std::endl << std::endl;
  std::cout << "List newsgroups - 1" << std::endl;
  std::cout << "Create newsgroup - 2" << std::endl;
  std::cout << "Delete newsgroup - 3" << std::endl;
  std::cout << "List articles - 4" << std::endl;
  std::cout << "Create article - 5" << std::endl;
  std::cout << "Delete article - 6" << std::endl;
  std::cout << "Get article - 7" << std::endl;
  int selection;
  std::cin >> selection;
  switch (selection){
    case 1:
      return Selection::LISTNG;
    case 2:
      return Selection::CREATENG;
    case 3:
      return Selection::DELETENG;
    case 4:
        return Selection::LISTART;
    case 5:
        return Selection::CREATEART;
    case 6:
        return Selection::DELETEART;
    case 7:
        return Selection::GETART;
    default:
        return Selection::WRONG;
  }
}

void Client::listNewsgroups(){
  writeCode(Protocol::COM_LIST_NG);
  writeCode(Protocol::COM_END);
  if (!(readCode() == Protocol::ANS_LIST_NG)){
    std::cerr << "Protocol does not match" << std::endl;
    return;
  }
  if (!(readCode() == Protocol::PAR_NUM)){
    std::cerr << "Protocol does not match" << std::endl;
    return;
  }
  int n = readNumber();
  for (int i = 0; i != n; ++i){
    if (!(readCode() == Protocol::PAR_NUM)){
      std::cerr << "Protocol does not match" << std::endl;
      return;
    }
    int id = readNumber();
    if (!(readCode() == Protocol::PAR_STRING)){
      std::cerr << "Protocol does not match" << std::endl;
      return;
    }
    int nbrChars = readNumber();
    std::string name = readString(nbrChars);
    std::cout << id << " " << name << std::endl;
  }
}

void Client::createNewsgroup(std::string& name){
  writeCode(Protocol::COM_CREATE_NG);
  writeString(name);
  writeCode(Protocol::COM_END);
  if (!(readCode() == Protocol::ANS_CREATE_NG)){
    std::cerr << "Protocol does not match - 1" << std::endl;
    return;
  }
  unsigned char code = readCode();
  if (code == Protocol::ANS_ACK){
    std::cout << "Newsgroup created" << std::endl;
  }else if (code == Protocol::ANS_NAK){
    if (!(readCode() == Protocol::ERR_NG_ALREADY_EXISTS)){
      std::cerr << "Protocol does not match - 2" << std::endl;
      return;
    }
    std::cout << "Newsgroup already exists" << std::endl;
  }else{
    std::cerr << "Protocol does not match - 3" << std::endl;
    return;
  }
  if (!(readCode() == Protocol::ANS_END)){
    std::cerr << "Protocol does not match - 3" << std::endl;
    return;
  }

}

void Client::interact(){
  while(true){
    Selection sel = queryUser();
    switch (sel){

      case LISTNG: {
        listNewsgroups();
        break;
      }
      case CREATENG: {
        std::cout << "What should the newsgroup be called?" << std::endl;
        std::string name;
        std::cin >> name;
        createNewsgroup(name);
        break;
      }
      case DELETENG: {

        break;
      }
      case LISTART: {

        break;
      }
      case CREATEART: {

        break;
      }
      case DELETEART: {

        break;
      }
      case GETART: {

        break;
      }
      default:
        std::cout << "Wrong selection" << std::endl;



    }
  }


}










int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: myclient host-name port-number" << std::endl;
		exit(1);
	}

	int port = -1;
	try {
		port = std::stoi(argv[2]);
	} catch (std::exception& e) {
		std::cerr << "Wrong port number. " << e.what() << std::endl;
		exit(1);
	}

	Connection conn(argv[1], port);
	if (!conn.isConnected()) {
		std::cerr << "Connection attempt failed" << std::endl;
		exit(1);
	}
  Client client(conn);
  client.interact();
}

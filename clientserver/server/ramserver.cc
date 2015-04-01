/* myserver.cc: sample server program */
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"

#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

using namespace std;

/*
 * Read an integer from a client.
 */
int readCode(const shared_ptr<Connection>& conn) {
	return conn->read();
}

/*
 * Read an integer from a client.
 */
int readInt(const shared_ptr<Connection>& conn) {
	unsigned char byte1 = conn->read();
	unsigned char byte2 = conn->read();
	unsigned char byte3 = conn->read();
	unsigned char byte4 = conn->read();
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

/*
 * Read an integer from a client.
 */
string readString(const shared_ptr<Connection>& conn, int n) {
	string s;
	for(int i = 0; i < n; ++i){
		s += static_cast<char>(readInt(conn));
	}
	return s;
}

/*
 * Send a string to a client.
 */
void writeString(const shared_ptr<Connection>& conn, const string& s) {
	for (char c : s) {
		conn->write(c);
	}
	conn->write('$');
}

/*
 * Send an integer to the server as four bytes.
 */
void writeCode(const shared_ptr<Connection>& conn, int value) {
	conn->write(value & 0xFF);
}

/*
 * Send an integer to the server as four bytes.
 */
void writeNumber(const shared_ptr<Connection>& conn, int value) {
	conn->write((value >> 24) & 0xFF);
	conn->write((value >> 16) & 0xFF);
	conn->write((value >> 8) & 0xFF);
	conn->write(value & 0xFF);
}

int main(int argc, char* argv[]){
	if (argc != 2) {
		cerr << "Usage: myserver port-number" << endl;
		exit(1);
	}
	
	int port = -1;
	try {
		port = stoi(argv[1]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}
	
	Server server(port);
	if (!server.isReady()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}
	
	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
				int kase = readCode(conn);
				cout << "kase: " << kase << endl;
				switch(kase){
					case Protocol::COM_LIST_NG:
						cout << "@1" << endl;
						break;
					case Protocol::COM_CREATE_NG:
						cout << "create" << endl;
						if(readCode(conn) == Protocol::PAR_STRING){
							int n = readInt(conn);
							string name = readString(conn, n);
							cout << "n: " << n << ", name: " << name << endl;
							writeCode(conn, Protocol::ANS_CREATE_NG);
							writeCode(conn, Protocol::ANS_ACK);
							writeCode(conn, Protocol::ANS_END);
						}
						break;
					case Protocol::COM_DELETE_NG:
						cout << "@3" << endl;
						break;
					case Protocol::COM_LIST_ART:
						cout << "@4" << endl;
						break;
					case Protocol::COM_CREATE_ART:
						cout << "@5" << endl;
						break;
					case Protocol::COM_DELETE_ART:
						cout << "@6" << endl;
						break;
					case Protocol::COM_GET_ART:
						cout << "@7" << endl;
						break;
					case Protocol::COM_END:
						cout << "@8" << endl;
						break;
					default:
						cout << "skogen";
						break;
				}
			} catch (ConnectionClosedException&) {
				server.deregisterConnection(conn);
				cout << "Client closed connection" << endl;
			}
		} else {
			conn = make_shared<Connection>();
			server.registerConnection(conn);
			cout << "New client connects" << endl;
		}
	}
}
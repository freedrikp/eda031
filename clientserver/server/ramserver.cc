/* myserver.cc: sample server program */
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "memoryDatabase.h"
#include "newsgroup.h"
#include "nonewsgroupexception.h"

#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

using namespace std;

/*
* Read an integer from a client.
*/
unsigned char readCode(const shared_ptr<Connection>& conn) {
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
		s += readCode(conn);
	}
	return s;
}

/*
* Send a string to a client.
*/
void writeString(const shared_ptr<Connection>& conn, const string& s) {
	conn->write(Protocol::PAR_STRING);
	size_t value = s.size();

	conn->write((value >> 24) & 0xFF);
	conn->write((value >> 16) & 0xFF);
	conn->write((value >> 8) & 0xFF);
	conn->write(value & 0xFF);

	for (char c : s) {
		conn->write(c);
	}

}

/*
* Send an code to the server as one byte.
*/
void writeCode(const shared_ptr<Connection>& conn, unsigned char value) {
	cout << "writeCode with parameter: " << (value&0xFF) << endl;
	conn->write(value);
}

/*
* Send an integer to the server as four bytes.
*/
void writeNumber(const shared_ptr<Connection>& conn, int value) {
	conn->write(Protocol::PAR_NUM);
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

	MemoryDatabase database;

	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
				int kase = readCode(conn);
				cout << "kase: " << kase << endl;
				switch(kase){
					case Protocol::COM_LIST_NG:{
						cout << "@COM_LIST_NG" << endl;
						if(readCode(conn) != Protocol::COM_END){
							cout << "Protocol violation on create newsgroup" <<endl;
							break;
						}
						writeCode(conn, Protocol::ANS_LIST_NG);
						vector<Newsgroup> groups = database.getNewsgroups();
						cout << "writing number of groups:" << groups.size() <<endl;
						writeNumber(conn, groups.size());
						for(auto it = groups.begin(); it < groups.end(); ++it){
							cout << "writing nummber: "<< it->getID() << " and name " <<it->getName()<<endl;
							writeNumber(conn, it->getID());
							writeString(conn, it->getName());
						}
						writeCode(conn, Protocol::ANS_END);
						break;
					}
					case Protocol::COM_CREATE_NG:{
						cout << "@COM_CREATE_NG" << endl;
						if(readCode(conn) == Protocol::PAR_STRING){
							int n = readInt(conn);
							string name = readString(conn, n);
							cout << "n: " << n << ", name: " << name << endl;
							if(readCode(conn) != Protocol::COM_END){
								cout << "Protocol violation on list newsgroups" << endl;
							}
							writeCode(conn, Protocol::ANS_CREATE_NG);
							if(database.addNewsgroup(name)){
								writeCode(conn, Protocol::ANS_ACK);
							} else {
								writeCode(conn, Protocol::ANS_NAK);
								writeCode(conn, Protocol::ERR_NG_ALREADY_EXISTS);
							}
							writeCode(conn, Protocol::ANS_END);
						} else {
							cout << "Protocol violation on list newsgroups" << endl;
						}
						break;
					}
					case Protocol::COM_DELETE_NG:{
						cout << "@COM_DELETE_NG" << endl;
						size_t nGroupID = static_cast<size_t>(readInt(conn));
						if(readCode(conn) != Protocol::COM_END){
							cout << "Protocol violation on delete newsgroup" << endl;
							break;
						}
						writeCode(conn, Protocol::ANS_CREATE_NG);
						if(database.removeNewsgroup(nGroupID)){
							writeCode(conn, Protocol::ANS_ACK);
						} else {
							writeCode(conn, Protocol::ANS_NAK);
							writeCode(conn, Protocol::ERR_NG_DOES_NOT_EXIST);
						}
						writeCode(conn, Protocol::ANS_END);
						break;
					}
					case Protocol::COM_LIST_ART:{
						cout << "@COM_LIST_ART" << endl;
						size_t nGroupID = static_cast<size_t>(readInt(conn));
						if(readCode(conn) != Protocol::COM_END){
							cout << "Protocol violation on list articles" << endl;
							break;
						}
						try{
							database.getArticles(nGroupID);
						} catch (NoNewsgroupException nne){
							writeCode(conn, Protocol::ANS_NAK);
							writeCode(conn, Protocol::ERR_NG_DOES_NOT_EXIST);
						}
						writeCode(conn, Protocol::ANS_END);
						break;
					}
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
					cout << "Det gick till skogen";
					break;
				}
				cout << "current database"<<endl;
				vector<Newsgroup> groups = database.getNewsgroups();
				cout << groups.size()<<endl;
				for(auto it = groups.begin(); it < groups.end(); ++it){
					cout << it->getID()<<endl;
					cout << it->getName()<<endl;
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

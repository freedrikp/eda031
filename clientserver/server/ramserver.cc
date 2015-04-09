/* myserver.cc: sample server program */
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "memoryDatabase.h"
#include "newsgroup.h"
#include "article.h"
#include "nonewsgroupexception.h"
#include "noarticleexception.h"
#include "protocolviolationexception.h"
#include "servermessagehandler.h"

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

int readBytes(const shared_ptr<Connection>& conn) {
	unsigned char byte1 = conn->read();
	unsigned char byte2 = conn->read();
	unsigned char byte3 = conn->read();
	unsigned char byte4 = conn->read();
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

/*
* Read an integer from a client.
*/
int readInt(const shared_ptr<Connection>& conn) {
	if(readCode(conn) != Protocol::PAR_NUM){
		throw ProtocolViolationException("Read Int");
	}
	return readBytes(conn);
}

/*
* Read an integer from a client.
*/
string readString(const shared_ptr<Connection>& conn) {
	if(readCode(conn) != Protocol::PAR_STRING){
		throw ProtocolViolationException("Read String");
	}
	string s;
	int n = readBytes(conn);

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
	cout << "Starting server... " << endl;
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
	cout << "Now listening on port " << argv[1] << "." << endl;
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
							throw ProtocolViolationException("Create ng");
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
						string name = readString(conn);
						if(readCode(conn) != Protocol::COM_END){
								throw ProtocolViolationException("List ng");
						}
						writeCode(conn, Protocol::ANS_CREATE_NG);
						if(database.addNewsgroup(name)){
							writeCode(conn, Protocol::ANS_ACK);
						} else {
							writeCode(conn, Protocol::ANS_NAK);
							writeCode(conn, Protocol::ERR_NG_ALREADY_EXISTS);
						}
						writeCode(conn, Protocol::ANS_END);
						
						break;
					}
					case Protocol::COM_DELETE_NG:{
						cout << "@COM_DELETE_NG" << endl;
						size_t nGroupID = static_cast<size_t>(readInt(conn));
						if(readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("Delete ng");
						}
						writeCode(conn, Protocol::ANS_DELETE_NG);
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
							throw ProtocolViolationException("List art");
						}
						writeCode(conn, Protocol::ANS_LIST_ART);
						try{
							cout << "# apa 1" << endl;
							vector<Article> articles = database.getArticles(nGroupID);
							cout << "# apa 2" << endl;
							writeCode(conn, Protocol::ANS_ACK);
							writeNumber(conn, articles.size());
							for(auto it = articles.begin(); it < articles.end(); ++it){
								cout << "writing number: "<< it->getID() << " and name " <<it->getTitle()<<endl;
								writeNumber(conn, it->getID());
								writeString(conn, it->getTitle());
							}

						} catch (NoNewsgroupException nne){
							cout << "# apa 9" << endl;
							writeCode(conn, Protocol::ANS_NAK);
							writeCode(conn, Protocol::ERR_NG_DOES_NOT_EXIST);
						}
						writeCode(conn, Protocol::ANS_END);
						break;
					}
					case Protocol::COM_CREATE_ART:{
						int newsgroupId = readInt(conn);
						string author = readString(conn);
						string title = readString(conn);
						string text = readString(conn);
						if(readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("Create art");
						}
						writeCode(conn, Protocol::ANS_CREATE_ART);
						if(database.addArticle(newsgroupId, author, title, text)){
							writeCode(conn, Protocol::ANS_ACK);
						}else {

							writeCode(conn, Protocol::ANS_NAK);
							writeCode(conn,Protocol::ERR_NG_DOES_NOT_EXIST);
						}
						writeCode(conn, Protocol::ANS_END);
						break;
					}

					case Protocol::COM_DELETE_ART:{
						int newsgroupId = readInt(conn);
						int articleId = readInt(conn);
						if(readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("Delete art");
						}
						writeCode(conn, Protocol::ANS_DELETE_ART);
						try{
							database.removeArticle(newsgroupId, articleId);
							writeCode(conn, Protocol::ANS_ACK);
						}catch(NoNewsgroupException e){
							writeCode(conn, Protocol::ANS_NAK);
							writeCode(conn,Protocol::ERR_NG_DOES_NOT_EXIST);
						}catch(NoArticleException e){
							writeCode(conn, Protocol::ANS_NAK);
							writeCode(conn,Protocol::ERR_ART_DOES_NOT_EXIST);
						}
						writeCode(conn, Protocol::ANS_END);
						break;
					}

					case Protocol::COM_GET_ART:{
						int newsgroupId = readInt(conn);
						int articleId = readInt(conn);
						if(readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("Get art");
						}
						writeCode(conn, Protocol::ANS_GET_ART);
						try{
							Article article = database.getArticle(newsgroupId, articleId);
							writeCode(conn, Protocol::ANS_ACK);
							writeString(conn, article.getTitle());
							writeString(conn, article.getAuthor());
							writeString(conn, article.getText());
						}catch(NoNewsgroupException e){
							writeCode(conn, Protocol::ANS_NAK);
							writeCode(conn,Protocol::ERR_NG_DOES_NOT_EXIST);
						}catch(NoArticleException e){
							writeCode(conn, Protocol::ANS_NAK);
							writeCode(conn,Protocol::ERR_ART_DOES_NOT_EXIST);
						}
						writeCode(conn, Protocol::ANS_END);
						break;
					}

					case Protocol::COM_END:
					throw ProtocolViolationException("Unexpected end");
					break;

					default:
					throw ProtocolViolationException("wtf");
					break;
				}	
				cout << "current database"<<endl;
				vector<Newsgroup> groups = database.getNewsgroups();
				cout << groups.size()<<endl;
				for(auto it = groups.begin(); it < groups.end(); ++it){
					cout << it->getID()<<endl;
					cout << it->getName()<<endl;
				}
			} catch (ProtocolViolationException& e) {
				cout << e.what() << endl;
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

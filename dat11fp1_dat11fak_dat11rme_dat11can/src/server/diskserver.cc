#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "diskdatabase.h"
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

int main(int argc, char* argv[]){
	cout << "Starting disk server... " << endl;
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

	DiskDatabase database;
	cout << "Now listening on port " << argv[1] << endl;
	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
				int kase = ServerMessageHandler::readCode(conn);
				switch(kase){
					case Protocol::COM_LIST_NG:
					ServerMessageHandler::listNewsGroups(conn, database);
					break;
					case Protocol::COM_CREATE_NG:
					ServerMessageHandler::createNewsGroups(conn, database);
					break;
					case Protocol::COM_DELETE_NG:
					ServerMessageHandler::deleteNewsGroups(conn, database);
					break;
					case Protocol::COM_LIST_ART:
					ServerMessageHandler::listArticle(conn, database);
					break;
					case Protocol::COM_CREATE_ART:
					ServerMessageHandler::createArticle(conn, database);
					break;
					case Protocol::COM_DELETE_ART:
					ServerMessageHandler::deleteArticle(conn, database);
					break;
					case Protocol::COM_GET_ART:
					ServerMessageHandler::getArticle(conn, database);
					break;
					case Protocol::COM_END:
					throw ProtocolViolationException("Unexpected COM_END.");
					break;
					default:
					throw ProtocolViolationException("Unknown error.");
					break;
				}
				vector<Newsgroup> groups = database.getNewsgroups();
			} catch (ProtocolViolationException& e) {
				cout << e.what() << endl;
				server.deregisterConnection(conn);
				cout << "Closed connecion to misbehaving client" << endl;
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

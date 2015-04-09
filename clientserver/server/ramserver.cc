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

void listNewsGroups(const shared_ptr<Connection>& conn, MemoryDatabase& database);

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
	cout << "Now listening on port " << argv[1] << endl;
	while (true) {

		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
				int kase = ServerMessageHandler::readCode(conn);
				switch(kase){
					case Protocol::COM_LIST_NG:{
						listNewsGroups(conn, database);
						break;
					}
					case Protocol::COM_CREATE_NG:{
						string name = ServerMessageHandler::readString(conn);
						if(ServerMessageHandler::readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("COM_CREATE_NG");
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_CREATE_NG);
						if(database.addNewsgroup(name)){
							ServerMessageHandler::writeCode(conn, Protocol::ANS_ACK);
						} else {
							ServerMessageHandler::writeCode(conn, Protocol::ANS_NAK);
							ServerMessageHandler::writeCode(conn, Protocol::ERR_NG_ALREADY_EXISTS);
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_END);
						
						break;
					}
					case Protocol::COM_DELETE_NG:{
						size_t nGroupID = static_cast<size_t>(ServerMessageHandler::readInt(conn));
						if(ServerMessageHandler::readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("COM_DELETE_NG");
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_DELETE_NG);
						if(database.removeNewsgroup(nGroupID)){
							ServerMessageHandler::writeCode(conn, Protocol::ANS_ACK);
						} else {
							ServerMessageHandler::writeCode(conn, Protocol::ANS_NAK);
							ServerMessageHandler::writeCode(conn, Protocol::ERR_NG_DOES_NOT_EXIST);
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_END);
						break;
					}
					case Protocol::COM_LIST_ART:{
						size_t nGroupID = static_cast<size_t>(ServerMessageHandler::readInt(conn));
						if(ServerMessageHandler::readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("COM_LIST_ART");
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_LIST_ART);
						try{
							vector<Article> articles = database.getArticles(nGroupID);
							ServerMessageHandler::writeCode(conn, Protocol::ANS_ACK);
							ServerMessageHandler::writeNumber(conn, articles.size());
							for(auto it = articles.begin(); it < articles.end(); ++it){
								ServerMessageHandler::writeNumber(conn, it->getID());
								ServerMessageHandler::writeString(conn, it->getTitle());
							}

						} catch (NoNewsgroupException nne){
							ServerMessageHandler::writeCode(conn, Protocol::ANS_NAK);
							ServerMessageHandler::writeCode(conn, Protocol::ERR_NG_DOES_NOT_EXIST);
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_END);
						break;
					}
					case Protocol::COM_CREATE_ART:{
						int newsgroupId = ServerMessageHandler::readInt(conn);
						string author = ServerMessageHandler::readString(conn);
						string title = ServerMessageHandler::readString(conn);
						string text = ServerMessageHandler::readString(conn);
						if(ServerMessageHandler::readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("COM_CREATE_ART");
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_CREATE_ART);
						if(database.addArticle(newsgroupId, author, title, text)){
							ServerMessageHandler::writeCode(conn, Protocol::ANS_ACK);
						}else {

							ServerMessageHandler::writeCode(conn, Protocol::ANS_NAK);
							ServerMessageHandler::writeCode(conn,Protocol::ERR_NG_DOES_NOT_EXIST);
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_END);
						break;
					}

					case Protocol::COM_DELETE_ART:{
						int newsgroupId = ServerMessageHandler::readInt(conn);
						int articleId = ServerMessageHandler::readInt(conn);
						if(ServerMessageHandler::readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("COM_DELETE_ART");
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_DELETE_ART);
						try{
							database.removeArticle(newsgroupId, articleId);
							ServerMessageHandler::writeCode(conn, Protocol::ANS_ACK);
						}catch(NoNewsgroupException e){
							ServerMessageHandler::writeCode(conn, Protocol::ANS_NAK);
							ServerMessageHandler::writeCode(conn,Protocol::ERR_NG_DOES_NOT_EXIST);
						}catch(NoArticleException e){
							ServerMessageHandler::writeCode(conn, Protocol::ANS_NAK);
							ServerMessageHandler::writeCode(conn,Protocol::ERR_ART_DOES_NOT_EXIST);
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_END);
						break;
					}

					case Protocol::COM_GET_ART:{
						int newsgroupId = ServerMessageHandler::readInt(conn);
						int articleId = ServerMessageHandler::readInt(conn);
						if(ServerMessageHandler::readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("COM_GET_ART");
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_GET_ART);
						try{
							Article article = database.getArticle(newsgroupId, articleId);
							ServerMessageHandler::writeCode(conn, Protocol::ANS_ACK);
							ServerMessageHandler::writeString(conn, article.getTitle());
							ServerMessageHandler::writeString(conn, article.getAuthor());
							ServerMessageHandler::writeString(conn, article.getText());
						}catch(NoNewsgroupException e){
							ServerMessageHandler::writeCode(conn, Protocol::ANS_NAK);
							ServerMessageHandler::writeCode(conn,Protocol::ERR_NG_DOES_NOT_EXIST);
						}catch(NoArticleException e){
							ServerMessageHandler::writeCode(conn, Protocol::ANS_NAK);
							ServerMessageHandler::writeCode(conn,Protocol::ERR_ART_DOES_NOT_EXIST);
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_END);
						break;
					}

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

void listNewsGroups(const shared_ptr<Connection>& conn, MemoryDatabase& database){
	if(ServerMessageHandler::readCode(conn) != Protocol::COM_END){
		throw ProtocolViolationException("COM_LIST_NG");
	}
	ServerMessageHandler::writeCode(conn, Protocol::ANS_LIST_NG);
	vector<Newsgroup> groups = database.getNewsgroups();
	ServerMessageHandler::writeNumber(conn, groups.size());
	for(auto it = groups.begin(); it < groups.end(); ++it){
		ServerMessageHandler::writeNumber(conn, it->getID());
		ServerMessageHandler::writeString(conn, it->getName());
	}
	ServerMessageHandler::writeCode(conn, Protocol::ANS_END);
}
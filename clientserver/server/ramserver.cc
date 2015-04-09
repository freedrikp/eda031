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
				int kase = ServerMessageHandler::readCode(conn);
				cout << "kase: " << kase << endl;
				switch(kase){
					case Protocol::COM_LIST_NG:{
						cout << "@COM_LIST_NG" << endl;
						if(ServerMessageHandler::readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("Create ng");
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_LIST_NG);
						vector<Newsgroup> groups = database.getNewsgroups();
						cout << "writing number of groups:" << groups.size() <<endl;
						ServerMessageHandler::writeNumber(conn, groups.size());
						for(auto it = groups.begin(); it < groups.end(); ++it){
							cout << "writing nummber: "<< it->getID() << " and name " <<it->getName()<<endl;
							ServerMessageHandler::writeNumber(conn, it->getID());
							ServerMessageHandler::writeString(conn, it->getName());
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_END);
						break;
					}
					case Protocol::COM_CREATE_NG:{
						cout << "@COM_CREATE_NG" << endl;
						string name = ServerMessageHandler::readString(conn);
						if(ServerMessageHandler::readCode(conn) != Protocol::COM_END){
								throw ProtocolViolationException("List ng");
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
						cout << "@COM_DELETE_NG" << endl;
						size_t nGroupID = static_cast<size_t>(ServerMessageHandler::readInt(conn));
						if(ServerMessageHandler::readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("Delete ng");
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
						cout << "@COM_LIST_ART" << endl;
						size_t nGroupID = static_cast<size_t>(ServerMessageHandler::readInt(conn));
						if(ServerMessageHandler::readCode(conn) != Protocol::COM_END){
							throw ProtocolViolationException("List art");
						}
						ServerMessageHandler::writeCode(conn, Protocol::ANS_LIST_ART);
						try{
							cout << "# apa 1" << endl;
							vector<Article> articles = database.getArticles(nGroupID);
							cout << "# apa 2" << endl;
							ServerMessageHandler::writeCode(conn, Protocol::ANS_ACK);
							ServerMessageHandler::writeNumber(conn, articles.size());
							for(auto it = articles.begin(); it < articles.end(); ++it){
								cout << "writing number: "<< it->getID() << " and name " <<it->getTitle()<<endl;
								ServerMessageHandler::writeNumber(conn, it->getID());
								ServerMessageHandler::writeString(conn, it->getTitle());
							}

						} catch (NoNewsgroupException nne){
							cout << "# apa 9" << endl;
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
							throw ProtocolViolationException("Create art");
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
							throw ProtocolViolationException("Delete art");
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
							throw ProtocolViolationException("Get art");
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

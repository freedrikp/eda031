#include <string>
#include "connection.h"

using namespace std;

class ServerMessageHandler{
public:
	static void listNewsGroups(const shared_ptr<Connection>& conn, Database& database){
		if(readCode(conn) != Protocol::COM_END){
			throw ProtocolViolationException("COM_LIST_NG");
		}
		writeCode(conn, Protocol::ANS_LIST_NG);
		vector<Newsgroup> groups = database.getNewsgroups();
		writeNumber(conn, groups.size());
		for(auto it = groups.begin(); it < groups.end(); ++it){
			writeNumber(conn, it->getID());
			writeString(conn, it->getName());
		}
		writeCode(conn, Protocol::ANS_END);
	}

	static void createNewsGroups(const shared_ptr<Connection>& conn, Database& database){
		string name = readString(conn);
		if(readCode(conn) != Protocol::COM_END){
			throw ProtocolViolationException("COM_CREATE_NG");
		}
		writeCode(conn, Protocol::ANS_CREATE_NG);
		if(database.addNewsgroup(name)){
			writeCode(conn, Protocol::ANS_ACK);
		} else {
			writeCode(conn, Protocol::ANS_NAK);
			writeCode(conn, Protocol::ERR_NG_ALREADY_EXISTS);
		}
		writeCode(conn, Protocol::ANS_END);
	}			

	static void deleteNewsGroups(const shared_ptr<Connection>& conn, Database& database){
		size_t nGroupID = static_cast<size_t>(readInt(conn));
		if(readCode(conn) != Protocol::COM_END){
			throw ProtocolViolationException("COM_DELETE_NG");
		}
		writeCode(conn, Protocol::ANS_DELETE_NG);
		if(database.removeNewsgroup(nGroupID)){
			writeCode(conn, Protocol::ANS_ACK);
		} else {
			writeCode(conn, Protocol::ANS_NAK);
			writeCode(conn, Protocol::ERR_NG_DOES_NOT_EXIST);
		}
		writeCode(conn, Protocol::ANS_END);
	}

	static void listArticle(const shared_ptr<Connection>& conn, Database& database){
		size_t nGroupID = static_cast<size_t>(readInt(conn));
		if(readCode(conn) != Protocol::COM_END){
			throw ProtocolViolationException("COM_LIST_ART");
		}
		writeCode(conn, Protocol::ANS_LIST_ART);
		try{
			vector<Article> articles = database.getArticles(nGroupID);
			writeCode(conn, Protocol::ANS_ACK);
			writeNumber(conn, articles.size());
			for(auto it = articles.begin(); it < articles.end(); ++it){
				writeNumber(conn, it->getID());
				writeString(conn, it->getTitle());
			}

		} catch (NoNewsgroupException nne){
			writeCode(conn, Protocol::ANS_NAK);
			writeCode(conn, Protocol::ERR_NG_DOES_NOT_EXIST);
		}
		writeCode(conn, Protocol::ANS_END);
	}

	static void createArticle(const shared_ptr<Connection>& conn, Database& database){
		int newsgroupId = readInt(conn);
		string author = readString(conn);
		string title = readString(conn);
		string text = readString(conn);
		if(readCode(conn) != Protocol::COM_END){
			throw ProtocolViolationException("COM_CREATE_ART");
		}
		writeCode(conn, Protocol::ANS_CREATE_ART);
		if(database.addArticle(newsgroupId, author, title, text)){
			writeCode(conn, Protocol::ANS_ACK);
		}else {

			writeCode(conn, Protocol::ANS_NAK);
			writeCode(conn,Protocol::ERR_NG_DOES_NOT_EXIST);
		}
		writeCode(conn, Protocol::ANS_END);
	}

	static void deleteArticle(const shared_ptr<Connection>& conn, Database& database){
		int newsgroupId = readInt(conn);
		int articleId = readInt(conn);
		if(readCode(conn) != Protocol::COM_END){
			throw ProtocolViolationException("COM_DELETE_ART");
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
	}

	static void getArticle(const shared_ptr<Connection>& conn, Database& database){
		int newsgroupId = readInt(conn);
		int articleId = readInt(conn);
		if(readCode(conn) != Protocol::COM_END){
			throw ProtocolViolationException("COM_GET_ART");
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
	}

	/*
	* Read an integer from a client.
	*/
	static unsigned char readCode(const shared_ptr<Connection>& conn) {
		return conn->read();
	}				

private:
	static int readBytes(const shared_ptr<Connection>& conn) {
		unsigned char byte1 = conn->read();
		unsigned char byte2 = conn->read();
		unsigned char byte3 = conn->read();
		unsigned char byte4 = conn->read();
		return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
	}

	/*
	* Read an integer from a client.
	*/
	static int readInt(const shared_ptr<Connection>& conn) {
		if(readCode(conn) != Protocol::PAR_NUM){
			throw ProtocolViolationException("readInt");
		}
		return readBytes(conn);
	}

	/*
	* Read an integer from a client.
	*/
	static string readString(const shared_ptr<Connection>& conn) {
		if(readCode(conn) != Protocol::PAR_STRING){
			throw ProtocolViolationException("readString");
		}
		string s;
		int n = readBytes(conn);

		for(int i = 0; i < n; ++i){
			s += readCode(conn);
		}
		return s;
	}

	/*
	* Send an code to the server as one byte.
	*/
	static void writeCode(const shared_ptr<Connection>& conn, unsigned char value) {
		conn->write(value);
	}

	static void writeBytes(const shared_ptr<Connection>& conn, int value) {
		conn->write((value >> 24) & 0xFF);
		conn->write((value >> 16) & 0xFF);
		conn->write((value >> 8) & 0xFF);
		conn->write(value & 0xFF);
	}

	/*
	* Send an integer to the server as four bytes.
	*/
	static void writeNumber(const shared_ptr<Connection>& conn, int value) {
		conn->write(Protocol::PAR_NUM);
		writeBytes(conn, value);
	}

	/*
	* Send a string to a client.
	*/
	static void writeString(const shared_ptr<Connection>& conn, const string& s) {
		conn->write(Protocol::PAR_STRING);
		size_t value = s.size();
		writeBytes(conn, value);
		for (char c : s) {
			conn->write(c);
		}

	}
};

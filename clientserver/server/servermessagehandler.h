#include <string>
#include "connection.h"

using namespace std;

class ServerMessageHandler{
public:
	/*
	* Read an integer from a client.
	*/
	static unsigned char readCode(const shared_ptr<Connection>& conn) {
		return conn->read();
	}

	/*
	* Read an integer from a client.
	*/
	static int readInt(const shared_ptr<Connection>& conn) {
		if(readCode(conn) != Protocol::PAR_NUM){
			throw ProtocolViolationException("Read Int");
		}
		return readBytes(conn);
	}

	/*
	* Read an integer from a client.
	*/
	static string readString(const shared_ptr<Connection>& conn) {
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
	static void writeString(const shared_ptr<Connection>& conn, const string& s) {
		conn->write(Protocol::PAR_STRING);
		size_t value = s.size();
		writeBytes(conn, value);
		for (char c : s) {
			conn->write(c);
		}

	}

	/*
	* Send an code to the server as one byte.
	*/
	static void writeCode(const shared_ptr<Connection>& conn, unsigned char value) {
		conn->write(value);
	}

	/*
	* Send an integer to the server as four bytes.
	*/
	static void writeNumber(const shared_ptr<Connection>& conn, int value) {
		conn->write(Protocol::PAR_NUM);
		writeBytes(conn, value);
	}

private:
	static int readBytes(const shared_ptr<Connection>& conn) {
		unsigned char byte1 = conn->read();
		unsigned char byte2 = conn->read();
		unsigned char byte3 = conn->read();
		unsigned char byte4 = conn->read();
		return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
	}

	static void writeBytes(const shared_ptr<Connection>& conn, int value) {
		conn->write((value >> 24) & 0xFF);
		conn->write((value >> 16) & 0xFF);
		conn->write((value >> 8) & 0xFF);
		conn->write(value & 0xFF);
	}
};

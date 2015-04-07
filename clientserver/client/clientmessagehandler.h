#include <string>
#include "connection.h"

class ClientMessageHandler{
public:
  ClientMessageHandler(Connection& conn);
  ~ClientMessageHandler() = default;
  void listNewsgroups();
  void createNewsgroup(std::string& name);
  void deleteNewsgroup(int groupID);
  void listArticles(int groupID);
  void createArticle(int groupID, std::string title, std::string author, std::string text);
  void deleteArticle(int groupID, int articleID);
  void getArticle(int groupID, int articleID);

private:
  const Connection& conn;

  unsigned char readCode();
  void writeCode(unsigned char value);
  int readNumber();
  void writeNumber(int value);
  std::string readString();
  void writeString(const std::string& s);
};

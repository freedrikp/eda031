#include <iostream>
#include <string>
#include "clientmessagehandler.h"


enum Selection {LIST_NG = 1,CREATE_NG = 2,DELETE_NG = 3,LIST_ART = 4,CREATE_ART = 5,DELETE_ART = 6,GET_ART = 7};


Selection queryUserMenu(){
  std::cout << "------------------------------" << std::endl;
  std::cout << "What is my bidding, master?" << std::endl << std::endl;
  std::cout << "List newsgroups - 1" << std::endl;
  std::cout << "Create newsgroup - 2" << std::endl;
  std::cout << "Delete newsgroup - 3" << std::endl;
  std::cout << "List articles - 4" << std::endl;
  std::cout << "Create article - 5" << std::endl;
  std::cout << "Delete article - 6" << std::endl;
  std::cout << "Get article - 7" << std::endl;
  std::cout << "------------------------------" << std::endl << "Input number: ";
  int selection;
  std::cin >> selection;
  std::cout << std::endl;
  return static_cast<Selection>(selection);
}


void interact(ClientMessageHandler& client){
  while(true){
    Selection sel = queryUserMenu();
    switch (sel){

      case LIST_NG: {
        client.listNewsgroups();
        break;
      }
      case CREATE_NG: {
        std::cout << "What should the newsgroup be called?" << std::endl;
        std::string name;
        std::cin.ignore();
        std::getline(std::cin,name);
        client.createNewsgroup(name);
        break;
      }
      case DELETE_NG: {
        std::cout << "Which newsgroup do you want to remove?" << std::endl;
        int groupID;
        std::cin >> groupID;
        client.deleteNewsgroup(groupID);
        break;
      }
      case LIST_ART: {
        std::cout << "For which newsgroup do you wish to list articles?" << std::endl;
        int groupID;
        std::cin >> groupID;
        client.listArticles(groupID);
        break;
      }
      case CREATE_ART: {
        std::cout << "For which newsgroup do you wish to create the article?" << std::endl;
        int groupID;
        std::cin >> groupID;
        std::cout << "What should the title be?" << std::endl;
        std::string title;
        std::cin.ignore();
        std::getline(std::cin,title);
        std::cout << "Who is the author?" << std::endl;
        std::string author;
        std::getline(std::cin,author);
        std::cout << "What is the text of the article?" << std::endl;
        std::string text;
        std::getline(std::cin,text);
        client.createArticle(groupID,title,author,text);
        break;
      }
      case DELETE_ART: {
        std::cout << "In which newsgroup is the article located?" << std::endl;
        int groupID;
        std::cin >> groupID;
        std::cout << "Which article is it?" << std::endl;
        int articleID;
        std::cin >> articleID;
        client.deleteArticle(groupID,articleID);
        break;
      }
      case GET_ART: {
        std::cout << "In which newsgroup is the article located?" << std::endl;
        int groupID;
        std::cin >> groupID;
        std::cout << "Which article is it?" << std::endl;
        int articleID;
        std::cin >> articleID;
        client.getArticle(groupID,articleID);
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

  ClientMessageHandler client(conn);
  interact(client);
}

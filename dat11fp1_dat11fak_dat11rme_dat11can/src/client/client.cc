#include <iostream>
#include <string>
#include "clientmessagehandler.h"


enum Selection {LIST_NG = 1,CREATE_NG = 2,DELETE_NG = 3,LIST_ART = 4,CREATE_ART = 5,DELETE_ART = 6,GET_ART = 7};

bool headingNewline = false;

int promptInt(std::string message){
  std::cout << message << std::endl;
  int input;
  while(!(std::cin >> input).good()){
	  std::cin.clear();
    std::cin.ignore(10000,'\n');
    std::cout << "Not a number, try again!" << std::endl;
  }
  std::cout << std::endl;
  headingNewline = true;
  return input;
}

std::string promptString(std::string message){
  std::cout << message << std::endl;
  std::string input;
  if (headingNewline){
    std::cin.ignore();
  }
  std::getline(std::cin,input);
  std::cout << std::endl;
  headingNewline = false;
  return input;
}

Selection queryUserMenu(){
  std::cout << "------------------------------" << std::endl;
  std::cout << "What is thy bidding, my master?" << std::endl << std::endl;
  std::cout << "List newsgroups - 1" << std::endl;
  std::cout << "Create newsgroup - 2" << std::endl;
  std::cout << "Delete newsgroup - 3" << std::endl;
  std::cout << "List articles - 4" << std::endl;
  std::cout << "Create article - 5" << std::endl;
  std::cout << "Delete article - 6" << std::endl;
  std::cout << "Get article - 7" << std::endl;
  std::cout << "------------------------------" << std::endl;
  int selection = promptInt("Input number: ");
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
        std::string name = promptString("What should the newsgroup be called?");
        client.createNewsgroup(name);
        break;
      }
      case DELETE_NG: {
        int groupID = promptInt("Which newsgroup do you want to remove?");
        client.deleteNewsgroup(groupID);
        break;
      }
      case LIST_ART: {
        int groupID = promptInt("For which newsgroup do you wish to list articles?");
        client.listArticles(groupID);
        break;
      }
      case CREATE_ART: {
        int groupID = promptInt("For which newsgroup do you wish to create the article?");
        std::string title = promptString("What should the title be?");
        std::string author = promptString("Who is the author?");
        std::string text = promptString("What is the text of the article?");
        client.createArticle(groupID,title,author,text);
        break;
      }
      case DELETE_ART: {
        int groupID = promptInt("In which newsgroup is the article located?");
        int articleID = promptInt("Which article is it?");
        client.deleteArticle(groupID,articleID);
        break;
      }
      case GET_ART: {
        int groupID = promptInt("In which newsgroup is the article located?");
        int articleID = promptInt("Which article is it?");
        client.getArticle(groupID,articleID);
        break;
      }
      default:
	      std::cout << "Wrong selection: " <<sel<< std::endl;
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



  ClientMessageHandler client(argv[1],port);
  interact(client);
}

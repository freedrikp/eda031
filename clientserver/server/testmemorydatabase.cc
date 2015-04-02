#include <string>
#include "memoryDatabase.h"
#include <iostream>

int main(){
  MemoryDatabase memdb;
  std::string start = "a";
  for (char i = 'b'; i != 'b' + 10; ++i){
    std::cout << std::boolalpha << "Adding newsgroup: " <<  memdb.addNewsgroup(start+i) << std::endl;
  }
  for (auto elem : memdb.getNewsgroups()){
      std::cout << elem.getID() << " " << elem.getName() << std::endl;
  }

  std::cout << "Trying to add existing newsgroup: " << memdb.addNewsgroup("ab") << std::endl;
  for (int i = 0; i != 10; ++i){
    std::cout << "Adding article: " <<  memdb.addArticle(i+1,start+static_cast<char>(i + 'b'), "fredrik", "baaaaaaaast") << std::endl;
  }


    for (auto elem : memdb.getNewsgroups()){
      std::cout << "loop" << std::endl;
      for (auto elem1 : memdb.getArticles(elem.getID())){
          std::cout << elem1.getID() << " " << elem1.getTitle() << " " << elem1.getAuthor() << " " << elem1.getText() << std::endl;
        }
    }
}

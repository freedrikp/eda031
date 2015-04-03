#include <string>
#include "memoryDatabase.h"
#include <iostream>

void printStuff(MemoryDatabase memdb){
  std::cout << "Groups and Articles: " << std::endl;
  for (auto elem : memdb.getNewsgroups()){
    auto list = memdb.getArticles(elem.getID());
    //std::cout << "loop, size of list: " << list.size() << std::endl;
    std::cout << "group: " <<elem.getID() << " ";
    if (list.empty()){
      std::cout << std::endl;
    }
     for (auto elem1 : list){
        std::cout << "article: "<<elem1.getID() << " " << elem1.getTitle() << " " << elem1.getAuthor() << " " << elem1.getText() << std::endl;
      }
  }
}

int main(){
  MemoryDatabase memdb;
  std::string start = "a";
  for (char i = 'b'; i != 'b' + 10; ++i){
    std::cout << std::boolalpha << "Adding newsgroup: " <<  memdb.addNewsgroup(start+i) << std::endl;
  }
  std::cout << "Articles:" << std::endl;
  for (auto elem : memdb.getNewsgroups()){
      std::cout << elem.getID() << " " << elem.getName() << std::endl;
  }

  std::cout << "Trying to add existing newsgroup: " << memdb.addNewsgroup("ab") << std::endl;
  for (int i = 0; i != 10; ++i){
    std::cout << "Adding article to group " << i+1 << " : " <<  memdb.addArticle(i+1,start, "fredrik", "baaaaaaaast") << std::endl;
  }


    printStuff(memdb);

    std::cout << "Removing newsgroup 5: " << memdb.removeNewsgroup(5) << std::endl;

    printStuff(memdb);

    std::cout << "Removing article in group 3 article 1: " << memdb.removeArticle(3,1) << std::endl;

    printStuff(memdb);

    std::cout << "Removing nonexistent newsgroup: " << memdb.removeNewsgroup(100) << std::endl;

    std::cout << "Removing nonexistent article: " << memdb.removeArticle(200,33) << std::endl;

    std::cout << "Getting article 1 in group 7: ";

    Article elem1 = memdb.getArticle(7,1);
    std::cout << "article: "<<elem1.getID() << " " << elem1.getTitle() << " " << elem1.getAuthor() << " " << elem1.getText() << std::endl;

}

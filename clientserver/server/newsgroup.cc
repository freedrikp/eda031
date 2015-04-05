#include "newsgroup.h"
#include "article.h"
#include <vector>


Newsgroup::Newsgroup(std::string name, size_t id): name(name), id(id), article_counter(0){}

size_t Newsgroup::getID() const{
  return id;
}

std::string Newsgroup::getName() const{
  return name;
}

Newsgroup::~news_group(){
}

void Newsgroup::new_article(std::string& title, std::string& author, std::string& content){
      article art(article_ids, title, author,content);
      article_counter++;
      add_article(art);
}

void Newsgroup::add_article(article a){
      articles.push_back(a);
}

bool Newsgroup::remove_article(int id){

}

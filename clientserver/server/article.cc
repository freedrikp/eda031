#include "article.h"

Article::Article(std::string title, std::string author, std::string text, size_t id): title(title), author(author), text(text), id(id){}

size_t Article::getID(){
  return id;
}

std::string Article::getTitle(){
  return title;
}

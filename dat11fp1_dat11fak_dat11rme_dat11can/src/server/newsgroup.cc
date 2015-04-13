#include "newsgroup.h"
#include "article.h"
#include <vector>


Newsgroup::Newsgroup(std::string name, size_t id): name(name), id(id){}

size_t Newsgroup::getID() const{
  return id;
}

std::string Newsgroup::getName() const{
  return name;
}

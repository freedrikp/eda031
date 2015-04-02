#include "newsgroup.h"

Newsgroup::Newsgroup(std::string name, size_t id): name(name), id(id){}

size_t Newsgroup::getID(){
  return id;
}

std::string Newsgroup::getName(){
  return name;
}

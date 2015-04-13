#ifndef NGRP_H
#define NGRP_H
#include <string>
#include <cstdlib>

class Newsgroup{
public:
  Newsgroup(std::string name, size_t id);
  size_t getID() const;
  std::string getName() const;
  ~Newsgroup() = default;
private:
  std::string name;
  size_t id;
};
#endif

#ifndef NGRP_H
#define NGRP_H
#include <string>
#include <cstdlib>

class Newsgroup{
public:
  Newsgroup(std::string name, size_t id);
  size_t getID();
  std::string getName();
private:
  std::  string name;
  size_t id;
};
#endif

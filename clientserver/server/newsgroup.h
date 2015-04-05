#ifndef NGRP_H
#define NGRP_H
#include <string>
#include <cstdlib>

class Newsgroup{
public:
  Newsgroup(std::string name, size_t id);
  size_t getID() const;
  std::string getName() const;
  void new_article(std::string& content, std::string& title, std::string& author);
  bool remove_article(int id);
  /*Vet inte om vi kom fram till att använda vector eller inte, kör på den så länge iaf,
  fungerar ju men unorderset också ju. De ska sorteras på äldst först, men det kan man ju isf använda
  id som nyckel eller så lägger man till en sortera i article (gör inte det nu iaf) */
  std::vector<article>* list_articles();
  /* Man skulle väl kunna skapa en artikel och senare lägga till newsgroups? då e det enklast med add_article*/
  void add_article(article art);
  /* Sedan behövs något för att plocka ut rätt, antingen på id eller namn när man ska presentera,
  beror också på lagringsförfarande*/
  ~Newsgroup();
private:
  std::string name;
  size_t id;
  std::vector<article> articles;
  int article_counter;
};
#endif

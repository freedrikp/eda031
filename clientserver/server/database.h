#ifndef DATABASE_H
#define DATABASE_H
#include <cstdlib>
#include <string>
#include <vector>
#include "article.h"
#include "newsgroup.h"

class Database{
public:
  virtual std::vector<Newsgroup> getNewsgroups() = 0;
  virtual std::vector<Article> getArticles(size_t nGroupID) = 0;
  virtual bool addNewsgroup(std::string name) = 0;
  virtual bool removeNewsgroup(size_t nGroupID) = 0;
  virtual bool addArticle(size_t nGroupID, std::string title, std::string author, std::string text) = 0;
  virtual bool removeArticle(size_t nGroupID, size_t articleID) = 0;
};
#endif

#ifndef MEMDB_H
#define MEMDB_H
#include "database.h"

class MemoryDatabase : Databse {
public:
  vector<Newsgroup> Database::getNewsgroups();
  vector<Article> Database::getArticles(size_t nGroupID);
  size_t Database::addNewsgroup(string name);
  bool Database::removeNewsgroup(size_t nGroupID);
  size_t Database::addArticle(size_t nGroupID, string title, string author, string text);
  bool Database::removeArticle(size_t nGroupID, size_t articleID);
}
#endif

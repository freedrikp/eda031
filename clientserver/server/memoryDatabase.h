#ifndef MEMDB_H
#define MEMDB_H
#include "database.h"
#include <unordered_map>
#include "newsgroup.h"
#include "article.h"

class MemoryDatabase : Database {
public:
  MemoryDatabase() = default;
  ~MemoryDatabase() = default;
  std::vector<Newsgroup> getNewsgroups();
  std::vector<Article> getArticles(size_t nGroupID);
  bool addNewsgroup(std::string name);
  bool removeNewsgroup(size_t nGroupID);
  bool addArticle(size_t nGroupID, std::string title, std::string author, std::string text);
  bool removeArticle(size_t nGroupID, size_t articleID);
private:
  std::unordered_map<size_t,Newsgroup> newsGroups;
  std::unordered_map<size_t,std::unordered_map<size_t,Article>> articles;
  std::unordered_map<size_t,size_t> articleCounters;
  size_t newsGroupCounter;
};
#endif

#ifndef MEMDB_H
#define MEMDB_H
#include "database.h"
#include <unordered_map>
#include "newsgroup.h"
#include "article.h"

class MemoryDatabase : Database {
public:
  MemoryDatabase();
  ~MemoryDatabase() = default;
  std::vector<Newsgroup> getNewsgroups();
  std::vector<Article> getArticles(size_t nGroupID);
  Article getArticle(size_t nGroupID, size_t articleID);
  bool addNewsgroup(std::string name);
  void removeNewsgroup(size_t nGroupID);
  void addArticle(size_t nGroupID, std::string title, std::string author, std::string text);
  void removeArticle(size_t nGroupID, size_t articleID);
private:
  std::unordered_map<size_t,Newsgroup> newsGroups;
  std::unordered_map<size_t,std::unordered_map<size_t,Article>> articles;
  std::unordered_map<size_t,size_t> articleCounters;
  size_t newsGroupCounter;
};
#endif

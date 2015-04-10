#ifndef DSKDB_H
#define DSKDB_H
#include "database.h"
#include <unordered_map>
#include "newsgroup.h"
#include "article.h"
#include <fstream>

class DiskDatabase : public Database {
public:
  DiskDatabase();
  ~DiskDatabase() = default;
  std::vector<Newsgroup> getNewsgroups();
  std::vector<Article> getArticles(size_t nGroupID);
  Article getArticle(size_t nGroupID, size_t articleID);
  bool addNewsgroup(std::string name);
  bool removeNewsgroup(size_t nGroupID);
  bool addArticle(size_t nGroupID, std::string title, std::string author, std::string text);
  void removeArticle(size_t nGroupID, size_t articleID);
private:
  std::unordered_map<size_t,Newsgroup> newsGroups;
  std::unordered_map<size_t,std::unordered_map<size_t,Article>> articles;
  std::unordered_map<size_t,size_t> articleCounters;
  size_t newsGroupCounter;
  static std::string const ROOTPATH;
  static std::string const NEWSGROUPSFILE;
  static std::string const NG_IDCOUNTFILE;
  static std::string const NG_NAMEFILE;
  static std::string const ART_IDCOUNTFILE;
  static std::string const ART_TITLEFILE;
  static std::string const ART_AUTHORFILE;
  static std::string const ART_TEXTFILE;

  static bool fileExists(const std::string fileName)
  {
    std::ifstream infile(fileName);
    return infile.good();
  }
};
#endif

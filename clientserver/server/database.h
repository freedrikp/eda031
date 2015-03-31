#include <cstdlib>
#include <string>

class Database{
public:
  virtual vector<Newsgroup> getNewsgroups() = 0;
  virtual vector<Article> getArticles(size_t nGroupID) = 0;
  virtual size_t addNewsgroup(string name) = 0;
  virtual bool removeNewsgroup(size_t nGroupID) = 0;
  virtual size_t addArticle(size_t nGroupID, string title, string author, string text) = 0;
  virtual bool removeArticle(size_t articleID) = 0;
};

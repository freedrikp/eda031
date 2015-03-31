#include <string>
#include <cstdlib>

class Article {
public:
  Article(string title, string author, string text);
private:
  string title;
  string author;
  string text;
  size_t id;

};

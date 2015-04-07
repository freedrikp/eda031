#ifndef ART_H
#define ART_H
#include <string>
#include <cstdlib>

class Article {
public:
  Article(std::string title, std::string author, std::string text, size_t id);
  size_t getID() const;
  std::string getTitle() const;
  std::string getAuthor() const;
  std::string getText() const;
  ~Article() = default;
private:
  std::string title;
  std::string author;
  std::string text;
  size_t id;

};
#endif

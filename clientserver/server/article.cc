#include "article.h"

Article::Article(std::string title, std::string author, std::string text, size_t id): title(title), author(author), text(text), id(id){}

size_t Article::getID() const{
  return id;
}

std::string Article::getTitle() const{
  return title;
}

std::string Article::getAuthor() const{
  return author;
}

std::string Article::getText() const{
  return text;
}

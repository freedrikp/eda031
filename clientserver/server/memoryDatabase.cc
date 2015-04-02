#include "memoryDatabase.h"
#include <algorithm>
#include <utility>



std::vector<Newsgroup> MemoryDatabase::getNewsgroups(){
  std::vector<Newsgroup> result;
  for (auto it = newsGroups.begin(); it != newsGroups.end(); ++it){
    result.push_back(it->second);
  }
  sort(result.begin(),result.end(),[](Newsgroup& group1, Newsgroup& group2){return group1.getID() < group2.getID();});
  return result;
};

std::vector<Article> MemoryDatabase::getArticles(size_t nGroupID){
  std::vector<Article> result;
  if (articles.find(nGroupID) != articles.end()){
    for (auto it = articles[nGroupID].begin(); it != articles[nGroupID].end();                ++it){
      result.push_back(it->second);
    }
    sort(result.begin(),result.end(),[](Article& art1, Article& art2){return art1.getID() < art2.getID();});
}
  return result;
};

bool MemoryDatabase::addNewsgroup(std::string name){
  if (find_if(newsGroups.begin(), newsGroups.end(),[&name](std::pair<size_t,Newsgroup>& elem){return name == elem.second.getName();}) != newsGroups.end()){
      return false;
  }
  ++newsGroupCounter;
  Newsgroup group(name,newsGroupCounter);
  newsGroups[newsGroupCounter] = group;
  return true;
};

bool MemoryDatabase::removeNewsgroup(size_t nGroupID){
  auto it = newsGroups.find(nGroupID);
  if (it != newsGroups.end()){
    newsGroups.erase(it);
    articles[nGroupID].clear();
    return true;
  }else{
    return false;
  }
};

bool MemoryDatabase::addArticle(size_t nGroupID, std::string title, std::string author, std::string text){
   if (newsGroups.find(nGroupID) != newsGroups.end()){
     if (find_if(articles[nGroupID].begin(), articles[nGroupID].end(),[&title](std::pair<size_t,Article>& elem){return title == elem.second.getTitle();}) != articles[nGroupID].end()){
         return false;
     }
     ++articleCounters[nGroupID];
     Article art(title, author, text, articleCounters[nGroupID]);
     //articles[articleCounters[nGroupID]] = art;
     articles[articleCounters[nGroupID]].insert(std::pair<size_t,Article>(articleCounters[nGroupID],art));
     return true;
   }
     return false;
 };

bool MemoryDatabase::removeArticle(size_t nGroupID, size_t articleID){
  if (newsGroups.find(nGroupID) != newsGroups.end()){
      articles[nGroupID].erase(articles[nGroupID].find(articleID));
      return true;
  }
  return false;
};

#include "diskdatabase.h"
#include <algorithm>
#include <utility>
#include "nonewsgroupexception.h"
#include "noarticleexception.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

DiskDatabase::DiskDatabase(): newsGroupCounter(0) {
  cout << "Initializing diskdata (" << mkdir("diskdata", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) << ")" << endl;
}

bool DiskDatabase::addNewsgroup(std::string name){
  if (find_if(newsGroups.begin(), newsGroups.end(),[&name](std::unordered_map<size_t,Newsgroup>::value_type& elem){return name == elem.second.getName();}) != newsGroups.end()){
      return false;
  }
  ++newsGroupCounter;
  Newsgroup group(name,newsGroupCounter);
  newsGroups.insert(std::pair<size_t,Newsgroup>(newsGroupCounter,group));
  return true;
}

std::vector<Newsgroup> DiskDatabase::getNewsgroups(){
  std::vector<Newsgroup> result;
  for (auto it = newsGroups.begin(); it != newsGroups.end(); ++it){
    result.push_back(it->second);
  }
  sort(result.begin(),result.end(),[](const Newsgroup& group1, const Newsgroup& group2){return group1.getID() < group2.getID();});
  return result;
}

bool DiskDatabase::removeNewsgroup(size_t nGroupID){
  auto it = newsGroups.find(nGroupID);
  if (it != newsGroups.end()){
    newsGroups.erase(it);
    articles[nGroupID].clear();
    return true;
  }else{
    return false;
  }
}

bool DiskDatabase::addArticle(size_t nGroupID, std::string title, std::string author, std::string text){
   if (newsGroups.find(nGroupID) != newsGroups.end()){
     ++articleCounters[nGroupID];
     Article art(title, author, text, articleCounters[nGroupID]);
     articles[nGroupID].insert(std::pair<size_t,Article>(articleCounters[nGroupID],art));
     return true;
   }else{
     return false;
   }
 }

std::vector<Article> DiskDatabase::getArticles(size_t nGroupID){
  std::vector<Article> result;
  if (newsGroups.find(nGroupID) != newsGroups.end()){
    for (auto it = articles[nGroupID].begin(); it != articles[nGroupID].end(); ++it){
      result.push_back(it->second);
    }
    sort(result.begin(),result.end(),[](const Article& art1, const Article& art2){return art1.getID() < art2.getID();});
}else{
  throw NoNewsgroupException();
}
  return result;
}

Article DiskDatabase::getArticle(size_t nGroupID, size_t articleID){
  if (newsGroups.find(nGroupID) != newsGroups.end()){
      auto it = articles[nGroupID].find(articleID);
      if (it != articles[nGroupID].end()){
        return it->second;
      }else{
        throw NoArticleException();
      }
  }else{
    throw NoNewsgroupException();
  }
}





void DiskDatabase::removeArticle(size_t nGroupID, size_t articleID){
  if (newsGroups.find(nGroupID) != newsGroups.end()){
    auto it = articles[nGroupID].find(articleID);
    if (it != articles[nGroupID].end()){
      articles[nGroupID].erase(it);
    }else{
      throw NoArticleException();
    }
  }else{
    throw NoNewsgroupException();
  }
}

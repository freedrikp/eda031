#include "diskdatabase.h"
#include <algorithm>
#include <utility>
#include "nonewsgroupexception.h"
#include "noarticleexception.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <fstream>

using namespace std;
const string DiskDatabase::ROOTPATH = "diskdata";
const string DiskDatabase::NEWSGROUPSFILE = "newsgroups";
const string DiskDatabase::NG_IDCOUNTFILE = "ng_idcount";
const string DiskDatabase::NG_NAMEFILE = "ng_name";
const string DiskDatabase::ART_IDCOUNTFILE = "art_idcount";
const string DiskDatabase::ART_NAMEFILE = "art_name";
const string DiskDatabase::ART_AUTHORFILE = "art_author";
const string DiskDatabase::ART_TEXTFILE = "art_text";

DiskDatabase::DiskDatabase(): newsGroupCounter(0) {
  int status = mkdir(ROOTPATH.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  cout << "Initializing diskdata (" << status << ")" << endl;
}

bool DiskDatabase::addNewsgroup(std::string name){
  string path = ROOTPATH + "/" + name;
  int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (status == -1){
    return false;
  }
  return true;
}

std::vector<Newsgroup> DiskDatabase::getNewsgroups(){
  std::vector<Newsgroup> result;

  DIR* rootdir;
  struct dirent *ent;
  if ((rootdir = opendir(ROOTPATH.c_str())) != NULL) {
  /* print all the files and directories within directory */
    while ((ent = readdir(rootdir)) != NULL) {
      if (ent->d_type == DT_DIR){
        size_t newsGroupID = atoi(ent->d_name);
        string newsGroupName = "FATAL_ERROR_OOPS";
        ifstream ifs (ROOTPATH+"/"+ent->d_name+"/"+NG_NAMEFILE, ifstream::in);
        if(ifs.good()){
          getline(ifs, newsGroupName);
          printf ("%s\n", ent->d_name);
          result.push_back({newsGroupName, newsGroupID});
        }
        ifs.close();
      }
    }
    closedir (rootdir);
  }

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

  DIR* rootdir;
  struct dirent *ent;
  if ((rootdir = opendir((ROOTPATH+"/"+to_string(nGroupID)).c_str())) != NULL) {
  /* print all the files and directories within directory */
    while ((ent = readdir(rootdir)) != NULL) {
      if (ent->d_type == DT_DIR){
        size_t articleID = atoi(ent->d_name);
        string articleName = "FATAL_ERROR_OOPS";
        string articleAuthor = "FATAL_ERROR_OOPS";
        string articleText = "";
        string articlePath = ROOTPATH+"/"+to_string(nGroupID)+"/"+ent->d_name+"/";
        ifstream name_ifs (articlePath+ART_NAMEFILE, ifstream::in);
        ifstream auth_ifs (articlePath+ART_AUTHORFILE, ifstream::in);
        ifstream text_ifs (articlePath+ART_TEXTFILE, ifstream::in);
        if(name_ifs.good() && auth_ifs.good() && text_ifs.good()){
          getline(name_ifs, articleName);
          getline(auth_ifs, articleAuthor);
          while(text_ifs.good()){
            string line;
            getline(text_ifs, line);
            articleText += line + "\n";
          }
          result.push_back({articleName, articleAuthor, articleText, articleID});
        }
      }
    }
    closedir (rootdir);
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

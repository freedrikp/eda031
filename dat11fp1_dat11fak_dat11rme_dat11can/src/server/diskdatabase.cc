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
#include <unistd.h>
#include <stdio.h>
#include <sstream>

using namespace std;
const string DiskDatabase::ROOTPATH = "diskdata";
const string DiskDatabase::NEWSGROUPSFILE = "/newsgroups";
const string DiskDatabase::NG_IDCOUNTFILE = "/ng_idcount";
const string DiskDatabase::NG_NAMEFILE = "/ng_name";
const string DiskDatabase::ART_IDCOUNTFILE = "/art_idcount";
const string DiskDatabase::ART_TITLEFILE = "/art_title";
const string DiskDatabase::ART_AUTHORFILE = "/art_author";
const string DiskDatabase::ART_TEXTFILE = "/art_text";

DiskDatabase::DiskDatabase() {
  int status = mkdir(ROOTPATH.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  cout << "Initializing diskdata (" << status << ")" << endl;
  
  if(!DiskDatabase::fileExists(ROOTPATH+NEWSGROUPSFILE)){
    ofstream ofs_newsgroups(ROOTPATH+NEWSGROUPSFILE);
    ofs_newsgroups.close();
  }

  if(!DiskDatabase::fileExists(ROOTPATH+NG_IDCOUNTFILE)){
    ofstream ofs_ng_idcount(ROOTPATH+NG_IDCOUNTFILE);
    ofs_ng_idcount << "0";
    ofs_ng_idcount.close();
  }
}

bool DiskDatabase::addNewsgroup(string name){
  ifstream ifs_newsgroups (ROOTPATH+NEWSGROUPSFILE, ifstream::in);
  while(ifs_newsgroups.good()){
    string line;
    getline(ifs_newsgroups, line);
    if(name.compare(line) == 0){
      return false;
    }
  }

  size_t newsGroupID = 0;
  ifstream ifs_ng_idcount (ROOTPATH+NG_IDCOUNTFILE, ifstream::in);
  if(ifs_ng_idcount.good()){
    ifs_ng_idcount >> newsGroupID;
  }
  ifs_ng_idcount.close();
  ++newsGroupID;

  string groupPath = ROOTPATH+"/"+to_string(newsGroupID);
  mkdir(groupPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  ofstream ofs_ng_name(groupPath+NG_NAMEFILE);
  ofs_ng_name << name;
  ofs_ng_name.close();

  ofstream ofs_art_idcount(groupPath+ART_IDCOUNTFILE);
  ofs_art_idcount << "0";
  ofs_art_idcount.close();

  ofstream ofs_newsgroups (ROOTPATH+NEWSGROUPSFILE, ios_base::app);
  ofs_newsgroups << name << endl;

  ofstream ofs_ng_idcount(ROOTPATH+NG_IDCOUNTFILE);
  ofs_ng_idcount << newsGroupID;
  ofs_ng_idcount.close();

  return true;
}

std::vector<Newsgroup> DiskDatabase::getNewsgroups(){
  std::vector<Newsgroup> result;

  DIR* rootdir;
  struct dirent *ent;
  if ((rootdir = opendir(ROOTPATH.c_str())) != NULL) {
    while ((ent = readdir(rootdir)) != NULL) {
      if (ent->d_type == DT_DIR){
        size_t newsGroupID = atoi(ent->d_name);
        string newsGroupName = "FATAL_ERROR_OOPS";
        ifstream ifs (ROOTPATH+"/"+ent->d_name+NG_NAMEFILE, ifstream::in);
        if(ifs.good()){
          getline(ifs, newsGroupName);
          result.push_back({newsGroupName, newsGroupID});
        }
        ifs.close();
      }
    }
    closedir (rootdir);
  }

  sort(result.begin(),result.end(),[](const Newsgroup& group1, const Newsgroup& group2){return group1.getID() < group2.getID();});
  return result;
}

bool DiskDatabase::removeNewsgroup(size_t nGroupID){
  string groupPath = ROOTPATH+"/"+to_string(nGroupID);
  if(!DiskDatabase::fileExists(groupPath+NG_NAMEFILE)){
    return false;
  }

  DIR* rootdir;
  struct dirent *ent;
  if ((rootdir = opendir((ROOTPATH+"/"+to_string(nGroupID)).c_str())) != NULL) {
    while ((ent = readdir(rootdir)) != NULL) {
      if (ent->d_type == DT_DIR){
        size_t articleID = atoi(ent->d_name);
        removeArticle(nGroupID, articleID);
      }
    }
  }
  closedir(rootdir);

  ifstream ng_ifs (groupPath+NG_NAMEFILE, ifstream::in);
  string newsGroupName;
  ng_ifs >> newsGroupName;
  ng_ifs.close();

  remove((groupPath+ART_IDCOUNTFILE).c_str());
  remove((groupPath+NG_NAMEFILE).c_str());
  
  if(rmdir(groupPath.c_str()) == 0){
    stringstream ss;
    ifstream ifs_newsgroups(ROOTPATH+NEWSGROUPSFILE, ifstream::in);
    while(ifs_newsgroups.good()){
      string line;
      getline(ifs_newsgroups, line);
      if(newsGroupName.compare(line) != 0){
        ss << line;
        if(ifs_newsgroups.good())
          ss << endl;
      }
    }
    ifs_newsgroups.close();

    ofstream ofs_newsgroups(ROOTPATH+NEWSGROUPSFILE);
    ofs_newsgroups << ss.rdbuf();
    ofs_newsgroups.close();

    return true;
  } else
  return false;
}

bool DiskDatabase::addArticle(size_t nGroupID, string title, string author, string text){
  string groupPath = ROOTPATH+"/"+to_string(nGroupID);

  size_t articleID = 0;
  ifstream ifs_art_idcount (groupPath+ART_IDCOUNTFILE, ifstream::in);
  if(!ifs_art_idcount.good()){
    return false;
  }
  ifs_art_idcount >> articleID;
  ifs_art_idcount.close();
  ++articleID;

  string articlePath = groupPath+"/"+to_string(articleID);
  mkdir(articlePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  ofstream ofs_art_title(articlePath+ART_TITLEFILE);
  ofs_art_title << title;
  ofs_art_title.close();

  ofstream ofs_art_auth(articlePath+ART_AUTHORFILE);
  ofs_art_auth << author;
  ofs_art_auth.close();

  ofstream ofs_art_text(articlePath+ART_TEXTFILE);
  ofs_art_text << text;
  ofs_art_text.close();

  ofstream ofs_art_idcount(groupPath+ART_IDCOUNTFILE);
  ofs_art_idcount << articleID;
  ofs_art_idcount.close();
  return true;
}

std::vector<Article> DiskDatabase::getArticles(size_t nGroupID){
  std::vector<Article> result;
  DIR* rootdir;
  struct dirent *ent;
  if ((rootdir = opendir((ROOTPATH+"/"+to_string(nGroupID)).c_str())) != NULL) {
    while ((ent = readdir(rootdir)) != NULL) {
      if (ent->d_type == DT_DIR){
        size_t articleID = atoi(ent->d_name);
        result.push_back(getArticle(nGroupID, articleID));
      }
    }
    closedir (rootdir);
  }else{
    throw NoNewsgroupException();
  }
  sort(result.begin(),result.end(),[](const Article& art1, const Article& art2){return art1.getID() < art2.getID();});
  return result;
}

Article DiskDatabase::getArticle(size_t nGroupID, size_t articleID){
  string articleName = "FATAL_ERROR_OOPS";
  string articleAuthor = "FATAL_ERROR_OOPS";
  string articleText = "";
  string groupPath = ROOTPATH+"/"+to_string(nGroupID);
  ifstream ng_ifs (groupPath+NG_NAMEFILE, ifstream::in);
  if(ng_ifs.good()){
    string articlePath = groupPath+"/"+to_string(articleID);
    ifstream name_ifs (articlePath+ART_TITLEFILE, ifstream::in);
    ifstream auth_ifs (articlePath+ART_AUTHORFILE, ifstream::in);
    ifstream text_ifs (articlePath+ART_TEXTFILE, ifstream::in);
    if(name_ifs.good() && auth_ifs.good()){
      getline(name_ifs, articleName);
      getline(auth_ifs, articleAuthor);
      while(text_ifs.good()){
        string line;
        getline(text_ifs, line);
        articleText += line;
        if(text_ifs.good())
          articleText += "\n";
      }
      return {articleName, articleAuthor, articleText, articleID};
    }else{
      throw NoArticleException();
    }
  }else{
    throw NoNewsgroupException();
  }
}

void DiskDatabase::removeArticle(size_t nGroupID, size_t articleID){
  string groupPath = ROOTPATH+"/"+to_string(nGroupID)+"/";
  string articlePath = groupPath+to_string(articleID);

  if(!DiskDatabase::fileExists(groupPath+NG_NAMEFILE)){
    throw NoNewsgroupException();
  }

  if(!DiskDatabase::fileExists(articlePath+ART_TITLEFILE)){
    throw NoArticleException();
  }

  remove((articlePath+ART_TITLEFILE).c_str());
  remove((articlePath+ART_AUTHORFILE).c_str());
  remove((articlePath+ART_TEXTFILE).c_str());
  rmdir(articlePath.c_str());
}

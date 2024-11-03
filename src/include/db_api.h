
#ifndef DB_API_H
#define DB_API_H

// #include "bits/stdc++.h"
#include "sqlite_orm.h"

using namespace sqlite_orm;

// visited links
struct Link { 
    int id;
    std::string url;
    std::string title;
    int visited_time;
};

// bookmark
struct Bookmark {
    int id;  
    std::string url;
    std::string icon;
    std::string description;
};

// note
struct Note {
    int id;  
    std::string title;
    std::string content;
    // int created_time;
};

// main db object
class DBController {
    public:
        DBController();
        // ~DBCOntroller();
    
};

#endif // DB_API_H
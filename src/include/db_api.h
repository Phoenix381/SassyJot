
#ifndef DB_API_H
#define DB_API_H

// #include "bits/stdc++.h"
#include <QString>
#include <QObject>
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
class DBController : public QObject {
    Q_OBJECT
private:
        inline static auto storage =  make_storage("db.sqlite",
            make_table("links",
               make_column("id", &Link::id, primary_key().autoincrement()),
               make_column("url", &Link::url),
               make_column("title", &Link::title),
               make_column("visited_time", &Link::visited_time)
            ),
            make_table("bookmarks",
               make_column("id", &Bookmark::id, primary_key().autoincrement()),
               make_column("url", &Bookmark::url),
               make_column("icon", &Bookmark::icon),
               make_column("description", &Bookmark::description)
            )
        );
public:
        DBController();
        // ~DBCOntroller();
    
public slots:
    // CRUD functions
    int addLink(Link);
    int addBookmark(QString, QString, QString);
    void removeBookmark(QString);
    bool checkBookmark(QString);
};

#endif // DB_API_H
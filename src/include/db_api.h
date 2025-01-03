
#ifndef DB_API_H
#define DB_API_H

// #include "bits/stdc++.h"
#include <QString>
#include <QObject>
#include "sqlite_orm.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

using namespace sqlite_orm;

// =============================================================================
// db objects
// =============================================================================

// settings
struct Setting {
    std::string key;
    std::string value;
};

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

// workspace
struct Workspace {
    int id;    
    std::string name;
    std::string color;
    std::string description;
};

// note
struct Note {
    int id;  
    std::string title;
    std::string content;
    // int created_time;
};

// =============================================================================
// db objects, handles CRUD as QT events
// =============================================================================

class DBController : public QObject {
    Q_OBJECT
private:
    inline static auto storage =  make_storage("db.sqlite",
        // settings
        make_table("settings",
            make_column("key", &Setting::key, primary_key()),
            make_column("value", &Setting::value)
        ),  
        // visited links
        make_table("links",
            make_column("id", &Link::id, primary_key().autoincrement()),
            make_column("url", &Link::url),
            make_column("title", &Link::title),
            make_column("visited_time", &Link::visited_time)
        ),
        // bookmarks
        make_table("bookmarks",
            make_column("id", &Bookmark::id, primary_key().autoincrement()),
            make_column("url", &Bookmark::url),
            make_column("icon", &Bookmark::icon),
            make_column("description", &Bookmark::description)
        ),
        // workspaces
        make_table("workspaces",
            make_column("id", &Workspace::id, primary_key().autoincrement()),
            make_column("name", &Workspace::name),
            make_column("color", &Workspace::color),
            make_column("description", &Workspace::description)
        )
    );
public:
        DBController();
        // ~DBCOntroller();
    
public slots:
    // CRUD functions
    // settings
    void updateSetting(QString, QString);
    QString getSetting(QString);
    
    // urls
    int addLink(Link);
    int addBookmark(QString, QString, QString);
    void removeBookmark(QString);
    bool checkBookmark(QString);

    // workspaces
    int addWorkspace(QString, QString, QString);
    void removeWorkspace(QString);
    void getWorkspaces();
    QString getCurrentWorkspaceColor();
signals:
    void workspacesReady(QString);
};

#endif // DB_API_H
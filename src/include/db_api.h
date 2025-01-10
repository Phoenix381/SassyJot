
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

// workspace url
struct WorkspaceUrl {
    int id;
    int order;
    int workspace_id;
    std::string url;
};

// note
struct Note {
    int id;
    int workspace_id;
    std::string title;
    std::string content;
    int group_id;
    // int created_time;
};

// note group
// TODO description etc
struct NoteGroup {
    int id;
    std::string color;
    std::string name;
};

// note link
// TODO link description
struct NoteLink {
    int source_id;
    int target_id;
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
        ),
        // workspace urls
        make_table("workspace_urls",
            make_column("id", &WorkspaceUrl::id, primary_key().autoincrement()),
            make_column("order", &WorkspaceUrl::order),
            make_column("workspace_id", &WorkspaceUrl::workspace_id),
            make_column("url", &WorkspaceUrl::url)
        ),
        // notes
        make_table("notes",
            make_column("id", &Note::id, primary_key().autoincrement()),
            make_column("workspace_id", &Note::workspace_id),
            make_column("title", &Note::title),
            make_column("content", &Note::content)
        ),
        // note links
        make_table("note_links",
            make_column("source_id", &NoteLink::source_id),
            make_column("target_id", &NoteLink::target_id),
            primary_key(&NoteLink::source_id, &NoteLink::target_id)
        ),
        // note groups
        make_table("note_groups",
            make_column("id", &NoteGroup::id, primary_key().autoincrement()),
            make_column("color", &NoteGroup::color),
            make_column("name", &NoteGroup::name)
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
    void selectWorkspace(int);
    void updateWorkspaceUrl(int, QString);
    void removeWorkspaceUrl(int);
    std::vector<WorkspaceUrl> getWorkspaceUrls(int);

    // notes
    void addNote(QString, QString, int);
    void removeNote(int);
    std::vector<Note> getCurrentWorkspaceNotes();

    // note links
    void addNoteLink(int, int);
    void removeNoteLink(int, int);
    void removeAllNoteLinks(int);
    std::vector<NoteLink> getCurrentWorkspaceNoteLinks();

    // note groups
    void addNoteGroup(QString, QString);
    void removeNoteGroup(int);
    std::vector<NoteGroup> getNoteGroups();
signals:
    void workspacesReady(QString);
};

#endif // DB_API_H
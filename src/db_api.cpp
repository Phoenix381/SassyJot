
#include "bits/stdc++.h"
#include "include/db_api.h"

#include <iostream>

// =============================================================================
// db object initialization
// =============================================================================

DBController::DBController() {
// make_column("id", &UserType::id, primary_key().autoincrement()),
// make_column("name", &UserType::name, default_value("name_placeholder"))));
// Link link{0, "https://www.google.com", "Google", 0};
// auto insertedId = storage.insert(link);
// std::cout << "insertedId = " << insertedId << std::endl;

   // syncync schema on startup
   storage.sync_schema();

   // creating default workspace if workspace table is empty
   auto workspaces = storage.get_all<Workspace>();
   if(workspaces.size() == 0) {
      addWorkspace("Default", "#808080", "Default workspace");
      updateSetting("workspace", "1");
   }

   // default note group
   auto noteGroups = storage.get_all<NoteGroup>();
   if(noteGroups.size() == 0) {
      addNoteGroup("#808080", "General");  
   }

   // test notes
   auto notes = storage.get_all<Note>();
   if(notes.size() == 0) {
      addNote("test", "```python\nimport test\n\ndef func():\n\treturn 1```", 1);
      addNote("test2", "# test2\n$ \\sum_i^n a_n = a_1 + \\dots + a_n $", 1);

      addNoteLink(1, 2);
   }
}

// =============================================================================
// settings related
// =============================================================================

// updating one setting by key
void DBController::updateSetting(QString key, QString value) {
   // upsert to table
   try {
      // if(getSetting(key) == "") {
      //    storage.insert(Setting{key.toStdString(), value.toStdString()});
      // } else {
      //    storage.update(Setting{key.toStdString(), value.toStdString()});
      // }
      storage.replace(Setting{key.toStdString(), value.toStdString()});
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in updateSetting" << std::endl;
   }
}

// get setting by key
QString DBController::getSetting(QString key) {
   try {
      auto setting = storage.get<Setting>(key.toStdString());

      return QString::fromStdString(setting.value);
   } catch (std::system_error e) {
       std::cout << e.what() << std::endl;
   } catch (...){
       std::cout << "unknown exeption in getSetting" << std::endl;
   }

   // default is empty string
   return QString();
}

// =============================================================================
// link related
// =============================================================================

int DBController::addLink(Link link) {
   return storage.insert(link);
}

int DBController::addBookmark(QString url, QString icon, QString title) {
   Bookmark bookmark{-1, url.toStdString(), icon.toStdString(), title.toStdString()};
   std::cout << "Adding bookmark: " << url.toStdString() << std::endl;
   try {
      return storage.insert(bookmark);
   } catch (std::system_error e) {
       std::cout << e.what() << std::endl;
   } catch (...){
       std::cout << "unknown exeption in addBookmark" << std::endl;
   }

   return -1;
}

void DBController::removeBookmark(QString url) {
   // find by url
   std::cout << "Removing bookmark: " << url.toStdString() << std::endl;

   try {
      storage.remove_all<Bookmark>(where(c(&Bookmark::url) == url.toStdString()));
      // return storage.insert(bookmark);
   } catch (std::system_error e) {
       std::cout << e.what() << std::endl;
   } catch (...){
       std::cout << "unknown exeption in removeBookmark" << std::endl;
   }
   // storage.remove(url.toStdString());
}

bool DBController::checkBookmark(QString url) {
   try {
      auto bookmark = storage.get_all<Bookmark>(where(c(&Bookmark::url) == url.toStdString()));
      if(bookmark.size() > 0) {
         return true;
      }
   } catch (std::system_error e) {
       std::cout << e.what() << std::endl;
   } catch (...){
       std::cout << "unknown exeption in checkBookmark" << std::endl;
   } 

   return false;
}

// =============================================================================
// workspace related
// =============================================================================

// adding with full data
int DBController::addWorkspace(QString name, QString color, QString description) {
   Workspace workspace{-1, name.toStdString(), color.toStdString(), description.toStdString()};
   std::cout << "Adding workspace: " << name.toStdString() << std::endl;
   try {
      // TODO check for empty strings
      int id = storage.insert(workspace);
      updateSetting("workspace", QString::number(id));
      return id;
   } catch (std::system_error e) {
       std::cout << e.what() << std::endl;
   } catch (...){
       std::cout << "unknown exeption in addWorkspace" << std::endl;
   }

   return -1;
}
   
// removing using name
void DBController::removeWorkspace(QString name) {
   // TODO handle dependent data

   std::cout << "Removing workspace: " << name.toStdString() << std::endl;

   try {
      storage.remove_all<Workspace>(where(c(&Workspace::name) == name.toStdString()));
   } catch (std::system_error e) {
       std::cout << e.what() << std::endl;
   } catch (...){
       std::cout << "unknown exeption in removeWorkspace" << std::endl;
   }
}

// get all as json
void DBController::getWorkspaces() {
   std::vector<Workspace> workspaces;
    // workspaces;

   try {
      workspaces = storage.get_all<Workspace>();
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;
      return;
   } catch (...){
      std::cout << "unknown exeption in getWorkspaces" << std::endl;
      return;
   }

   // auto workspaces = db->getWorkspaces();
   QJsonArray workspaceArray;

   for (auto workspace : workspaces) {
      QJsonObject workspaceObject;

      workspaceObject.insert("id", QString::number(workspace.id));
      workspaceObject.insert("name", QString::fromStdString(workspace.name));
      workspaceObject.insert("color", QString::fromStdString(workspace.color));
      workspaceObject.insert("description", QString::fromStdString(workspace.description));
      
      workspaceArray.append(workspaceObject);
   }

   emit workspacesReady(QJsonDocument(workspaceArray).toJson(QJsonDocument::Compact));
}

// getting current workspace color
QString DBController::getCurrentWorkspaceColor() {
   try {
      auto id = getSetting("workspace");
      // TODO check if id is valid
      auto workspace = storage.get<Workspace>(id.toInt());
      return QString::fromStdString(workspace.color);
   } catch (std::system_error e) {
       std::cout << e.what() << std::endl;
   } catch (...){
       std::cout << "unknown exeption in getWorkspaceColor" << std::endl;
   }
   
   // default
   return "#808080";
}

// selecting workspace
void DBController::selectWorkspace(int id) {
   updateSetting("workspace", QString::number(id));
}

// saving url to some workspace
void DBController::updateWorkspaceUrl(int index, QString url) {
   int workspaceId = getSetting("workspace").toInt();

   try {
      // check if url already exists
      auto urls = storage.get_all<WorkspaceUrl>(where(
         c(&WorkspaceUrl::order) == index && c(&WorkspaceUrl::workspace_id) == workspaceId
         )
      );
         
      if(urls.size() > 0) {
         storage.update_all(  
            set(c(&WorkspaceUrl::url) = url.toStdString()),
            where(
               c(&WorkspaceUrl::order) == index && c(&WorkspaceUrl::workspace_id) == workspaceId
            )
         );
      } else {
         storage.insert(WorkspaceUrl{-1, index, workspaceId, url.toStdString()});
      }
   } catch (std::system_error e) {
       std::cout << e.what() << std::endl;
   } catch (...){
       std::cout << "unknown exeption in updateWorkspaceUrl" << std::endl;
   }
}

// removing url from workspace
void DBController::removeWorkspaceUrl(int index) {
   int workspaceId = getSetting("workspace").toInt();

   try {
      // decrease index for all geq than id
      storage.update_all(
         set(c(&WorkspaceUrl::order) = c(&WorkspaceUrl::order) - 1),
         where(c(&WorkspaceUrl::order) >= index)
      );
      // remove from workspace
      storage.remove_all<WorkspaceUrl>(
         where(
            c(&WorkspaceUrl::order) == index && c(&WorkspaceUrl::workspace_id) == workspaceId
         )
      );
   } catch (std::system_error e) {
       std::cout << e.what() << std::endl;
   } catch (...){
       std::cout << "unknown exeption in removeWorkspaceUrl" << std::endl;
   }
}

// get aall workspace  urls by id
std::vector<WorkspaceUrl> DBController::getWorkspaceUrls(int) {
   int workspaceId = getSetting("workspace").toInt();

   return storage.get_all<WorkspaceUrl>(where(c(&WorkspaceUrl::workspace_id) == workspaceId));
}

// =============================================================================
// note related
// =============================================================================

// add note to current workspace, reeturn id
int DBController::addNote(QString title, QString content, int group_id = 1) {
   int workspaceId = getSetting("workspace").toInt();

   try {
      return storage.insert(Note{-1, workspaceId, title.toStdString(), content.toStdString()});
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in addNote" << std::endl;
   }

   return -1;
}

// updating  note by id
void DBController::updateNote(int id, QString title, QString content) {
   auto note = storage.get<Note>(id);

   try {
      note.title = title.toStdString();
      note.content = content.toStdString();

      storage.update(note);
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in updateNote" << std::endl;
   }
}

// remove note by id
void DBController::removeNote(int id) {
   try {  
      storage.remove_all<Note>(where(c(&Note::id) == id));
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in removeNote" << std::endl;
   }
}

// get all notes for current workspace
std::vector<Note> DBController::getCurrentWorkspaceNotes() {
   int workspaceId = getSetting("workspace").toInt();

   try {
      return storage.get_all<Note>(where(c(&Note::workspace_id) == workspaceId));
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in getNotes" << std::endl;
   }

   return {};
}

// =============================================================================
// note link related
// =============================================================================

// adding link between two notes
void DBController::addNoteLink(int source, int target) {
   std::cout << "Adding link: " << source << " -> " << target << std::endl;

   try {
      storage.insert(NoteLink{source, target});
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in addNoteLink" << std::endl;
   }
}

// removing link between two notes
void DBController::removeNoteLink(int source, int target) {
   try {  
      storage.remove_all<NoteLink>(where(c(&NoteLink::source_id) == source && c(&NoteLink::target_id) == target));
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in removeNoteLink" << std::endl;
   }
}

// remove all links of note
void DBController::removeAllNoteLinks(int noteId) {
   try {
      storage.remove_all<NoteLink>(where(c(&NoteLink::source_id) == noteId || c(&NoteLink::target_id) == noteId));
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in removeAllNoteLinks" << std::endl;
   }
}

// get all links of workspace
std::vector<NoteLink> DBController::getCurrentWorkspaceNoteLinks() {
   int workspaceId = getSetting("workspace").toInt();

   try {
      // using join
      auto links = storage.select(
         columns(&NoteLink::source_id, &NoteLink::target_id),
         left_join<Note>(
            on(c(&Note::id) == &NoteLink::source_id)
         ),
         where(
            c(&Note::workspace_id) == workspaceId  
         )
      );

      // creating NoteLink vector
      std::vector<NoteLink> noteLinks;
      for (auto link : links) {
         auto noteLink = std::make_from_tuple<NoteLink>(link);
         noteLinks.push_back(noteLink);
      }

      return noteLinks;
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in getCurrentWorkspaceNoteLinks" << std::endl;
   }

   return {};
}

// =============================================================================
// note group related
// =============================================================================

// adding group
void DBController::addNoteGroup(QString color, QString name) {
   try {
      storage.insert(NoteGroup{-1, color.toStdString(), name.toStdString()});
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in addNoteGroup" << std::endl;
   }
}

// removing group by id
void DBController::removeNoteGroup(int id) {
   try {  
      storage.remove<NoteGroup>(id);
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in removeNoteGroup" << std::endl;
   }
}

// get all groups
std::vector<NoteGroup> DBController::getNoteGroups() {
   try {
      return storage.get_all<NoteGroup>();
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;  
   } catch (...){
      std::cout << "unknown exeption in getNoteGroups" << std::endl;
   }

   return {};
}

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
      return storage.insert(workspace);
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

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

   storage.sync_schema();
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
       std::cout << "unknown exeption" << std::endl;
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
       std::cout << "unknown exeption" << std::endl;
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
       std::cout << "unknown exeption" << std::endl;
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
      return storage.insert(workspace);
   } catch (std::system_error e) {
       std::cout << e.what() << std::endl;
   } catch (...){
       std::cout << "unknown exeption" << std::endl;
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
       std::cout << "unknown exeption" << std::endl;
   }
}

// get all
std::vector<Workspace> DBController::getWorkspaces() {
   try {
      return storage.get_all<Workspace>();
   } catch (std::system_error e) {
      std::cout << e.what() << std::endl;
   } catch (...){
      std::cout << "unknown exeption" << std::endl;
   }
}
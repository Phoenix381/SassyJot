
#include "include/click_handler.h"

// =============================================================================
// db interaction
// =============================================================================
void ClickHandler::addLink(QString url, QString title) {
    std::cout << "Adding link: " << url.toStdString() << " - " << title.toStdString() << std::endl;   
}
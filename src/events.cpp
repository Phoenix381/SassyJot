
#include "include/app_window.h"

// =============================================================================
// connecting corresponding app events
// =============================================================================

void AppWindow::registerEvents() {
    // connect(handler, &ClickHandler::addBookmarkEvent, db, &DBController::addBookmark);

    // connect(handler, &ClickHandler::workspacesRequested, this, &AppWindow::getWorkspaces);

    // Add a context menu to create new tabs
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &AppWindow::customContextMenuRequested, this, &AppWindow::showContextMenu);
}
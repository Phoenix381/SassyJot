
#include "include/app_window.h"
#include "include/controllers.h"

// =============================================================================
// setting up main app shortcuts
// =============================================================================

void AppWindow::registerHotkeys() {
    connect(new QShortcut(QKeySequence("Ctrl+T"), this), &QShortcut::activated, tabController, &TabController::requestEmptyTab);
    connect(new QShortcut(QKeySequence("Ctrl+W"), this), &QShortcut::activated, tabController, &TabController::closeCurrentTab);
    connect(new QShortcut(QKeySequence("Ctrl+Tab"), this), &QShortcut::activated, tabController, &TabController::nextTab);
    connect(new QShortcut(QKeySequence("Ctrl+Shift+Tab"), this), &QShortcut::activated, tabController, &TabController::prevTab);
    connect(new QShortcut(QKeySequence("Ctrl+R"), this), &QShortcut::activated, tabController, &TabController::pageReload);
    
    connect(new QShortcut(QKeySequence("Ctrl+D"), this), &QShortcut::activated, this, &AppWindow::favDialog);

    connect(new QShortcut(QKeySequence("Esc"), this), &QShortcut::activated, this, &AppWindow::focus);
}
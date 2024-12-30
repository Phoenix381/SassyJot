
#include "include/app_window.h"

// =============================================================================
// setting up main app shortcuts
// =============================================================================
void AppWindow::registerHotkeys() {
    connect(new QShortcut(QKeySequence("Ctrl+T"), this), &QShortcut::activated, this, &AppWindow::requestNewTab);
    connect(new QShortcut(QKeySequence("Ctrl+W"), this), &QShortcut::activated, this, &AppWindow::closeCurrentTab);
    connect(new QShortcut(QKeySequence("Ctrl+Tab"), this), &QShortcut::activated, this, &AppWindow::nextTab);
    connect(new QShortcut(QKeySequence("Ctrl+Shift+Tab"), this), &QShortcut::activated, this, &AppWindow::prevTab);
    connect(new QShortcut(QKeySequence("Ctrl+R"), this), &QShortcut::activated, this, &AppWindow::reload);
    connect(new QShortcut(QKeySequence("Ctrl+D"), this), &QShortcut::activated, this, &AppWindow::favDialog);
    connect(new QShortcut(QKeySequence("Esc"), this), &QShortcut::activated, this, &AppWindow::focus);
}

#include "include/app_window.h"

// =============================================================================
// workspace management functions
// =============================================================================

// workspace - name, color, description

// send JSON list of all workspaces
void AppWindow::getWorkspaces() {
	auto workspaces = db->getWorkspaces();
	QJsonArray workspaceArray;

	for (auto workspace : workspaces) {
		QJsonObject workspaceObject;

		workspaceObject.insert("name", QString::fromStdString(workspace.name));
		workspaceObject.insert("color", QString::fromStdString(workspace.color));
		workspaceObject.insert("description", QString::fromStdString(workspace.description));
		
		workspaceArray.append(workspaceObject);
	}

	emit handler->workspacesReady(QJsonDocument(workspaceArray).toJson(QJsonDocument::Compact));
}
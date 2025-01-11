
#include "include/app_window.h"

// =============================================================================
// note management functions
// =============================================================================

// get all nodes from current workspace
QString AppWindow::getNodes() {
    auto nodes = db->getCurrentWorkspaceNotes();

    QJsonArray nodesArray;

    // id, workspace_id, title, content, group_id
    for (auto node : nodes) {
        QJsonObject nodeObject;

        nodeObject.insert("id", QString::number(node.id));
        nodeObject.insert("workspace_id", QString::number(node.workspace_id));
        nodeObject.insert("title", QString::fromStdString(node.title));
        nodeObject.insert("content", QString::fromStdString(node.content));
        nodeObject.insert("group_id", QString::number(node.group_id));

        nodesArray.append(nodeObject);
    }

    return QJsonDocument(nodesArray).toJson(QJsonDocument::Compact);
}

// links between these nodes
QString AppWindow::getLinks() {
    auto links = db->getCurrentWorkspaceNoteLinks();

    QJsonArray linksArray;

    // source_id, target_id
    for (auto link : links) {
        QJsonObject linkObject;

        linkObject.insert("source", QString::number(link.source_id));
        linkObject.insert("target", QString::number(link.target_id));

        linksArray.append(linkObject);
    }

    return QJsonDocument(linksArray).toJson(QJsonDocument::Compact);
}
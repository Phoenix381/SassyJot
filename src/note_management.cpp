
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

    // qDebug() << QJsonDocument(nodesArray).toJson(QJsonDocument::Compact);
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

    // qDebug() << QJsonDocument(linksArray).toJson(QJsonDocument::Compact);
    return QJsonDocument(linksArray).toJson(QJsonDocument::Compact);
}

// add new note
void AppWindow::addNote(QString title, QString content, int group_id) {
    // adding note
    auto id = db->addNote(title, content, group_id);

    // check for links and create if needed
    // links are #(\d+) , selecting capture groups (1 is first group, 0 is whole match)
    QRegularExpression link_regex("#(\\d+)");
    auto links = link_regex.globalMatch(content);

    while (links.hasNext()) {
        auto link = links.next();   
        auto link_id = link.captured(1).toInt();
        db->addNoteLink(id, link_id);
    }
}
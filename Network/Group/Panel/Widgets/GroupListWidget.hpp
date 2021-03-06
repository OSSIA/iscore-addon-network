#pragma once
#include <score/model/Identifier.hpp>

#include <QList>
#include <QWidget>

namespace Network
{

class Group;
class GroupManager;
class GroupWidget;
class GroupListWidget : public QWidget
{
public:
  GroupListWidget(const GroupManager& mgr, QWidget* parent);

private:
  void addGroup(const Id<Group>& id);
  void removeGroup(const Id<Group>& id);

  const GroupManager& m_mgr;
  QList<GroupWidget*> m_widgets;
};
}

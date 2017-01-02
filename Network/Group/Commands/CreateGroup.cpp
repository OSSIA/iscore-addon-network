
#include <iscore/tools/IdentifierGeneration.hpp>
#include <vector>

#include "CreateGroup.hpp"
#include <Network/Group/Group.hpp>
#include <Network/Group/GroupManager.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/model/path/ObjectPath.hpp>


namespace Network
{
namespace Command
{
CreateGroup::CreateGroup(ObjectPath&& groupMgrPath, QString groupName):
    m_path{groupMgrPath},
    m_name{groupName}
{
    auto& mgr = m_path.find<GroupManager>();
    m_newGroupId = getStrongId(mgr.groups());
}

void CreateGroup::undo() const
{
    m_path.find<GroupManager>().removeGroup(m_newGroupId);
}

void CreateGroup::redo() const
{
    auto& mgr = m_path.find<GroupManager>();
    mgr.addGroup(new Group{m_name, m_newGroupId, &mgr});
}

void CreateGroup::serializeImpl(DataStreamInput& s) const
{
    s << m_path << m_name << m_newGroupId;
}

void CreateGroup::deserializeImpl(DataStreamOutput& s)
{
    s >> m_path >> m_name >> m_newGroupId;
}
}
}
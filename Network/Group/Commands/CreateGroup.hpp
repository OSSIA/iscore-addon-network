#pragma once
#include <Network/Group/Commands/DistributedScenarioCommandFactory.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <iscore/command/Command.hpp>
#include <iscore/model/path/ObjectPath.hpp>
#include <QString>

#include <iscore/model/Identifier.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace Network
{
class Group;

namespace Command
{
class CreateGroup : public iscore::Command
{
        ISCORE_COMMAND_DECL(DistributedScenarioCommandFactoryName(), CreateGroup, "CreateGroup")
        public:
        CreateGroup(ObjectPath&& groupMgrPath, QString groupName);

        void undo() const override;
        void redo() const override;

        void serializeImpl(DataStreamInput & s) const override;
        void deserializeImpl(DataStreamOutput & s) override;

    private:
        ObjectPath m_path;
        QString m_name;
        Id<Group> m_newGroupId;
};
}
}
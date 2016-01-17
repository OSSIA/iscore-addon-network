#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <boost/optional/optional.hpp>
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <iscore/serialization/VisitorCommon.hpp>

#include <QString>

#include "DistributedScenario/Group.hpp"
#include "DistributedScenario/GroupManager.hpp"
#include "DistributedScenario/GroupMetadata.hpp"
#include "DistributedScenario/GroupMetadataWidget.hpp"
#include "NetworkDocumentPlugin.hpp"
#include <Process/Process.hpp>
#include "Repartition/session/Session.hpp"
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>
#include <iscore/tools/NotifyingMap.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include "session/../client/LocalClient.hpp"

class QWidget;
struct VisitorVariant;

namespace Network
{

// TODO refactor me
class ScenarioFindConstraintVisitor
{
    public:
        std::vector<Scenario::ConstraintModel*> constraints;

        void visit(Scenario::ScenarioModel& s)
        {
            constraints.reserve(constraints.size() + s.constraints.size());
            for(auto& constraint : s.constraints)
            {
                constraints.push_back(&constraint);
                visit(constraint);
            }
        }

        void visit(Scenario::ConstraintModel& c)
        {
            for(auto& proc : c.processes)
            {
                if(auto scenario = dynamic_cast<Scenario::ScenarioModel*>(&proc))
                {
                    visit(*scenario);
                }
            }
        }
};


class ScenarioFindEventVisitor
{
    public:
        std::vector<Scenario::EventModel*> events;

        void visit(Scenario::ConstraintModel& c)
        {
            for(auto& proc : c.processes)
            {
                if(auto scenario = dynamic_cast<Scenario::ScenarioModel*>(&proc))
                {
                    visit(*scenario);
                }
            }
        }

        void visit(Scenario::ScenarioModel& s)
        {
            events.reserve(events.size() + s.events.size());
            for(auto& event : s.events)
            {
                events.push_back(&event);
            }
            for(auto& constraint : s.constraints)
            {
                visit(constraint);
            }
        }
};

NetworkDocumentPlugin::NetworkDocumentPlugin(
        NetworkPolicyInterface *policy,
        iscore::Document& doc):
    iscore::DocumentPluginModel{doc, "NetworkDocumentPlugin", &doc.model()},
    m_policy{policy},
    m_groups{new GroupManager{this}}
{
    m_policy->setParent(this);
    using namespace std;

    // Base group set-up
    auto baseGroup = new Group{"Default", Id<Group>{0}, groupManager()};
    baseGroup->addClient(m_policy->session()->localClient().id());
    groupManager()->addGroup(baseGroup);

    // Create it for each constraint / event.
    Scenario::ScenarioDocumentModel* bem = safe_cast<Scenario::ScenarioDocumentModel*>(&doc.model().modelDelegate());
    {
        ScenarioFindConstraintVisitor v;
        v.visit(bem->baseConstraint());// TODO this doesn't match baseconstraint
        for(Scenario::ConstraintModel* constraint : v.constraints)
        {
            for(const auto& plugid : elementPlugins())
            {
                if(constraint->pluginModelList.canAdd(plugid))
                    constraint->pluginModelList.add(
                                makeElementPlugin(constraint,
                                                  plugid,
                                                  constraint));
            }
        }
    }

    {
        ScenarioFindEventVisitor v;
        v.visit(bem->baseConstraint());

        for(Scenario::EventModel* event : v.events)
        {
            for(const auto& plugid : elementPlugins())
            {
                if(event->pluginModelList.canAdd(plugid))
                {
                    event->pluginModelList.add(
                                makeElementPlugin(event,
                                                  plugid,
                                                  event));
                }
            }
        }
    }
    // TODO here we have to instantiate Network "OSSIA" policies. OR does it go with GroupMetadata?
}

NetworkDocumentPlugin::NetworkDocumentPlugin(const VisitorVariant &loader,
                                             iscore::Document& doc):
    iscore::DocumentPluginModel{doc, "NetworkDocumentPlugin", &doc.model()}
{
    deserialize_dyn(loader, *this);
}

void NetworkDocumentPlugin::serialize(const VisitorVariant & vis) const
{
    serialize_dyn(vis, *this);
}

void NetworkDocumentPlugin::setPolicy(NetworkPolicyInterface * pol)
{
    delete m_policy;
    m_policy = pol;

    emit sessionChanged();
}

std::vector<iscore::ElementPluginModelType> NetworkDocumentPlugin::elementPlugins() const
{
    return {GroupMetadata::staticPluginId()};
}

void NetworkDocumentPlugin::setupGroupPlugin(GroupMetadata* plug)
{
    connect(m_groups, &GroupManager::groupRemoved,
            plug, [=] (const Id<Group>& id)
    { if(plug->group() == id) plug->setGroup(m_groups->defaultGroup()); });
}



////// Methods relative to GroupMetadata //////
iscore::ElementPluginModel*
NetworkDocumentPlugin::makeElementPlugin(
        const QObject* element,
        iscore::ElementPluginModelType type,
        QObject* parent)
{
    switch(type)
    {
        case GroupMetadata::staticPluginId():
        {
            if((element->metaObject()->className() == QString{"ConstraintModel"})
            || (element->metaObject()->className() == QString{"EventModel"}))
            {
                auto plug = new GroupMetadata{element, m_groups->defaultGroup(), parent};

                setupGroupPlugin(plug);

                return plug;
            }

            break;
        }
    }

    return nullptr;
}

iscore::ElementPluginModel*
NetworkDocumentPlugin::loadElementPlugin(
        const QObject* element,
        const VisitorVariant& vis,
        QObject* parent)
{
    if(element->metaObject()->className() == QString{"ConstraintModel"}
    || element->metaObject()->className() == QString{"EventModel"})
    {
        auto plug = deserialize_dyn(vis, [&] (auto&& deserializer)
        { return new GroupMetadata{element, deserializer, parent}; });

        setupGroupPlugin(plug);

        return plug;
    }

    ISCORE_ABORT;
    return nullptr;
}


iscore::ElementPluginModel *NetworkDocumentPlugin::cloneElementPlugin(
        const QObject* element,
        iscore::ElementPluginModel* elt,
        QObject *parent)
{
    if(elt->elementPluginId() == GroupMetadata::staticPluginId())
    {
        auto newelt = static_cast<GroupMetadata*>(elt)->clone(element, parent);
        setupGroupPlugin(newelt);
        return newelt;
    }

    return nullptr;
}


////// Method relative to GRoupMetadataWidget //////
QWidget *NetworkDocumentPlugin::makeElementPluginWidget(
        const iscore::ElementPluginModel *var,
        QWidget* widg) const
{
    return new GroupMetadataWidget{
                static_cast<const GroupMetadata&>(*var), m_groups, widg};
}
}


#include <NetworkApplicationPlugin.hpp>
#include <iscore_plugin_network.hpp>

#include <iscore/tools/ForEachType.hpp>
#include "DistributedScenario/Commands/DistributedScenarioCommandFactory.hpp"
#include "DistributedScenario/Panel/GroupPanelFactory.hpp"
#include <iscore/plugins/qt_interfaces/GUIApplicationContextPlugin_QtInterface.hpp>
#include <iscore_addon_network_commands_files.hpp>
#include <DocumentPlugins/NetworkDocumentPlugin.hpp>

#include <iscore/plugins/customfactory/FactorySetup.hpp>

#include <iscore/command/CommandGeneratorMap.hpp>
#include <unordered_map>
namespace iscore {

class PanelFactory;
}  // namespace iscore
iscore_addon_network::iscore_addon_network()
{
}

iscore_addon_network::~iscore_addon_network()
{

}

// Interfaces implementations :
iscore::GUIApplicationContextPlugin*
iscore_addon_network::make_applicationPlugin(
        const iscore::GUIApplicationContext& app)
{
    return new Network::NetworkApplicationPlugin{app};
}

std::vector<std::unique_ptr<iscore::InterfaceBase>>
iscore_addon_network::factories(
        const iscore::ApplicationContext& ctx,
        const iscore::InterfaceKey& key) const
{
    return instantiate_factories<
            iscore::ApplicationContext,
        FW<iscore::DocumentPluginFactory,
             Network::DocumentPluginFactory>,
        FW<iscore::PanelDelegateFactory,
            Network::PanelDelegateFactory>
    >(ctx, key);
}

std::pair<const CommandGroupKey, CommandGeneratorMap> iscore_addon_network::make_commands()
{
    using namespace Network;
    using namespace Network::Command;
    std::pair<const CommandGroupKey, CommandGeneratorMap> cmds{
        DistributedScenarioCommandFactoryName(), CommandGeneratorMap{}};

    using Types = TypeList<
#include <iscore_addon_network_commands.hpp>
      >;
    for_each_type<Types>(iscore::commands::FactoryInserter{cmds.second});


    return cmds;
}

#include <NetworkApplicationPlugin.hpp>
#include <iscore_plugin_network.hpp>

#include <iscore/tools/ForEachType.hpp>
#include "DistributedScenario/Commands/DistributedScenarioCommandFactory.hpp"
#include "DistributedScenario/Panel/GroupPanelFactory.hpp"
#include <iscore/plugins/qt_interfaces/GUIApplicationContextPlugin_QtInterface.hpp>
#include <iscore_addon_network_commands_files.hpp>
#include <DocumentPlugins/NetworkDocumentPlugin.hpp>

#include <iscore/plugins/customfactory/FactorySetup.hpp>
namespace iscore {

class PanelFactory;
}  // namespace iscore

#define PROCESS_NAME "Network Process"

iscore_addon_network::iscore_addon_network() :
    QObject {},
        iscore::GUIApplicationContextPlugin_QtInterface {}//,
        //iscore::SettingsDelegateFactoryInterface_QtInterface {}
{
}

// Interfaces implementations :
//////////////////////////
/*
iscore::SettingsDelegateFactoryInterface* iscore_plugin_network::settings_make()
{
    return new NetworkSettings;
}
*/
iscore::GUIApplicationContextPlugin*
iscore_addon_network::make_applicationPlugin(
        const iscore::ApplicationContext& app)
{
    return new Network::NetworkApplicationPlugin{app};
}

std::vector<iscore::PanelFactory*>
iscore_addon_network::panels()
{
    return {new Network::GroupPanelFactory};
}

std::vector<std::unique_ptr<iscore::FactoryInterfaceBase>>
iscore_addon_network::factories(
        const iscore::ApplicationContext& ctx,
        const iscore::AbstractFactoryKey& key) const
{
    return instantiate_factories<
            iscore::ApplicationContext,
    TL<
        FW<iscore::DocumentPluginFactory,
             Network::DocumentPluginFactory>
    >>(ctx, key);
}


#include <iscore/command/CommandGeneratorMap.hpp>
#include <unordered_map>

std::pair<const CommandParentFactoryKey, CommandGeneratorMap> iscore_addon_network::make_commands()
{
    using namespace Network;
    using namespace Network::Command;
    std::pair<const CommandParentFactoryKey, CommandGeneratorMap> cmds{
        DistributedScenarioCommandFactoryName(), CommandGeneratorMap{}};

    using Types = TypeList<
#include <iscore_addon_network_commands.hpp>
      >;
    for_each_type<Types>(iscore::commands::FactoryInserter{cmds.second});


    return cmds;
}

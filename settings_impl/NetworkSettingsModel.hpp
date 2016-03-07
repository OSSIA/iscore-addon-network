#pragma once
#include <iscore/plugins/settingsdelegate/SettingsDelegateModelInterface.hpp>
#include <QString>

namespace iscore
{
    class SettingsDelegatePresenterInterface;
}
// TODO find a better way...
#define SETTINGS_CLIENTPORT "iscore_plugin_network/ClientPort"
#define SETTINGS_MASTERPORT "iscore_plugin_network/MasterPort"
#define SETTINGS_CLIENTNAME "iscore_plugin_network/ClientName"

namespace Network
{
class NetworkSettingsModel : public iscore::SettingsDelegateModelInterface
{
        Q_OBJECT
    public:
        NetworkSettingsModel();

        void setClientName(QString txt);
        QString getClientName() const;
        void setClientPort(int val);
        int getClientPort() const;
        void setMasterPort(int val);
        int getMasterPort() const;

        void setFirstTimeSettings() override;

    signals:
        void clientNameChanged();
        void clientPortChanged();
        void masterPortChanged();

    private:
        int masterPort;
        int clientPort;
        QString clientName;
};

ISCORE_SETTINGS_PARAMETER(NetworkSettingsModel, ClientName)
ISCORE_SETTINGS_PARAMETER(NetworkSettingsModel, ClientPort)
ISCORE_SETTINGS_PARAMETER(NetworkSettingsModel, MasterPort)

}
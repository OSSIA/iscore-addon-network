#pragma once
#include "Session.hpp"

class QObject;
#include <iscore/tools/SettableIdentifier.hpp>

namespace Network
{
class LocalClient;
class RemoteClient;
class ClientSession : public Session
{
    public:
        ClientSession(
                RemoteClient* master,
                LocalClient* client,
                Id<Session> id,
                QObject* parent = nullptr);

        RemoteClient* master() const
        {
            return m_master;
        }

    private:
        RemoteClient* m_master{};
};
}

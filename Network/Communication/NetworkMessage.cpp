#include "NetworkMessage.hpp"

#include <score/model/Identifier.hpp>

#include <QDataStream>

#include <Network/Client/LocalClient.hpp>
#include <Network/Session/Session.hpp>

namespace Network
{
QDataStream& operator<<(QDataStream& s, const Network::NetworkMessage& m)
{
  s << m.address << m.sessionId << m.clientId << m.data;
  return s;
}

QDataStream& operator>>(QDataStream& s, Network::NetworkMessage& m)
{
  s >> m.address >> m.sessionId >> m.clientId >> m.data;
  return s;
}

NetworkMessage::NetworkMessage(
    Network::Session& s,
    QByteArray&& addr,
    QByteArray&& data)
    : address{addr}
    , sessionId{s.id()}
    , clientId{s.localClient().id()}
    , data{data}
{
}
}

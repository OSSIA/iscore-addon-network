#pragma once
#include <QList>
#include <QMap>
#include <QString>
#include <functional>
#include <iscore/tools/std/HashMap.hpp>
#include <Network/Communication/NetworkMessage.hpp>

namespace std
{
template<>
struct hash<QByteArray>
{
  std::size_t operator()(const QByteArray& id) const
  {
    return qHash(id);
  }
};
}
namespace Network
{
struct NetworkMessage;

template<typename... Args> struct dummy {} ;


class MessageMapper
{
  template<typename Fun, typename... Args>
  auto addHandler_impl_sub(QDataStream& s, Fun f, dummy<>, Args&&... args)
  {
    f(std::forward<Args>(args)...);
  }

  template<typename Fun, typename Arg, typename... Args, typename... Args2>
  auto addHandler_impl_sub(QDataStream& s, Fun f, dummy<Arg, Args...>, Args2&&... args)
  {
    Arg a;
    s >> a;
    addHandler_impl_sub(s, f, dummy<Args...>{}, std::forward<Args2>(args)..., a);
  }

  template<typename Fun, typename... Args>
  auto addHandler_impl(Fun f, void (Fun::*) (NetworkMessage, Args...) const)
  {
    return [=] (NetworkMessage m) {
      QDataStream s{m.data};

      addHandler_impl_sub(s, f, dummy<Args...>{}, m);
    };
  }

  template<typename Fun, typename... Args>
  auto addHandler_impl(Fun f, void (Fun::*) (NetworkMessage, Args...))
  {
    return [=] (NetworkMessage m) {
      QDataStream s{m.data};

      addHandler_impl_sub(s, f, dummy<Args...>{}, m);
    };
  }
public:

  template<typename Fun, typename... Args>
  void addHandler_(const QByteArray& data, Fun f)
  {
    m_handlers[data] = addHandler_impl(f, &Fun::operator());
  }

  void addHandler(QByteArray addr, std::function<void(NetworkMessage)> fun);
  void map(NetworkMessage m);

  bool contains(const QByteArray& b) const;
private:
  iscore::hash_map<QByteArray, std::function<void(NetworkMessage)>> m_handlers;
};
}

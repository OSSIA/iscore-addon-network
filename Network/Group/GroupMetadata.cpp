#include "GroupMetadata.hpp"

#include <score/serialization/VisitorCommon.hpp>
#include <wobjectimpl.h>
W_OBJECT_IMPL(Network::GroupMetadata)
struct VisitorVariant;

namespace Network
{

GroupMetadata::GroupMetadata(Id<Component> self, Id<Group> id, QObject* parent)
    : score::SerializableComponent{self, "GroupMetadata", parent}, m_id{id}
{
}

GroupMetadata::~GroupMetadata() {}

void GroupMetadata::setGroup(const Id<Group>& id)
{
  if (id != m_id)
  {
    m_id = id;
    groupChanged(id);
  }
}
}

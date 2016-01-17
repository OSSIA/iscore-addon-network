
#include <boost/optional/optional.hpp>
#include <QJsonObject>
#include <QJsonValue>
#include <algorithm>

#include "GroupMetadata.hpp"
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONValueVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/tools/SettableIdentifier.hpp>

template <typename T> class Reader;
template <typename T> class Writer;

template<>
void Visitor<Reader<DataStream>>::readFrom(
        const Network::GroupMetadata& elt)
{
    readFrom(elt.group());
    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Network::GroupMetadata& elt)
{
    Id<Network::Group> id;
    m_stream >> id;
    elt.setGroup(id);

    checkDelimiter();
}


template<>
void Visitor<Reader<JSONObject>>::readFrom(
        const Network::GroupMetadata& elt)
{
    m_obj["Group"] = toJsonValue(elt.group());
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Network::GroupMetadata& elt)
{
    elt.setGroup(fromJsonValue<Id<Network::Group>>(m_obj["Group"]));
}

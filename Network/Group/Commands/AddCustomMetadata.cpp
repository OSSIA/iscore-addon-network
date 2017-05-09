#include <Network/Group/Commands/AddCustomMetadata.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <iscore/document/DocumentContext.hpp>
#include <iscore/selection/SelectionStack.hpp>
#include <iscore/model/path/PathSerialization.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>

namespace Network
{
namespace Command
{
AddCustomMetadata::AddCustomMetadata(
    const QList<const Scenario::ConstraintModel*>& c,
    const QList<const Scenario::EventModel*>& e,
    const QList<const Scenario::TimeNodeModel*>& n,
    const std::vector<std::pair<QString, QString> >& meta)
{
  m_constraints.reserve(c.size());
  for(auto elt : c)
  {
    MetadataUndoRedo<Scenario::ConstraintModel> m;
    m.path = iscore::IDocument::path(*elt);
    m.before = elt->metadata().getExtendedMetadata();
    m.after = m.before;
    for(const auto& e : meta)
      m.after[e.first] = e.second;

    m_constraints.push_back(std::move(m));
  }

  m_events.reserve(e.size());
  for(auto elt : e)
  {
    MetadataUndoRedo<Scenario::EventModel> m;
    m.path = iscore::IDocument::path(*elt);
    m.before = elt->metadata().getExtendedMetadata();
    m.after = m.before;
    for(const auto& e : meta)
      m.after[e.first] = e.second;

    m_events.push_back(std::move(m));
  }

  m_nodes.reserve(n.size());
  for(auto elt : n)
  {
    MetadataUndoRedo<Scenario::TimeNodeModel> m;
    m.path = iscore::IDocument::path(*elt);
    m.before = elt->metadata().getExtendedMetadata();
    m.after = m.before;
    for(const auto& e : meta)
      m.after[e.first] = e.second;

    m_nodes.push_back(std::move(m));
  }

}

void AddCustomMetadata::undo() const
{
  for(auto& elt : m_constraints)
  {
    elt.path.find().metadata().setExtendedMetadata(elt.before);
  }
  for(auto& elt : m_events)
  {
    elt.path.find().metadata().setExtendedMetadata(elt.before);
  }
  for(auto& elt : m_nodes)
  {
    elt.path.find().metadata().setExtendedMetadata(elt.before);
  }
}

void AddCustomMetadata::redo() const
{
  for(auto& elt : m_constraints)
  {
    elt.path.find().metadata().setExtendedMetadata(elt.after);
  }
  for(auto& elt : m_events)
  {
    elt.path.find().metadata().setExtendedMetadata(elt.after);
  }
  for(auto& elt : m_nodes)
  {
    elt.path.find().metadata().setExtendedMetadata(elt.after);
  }

}

void AddCustomMetadata::serializeImpl(DataStreamInput& s) const
{
  s << m_constraints << m_events << m_nodes;

}

void AddCustomMetadata::deserializeImpl(DataStreamOutput& s)
{
  s >> m_constraints >> m_events >> m_nodes;
}
}

void SetCustomMetadata(const iscore::DocumentContext& ctx,
                       std::vector<std::pair<QString, QString> > md)
{
  auto sel = ctx.selectionStack.currentSelection();

  auto cmd = new Command::AddCustomMetadata{
             filterSelectionByType<Scenario::ConstraintModel>(sel)
             , filterSelectionByType<Scenario::EventModel>(sel)
             , filterSelectionByType<Scenario::TimeNodeModel>(sel)
             , md};

  CommandDispatcher<>{ctx.commandStack}.submitCommand(cmd);
}

}




template <typename T>
struct TSerializer<DataStream, Network::Command::MetadataUndoRedo<T>>
{
  static void readFrom(
      DataStream::Serializer& s,
      const Network::Command::MetadataUndoRedo<T>& obj)
  {
    s.stream() << obj.path << obj.before << obj.after;
  }

  static void writeTo(
      DataStream::Deserializer& s, Network::Command::MetadataUndoRedo<T>& obj)
  {
    s.stream() >> obj.path >> obj.before >> obj.after;
  }
};

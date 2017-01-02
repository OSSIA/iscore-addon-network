#include <Network/Group/Group.hpp>
#include <iscore/tools/std/Optional.hpp>
#include <QBoxLayout>
#include <QLabel>
#include <QObject>

#include <QPushButton>
#include <QString>

#include "GroupWidget.hpp"
#include <iscore/model/Identifier.hpp>

namespace Network
{
GroupWidget::GroupWidget(Group* group, QWidget* parent):
    QWidget{parent},
    m_group{group}
{
    auto lay = new QHBoxLayout{this};
    lay->addWidget(new QLabel{group->name()});

    auto rename = new QPushButton(QObject::tr("Rename"));
    lay->addWidget(rename);

    auto remove = new QPushButton(QObject::tr("Remove"));
    lay->addWidget(remove);

    // TODO connect add/remove group
}

Id<Group> GroupWidget::id() const
{
    return m_group->id();
}
}

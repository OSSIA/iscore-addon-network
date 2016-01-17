#include <iscore/document/DocumentInterface.hpp>
#include <QBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <qnamespace.h>
#include <QObject>
#include <QPushButton>
#include <QWidget>

#include "DistributedScenario/Commands/CreateGroup.hpp"
#include "GroupPanelView.hpp"
#include "Widgets/GroupListWidget.hpp"
#include "Widgets/GroupTableWidget.hpp"
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/plugins/panel/PanelView.hpp>
#include <iscore/tools/ObjectPath.hpp>
#include <iscore/document/DocumentContext.hpp>
#include "DistributedScenario/GroupManager.hpp"
#include "Repartition/session/Session.hpp"


namespace Network
{
static const iscore::DefaultPanelStatus status{false, Qt::RightDockWidgetArea, 1, QObject::tr("Groups")};
const iscore::DefaultPanelStatus &GroupPanelView::defaultPanelStatus() const
{
    return status;
}

GroupPanelView::GroupPanelView(QObject* v):
    iscore::PanelView{v},
    m_widget{new QWidget}
{
    auto lay = new QVBoxLayout;
    m_widget->setLayout(lay);
}

QWidget*GroupPanelView::getWidget()
{
    return m_widget;
}

void GroupPanelView::setView(const GroupManager* mgr,
                             const Session* session)
{
    // Make the containing widget
    delete m_subWidget;
    m_subWidget = new QWidget;

    auto lay = new QVBoxLayout;
    m_subWidget->setLayout(lay);

    m_widget->layout()->addWidget(m_subWidget);

    // The sub-widgets (group data presentation)
    m_subWidget->layout()->addWidget(new QLabel{session->metaObject()->className()});
    m_subWidget->layout()->addWidget(new GroupListWidget{mgr, m_subWidget});

    // Add group button
    auto button = new QPushButton{tr("Add group")};
    ObjectPath mgrpath{iscore::IDocument::unsafe_path(mgr)};
    connect(button, &QPushButton::pressed, this, [=] ( )
    {
        bool ok;
        QString text = QInputDialog::getText(m_widget, tr("New group"),
                                             tr("Group name:"), QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty())
        {
            auto cmd = new Command::CreateGroup{ObjectPath{mgrpath}, text};

            CommandDispatcher<> dispatcher{
                iscore::IDocument::documentContext(*mgr).commandStack
            };
            dispatcher.submitCommand(cmd);
        }
    });
    m_subWidget->layout()->addWidget(button);

    // Group table
    m_subWidget->layout()->addWidget(new GroupTableWidget{mgr, session, m_widget});
}

void GroupPanelView::setEmptyView()
{
    delete m_subWidget;
    m_subWidget = nullptr;
}
}

#pragma once
#include <QWidget>

class QCheckBox;

namespace Network
{
class GroupTableCheckbox : public QWidget
{
        Q_OBJECT
    public:
        GroupTableCheckbox();

        int state();

    signals:
        void stateChanged(int);

    public slots:
        void setState(int state);

    private:
        QCheckBox* m_cb;

};
}

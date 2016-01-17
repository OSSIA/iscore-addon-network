#pragma once
#include <QFrame>
#include <array>

class QEvent;
class QLineEdit;
class QObject;
class QWidget;


namespace Network
{
// Found on stackoverflow :
// http://stackoverflow.com/questions/9306335/an-ip-address-widget-for-qt-similar-to-mfcs-ip-address-control
class IpWidget : public QFrame
{
        Q_OBJECT

        enum
        {
            QTUTL_IP_SIZE   = 4,
            MAX_DIGITS      = 3
        };
    public:
        explicit IpWidget(QWidget *parent = 0);
        ~IpWidget();

        virtual bool eventFilter( QObject *obj, QEvent *event );

        std::array<QLineEdit*, QTUTL_IP_SIZE> lineEdits;
    public slots:
        void slotTextChanged( QLineEdit* pEdit );

    signals:
        void signalTextChanged( QLineEdit* pEdit );

    private:
        void MoveNextLineEdit (int i);
        void MovePrevLineEdit (int i);
};

}

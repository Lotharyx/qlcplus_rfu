#ifndef RFU_ADVERTISER_H
#define RFU_ADVERTISER_H

#include <QUdpSocket>
#include <QObject>

class rfu_advertiser : public QUdpSocket {
        Q_OBJECT
    public:
        rfu_advertiser();

    private slots:
        void packetWaiting();

    private:
        static const QByteArray s_response;
};

#endif // RFU_ADVERTISER_H

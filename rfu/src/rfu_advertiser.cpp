#include "rfu_advertiser.h"
#include <QNetworkDatagram>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>

const QByteArray rfu_advertiser::s_response("YESIAMANRFUPLEASEDTOSEEU");

rfu_advertiser::rfu_advertiser() {
    connect(this, SIGNAL(readyRead()), this, SLOT(packetWaiting()));

    if(!bind(QHostAddress::AnyIPv4, 16768)) {
        // Dear me..
        qWarning() << "rfu_advertiser couldn't bind to port";
    }
}


void rfu_advertiser::packetWaiting() {
    // Receive a datagram of 24 bytes max.  That's what the rfu client
    // will broadcast when it's looking for us.
    QNetworkDatagram datagram = receiveDatagram(24);

    if(datagram.isValid()) {
        if(datagram.data() == QString("HELLOISTHEREANRFUAROUND?")) {
            QNetworkDatagram response(s_response, datagram.senderAddress(), datagram.senderPort());
            writeDatagram(response);
            // We're not going to bother looking to see if the client got our answer.
            // If they did, they'll try to establish a TCP connection.  If they didn't,
            // they'll probably broadcast again at some point.
        }
    }
}

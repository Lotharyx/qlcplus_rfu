#include "rfu_socket.h"

#include <QTcpSocket>

rfu_socket::rfu_socket(QObject *parent) : QTcpServer(parent)
{
    // Start up the server, yo!
    connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    listen(QHostAddress::AnyIPv4, 16768);
}

rfu_socket::~rfu_socket() {
    m_actors.clear();
    close();
}

void rfu_socket::onNewConnection() {
    QTcpSocket * socket = nextPendingConnection();
    std::shared_ptr<rfu_actor> actor = std::make_shared<rfu_actor>(socket, parent());
    m_actors.push_back(actor);
}


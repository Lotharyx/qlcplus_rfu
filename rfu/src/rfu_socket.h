#ifndef RFU_SOCKET_H
#define RFU_SOCKET_H

#include <QList>
#include <QObject>
#include <QTcpServer>

#include "rfu_actor.h"

class rfu_socket : public QTcpServer
{
    Q_OBJECT
public:
    explicit rfu_socket(QObject *parent = nullptr);
    ~rfu_socket();

signals:

private:
    QList<std::shared_ptr<rfu_actor>> m_actors;
private slots:
    void onNewConnection();

};

#endif // RFU_SOCKET_H

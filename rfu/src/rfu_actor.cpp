#include "rfu_actor.h"
#include "rfu.h"

static const QChar comma(',');
static const QChar semicolon(';');

rfu_actor::rfu_actor(QTcpSocket * socket, QObject *parent) : QObject(parent) {
    m_socket = socket;
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    m_dispatch["ls"] = &rfu_actor::listScenes;
    m_dispatch["lf"] = &rfu_actor::listFixtures;
    m_dispatch["ts"] = &rfu_actor::toggleScene;
    m_dispatch["pt"] = &rfu_actor::getFixturePT;
    m_dispatch["mv"] = &rfu_actor::moveFixture;
}

void rfu_actor::onReadyRead() {
    qint64 bytes_available = m_socket->bytesAvailable();
    QString data = QString(m_socket->read(bytes_available)).trimmed();
    QStringList commands = data.split(semicolon);

    for(auto & command : commands) {
        if(command.length() > 0) {
            // qInfo() << "Dispatching " << command << "...";
            dispatch(command);
        }
    }
}



void rfu_actor::dispatch(QString & data) {
    QString command = data.section(comma, 0, 0);

    if(m_dispatch.contains(command))
        (this->*m_dispatch[command])(data);
}


void rfu_actor::listScenes(QString & data) {
    Q_UNUSED(data);
    rfu * the_rfu = dynamic_cast<rfu *>(parent());

    if(the_rfu != nullptr) {
        QByteArray response;
        the_rfu->getSceneList(response);
        m_socket->write(response);
    }
}


void rfu_actor::listFixtures(QString & data) {
    rfu * the_rfu = dynamic_cast<rfu *>(parent());

    if(the_rfu != nullptr) {
        QByteArray response;
        the_rfu->getFixtureList(data.section(comma, 1, 1).toULong(), response);
        m_socket->write(response);
    }
}


void rfu_actor::toggleScene(QString & data) {
    quint32 s_id = data.section(comma, 1, 1).toULong();
    quint32 onoff = data.section(comma, 2, 2).toULong();
    rfu * the_rfu = dynamic_cast<rfu *>(parent());

    if(the_rfu != nullptr) {
        if(onoff == 1)
            the_rfu->activateScene(s_id);
        else
            the_rfu->deactivateScene(s_id);
    }
}


void rfu_actor::getFixturePT(QString & data) {
    quint32 scene = data.section(comma, 1, 1).toULong();
    quint32 fixture = data.section(comma, 2, 2).toULong();
    quint32 head = data.section(comma, 3, 3).toULong();

    rfu * the_rfu = dynamic_cast<rfu *>(parent());

    if(the_rfu != nullptr) {
        QByteArray response;
        the_rfu->getFixturePT(scene, fixture, head, response);
        m_socket->write(response);
    }
}


void rfu_actor::moveFixture(QString & data) {
    quint32 scene = data.section(comma, 1, 1).toULong();
    quint32 fixture = data.section(comma, 2, 2).toULong();
    quint32 head = data.section(comma, 3, 3).toULong();
    qint16 d_x = data.section(comma, 4, 4).toShort();
    qint16 d_y = data.section(comma, 5, 5).toShort();
    rfu * the_rfu = dynamic_cast<rfu *>(parent());

    if(the_rfu != nullptr) {
        QByteArray response;
        the_rfu->offsetFixturePT(scene, fixture, head, d_x, d_y);
    }

}


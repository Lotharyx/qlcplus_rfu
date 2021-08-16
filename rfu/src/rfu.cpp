#include "rfu.h"
#include "rfu_socket.h"
#include "rfu_advertiser.h"
#include "scene.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

void rfu::run(Doc * doc) {
    // Forking-A!
    instance().set_doc(doc);
}

rfu & rfu::instance() {
    static rfu * instance = new rfu();
    return *instance;
}

rfu::rfu(QObject *parent) : QObject(parent) {
    m_socket = new rfu_socket(this);
    m_advertiser = new rfu_advertiser();
    m_selected_func = nullptr;
}

rfu::~rfu() {
    delete m_socket;
    delete m_advertiser;
}

void rfu::set_doc(Doc * doc) {
    m_doc = doc;
}


void rfu::getSceneList(QByteArray & output) {
    if(m_doc != nullptr) {
        QJsonDocument jdoc;
        QJsonObject scenes_object;
        QJsonArray scene_array;
        auto fns = m_doc->functionsByType(Function::Type::SceneType);

        for(auto & fn : fns) {
            Scene * scene = dynamic_cast<Scene *>(fn);

            if(scene != nullptr) {
                if(scene->isRFUEnabled()) {
                    QJsonObject scene_object;
                    scene_object["id"] = (qint64)scene->id();
                    scene_object["name"] = scene->name();
                    scene_array.append(scene_object);
                }
            } else {
                qDebug() << "Asked for scenes and got something that wasn't a scene";
            }
        }

        scenes_object["scenes"] = scene_array;
        jdoc.setObject(scenes_object);
        output.append(jdoc.toJson());
    }
}


void rfu::getFixtureList(quint32 scene_id, QByteArray & output) {
    Scene * scene = dynamic_cast<Scene *>(m_doc->function(scene_id));

    if(scene != nullptr) {
        QList<quint32> fs = scene->fixtures();

        if(fs.length() > 0) {
            QJsonDocument jdoc;
            QJsonObject fixtures_object;
            QJsonArray fixture_array;

            for(const auto & fid : fs) {
                Fixture * fix = m_doc->fixture(fid);

                if(fix != nullptr) {
                    for(int head = 0; head < fix->heads(); ++head) {
                        const quint32 panLSB = fix->channelNumber(QLCChannel::Pan, QLCChannel::LSB, head);
                        const quint32 tiltLSB = fix->channelNumber(QLCChannel::Tilt, QLCChannel::LSB, head);
                        QJsonObject fixture_object;
                        fixture_object["id"] = (qint64)fid;
                        fixture_object["name"] = fix->name();
                        fixture_object["head"] = (qint64)head;
                        fixture_object["panbits"] = ((panLSB == QLCChannel::invalid()) ? "8bit" : "16bit");
                        fixture_object["tiltbits"] = ((tiltLSB == QLCChannel::invalid()) ? "8bit" : "16bit");
                        fixture_array.append(fixture_object);
                    }
                }
            }

            fixtures_object["fixtures"] = fixture_array;
            jdoc.setObject(fixtures_object);
            output.append(jdoc.toJson());
        }
    }
}


void rfu::activateScene(quint32 scene_id) {
    Scene * scene = dynamic_cast<Scene *>(m_doc->function(scene_id));

    if(scene != nullptr) {
        scene->start(m_doc->masterTimer(), FunctionParent(FunctionParent::Master, 9999));
    }
}


void rfu::deactivateScene(quint32 scene_id) {
    Scene * scene = dynamic_cast<Scene *>(m_doc->function(scene_id));

    if(scene != nullptr) {
        scene->stop(FunctionParent(FunctionParent::Master, 9999));
    }
}


void rfu::getFixturePT(quint32 scene_id, quint32 fixture_id, quint32 head, QByteArray & output) {
    Scene * scene = dynamic_cast<Scene *>(m_doc->function(scene_id));

    if(scene != nullptr) {
        Fixture * fix = m_doc->fixture(fixture_id);

        if(fix != nullptr) {
            quint16 panvalue = 0;
            quint16 tiltvalue = 0;
            quint32 panMSB = fix->channelNumber(QLCChannel::Pan, QLCChannel::MSB, head);
            quint32 panLSB = fix->channelNumber(QLCChannel::Pan, QLCChannel::LSB, head);
            quint32 tiltMSB = fix->channelNumber(QLCChannel::Tilt, QLCChannel::MSB, head);
            quint32 tiltLSB = fix->channelNumber(QLCChannel::Tilt, QLCChannel::LSB, head);

            if(panMSB != QLCChannel::invalid())
                panvalue = scene->value(fixture_id, panMSB) << 8;

            if(panLSB != QLCChannel::invalid())
                panvalue += scene->value(fixture_id, panLSB);

            if(tiltMSB != QLCChannel::invalid())
                tiltvalue = scene->value(fixture_id, tiltMSB) << 8;

            if(tiltLSB != QLCChannel::invalid())
                tiltvalue += scene->value(fixture_id, tiltLSB);

            QJsonDocument jdoc;
            QJsonObject ptobject;
            QJsonObject valsobject;
            valsobject["pan"] = (qint64)panvalue;
            valsobject["tilt"] = (qint64)tiltvalue;
            ptobject["pt"] = valsobject;
            jdoc.setObject(ptobject);
            output.append(jdoc.toJson());
        }
    }
}


void rfu::offsetFixturePT(quint32 scene_id, quint32 fixture_id, quint32 head, qint16 d_x, qint16 d_y) {
    Scene * scene = dynamic_cast<Scene *>(m_doc->function(scene_id));

    if(scene != nullptr) {
        Fixture * fix = m_doc->fixture(fixture_id);

        if(fix != nullptr) {
            qint32 panvalue = 0;
            qint32 tiltvalue = 0;
            quint32 panMSB = fix->channelNumber(QLCChannel::Pan, QLCChannel::MSB, head);
            quint32 panLSB = fix->channelNumber(QLCChannel::Pan, QLCChannel::LSB, head);
            quint32 tiltMSB = fix->channelNumber(QLCChannel::Tilt, QLCChannel::MSB, head);
            quint32 tiltLSB = fix->channelNumber(QLCChannel::Tilt, QLCChannel::LSB, head);

            if(panMSB != QLCChannel::invalid())
                panvalue = scene->value(fixture_id, panMSB) << 8;

            if(panLSB != QLCChannel::invalid())
                panvalue += scene->value(fixture_id, panLSB);

            if(tiltMSB != QLCChannel::invalid())
                tiltvalue = scene->value(fixture_id, tiltMSB) << 8;

            if(tiltLSB != QLCChannel::invalid())
                tiltvalue += scene->value(fixture_id, tiltLSB);

            panvalue += d_x;
            panvalue = std::max(std::min(panvalue, 65535), 0);
            tiltvalue += d_y;
            tiltvalue = std::max(std::min(tiltvalue, 65535), 0);

            if(panMSB != QLCChannel::invalid())
                scene->setValue(SceneValue(fixture_id, panMSB, ((panvalue >> 8) & 0x000000FF)), false, false);

            if(panLSB != QLCChannel::invalid())
                scene->setValue(SceneValue(fixture_id, panLSB, (panvalue & 0x000000FF)), false, false);

            if(tiltMSB != QLCChannel::invalid())
                scene->setValue(SceneValue(fixture_id, tiltMSB, ((tiltvalue >> 8) & 0x000000FF)), false, false);

            if(tiltLSB != QLCChannel::invalid())
                scene->setValue(SceneValue(fixture_id, tiltLSB, (tiltvalue & 0x000000FF)), false, false);
        }
    }

}

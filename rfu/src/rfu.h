#ifndef RFU_H
#define RFU_H

#include <QObject>
#include "doc.h"

class rfu_socket;
class rfu_advertiser;

class rfu : public QObject {
        Q_OBJECT
    public:
        static void run(Doc * doc);
    protected:
        static rfu & instance();

        rfu(QObject * parent = nullptr);
        ~rfu();

        void set_doc(Doc * doc);

        rfu_advertiser * m_advertiser;
        rfu_socket * m_socket;
        Doc * m_doc;
        Function * m_selected_func;

    public:
        void getSceneList(QByteArray & output);
        void getFixtureList(quint32 scene_id, QByteArray & output);
        void activateScene(quint32 scene_id);
        void deactivateScene(quint32 scene_id);
        void flagFixture(quint32 fixture_id);
        void getFixturePT(quint32 scene_id, quint32 fixture_id, quint32 head, QByteArray & output);
        void offsetFixturePT(quint32 scene_id, quint32 fixture_id, quint32 head, qint16 d_x, qint16 d_y);
};

#endif // RFU_H

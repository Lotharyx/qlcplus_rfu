#ifndef RFU_MESSAGES_H
#define RFU_MESSAGES_H

#include <QJsonDocument>

class rfu_messages
{
public:
    static QJsonDocument GetSceneList();
    static QJsonDocument SceneListResponse();
    static QJsonDocument SelectScene();

    static QJsonDocument GetFixtureList();
    static QJsonDocument FixtureListResponse();
    static QJsonDocument SelectFixture();





    virtual void f() = 0; // Maka de class abstract!
};

#endif // RFU_MESSAGES_H

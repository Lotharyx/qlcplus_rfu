#ifndef RFU_ACTOR_H
#define RFU_ACTOR_H

#include <QObject>
#include <QMap>
#include <QTcpSocket>

class rfu_actor : public QObject {
        Q_OBJECT
    public:
        explicit rfu_actor(QTcpSocket * socket, QObject *parent = nullptr);

    signals:

    private:
        QTcpSocket * m_socket;
        char in_buffer[512];
        typedef void (rfu_actor::* dispatch_fn)(QString &);
        QMap<QString, dispatch_fn> m_dispatch;

        quint32 selected_scene_id;
        quint32 selected_fixture_id;

        void dispatch(QString & data);

        /// \brief Sends a JSON array of scenes to the RFU
        /// Arguments: None
        /// Example: ls
        void listScenes(QString & data);
        /// \brief Sends a list of fixtures in the selected scene to the RFU
        /// Arguments: Scene ID
        /// Example: lf,2   -- lists fixtures used in scene 2
        void listFixtures(QString & data);
        /// \brief Turns a given scene on or off
        /// Arguments: Scene ID, 1 for on, 0 for off
        /// Example: ts,2,1 -- Turns scene 2 on
        void toggleScene(QString & data);
        /// \brief Sends the a fixture's current 16-bit pan/tilt values
        /// \details If the fixture is an 8-bit fixture, the data will be
        /// left-shifted 8 bits (the 8 MSBs are used).
        /// Arguments: Scene ID, Fixture ID
        /// Example: pt,2,4 -- gets PT for fixture id 4 in scene 2
        void getFixturePT(QString & data);
        /// \brief Applies an offset to a fixture's current position
        /// Arguments: Scene ID, Fixture ID, head, X offset, Y offset
        /// Example: mv,2,4,0,+2,-6  -- moves fixture 4 head 0 in scene 2 by +2x, -6y
        void moveFixture(QString & data);

    private slots:
        void onReadyRead();

};

#endif // RFU_ACTOR_H

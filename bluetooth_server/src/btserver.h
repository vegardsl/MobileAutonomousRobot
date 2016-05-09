#ifndef BTSERVER_H
#define BTSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>

#include <qbluetoothserver.h>
#include <qbluetoothsocket.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothhostinfo.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothuuid.h>

#include "ros/ros.h"
#include "geometry_msgs/Twist.h"

class BtServer : public QObject
{
  Q_OBJECT

public:
    explicit BtServer(QObject *parent = 0);
    ~BtServer();

    void startServer(const QBluetoothAddress &localAdapter = QBluetoothAddress());
    void stopServer();

//public slots:
    void sendMessage(const QString &message);

//signals:
//    void messageReceived(const QString &sender, const QString &message);
//    void clientConnected(const QString &name);
//   void clientDisconnected(const QString &name);


private Q_SLOTS:
    void clientConnected();
    void clientDisconnected();
    void readSocket();

private:
    QBluetoothServer *rfcommServer;
    QBluetoothServiceInfo serviceInfo;
    QList<QBluetoothSocket *> clientSockets;

  
  ros::NodeHandle po_nh;
  //ros::Timer po_timer;
  ros::Publisher cmd_vel_bt;
  geometry_msgs::Twist cmd_vel_msg;

  //void onTimer();

};

#endif // BTSERVER_H

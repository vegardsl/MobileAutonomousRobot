#include "btserver.h"

#include <QCoreApplication>

/**
* Bluetooth related code is copied from
* http://doc.qt.io/qt-5/qtbluetooth-btchat-example.html
* and adapted to fit this application.
*
*/

QBluetoothLocalDevice localDevice;
QString localDeviceName;
QBluetoothAddress localDeviceAddress;

//Service UUID
static const QLatin1String serviceUuid("e8e10f95-1a70-4b27-9ccf-02010264e9c8");

BtServer::BtServer(QObject *parent)
:   QObject(parent), rfcommServer(0)
{

  cmd_vel_bt = po_nh.advertise<geometry_msgs::Twist>( "cmd_vel_bt", 1 ) ;
  /*po_timer = po_nh.createTimer( ros::Duration( 0.1 ), 
                                boost::bind( &BtServer::onTimer, this ) 
                              );
*/
  // Set initial linear command state to zero
  cmd_vel_msg.linear.x = 0;
  cmd_vel_msg.linear.y = 0;
  cmd_vel_msg.linear.z = 0;

  // Set initial angular command state to zero
  cmd_vel_msg.angular.x = 0;
  cmd_vel_msg.angular.y = 0;
  cmd_vel_msg.angular.z = 0;

}

BtServer::~BtServer()
{
    //stopServer();
}

void BtServer::startServer(const QBluetoothAddress& localAdapter)
{
  if (rfcommServer)
    return;

  rfcommServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
  connect(rfcommServer, SIGNAL(newConnection()), this, SLOT(clientConnected()));
  bool result = rfcommServer->listen(localAdapter);
  if (!result) {
    qWarning() << "Cannot bind chat server to" << localAdapter.toString();
    return;
  }

  QBluetoothServiceInfo::Sequence classId;
  classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
  serviceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList,
                             classId);
  classId.prepend(QVariant::fromValue(QBluetoothUuid(serviceUuid)));

  serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);
  serviceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList,classId);

  serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("Bt Chat Server"));
  serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription,
                           tr("Example bluetooth chat server"));
  serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr("qt-project.org"));

  serviceInfo.setServiceUuid(QBluetoothUuid(serviceUuid));
  QBluetoothServiceInfo::Sequence publicBrowse;
  publicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
  serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList,
                           publicBrowse);

  QBluetoothServiceInfo::Sequence protocolDescriptorList;
  QBluetoothServiceInfo::Sequence protocol;
  protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
  protocolDescriptorList.append(QVariant::fromValue(protocol));
  protocol.clear();
  protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
           << QVariant::fromValue(quint8(rfcommServer->serverPort()));
  protocolDescriptorList.append(QVariant::fromValue(protocol));
  serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                           protocolDescriptorList);

  serviceInfo.registerService(localAdapter);

  qDebug() << "Server started";
}

void BtServer::stopServer()
{
    // Unregister service
    serviceInfo.unregisterService();

    // Close sockets
    qDeleteAll(clientSockets);

    // Close server
    delete rfcommServer;
    rfcommServer = 0;
  qDebug() << "Server stopped";
}

void BtServer::clientConnected()
{
  QBluetoothSocket *socket = rfcommServer->nextPendingConnection();
  if (!socket)
    return;

  connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
  connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
  clientSockets.append(socket);
  qDebug() << "Client connected";
}

void BtServer::clientDisconnected()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket)
        return;

    clientSockets.removeOne(socket);

    socket->deleteLater();

  // BT-connection is lost. Set cmd_vel_bt to zero.
  cmd_vel_msg.linear.x = 0;
  cmd_vel_msg.angular.z = 0;
  cmd_vel_bt.publish(cmd_vel_msg);
}

void BtServer::readSocket()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket)
        return;

    while (socket->canReadLine()) {
      QByteArray line = socket->readLine().trimmed();
      //qDebug() << "Reading...";
      //qDebug() << "BT msg (" << line.size() << "): " << line;

      char *data = line.data();
      while (*data) {
        //float ftemp = atof(data);
        char tmp = *data;
        if(tmp == 's')
        {
          tmp = *(data+1);
	  float ftemp = (float)tmp;
	  //std::cout << "s [" << (ftemp/100) << "]" << std::endl;
          cmd_vel_msg.linear.x = ftemp/100;
        }
        if(tmp == 'a')
        {
          tmp = *(data+1);
          float ftemp = (float)tmp;
          //std::cout << "a [" << (ftemp/100) << "]" << std::endl;
          cmd_vel_msg.angular.z = ftemp/100;
        }
        //float ftemp = (float)tmp;
        //std::cout << "[" << (ftemp/100) << "]" << std::endl; // << *data <<
        ++data;
      }
    }
  
  cmd_vel_bt.publish(cmd_vel_msg);
}
/*
void BtServer::onTimer()
{
  cmd_vel_bt.publish(cmd_vel_msg);
}
*/
void setupServer()
{
  if(localDevice.isValid())
  {

    qDebug() << localDeviceName;
    localDevice.powerOn();

    localDeviceName = localDevice.name();
    localDeviceAddress= localDevice.address();

    localDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);

    QList<QBluetoothAddress> remotes;
    remotes = localDevice.connectedDevices();

    qDebug() << localDeviceName;
    qDebug() << localDeviceAddress.toString();
  }
}

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  //setupServer();

  QList<QBluetoothHostInfo> localAdapters;
  localAdapters = QBluetoothLocalDevice::allDevices();

  qDebug() << "Local adapter: " << localAdapters.at(0).address().toString();
  if (localAdapters.count() >= 2) {
    
    qDebug() << "Local adapter: " << localAdapters.at(1).address().toString();
  }
  const QBluetoothHostInfo info = localAdapters.at(0);
  QBluetoothLocalDevice adapter(localAdapters.at(0).address());
  adapter.setHostMode(QBluetoothLocalDevice::HostDiscoverable);


  ros::init(argc, argv, "btserver");

  BtServer btServer;
  btServer.startServer(info.address());
  QString localName = QBluetoothLocalDevice().name();
  qDebug() << "Local name: " << localName;
  
  app.exec();

  return 0;
}

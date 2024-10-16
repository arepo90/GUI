#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <Qt3DWindow>
#include <QForwardRenderer>
#include <QOrbitCameraController>
#include <QCuboidMesh>
#include <QCylinderMesh>
#include <QPhongMaterial>
#include <QEntity>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <QCamera>
#include <QLabel>
#include <QImage>
#include <QObject>

#include <opencv2/opencv.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <QDebug>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

struct objectInfo{
    QVector3D size;
    QVector3D meshTranslation;
    QQuaternion meshRotation;
    QVector3D pivotTranslation;
    QQuaternion pivotRotation;
    QColor color;
};

class server : public QObject{
Q_OBJECT
public:
    server(QObject *parent = nullptr);
    ~server();
    void startListening();
signals:
    void packetReceived(vector<int> packets);
    void imgReceived(cv::Mat frame);
private:
    bool handshake(SOCKET client_socket);
    int PORT = 8000;
    SOCKET socket;
    std::thread thread;
    atomic<bool> isListening;
    void handleClient(SOCKET client_socket);
};

class axis{
private:
    vector<QColor> colors{Qt::red, Qt::green, Qt::blue};
    vector<Qt3DCore::QEntity*> axisEntities;
    QVector2D size;
    Qt3DCore::QEntity *createAxis(Qt3DCore::QEntity *parent, int axis);
public:
    axis(Qt3DCore::QEntity *parent, QVector2D size);
};

class robot{
private:
    vector<Qt3DCore::QEntity*> base;
    vector<Qt3DCore::QEntity*> arm;
    vector<Qt3DCore::QTransform*> pivots;
    Qt3DCore::QEntity *createCuboid(Qt3DCore::QEntity* parent, objectInfo info);
    Qt3DCore::QEntity *createArticulation(Qt3DCore::QEntity* parent, objectInfo info);
    int armSegments;
public:
    robot(Qt3DCore::QEntity* parent, vector<objectInfo> info, int armSegments);
    void initialize(Qt3DCore::QEntity *parent);
    void updatePivot(int pivotIndex, float angle, int axis);
};

void cnlog(const string& str, int lvl){
    if(lvl == 0) qDebug() << str << '\n';
}

#endif

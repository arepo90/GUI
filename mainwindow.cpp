#include "mainwindow.h"

// --- server ---
server::server(QObject *parent) : QObject(parent), isListening(false){
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        qDebug() << "WSAStartup failed\n";
        return;
    }

    // Create a listening socket
    this->socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket == INVALID_SOCKET)
    {
        qCritical() << "Socket creation failed\n";
        WSACleanup();
        return;
    }

    // Start listening for clients in a separate thread
    this->thread = std::thread(&server::startListening, this);
}

bool server::handshake(SOCKET client_socket){
    int handshakeMessage[3], handshakeAck = 400;
    if(recv(client_socket, (char*)handshakeMessage, sizeof(handshakeMessage), 0) <= 0){
        qDebug() <<"[e] Did not receive handshake\n";
        return false;
    }
    if(handshakeMessage[0] != 0){
        qDebug() << "[e] Invalid handshake message\n";
        return false;
    }
    if(send(client_socket, (char*)&handshakeAck, sizeof(handshakeAck), 0) == SOCKET_ERROR){
        qDebug() << "[e] Could not send handshake acknowledgment\n";
        return false;
    }
    return true;
}

void server::startListening(){
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    qDebug() << "binding...\n";
    // Bind the socket
    if (::bind(socket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR){
        qDebug() << "Bind failed: " << WSAGetLastError() << "\n";
        return;
    }

    qDebug() << "Listening...\n";
    // Listen for connections
    if (listen(socket, SOMAXCONN) == SOCKET_ERROR){
        qDebug() << "Listen failed: " << WSAGetLastError() << "\n";
        return;
    }

    isListening = true;
    qDebug() << "Waiting for a connection...\n";

    // Accept an incoming connection
    SOCKET clientSocket = accept(socket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET){
        qDebug() << "Accept failed: " << WSAGetLastError() << "\n";
    }
    else{
        //handshake
        std::thread(&server::handleClient, this, clientSocket).detach();
    }
}

void server::handleClient(SOCKET clientSocket){
    qDebug() << "Client connected!\n";

    vector<uchar> buffer;
    vector<int> data(3, -1);
    int metadata[5];
    while (isListening)
    {
        int bytesReceived = recv(clientSocket, (char*)metadata, sizeof(metadata), 0);
        data[0] = metadata[2];
        data[1] = metadata[3];
        data[2] = metadata[4];
        qDebug() << "awaiting " << metadata[0] << "B\n";
        emit packetReceived(data);
        buffer.resize(metadata[0]);
        bytesReceived = 0;
        while(bytesReceived < metadata[0]){
            int curr = recv(clientSocket, (char*)buffer.data() + bytesReceived, metadata[0] - bytesReceived, 0);
            if(curr == SOCKET_ERROR){
                qDebug() << "failed to receive\n";
                break;
            }
            else if(curr <= 0){
                qDebug() << "connection closed\n";
                break;
            }
            qDebug() << "received " << curr << "B\n";
            bytesReceived += curr;
        }
        if(buffer.empty()){
            qDebug() << "empty buffer\n";
            continue;
        }
        cv::Mat frame = imdecode(buffer, cv::IMREAD_COLOR);
        if(!frame.empty()){
            qDebug() << "got frame\n";
            emit imgReceived(frame);
        }
        //imshow("Source " + to_string(index), frame);

        /*
        int result = recv(clientSocket, (char*)data, sizeof(data), 0);
        if (result > 0)
        {
            // Emit signal with the received float values
            emit packetReceived(vector<int>(data, data+3));
        }
        else if (result == 0)
        {
            qDebug() << "Connection closed\n";
            break;
        }
        else
        {
            qDebug() << "Receive failed: " << WSAGetLastError() << "\n";
            break;
        }*/
    }

    closesocket(clientSocket);
    this->~server();
}

server::~server()
{
    isListening = false; // Stop listening
    if (thread.joinable())
        thread.join(); // Wait for the thread to finish

    closesocket(socket);
    WSACleanup();
}

// --- axis ---
axis::axis(Qt3DCore::QEntity *parent, QVector2D size){
    this->size = size;
    for(int i = 0; i < 3; i++){
        axisEntities.push_back(createAxis(parent, i));
    }
}

Qt3DCore::QEntity *axis::createAxis(Qt3DCore::QEntity *parent, int axis){
    Qt3DExtras::QCylinderMesh *segment = new Qt3DExtras::QCylinderMesh();
    Qt3DCore::QEntity *axisEntity = new Qt3DCore::QEntity(parent);
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(parent);
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
    QVector3D initialTranslation((axis==0?size.y()/2.0:0.0f), (axis==1?size.y()/2.0:0.0f), (axis==2?size.y()/2.0:0.0f));
    QQuaternion initialRotation = QQuaternion::fromEulerAngles(0.0f, (axis==2?90.0f:0.0f), (axis!=1?90.0f:0.0f));
    segment->setRadius(size.x());
    segment->setLength(size.y());
    material->setAmbient(colors[axis]);
    transform->setTranslation(initialTranslation);
    transform->setRotation(initialRotation);
    axisEntity->addComponent(segment);
    axisEntity->addComponent(transform);
    axisEntity->addComponent(material);
    return axisEntity;
}

// --- cuboid ---
robot::robot(Qt3DCore::QEntity* parent, vector<objectInfo> info, int armSegments){
    base.push_back(createCuboid(parent, info[0]));
    base.push_back(createArticulation(base.front(), info[1]));
    base.push_back(createArticulation(base.front(), info[2]));
    for(int i = 0; i < armSegments; i++){
        arm.push_back(createArticulation((i==0?base.front():arm.back()), info[i+3]));
    }
    this->armSegments = armSegments;
}

Qt3DCore::QEntity *robot::createCuboid(Qt3DCore::QEntity *parent, objectInfo info){
    Qt3DExtras::QCuboidMesh *mesh = new Qt3DExtras::QCuboidMesh();
    Qt3DCore::QEntity *cuboidEntity = new Qt3DCore::QEntity(parent);
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(parent);
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform(cuboidEntity);
    mesh->setXExtent(info.size.x());
    mesh->setYExtent(info.size.y());
    mesh->setZExtent(info.size.z());
    material->setAmbient(info.color);
    transform->setTranslation(info.meshTranslation);
    transform->setRotation(info.meshRotation);
    cuboidEntity->addComponent(mesh);
    cuboidEntity->addComponent(transform);
    cuboidEntity->addComponent(material);
    return cuboidEntity;
}

Qt3DCore::QEntity *robot::createArticulation(Qt3DCore::QEntity *parent, objectInfo info){
    Qt3DCore::QEntity *pivot = new Qt3DCore::QEntity(parent);
    Qt3DCore::QTransform *pivotTransform = new Qt3DCore::QTransform(pivot);
    pivotTransform->setTranslation(info.pivotTranslation);
    pivotTransform->setRotation(info.pivotRotation);
    pivot->addComponent(pivotTransform);
    Qt3DCore::QEntity *articulation = this->createCuboid(pivot, info);
    pivots.push_back(pivotTransform);
    return articulation;
}

void robot::updatePivot(int pivotIndex, float angle, int axis){
    int sign = (axis >= 0 ? 1 : -1);
    angle *= sign;
    pivots[pivotIndex]->setRotation(QQuaternion::fromEulerAngles((axis==0?angle:0.0f), (axis==1?angle:0.0f), (axis==2?angle:0.0f)));
}

#include "mainwindow.h"

// --- Helper funcs ---
int nMap(int n, int minIn, int maxIn, int minOut, int maxOut){
    return float((n - minIn)) / float((maxIn - minIn)) * (maxOut - minOut) + minOut;
}
int nMap(float n, float minIn, float maxIn, float minOut, float maxOut){
    return float((n - minIn)) / float((maxIn - minIn)) * (maxOut - minOut) + minOut;
}

// --- Controller (xbox) WIP ---
Controller::Controller(int dead_zone){
    this->dead_zone = dead_zone;
}

Controller::~Controller(){
    return; // bruh
}

std::vector<int> Controller::readState(){
    XINPUT_STATE state;
    XInputGetState(0, &state);
    std::vector<int> states;
    states.push_back(state.Gamepad.sThumbLX);
    states.push_back(state.Gamepad.sThumbLY);
    states.push_back(state.Gamepad.sThumbRX);
    states.push_back(state.Gamepad.sThumbRY);
    for(int i = 0; i < states.size(); i++){
        if(std::abs(states[i]) < dead_zone) states[i] = 0;
        else states[i] = nMap(states[i], -32768, 32768, -255, 255);
    }
    states.push_back(state.Gamepad.bLeftTrigger);
    states.push_back(state.Gamepad.bRightTrigger);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? 1 : 0);
    states.push_back((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? 1 : 0);
    return states;
}

// --- 3D viewer ---
ModelWidget::ModelWidget(QWidget *parent) : QWidget(parent){
    root = new Qt3DCore::QEntity();
    viewport = new Qt3DExtras::Qt3DWindow();
    viewport->setRootEntity(root);
    viewport->defaultFrameGraph()->setClearColor(QColor("#202020"));
    container = QWidget::createWindowContainer(viewport, this);
    container->setMinimumSize(QSize(320, 360));
    container->setMaximumSize(QSize(320, 360));
    this->loadModels();
    Qt3DRender::QCamera *camera = viewport->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(2.0f, 2.0f, 2.0f));
    camera->setViewCenter(QVector3D(0, 0.0f, 0));
    camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    Qt3DExtras::QOrbitCameraController *cam_controller = new Qt3DExtras::QOrbitCameraController(root);
    cam_controller->setLinearSpeed(10.0f);
    cam_controller->setLookSpeed(180.0f);
    cam_controller->setCamera(camera);
    container->show();
}

void ModelWidget::loadModels(){
    Qt3DCore::QEntity *light_entity = new Qt3DCore::QEntity(root);
    Qt3DRender::QDirectionalLight *directional_light = new Qt3DRender::QDirectionalLight(light_entity);
    directional_light->setColor("white");
    directional_light->setIntensity(0.75);
    directional_light->setWorldDirection(QVector3D(-1.0, -1.0, -1.0));
    light_entity->addComponent(directional_light);
    std::vector<QString> mesh_addresses = {
        "../../assets/body_nobands.obj",
        "../../assets/left_arm.obj",
        "../../assets/right_arm.obj",
        "../../assets/band.obj",
        "../../assets/band.obj"
    };
    std::vector<QQuaternion> mesh_rotations = {
        QQuaternion::fromEulerAngles(0, 0, 0),
        QQuaternion::fromEulerAngles(0, 0, 0),
        QQuaternion::fromEulerAngles(0, 0, 0),
        QQuaternion::fromEulerAngles(0, 0, 0),
        QQuaternion::fromEulerAngles(0, 0, 0)
    };
    std::vector<QVector3D> mesh_translations = {
        QVector3D(0, 0, 0),
        QVector3D(-0.27, -0.08, 0.0),
        QVector3D(-0.27, -0.08, 0.0),
        QVector3D(-0.02, -0.08, 0.0),
        QVector3D(-0.02, -0.08, 0.0)
    };
    std::vector<QQuaternion> pivot_rotations = {
        QQuaternion::fromEulerAngles(0, 0, 0),
        QQuaternion::fromEulerAngles(0, 0, 0),
        QQuaternion::fromEulerAngles(180, 0, 0),
        QQuaternion::fromEulerAngles(180, 0, 0),
        QQuaternion::fromEulerAngles(180, 0, 0)
    };
    std::vector<QVector3D> pivot_translations = {
        QVector3D(0, 0, 0),
        QVector3D(0.085, 0.087, 0.555),
        QVector3D(0.085, 0.087, 0.145),
        QVector3D(0, 0.087, 0.145),
        QVector3D(0, 0.087, 0.617)
    };
    Qt3DExtras::QPhongMaterial *mesh_material = new Qt3DExtras::QPhongMaterial();
    mesh_material->setDiffuse(QColor("#a6a6a6"));
    band_material = new Qt3DExtras::QPhongMaterial();
    band_material->setDiffuse(Qt::black);
    // part init
    for(int i = 0; i < mesh_addresses.size(); i++){
        Qt3DCore::QEntity *pivot_entity = new Qt3DCore::QEntity((i == 0 ? root : parts[0]));
        Qt3DCore::QTransform *pivot_transform = new Qt3DCore::QTransform(pivot_entity);
        pivot_transform->setTranslation(pivot_translations[i]);
        pivot_transform->setRotation(pivot_rotations[i]);
        pivot_entity->addComponent(pivot_transform);
        // pivot indicator - debug only
        /*Qt3DExtras::QCuboidMesh *pivotMesh = new Qt3DExtras::QCuboidMesh();
        pivotMesh->setXExtent(0.05);
        pivotMesh->setYExtent(0.05);
        pivotMesh->setZExtent(0.05);
        pivotEntity->addComponent(pivotMesh);
        pivotEntity->addComponent(material);*/
        Qt3DCore::QEntity *mesh_entity = new Qt3DCore::QEntity(pivot_entity);
        Qt3DCore::QTransform *mesh_transform = new Qt3DCore::QTransform(mesh_entity);
        Qt3DRender::QMesh *mesh = new Qt3DRender::QMesh();
        mesh->setSource(QUrl::fromLocalFile(mesh_addresses[i]));
        mesh_transform->setTranslation(mesh_translations[i]);
        mesh_transform->setRotation(mesh_rotations[i]);
        mesh_entity->addComponent(mesh);
        mesh_entity->addComponent((i < 3 ? mesh_material : band_material));
        mesh_entity->addComponent(mesh_transform);
        parts.push_back(pivot_entity);
        pivots.push_back(pivot_transform);
    }
    // axis init
    for(int i = 0; i < 3; i++){
        Qt3DExtras::QCylinderMesh *segment = new Qt3DExtras::QCylinderMesh();
        Qt3DCore::QEntity *axis_entity = new Qt3DCore::QEntity(root);
        Qt3DExtras::QPhongMaterial *axis_material = new Qt3DExtras::QPhongMaterial();
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
        QVector3D initial_translation((i == 0 ? 0.5f : 0.0f), (i == 1 ? 0.5f : 0.0f), (i == 2 ? 0.5f : 0.0f));
        QQuaternion initial_rotation = QQuaternion::fromEulerAngles(0.0f, (i == 2 ? 90.0f : 0.0f), (i != 1 ? 90.0f : 0.0f));
        segment->setRadius(0.001f);
        segment->setLength(1.0f);
        axis_material->setAmbient(i == 0 ? Qt::red : (i == 1 ? Qt::green : Qt::blue));
        transform->setTranslation(initial_translation);
        transform->setRotation(initial_rotation);
        axis_entity->addComponent(segment);
        axis_entity->addComponent(transform);
        axis_entity->addComponent(axis_material);
    }
}

// ONLY FOR BASE (ROOT) ENTITY
void ModelWidget::updateModel(float angleX, float angleY, float angleZ){
    pivots[0]->setRotation(QQuaternion::fromEulerAngles(angleX, angleY, angleZ));
}

// ONLY FOR ARTICULATION (PIVOT) ENTITIES
void ModelWidget::updatePivot(int index, float angle){
    pivots[index]->setRotation(QQuaternion::fromEulerAngles(pivots[index]->rotationX(), pivots[index]->rotationY(), angle));
}

void ModelWidget::updateColor(QColor color){
    band_material->setDiffuse(color);
}

// --- cam subsections ---
SubsectionWidget::SubsectionWidget(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);
    cameraView = new QLabel();
    cameraDropdown = new QComboBox();
    cameraDropdown->addItem("No Camera");
    id = -1;
    layout->addWidget(cameraDropdown);
    layout->addWidget(cameraView);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    connect(cameraDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SubsectionWidget::onCameraSelected);
    connect(cameraDropdown, &QComboBox::currentIndexChanged, this, [this]() { emit selectionChanged(); });
    cameraView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cameraView->setPixmap(QPixmap("../../assets/404.png").scaled((fullScreen ? QSize(960, 720) : QSize(480, 360)), Qt::KeepAspectRatio));
    is_active.store(false);
}

SubsectionWidget::~SubsectionWidget() {
    qDebug() << "subsection destructor";
}

void SubsectionWidget::updateAvailableOptions(const QSet<QString> &usedOptions) {
    auto * model = qobject_cast<QStandardItemModel*>(cameraDropdown->model());
    assert(model);
    if(!model) return;
    for (int i = 0; i < cameraDropdown->count(); i++) {
        QString option = cameraDropdown->itemText(i);
        auto * item = model->item(i);
        assert(item);
        if(!item) continue;
        item->setEnabled(!usedOptions.contains(option));
    }
}

void SubsectionWidget::setAvailableDevices(int num_cams) {
    for(int i = 1; i <= num_cams; i++){
        cameraDropdown->addItem(QString("Camera %1").arg(i), i);
    }
}

void SubsectionWidget::onCameraSelected(int index) {
    id = index-1;
    if(index <= 0) cameraView->setPixmap(QPixmap("../../assets/404.png").scaled((fullScreen ? QSize(960, 720) : QSize(480, 360)), Qt::KeepAspectRatio));
}

void SubsectionWidget::updateFrame(QImage img){
    cameraView->setPixmap(QPixmap::fromImage(img).scaled((fullScreen ? QSize(960, 720) : QSize(480, 360)), Qt::KeepAspectRatio));
}

void SubsectionWidget::mousePressEvent(QMouseEvent *event) {
    emit subsectionClicked(this);
    QWidget::mousePressEvent(event);
}

void SubsectionWidget::setFullScreenMode(bool fullScreen) {
    this->fullScreen = fullScreen;
}

std::pair<int, QString> SubsectionWidget::getCurrentSelection()  {
    return std::make_pair(id, cameraDropdown->currentText());
}

// --- main window ---
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), isEnlarged(false) {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    leftContainer = new QWidget(this);
    leftLayout = new QGridLayout(leftContainer);
    rightLayout = new QVBoxLayout;
    rightLayout->setSpacing(0);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->setFixedSize(1280, 720);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j){
            cam_map.insert({subsections.size(), -1});
            SubsectionWidget *widget = new SubsectionWidget(this);
            connect(widget, &SubsectionWidget::subsectionClicked, this, &MainWindow::handleSubsectionClick);
            connect(widget, &SubsectionWidget::selectionChanged, this, &MainWindow::updateDropdowns);
            subsections.push_back(widget);
            leftLayout->addWidget(widget, i, j);
        }
    }
    QLabel *rightLabelTop = new QLabel("Right Top Section");
    QLabel *rightLabelBottom = new QLabel("Right Bottom Section WIP");
    // 3D model
    model = new ModelWidget(this);
    rightLayout->addWidget(model);
    rightLayout->addWidget(rightLabelBottom);
    mainLayout->addWidget(leftContainer, 3); // 3/4 width
    mainLayout->addLayout(rightLayout, 1);  // 1/4 width
}

void MainWindow::setCamPorts(int num_cams){
    for(auto subsection : subsections){
        subsection->setAvailableDevices(num_cams);
    }
}

void MainWindow::updateFrame(int id, std::vector<unsigned char> data){
    cv::Mat frame = cv::imdecode(data, cv::IMREAD_COLOR);
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    int sub_id = 0;
    for(auto it = cam_map.begin(); it != cam_map.end(); it++){
        if(it->second == id) sub_id = it->first;
    }
    subsections[sub_id]->updateFrame(image);
}

MainWindow::~MainWindow(){
    qDebug() << "closing...\n";
    for(auto subsection : subsections){
        subsection->destroy();
    }
}

// SIGNAL INTERCEPT
void MainWindow::closeEvent(QCloseEvent* event) {
    emit windowClosing();
    event->accept();
}

// WIP
void MainWindow::updateState(std::vector<float> data){
    for(auto it : data){
        qDebug() << "ros2: " << it;
    }
    /*
    model->updatePivot(1, data[0]);
    model->updatePivot(2, data[1]);
    QColor color;
    if(data[2] < 0) color = QColor(nMap((int)data[2], -100, 0, 20, 250), 0, 0);
    else if(data[2] > 0) color = QColor(0, nMap((int)data[2], 0, 100, 20, 250), 0);
    else color = Qt::black;
    model->updateColor(color);
    model->updateColor(color);
    //model->updateModel(data[3], data[4], data[5]);
    //update ros-rotas implementation
    */
}

std::vector<int> MainWindow::scanVideoCaptureDevices() {
    std::vector<int> availableDevices;
    for (int i = 0; i < 3; i++) {
        cv::VideoCapture cap(i);
        if (cap.isOpened()) {
            availableDevices.push_back(i);
            cap.release();
        }
    }
    return availableDevices;
}

void MainWindow::updateDropdowns() {
    QSet<QString> usedOptions;
    /*for (auto subsection : subsections) {
        std::pair<int, QString> selection= subsection->getCurrentSelection();
        QString selection_text = selection.second;
        if(selection.second != QString("No Camera")) usedOptions.insert(selection.second);
    }*/
    for(int i = 0; i < subsections.size(); i++){
        std::pair<int, QString> selection = subsections[i]->getCurrentSelection();
        QString selection_text = selection.second;
        if(selection.first >= 0){
            usedOptions.insert(selection.second);
            cam_map[i] = selection.first;
        }
        else cam_map[i] = -1;
    }
    for (auto subsection : subsections) {
        subsection->updateAvailableOptions(usedOptions);
    }
    qDebug() << "window: updateDropdowns";
    emit selectionChanged(cam_map);
}

void MainWindow::handleSubsectionClick(SubsectionWidget *clickedWidget) {
    if (!isEnlarged) {
        for (auto subsection : subsections) {
            if(subsection == clickedWidget){
                clickedWidget->setFullScreenMode(true);
                leftLayout->addWidget(clickedWidget, 0, 0, 2, 2);
            }
            else{
                subsection->hide();
            }
        }
        isEnlarged = true;
    } else {
        int i = 0;
        for (auto subsection : subsections) {
            subsection->setFullScreenMode(false);
            subsection->show();
            leftLayout->addWidget(subsection, i / 2, i % 2);
            i++;
        }
        isEnlarged = false;
    }
}

// --- DEPRECATED CLASS - USE RTPSTREAMHANDLER INSTEAD ---
/*
RTPServer::RTPServer(int port, PayloadType type, QObject *parent) : QObject(parent){
    client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(client_socket == INVALID_SOCKET){
        qDebug("[e] Could not create socket");
        WSACleanup();
    }
    int recv_buff_size = 1024 * 1024;  // 1MB
    setsockopt(client_socket, SOL_SOCKET, SO_RCVBUF, (char*)&recv_buff_size, sizeof(recv_buff_size));
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);
    socket_address.sin_addr.s_addr = INADDR_ANY;
    if(::bind(client_socket, (struct sockaddr*)&socket_address, socket_address_size) == SOCKET_ERROR){
        qDebug() << "bind failed, error:" << WSAGetLastError();
    }
    is_running = true;
    stream = new Stream;
    stream->type = type;
    stream->is_initialized = false;
    if(type == PayloadType::AUDIO_PCM){
        int error;
        opus_decoder = opus_decoder_create(SAMPLE_RATE, 1, &error);
    }
    else listening_thread = std::thread(&RTPServer::startListening, this);
}

RTPServer::~RTPServer(){
    is_running = false;
    closesocket(client_socket);
    if(listening_thread.joinable()) listening_thread.join();
    if(stream->type == PayloadType::AUDIO_PCM) opus_decoder_destroy(opus_decoder);
}

void RTPServer::startListening(){
    std::vector<char> packet(MAX_PACKET_SIZE);
    int socket_address_size = sizeof(socket_address);
    sockaddr_in senderAddr;
    int senderAddrLen = sizeof(senderAddr);
    qDebug() << "server awaiting data";
    while(is_running){
        //packet.resize(MAX_PACKET_SIZE);
        //int bytes_received = recvfrom(client_socket, reinterpret_cast<char*>(packet.data()), MAX_PACKET_SIZE, 0, (struct sockaddr*)&socket_address, &socket_address_size);
        int bytes_received = recvfrom(client_socket, packet.data(), packet.size(), 0, (struct sockaddr*)&socket_address, &socket_address_size);
        if(bytes_received == SOCKET_ERROR){
            qDebug() << "socket error: " << WSAGetLastError();
            continue;
        }
        else if(bytes_received < sizeof(RTPHeader)) continue;
        qDebug() << "bytes received: " << bytes_received;


        header = new RTPHeader;
        std::memcpy(header, packet.data(), sizeof(RTPHeader));
        if(!stream->is_initialized) stream->is_initialized = true;
        else if(header->seq_num != stream->seq_num + 1 && stream->seq_num != 65535){
            qDebug() << "Packet loss detected! Expected: " << stream->seq_num + 1 << " Got: " << header->seq_num;
        }
        stream->seq_num = header->seq_num;


        if(stream->type == PayloadType::ROS2_ARRAY && floatCallback){
            //std::vector<float> data((bytes_received - sizeof(RTPHeader)) / sizeof(float));
            //std::memcpy(data.data(), packet.data() + sizeof(RTPHeader), bytes_received - sizeof(RTPHeader));
            std::vector<float> data((bytes_received - sizeof(RTPHeader)) / sizeof(float));
            std::memcpy(data.data(), packet.data() + sizeof(RTPHeader), bytes_received - sizeof(RTPHeader));
            floatCallback(data);
        }
        else if(stream->type == PayloadType::VIDEO_MJPEG && ucharCallback){
            //std::vector<unsigned char> data(bytes_received - sizeof(RTPHeader));
            //std::copy(packet.begin() + sizeof(RTPHeader), packet.begin() + bytes_received, data.begin());
            //maybeno std::memcpy(data.data(), packet.data() + sizeof(RTPHeader), bytes_received - sizeof(RTPHeader));
            std::vector<unsigned char> data(bytes_received - sizeof(RTPHeader));
            std::copy(packet.begin() + sizeof(RTPHeader), packet.begin() + bytes_received, data.begin());
            ucharCallback(data);
        }


        const int16_t* int16Ptr = reinterpret_cast<const int16_t*>(packet.data());
        size_t int16Size = bytes_received / 2;
        std::vector<int16_t> data(int16Ptr, int16Ptr + int16Size);


        RTPHeader* header = reinterpret_cast<RTPHeader*>(packet.data());
        char* payload = packet.data() + sizeof(RTPHeader);
        size_t payload_size = bytes_received - sizeof(RTPHeader);
        header->seq = ntohs(header->seq);
        header->timestamp = ntohl(header->timestamp);
        header->ssrc = ntohl(header->ssrc);


        //RTPHeader* header = new RTPHeader;
        //std::memcpy(header, packet.data(), sizeof(RTPHeader));

        if(!stream->is_initialized) stream->is_initialized = true;
        else if(header->seq != stream->last_seq + 1 && stream->last_seq != 65535){
            qDebug() << "Packet loss detected! Expected: " << stream->last_seq + 1 << " Got: " << header->seq;
        }
        stream->last_seq = header->seq;

        if(stream->type == PayloadType::ROS2_ARRAY && floatCallback){
            std::vector<float> data((bytes_received - sizeof(RTPHeader)) / sizeof(float));
            std::memcpy(data.data(), packet.data() + sizeof(RTPHeader), bytes_received - sizeof(RTPHeader));
            floatCallback(data);
        }
        else if((stream->type == PayloadType::AUDIO_PCM || stream->type == PayloadType::VIDEO_MJPEG) && ucharCallback){
            //std::vector<unsigned char> data(bytes_received - sizeof(RTPHeader));
            //std::copy(packet.begin() + sizeof(RTPHeader), packet.begin() + bytes_received, data.begin());
            //maybeno std::memcpy(data.data(), packet.data() + sizeof(RTPHeader), bytes_received - sizeof(RTPHeader));
            std::vector<unsigned char> data(bytes_received);
            std::copy(packet.begin(), packet.begin()+bytes_received, data.begin());
            qDebug() << "about to callback, data size: " << data.size();
            ucharCallback(data);
        }

        //if(data_callback) data_callback(payload);


        if(static_cast<PayloadType>(header->pt) == PayloadType::AUDIO_PCM || static_cast<PayloadType>(header->pt) == PayloadType::ROS2_ARRAY){
            if(data_callback) data_callback(header->ssrc, payload, payload_size);
        }
        else{
            if(stream->current_frame.data.empty()) stream->current_frame.timestamp = header->timestamp;
            stream->current_frame.data.insert(stream->current_frame.data.end(), payload, payload + payload_size);
            if(header->m){
                stream->current_frame.complete = true;
                if(data_callback) data_callback(header->ssrc, stream->current_frame.data.data(), stream->current_frame.data.size());
                stream->complete_frames.push(std::move(stream->current_frame));
                stream->current_frame = Frame();
                if(stream->complete_frames.size() > 30) stream->complete_frames.pop();
            }
        }

        //packet.clear();
    }
}

int RTPServer::audioCallback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData){
    RTPServer* self = static_cast<RTPServer*>(userData);
    return self->audioProcess(input, output, frameCount, timeInfo, statusFlags);
}

int RTPServer::audioProcess(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags){
    if(!is_running) return paContinue;
    unsigned char packet[4000];
    int socket_address_size = sizeof(socket_address);
    int bytes_received = recvfrom(client_socket, (char*)packet, sizeof(packet), 0, (struct sockaddr*)&socket_address, &socket_address_size);
    if(bytes_received > sizeof(uint32_t)) {
        opus_decode(opus_decoder, packet, bytes_received, (opus_int16*)output, AUDIO_BUFFER_SIZE, 0);
    }
    return paContinue;
}

void RTPServer::sendPacket(std::vector<int> data){
    std::vector<char> packet(data.size() * sizeof(int));
    std::memcpy(packet.data(), data.data(), data.size()*sizeof(int));
    if(sendto(client_socket, packet.data(), packet.size(), 0, (struct sockaddr*)&socket_address, sizeof(socket_address)) == SOCKET_ERROR){
        qDebug() << "[w] Packet send failed" << WSAGetLastError();
    }
}
*/

// --- ROTAS stream handler ---
RTPStreamHandler::RTPStreamHandler(int port, std::string address, PayloadType type, QObject *parent) : QObject(parent){
    stream = new Stream;
    stream->ssrc = 0;
    stream->seq_num = 0 & 0xFFFF;
    stream->timestamp = 0;
    stream->payload_type = type;
    // --- UDP Socket init ---
    // --- send ---
    send_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    send_socket_address.sin_family = AF_INET;
    send_socket_address.sin_port = htons(port + 1);
    inet_pton(AF_INET, address.c_str(), &send_socket_address.sin_addr);
    // -- recv --
    recv_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int recv_buff_size = 1024 * 1024;   // 1MB
    setsockopt(recv_socket, SOL_SOCKET, SO_RCVBUF, (char*)&recv_buff_size, sizeof(recv_buff_size));
    recv_socket_address.sin_family = AF_INET;
    recv_socket_address.sin_port = htons(port);
    recv_socket_address.sin_addr.s_addr = INADDR_ANY;
    bind(recv_socket, (struct sockaddr*)&recv_socket_address, socket_address_size);
}

RTPStreamHandler::~RTPStreamHandler(){
    closesocket(send_socket);
    closesocket(recv_socket);
}

void RTPStreamHandler::recvPacket(){
    std::vector<char> packet(MAX_PACKET_SIZE);
    int bytes_received = recvfrom(recv_socket, packet.data(), packet.size(), 0, (struct sockaddr*)&recv_socket_address, &socket_address_size);
    qDebug() << "received: " << bytes_received;
    if(bytes_received == SOCKET_ERROR){
        qDebug() << "[e] Packet recv failed. Winsock error: " << WSAGetLastError() << "\n";
        return;
    }
    RTPHeader* header = new RTPHeader;
    std::memcpy(header, packet.data(), sizeof(RTPHeader));
    if(header->seq_num != stream->seq_num){
        //qDebug() << "Packet loss detected! Expected: " << stream->seq_num + 1 << " Got: " << header->seq_num;
    }
    if(bytes_received <= sizeof(RTPHeader)){
        qDebug() << "empty packet";
        return;
    }
    if(stream->payload_type == PayloadType::ROS2_ARRAY && floatCallback){
        std::vector<float> data((bytes_received - sizeof(RTPHeader)) / sizeof(float));
        std::memcpy(data.data(), packet.data() + sizeof(RTPHeader), bytes_received - sizeof(RTPHeader));
        qDebug() << "about to call float " << data[0] << " " << data[1];
        floatCallback(data);
    }
    else if(stream->payload_type == PayloadType::VIDEO_MJPEG && ucharCallback){
        std::vector<unsigned char> data(bytes_received - sizeof(RTPHeader));
        std::memcpy(data.data(), packet.data() + sizeof(RTPHeader), bytes_received - sizeof(RTPHeader));
        ucharCallback(data);
    }
    else{
        qDebug() << "[e] Payload / Callback error on packet recv";
    }
}

void RTPStreamHandler::sendPacket(std::vector<int> data){
    // --- RTP header info ---
    RTPHeader header;
    header.version = 2;
    header.p = 0;
    header.x = 0;
    header.cc = 0;
    header.m = 1;
    header.pt = static_cast<uint8_t>(stream->payload_type);
    header.seq_num = stream->seq_num++;
    header.timestamp = stream->timestamp;
    header.ssrc = stream->ssrc;
    stream->timestamp += 100; // fix this shit
    // --- Prepare packet for send ---
    std::vector<char> packet((data.size() * sizeof(int)) + sizeof(RTPHeader));
    std::memcpy(packet.data(), &header, sizeof(RTPHeader));
    std::memcpy(packet.data() + sizeof(RTPHeader), data.data(), data.size() * sizeof(int));
    // --- Simulated network degradation (lowkey trash implementation but idc) ---
    /*
        this_thread::sleep_for(std::chrono::milliseconds(rand() % 150));    // ~100ms latency
        if(rand() % 10 == 0) return;    // ~10% packet loss
        */
    if(sendto(send_socket, packet.data(), packet.size(), 0, (struct sockaddr*)&send_socket_address, socket_address_size) == SOCKET_ERROR){
        qDebug() << "[w] Packet send failed. Winsock error: " << WSAGetLastError();
    }
}

AppHandler::AppHandler(int port, QObject* parent) : QObject(parent){
    window = new MainWindow;
    window->setWindowTitle("GUI debug");
    window->resize(1280, 720);
    QObject::connect(window, &MainWindow::windowClosing, [this](){ this->destroy(); });
    this->port = port;
    base_socket = new SocketStruct;
    base_socket->target_socket = new RTPStreamHandler(port, CLIENT_IP, PayloadType::ROS2_ARRAY);
    base_socket->target_socket->setFloatCallback([this](std::vector<float> data) { this->baseRecvCallback(data); });
    base_socket->is_recv_running.store(true);
    base_socket->is_send_running.store(true);
    //base_socket->target_socket->recvPacket();
    qRegisterMetaType<std::map<int, int>>("std::map<int,int>");
    connect(window, &MainWindow::selectionChanged, this, &AppHandler::activeCamsCallback);
}

AppHandler::~AppHandler(){
    base_socket->is_recv_running.store(false);
    base_socket->is_send_running.store(false);
    base_socket->recv_thread.join();
    base_socket->send_thread.join();
    base_socket->target_socket->destroy();
    for(int i = 0; i < video_sockets.size(); i++){
        video_sockets[i]->is_recv_running.store(false);
        video_sockets[i]->is_send_running.store(false);
        video_sockets[i]->recv_thread.join();
        video_sockets[i]->send_thread.join();
        video_sockets[i]->target_socket->destroy();
    }
    WSACleanup();
}

void AppHandler::init(){
    base_socket->target_socket->recvPacket();
    int num_cams = 0;
    {
        std::lock_guard<std::mutex> lock(base_socket->data_mutex);
        num_cams = (int)base_socket->float_data[1];
    }
    qDebug() << "num_cams: " << num_cams;
    window->setCamPorts(num_cams);
    for(int i = 0; i < num_cams; i++){
        SocketStruct* video_socket = new SocketStruct;
        video_socket->target_socket = new RTPStreamHandler(port + (2 * i) + 4, CLIENT_IP, PayloadType::VIDEO_MJPEG);
        video_sockets.push_back(std::move(video_socket));
    }
    for(int i = 0; i < video_sockets.size(); i++){
        video_sockets[i]->is_active.store(false);
        video_sockets[i]->is_send_running.store(true);
        video_sockets[i]->is_recv_running.store(true);
        video_sockets[i]->target_socket->setUCharCallback([this, i](std::vector<uchar> data) { window->updateFrame(i, data); });
        video_sockets[i]->recv_thread = std::thread([i, this](){
            while(video_sockets[i]->is_recv_running.load()){
                video_sockets[i]->target_socket->recvPacket();
            }
        });
        video_sockets[i]->send_thread = std::thread([i, this](){
            while(video_sockets[i]->is_send_running.load()){
                std::vector<int> data = {200, (int)video_sockets[i]->is_active.load() };
                video_sockets[i]->target_socket->sendPacket(data);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        });
        qDebug() << "[i] Created video socket " << i << " on port " << port + (2 * i) + 4;
    }
    //base_socket->recv_thread = std::thread(&AppHandler::baseRecvCallback, this);
    //base_socket->send_thread = std::thread(&AppHandler::baseSendCallback, this);
    window->show();
}

void AppHandler::activeCamsCallback(std::map<int, int> cam_map){
    for(int i = 0; i < video_sockets.size(); i++){
        video_sockets[i]->is_active.store(false);
    }
    for(auto it = cam_map.begin(); it != cam_map.end(); it++){
        if(it->second >= 0) video_sockets[it->second]->is_active.store(true);
        //qDebug() << "cams callback " << it->first << " " << it->second;
    }
}

void AppHandler::baseRecvCallback(std::vector<float> data){
    std::lock_guard<std::mutex> lock(base_socket->data_mutex);
    base_socket->float_data = data;
}

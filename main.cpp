/*
    ROBOTEC 2025

    DISCLAIMER:
    APP WILL ONLY RUN WITH THE FOLLOWING DEPENDENCIES:
    - QT6 + WIDGETS + 3D + RANDOM STUFF I DONT REMEMBER
    - OPENCV (binaries)
    - PORTAUDIO (vcpkg)
    - OPUS (vcpkg)
    - XINPUT (windows sdk)

    MAY OR MAY NOT WORK WITHOUT THEM (you can comment them in CMakeLists.txt and code lines themselves)
*/

#include "mainwindow.h"
#include <QApplication>

// --- tests n stuff ---
/*
#define AUDIO_PORT 8001
#define VIDEO_PORT 8002
#define SAMPLE_RATE2 48000
#define CHANNELS 1
#define FRAME_SIZE 960

SOCKET audioSocket, videoSocket;
//struct sockaddr_in serverAddr;
sockaddr_in audioAddr, videoAddr;
OpusDecoder* opusDecoder;
std::vector<opus_int16> audioBuffer(FRAME_SIZE);
// Audio callback
int audioCallback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    qDebug() << "audio start";
    SOCKET* sock = static_cast<SOCKET*>(userData);
    unsigned char recvBuffer[4000];
    int recvLen = recvfrom(*sock, (char*)recvBuffer, sizeof(recvBuffer), 0, NULL, NULL);
    if (recvLen > sizeof(uint32_t)) {
        opus_decode(opusDecoder, recvBuffer, recvLen, (opus_int16*)output, FRAME_SIZE, 0);
    }
    qDebug() << "audio end";
    return paContinue;
}

void videoStream() {
    videoSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    videoAddr.sin_family = AF_INET;
    videoAddr.sin_port = htons(VIDEO_PORT);
    videoAddr.sin_addr.s_addr = INADDR_ANY;
    if(::bind(videoSocket, (struct sockaddr*)&videoAddr, sizeof(videoAddr)) == SOCKET_ERROR) {
        qDebug() << "Failed to bind video socket: " << WSAGetLastError();
    }
    qDebug() << "start video thread";
    cv::Mat frame = cv::imread("../../assets/404.png", 0);
    window->updateFrame(frame);
    frame.release();
    uint32_t expectedSeqNum = 0;
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);
    while (true) {
        qDebug() << "video start";
        //unsigned char recvBuffer[65536];
        std::vector<uchar> recvBuffer(65536);
        int recvLen = recvfrom(videoSocket, (char*)recvBuffer.data(), recvBuffer.size(), 0, (sockaddr*)&senderAddr, &senderAddrSize);
        if(recvLen == SOCKET_ERROR) {
            int error = WSAGetLastError();
            qDebug() << "recvfrom() failed: " << error;
            continue;
        }
        qDebug() << "video mid";
        if (recvLen > sizeof(uint32_t)) {
            qDebug() << "size: " << recvBuffer.size();
            frame = cv::imdecode(recvBuffer, cv::IMREAD_COLOR);
            if (!frame.empty()) {
                //cv::imshow("Video Stream", frame);
                //cv::waitKey(1);
                window->updateFrame(frame);
            }
            //expectedSeqNum = seqNum + 1;
        }
        qDebug() << "video end";
    }
}
*/
/*
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    std::thread videoThread = std::thread(videoStream);

    audioSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    audioAddr.sin_family = AF_INET;
    audioAddr.sin_port = htons(AUDIO_PORT);
    audioAddr.sin_addr.s_addr = INADDR_ANY;
    ::bind(audioSocket, (struct sockaddr*)&audioAddr, sizeof(audioAddr));

    int error;
    opusDecoder = opus_decoder_create(SAMPLE_RATE2, CHANNELS, &error);

    Pa_Initialize();
    PaStream* stream;
    Pa_OpenDefaultStream(&stream, 0, 1, paInt16, SAMPLE_RATE2, FRAME_SIZE, audioCallback, &audioSocket);
    Pa_StartStream(stream);

    QObject::connect(window, &MainWindow::windowClosing, [stream, &videoThread](){
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        Pa_Terminate();
        opus_decoder_destroy(opusDecoder);
        closesocket(audioSocket);
        closesocket(videoSocket);
        WSACleanup();
        videoThread.join();
    });
*/

/*
    controller_socket = new RTPServer(8000, PayloadType::AUDIO_PCM);
    controller = new Controller(1200);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::thread controller_thread = std::thread(callback);
    controller_thread.join();
    //RTPServer* base_socket = new RTPServer(8000, PayloadType::ROS2_ARRAY);
    RTPServer* audio_socket = new RTPServer(8001, PayloadType::AUDIO_PCM);
    RTPServer* video_socket = new RTPServer(8002, PayloadType::VIDEO_MJPEG);
    Pa_Initialize();
    PaStream* stream;
    Pa_OpenDefaultStream(&stream, 0, 1, paInt16, SAMPLE_RATE, 2880, RTPServer::audioCallback, audio_socket);
    Pa_StartStream(stream);
    qDebug() << "done";
    //base_socket->setFloatCallback(std::bind(&MainWindow::updateState, window, std::placeholders::_1));
    //audio_socket->setUCharCallback(std::bind(&AudioPlayer::decodePlay, player, std::placeholders::_1));
    video_socket->setUCharCallback(std::bind(&MainWindow::updateFrame, window, std::placeholders::_1));
    QObject::connect(window, &MainWindow::windowClosing, [&base_socket, &stream, &audio_socket, &video_socket](){
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        Pa_Terminate();
        //base_socket->destroy();
        audio_socket->destroy();
        video_socket->destroy();
        WSACleanup();
    });
*/

// WORK IN PROGRESS OK ?😭😭😭😭😭


int main(int argc, char* argv[]){
    QApplication app(argc, argv);
    WSAData wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
    //AppHandler* app_handler = new AppHandler(8000);
    //app_handler->init();

    // DEBUG ONLY - ROTAS STREAM DISABLED
    MainWindow* window = new MainWindow;
    window->setWindowTitle("GUI debug");
    window->resize(1280, 720);
    window->show();

    return app.exec();
}

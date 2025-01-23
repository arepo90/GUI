#include "mainwindow.h"

vector<objectInfo> buildInfo(QVector3D baseSize, QVector3D supportSize, QVector3D armSize, int armSegments){
    objectInfo baseInfo = {
                   baseSize,
                   QVector3D(0.0f, 0.0f, 0.0f),
                   QQuaternion::fromEulerAngles(0.0f, 0.0f, 0.0f),
                   QVector3D(0.0f, 0.0f, 0.0f),
                   QQuaternion::fromEulerAngles(0.0f, 0.0f, 0.0f),
                   Qt::black
               },
        supportInfo = {
            supportSize,
            QVector3D(supportSize.x()/2, 0.0f, -(supportSize.z()-baseSize.z())/2.0),
            QQuaternion::fromEulerAngles(0.0f, 0.0f, 0.0f),
            QVector3D(baseSize.x()/2.0, (baseSize.y()-supportSize.y())/2.0, 0.0f),
            QQuaternion::fromEulerAngles(0.0f, 0.0f, 0.0f),
            Qt::black
        },
        armInfo = {
            armSize,
            QVector3D(0.0f, 0.0f, armSize.z()/2.0),
            QQuaternion::fromEulerAngles(0.0f, 0.0f, 0.0f),
            QVector3D(0.0f, 0.0f, armSize.z()/2.0),
            QQuaternion::fromEulerAngles(0.0f, 0.0f, 0.0f),
            Qt::black
        };
    vector<objectInfo> info{baseInfo, supportInfo, supportInfo};
    info[2].pivotTranslation.setX(-supportInfo.pivotTranslation.x());
    info[2].meshTranslation.setX(-supportInfo.meshTranslation.x());
    for(int i = 0; i < armSegments; i++){
        info.push_back(armInfo);
    }
    info[3].pivotTranslation.setZ(baseSize.z()/2.0);
    return info;
}

int main(int argc, char* argv[]){
    //App
    qDebug() << "starting...\n";
    QApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
    view->setRootEntity(rootEntity);

    /*  ToDo:
        2. Probar ventanas en blanco nuevas con layout
        3. Probar video de OpenCV en ventanas nueva
        4. Implementar ROTAS
    */

    /*  ACTUAL ToDo:
        1. Drop downs para subsections de izquierda
        2. Source scanner
        3. Refactorizar
        4. Implementar ROS2 topics
        5. Resize sections
    */


    vector<objectInfo> info = buildInfo(QVector3D(5.0f, 8.0f, 1.0f), QVector3D(1.0f, 1.0f, 3.0f), QVector3D(0.5f, 0.5f, 2.0f), 3);
    //Constructors
    robot robot(rootEntity, info, 3);
    axis axis(rootEntity, QVector2D(0.1f, 10.0f));
    /*server winsock;
    QObject::connect(&winsock, &server::packetReceived, [&robot](vector<int> packets){
        qDebug() << "Received: " << packets[0] << " " << packets[1] << " " << packets[2] << "\n";
        robot.updatePivot(0, (float)packets[0], 0);
        robot.updatePivot(1, (float)packets[1], 0);
        robot.updatePivot(3, (float)packets[2], 0);
    });*/

    //Camera
    Qt3DRender::QCamera *camera = view->camera();
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(20.0f, 20.0f, 20.0f));
    camera->setViewCenter(QVector3D(0, 0.0f, 0));
    camera->setUpVector(QVector3D(0.0f, 0.0f, 1.0f));
    camController->setLinearSpeed(0.0f);
    camController->setLookSpeed(180.0f);
    camController->setCamera(camera);

    //Sliders
    QSlider *pivot1Slider = new QSlider(Qt::Horizontal);
    pivot1Slider->setRange(-90, 90);
    QObject::connect(pivot1Slider, &QSlider::valueChanged, [&robot](int value) {
        robot.updatePivot(2, float(value), 2);
    });
    pivot1Slider->setMaximumHeight(20);
    pivot1Slider->setMaximumWidth(200);

    QSlider *pivot2Slider = new QSlider(Qt::Horizontal);
    pivot2Slider->setRange(0, 90);
    QObject::connect(pivot2Slider, &QSlider::valueChanged, [&robot](int value) {
        robot.updatePivot(3, float(value), 0);
    });
    pivot2Slider->setMaximumHeight(20);
    pivot2Slider->setMaximumWidth(200);

    QSlider *pivot3Slider = new QSlider(Qt::Horizontal);
    pivot3Slider->setRange(0, 90);
    QObject::connect(pivot3Slider, &QSlider::valueChanged, [&robot](int value) {
        robot.updatePivot(4, float(value), 0);
    });
    pivot3Slider->setMaximumHeight(20);
    pivot3Slider->setMaximumWidth(200);

    QSlider *pivot4Slider = new QSlider(Qt::Horizontal);
    pivot4Slider->setRange(-90, 90);
    QObject::connect(pivot4Slider, &QSlider::valueChanged, [&robot](int value) {
        robot.updatePivot(0, float(value), 0);
    });
    pivot4Slider->setMaximumHeight(20);
    pivot4Slider->setMaximumWidth(200);

    QSlider *pivot5Slider = new QSlider(Qt::Horizontal);
    pivot5Slider->setRange(-90, 90);
    QObject::connect(pivot5Slider, &QSlider::valueChanged, [&robot](int value) {
        robot.updatePivot(1, float(value), 0);
    });
    pivot4Slider->setMaximumHeight(20);
    pivot5Slider->setMaximumWidth(200);

    //qDebug() << "opening webcam...\n";
    cv::Mat fram = cv::imread("C:/Users/nabet/Desktop/bruh/pamache.png");
    //cv::VideoCapture cap = cv::VideoCapture(0);
    //cv::Mat frame;
    //cap >> frame;
    QImage img= QImage((uchar*) fram.data, fram.cols, fram.rows, fram.step, QImage::Format_BGR888);
    QPixmap pixel = QPixmap::fromImage(img);
    QLabel* label_image = new QLabel();
    label_image->setPixmap(pixel);
    label_image->setMaximumSize(640, 360);

    /*

    QWidget *view_container = QWidget::createWindowContainer(view);
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *screen = new QHBoxLayout();

    view_container->setStyleSheet("border: 2px solid red;");
    layout->addWidget(view_container);

    QLabel *label = new QLabel("Pivot 1");
    label->setMaximumSize(700, 30);
    label->setStyleSheet("border: 2px solid red;");
    layout->addWidget(label);
    layout->addWidget(pivot1Slider);
    label = new QLabel("Pivot 2");
    label->setMaximumSize(700, 30);
    layout->addWidget(label);
    layout->addWidget(pivot2Slider);
    label = new QLabel("Pivot 3");
    label->setMaximumSize(700, 0);
    layout->addWidget(label);
    layout->addWidget(pivot3Slider);
    label = new QLabel("Pivot 4");
    label->setMaximumSize(700, 30);
    layout->addWidget(label);
    layout->addWidget(pivot4Slider);
    label = new QLabel("Pivot 5");
    label->setMaximumSize(700, 30);
    layout->addWidget(label);
    layout->addWidget(pivot5Slider);

    QWidget *left_container = new QWidget;
    left_container->setStyleSheet("border: 2px solid white;");
    left_container->setLayout(layout);


    //screen->addWidget(container);
    //screen->addWidget(label_image);
    screen->addWidget(left_container);
    //screen->addLayout(layout);

    QObject::connect(&winsock, &server::imgReceived, [&robot, &label_image](cv::Mat frame){
        QImage img= QImage((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
        QPixmap pixel = QPixmap::fromImage(img);
        label_image->setPixmap(pixel);
    });

*/

    /*
    QHBoxLayout *mainLayout = new QHBoxLayout;

    // Left section
    QWidget *leftSection = new QWidget;
    leftSection->setStyleSheet("border: 2px solid white;");
    QGridLayout *leftLayout = new QGridLayout;
    leftLayout->setSpacing(0);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    // Load and add images to the 2x2 grid in the left section
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 2; ++col) {
            cv::Mat image = cv::imread("C:/Users/nabet/Desktop/bruh/pamache.png"); // Replace with your image path
            if (image.empty()) {
                qDebug("Failed to load image for left section!");
                return -1;
            }
            QImage qImage = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_BGR888);
            QLabel *imageLabel = new QLabel;
            imageLabel->setPixmap(QPixmap::fromImage(qImage).scaled(480, 270, Qt::KeepAspectRatio)); // Scale to fit
            imageLabel->setAlignment(Qt::AlignCenter);
            leftLayout->addWidget(imageLabel, row, col);
        }
    }
    leftSection->setLayout(leftLayout);

    // Right section
    QWidget *rightSection = new QWidget;
    rightSection->setStyleSheet("border: 2px solid red;");
    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->setSpacing(0);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    // Load and add images to the 2 vertical subsections in the right section
    for (int i = 0; i < 2; ++i) {
        cv::Mat image = cv::imread("C:/Users/nabet/Desktop/bruh/pamache.png"); // Replace with your image path
        if (image.empty()) {
            qDebug("Failed to load image for right section!");
            return -1;
        }
        QImage qImage = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_BGR888);
        QLabel *imageLabel = new QLabel;
        imageLabel->setPixmap(QPixmap::fromImage(qImage).scaled(320, 360, Qt::KeepAspectRatio)); // Scale to fit
        imageLabel->setAlignment(Qt::AlignCenter);
        rightLayout->addWidget(imageLabel);
    }
    rightSection->setLayout(rightLayout);

    // Add sections to the main layout
    mainLayout->addWidget(leftSection, 3); // 3/4 of the screen width
    mainLayout->addWidget(rightSection, 1); // 1/4 of the screen width

    QWidget mainWindow;
    //mainWindow.setLayout(screen);
    mainWindow.resize(1280, 720);
    mainWindow.setLayout(mainLayout);
    mainWindow.show();*/

    MainWindow window;
    window.setWindowTitle("maybe");
    window.resize(1280, 720); // Set resolution for the left section
    window.show();

    return app.exec();
}

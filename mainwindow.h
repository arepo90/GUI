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
#include <Qt3DRender/QCamera>
#include <QLabel>
#include <QImage>
#include <QObject>
#include <QMouseEvent>
#include <QPushButton>
#include <QMesh>
#include <QForwardRenderer>
#include <QComboBox>
#include <QStandardItemModel>
#include <QTimer>

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

class ImageSection : public QWidget { Q_OBJECT
public:
    explicit ImageSection(int id, const QString &imagePath, QWidget *parent = nullptr)
        : QWidget(parent), sectionId(id), imagePath(imagePath) {
        // Layout for this subsection
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);

        // Load the image
        cv::Mat image = cv::imread(imagePath.toStdString());
        if (image.empty()) {
            qDebug("Failed to load image for subsection %d!", id);
            return;
        }
        QImage qImage = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_BGR888);

        // QLabel to display the image
        imageLabel = new QLabel;
        imageLabel->setPixmap(QPixmap::fromImage(qImage).scaled(480, 270, Qt::KeepAspectRatio));
        imageLabel->setAlignment(Qt::AlignCenter);

        // QLabel for the tag
        tagLabel = new QLabel(QString("Cam %1").arg(id), this);
        tagLabel->setStyleSheet("color: white; background-color: black;");
        tagLabel->setParent(imageLabel); // Make it overlap the image

        layout->addWidget(imageLabel);
        setLayout(layout);
    }
    void setFullScreenMode(bool fullScreen) {
        if (fullScreen) {
            imageLabel->setPixmap(imageLabel->pixmap().scaled(960, 540, Qt::KeepAspectRatio)); // Enlarge
        } else {
            imageLabel->setPixmap(imageLabel->pixmap().scaled(480, 270, Qt::KeepAspectRatio)); // Return to normal size
        }
    }
    int getId() const { return sectionId; }
signals:
    void clicked(int id);
protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            emit clicked(sectionId);
        }
    }
private:
    int sectionId;
    QString imagePath;
    QLabel *imageLabel;
    QLabel *tagLabel;
};


class SubsectionWidget : public QWidget {
    Q_OBJECT
public:
    explicit SubsectionWidget(int deviceID, const QStringList &options, QWidget *parent = nullptr)
        : QWidget(parent), comboBox(new QComboBox), videoCapture(deviceID), label(new QLabel("No Selection")) {
        if(!videoCapture.isOpened()) {
            label->setText("Unable to access camera");
        }
        else{
            label->setText("Loading...");
        }
        comboBox->addItems(options);
        comboBox->setCurrentIndex(-1);

        label->setFixedSize(320, 240); // Adjust as needed
        label->setStyleSheet("border: 1px solid black; background-color: lightgray;");
        label->setAlignment(Qt::AlignCenter);

        // Timer to update the video feed
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SubsectionWidget::updateFrame);
        timer->start(30); // 30 ms ~ 33 FPS

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(comboBox);
        layout->addWidget(label);
        setLayout(layout);
        // Connect the dropdown to update the label
        connect(comboBox, &QComboBox::currentIndexChanged, this, [this]() {
                /*QString selectedText = comboBox->currentText();
                label->setText(selectedText.isEmpty() ? "No Selection" : selectedText);*/
            if(videoCapture.isOpened()){    //npi si esto funciona
                    videoCapture.release();
            }
                emit selectionChanged();
        });
    }

    QString getCurrentSelection() const {
        return comboBox->currentText();
    }

    void updateAvailableOptions(const QSet<QString> &usedOptions) {
        auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
        assert(model);
        if(!model) return;

        for (int i = 0; i < comboBox->count(); ++i) {
            QString option = comboBox->itemText(i);
            if (usedOptions.contains(option)) {
                //comboBox->setItemData(i, QColor(Qt::gray), Qt::ForegroundRole); // Gray out
                auto * item = model->item(i);
                assert(item);
                if(!item) return;
                item->setEnabled(false);
            } else {
                //comboBox->setItemData(i, QColor(Qt::black), Qt::ForegroundRole); // Reset color
                auto * item = model->item(i);
                assert(item);
                if(!item) return;
                item->setEnabled(true);
            }
        }
    }

    ~SubsectionWidget() {
        if (videoCapture.isOpened()) {
            videoCapture.release();
        }
    }
signals:
    void selectionChanged();
private slots:
    void updateFrame() {
        if (!videoCapture.isOpened()) return;

        cv::Mat frame;
        videoCapture >> frame; // Capture a frame
        if (frame.empty()) return;

        // Convert the frame to RGB format for QImage
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

        // Display the frame on the QLabel
        label->setPixmap(QPixmap::fromImage(image).scaled(label->size(), Qt::KeepAspectRatio));
    }
private:
    QComboBox *comboBox;
    QLabel *label;
    cv::VideoCapture videoCapture;
    QTimer *timer;
};

/*
class SubsectionWidget : public QWidget {
    Q_OBJECT

public:
    explicit SubsectionWidget(int deviceID, QWidget *parent = nullptr)
        : QWidget(parent), videoCapture(deviceID), label(new QLabel) {
        // Check if the video capture device is opened successfully
        if (!videoCapture.isOpened()) {
            label->setText("Unable to access camera");
        } else {
            label->setText("Loading...");
        }

        // Set up the label
        label->setFixedSize(320, 240); // Adjust as needed
        label->setStyleSheet("border: 1px solid black; background-color: lightgray;");
        label->setAlignment(Qt::AlignCenter);

        // Set up the layout
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);
        setLayout(layout);

        // Timer to update the video feed
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &SubsectionWidget::updateFrame);
        timer->start(30); // 30 ms ~ 33 FPS
    }

    ~SubsectionWidget() {
        if (videoCapture.isOpened()) {
            videoCapture.release();
        }
    }

private slots:
    void updateFrame() {
        if (!videoCapture.isOpened()) return;

        cv::Mat frame;
        videoCapture >> frame; // Capture a frame
        if (frame.empty()) return;

        // Convert the frame to RGB format for QImage
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

        // Display the frame on the QLabel
        label->setPixmap(QPixmap::fromImage(image).scaled(label->size(), Qt::KeepAspectRatio));
    }

private:
    cv::VideoCapture videoCapture;
    QLabel *label;
    QTimer *timer;
};
*/

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent) {

        /*Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();
        view->defaultFrameGraph()->setClearColor(QColor("#202020"));
        QWidget *container = QWidget::createWindowContainer(view);
        container->setMinimumSize(QSize(320, 360));
        //container->setMaximumSize(QSize(320, 360));
        container->show();

        // Root Entity
        Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

        // Load OBJ Mesh
        Qt3DRender::QMesh *mesh = new Qt3DRender::QMesh();
        mesh->setSource(QUrl::fromLocalFile("C:/Users/nabet/Desktop/bruh/assets/body.obj"));
        Qt3DRender::QMesh *mesh2 = new Qt3DRender::QMesh();
        mesh2->setSource(QUrl::fromLocalFile("C:/Users/nabet/Desktop/bruh/assets/left_arm.obj"));
        Qt3DRender::QMesh *mesh3 = new Qt3DRender::QMesh();
        mesh3->setSource(QUrl::fromLocalFile("C:/Users/nabet/Desktop/bruh/assets/right_arm.obj"));

        // Material
        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse(QColor("#a6a6a6"));

        vector<QColor> colors{Qt::red, Qt::green, Qt::blue};
        for(int axis = 0; axis < 3; axis++){
            Qt3DExtras::QCylinderMesh *segment = new Qt3DExtras::QCylinderMesh();
            Qt3DCore::QEntity *axisEntity = new Qt3DCore::QEntity(rootEntity);
            Qt3DExtras::QPhongMaterial *materialAx = new Qt3DExtras::QPhongMaterial();
            Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
            QVector3D initialTranslation((axis==0?1.0f/2.0:0.0f), (axis==1?1.0f/2.0:0.0f), (axis==2?1.0f/2.0:0.0f));
            QQuaternion initialRotation = QQuaternion::fromEulerAngles(0.0f, (axis==2?90.0f:0.0f), (axis!=1?90.0f:0.0f));
            segment->setRadius(0.001);
            segment->setLength(1);
            materialAx->setAmbient(colors[axis]);
            transform->setTranslation(initialTranslation);
            transform->setRotation(initialRotation);
            axisEntity->addComponent(segment);
            axisEntity->addComponent(transform);
            axisEntity->addComponent(materialAx);
        }

        // Create Object Entity
        Qt3DCore::QEntity *objEntity = new Qt3DCore::QEntity(rootEntity);
        QVector3D initialTranslation1(0, 0, 0);
        QQuaternion initialRotation1 = QQuaternion::fromEulerAngles(0, 0, 0);
        Qt3DCore::QTransform *transform1 = new Qt3DCore::QTransform();
        transform1->setTranslation(initialTranslation1);
        transform1->setRotation(initialRotation1);
        objEntity->addComponent(mesh);
        objEntity->addComponent(material);
        objEntity->addComponent(transform1);

        Qt3DCore::QEntity *pivot2 = new Qt3DCore::QEntity(objEntity);
        pivotTransform2 = new Qt3DCore::QTransform(pivot2);
        QQuaternion pivotRotation2 = QQuaternion::fromEulerAngles(0, 0, 0);
        pivotTransform2->setTranslation(QVector3D(0.085, 0.087, 0.555));
        pivotTransform2->setRotation(pivotRotation2);
        pivot2->addComponent(pivotTransform2);
        Qt3DCore::QEntity *objEntity2 = new Qt3DCore::QEntity(pivot2);
        Qt3DCore::QTransform *transform2 = new Qt3DCore::QTransform(objEntity2);
        QQuaternion initialRotation2 = QQuaternion::fromEulerAngles(0, 0, 0);
        transform2->setTranslation(QVector3D(-0.27, -0.08, 0.0));
        transform2->setRotation(initialRotation2);
        objEntity2->addComponent(mesh2);
        objEntity2->addComponent(material);
        objEntity2->addComponent(transform2);

        Qt3DCore::QEntity *pivot3 = new Qt3DCore::QEntity(objEntity);
        pivotTransform3 = new Qt3DCore::QTransform(pivot3);
        QQuaternion pivotRotation3 = QQuaternion::fromEulerAngles(180, 0, 0);
        pivotTransform3->setTranslation(QVector3D(0.085, 0.087, 0.145));
        pivotTransform3->setRotation(pivotRotation3);
        pivot3->addComponent(pivotTransform3);
        Qt3DCore::QEntity *objEntity3 = new Qt3DCore::QEntity(pivot3);
        Qt3DCore::QTransform *transform3 = new Qt3DCore::QTransform(objEntity3);
        QQuaternion initialRotation3 = QQuaternion::fromEulerAngles(0, 0, 0);
        transform3->setTranslation(QVector3D(-0.27, -0.08, 0.0));
        transform1->setRotation(initialRotation3);
        objEntity3->addComponent(transform3);
        objEntity3->addComponent(mesh3);
        objEntity3->addComponent(material);

        // Camera
        Qt3DRender::QCamera *camera = view->camera();
        camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
        camera->setPosition(QVector3D(2.0f, 2.0f, 2.0f));
        camera->setViewCenter(QVector3D(0, 0.0f, 0));
        camera->setUpVector(QVector3D(0.0f, 0.0f, 1.0f));
        Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
        camController->setLinearSpeed(10.0f);
        camController->setLookSpeed(180.0f);
        camController->setCamera(camera);

        // Set Root Entity
        view->setRootEntity(rootEntity);


        // Main layout
        mainLayout = new QHBoxLayout;

        QWidget *leftSection = new QWidget;
        leftSection->setStyleSheet("border: 2px solid white;");
        leftLayout = new QGridLayout;
        leftLayout->setSpacing(0);
        leftLayout->setContentsMargins(0, 0, 0, 0);
        QWidget *rightSection = new QWidget;
        rightSection->setStyleSheet("border: 2px solid red;");
        rightLayout = new QVBoxLayout;
        rightLayout->setSpacing(0);
        rightLayout->setContentsMargins(0, 0, 0, 0);

        // Load subsections
        for (int i = 0; i < 4; ++i) {
            ImageSection *section = new ImageSection(i + 1, "C:/Users/nabet/Desktop/bruh/assets/404.png");
            connect(section, &ImageSection::clicked, this, &MainWindow::handleSectionClick);
            sections.append(section);
            leftLayout->addWidget(section, i / 2, i % 2); // Arrange in 2x2 grid
        }
        leftSection->setLayout(leftLayout);

        rightLayout->addWidget(container);
        //for (int i = 0; i < 2; ++i) {
            cv::Mat image = cv::imread("C:/Users/nabet/Desktop/bruh/assets/404.png"); // Replace with your image path
            if (image.empty()) {
                qDebug("Failed to load image for right section!");
            }
            QImage qImage = QImage(image.data, image.cols, image.rows, image.step, QImage::Format_BGR888);
            QLabel *imageLabel = new QLabel;
            imageLabel->setPixmap(QPixmap::fromImage(qImage).scaled(320, 360, Qt::KeepAspectRatio)); // Scale to fit
            imageLabel->setAlignment(Qt::AlignCenter);
            rightLayout->addWidget(imageLabel);
        //}
        rightSection->setLayout(rightLayout);*/

        /*QWidget *leftSection = new QWidget;
        leftSection->setStyleSheet("border: 2px solid red;");
        leftLayout = new QVBoxLayout;
        leftLayout->setSpacing(0);
        leftLayout->setContentsMargins(0, 0, 0, 0);
        leftLayout->addWidget(container);
        leftSection->setLayout(leftLayout);

        QWidget *rightSection = new QWidget;
        rightSection->setStyleSheet("border: 2px solid red;");
        rightLayout = new QVBoxLayout;
        rightLayout->setSpacing(0);
        rightLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel("Pivot 1");
        label->setMaximumSize(700, 30);
        label->setStyleSheet("border: 2px solid red;");
        rightLayout->addWidget(label);
        label2 = new QLabel("Pivot 2");
        label2->setMaximumSize(700, 30);
        label2->setStyleSheet("border: 2px solid red;");

        QSlider *pivot1Slider = new QSlider(Qt::Horizontal);
        pivot1Slider->setRange(-180, 180);
        QObject::connect(pivot1Slider, &QSlider::valueChanged, [&pivotTransform2 = this->pivotTransform2, &label = this->label, &pos = this->pos](int value) {
            pivotTransform2->setRotation(QQuaternion::fromEulerAngles(0, 0, value));
            label->setText("pivot 2: " + QString::number(value));
        });
        QSlider *pivot1Slider2 = new QSlider(Qt::Horizontal);
        pivot1Slider2->setRange(-180, 180);
        QObject::connect(pivot1Slider2, &QSlider::valueChanged, [&pivotTransform3 = this->pivotTransform3, &label2 = this->label2, &pos = this->pos](int value) {
            pivotTransform3->setRotation(QQuaternion::fromEulerAngles(180, 0, value));
            label2->setText("pivot 3: " + QString::number(value));
        });
        rightLayout->addWidget(pivot1Slider);
        rightLayout->addWidget(label2);
        rightLayout->addWidget(pivot1Slider2);
        rightSection->setLayout(rightLayout);*/

        //mainLayout->addWidget(leftSection, 2); // 3/4 of the screen width
        //mainLayout->addWidget(rightSection, 1); // 1/4 of the screen width

        /*
        for (int i = 0; i < 4; ++i) {
            SubsectionWidget *widget = new SubsectionWidget(i, this);
            subsectionWidgets.append(widget);
        }

        // Layout for 2x2 grid
        QGridLayout *gridLayout = new QGridLayout;
        gridLayout->addWidget(subsectionWidgets[0], 0, 0);
        gridLayout->addWidget(subsectionWidgets[1], 0, 1);
        gridLayout->addWidget(subsectionWidgets[2], 1, 0);
        gridLayout->addWidget(subsectionWidgets[3], 1, 1);

        setLayout(gridLayout);*/

        //mainLayout->addWidget(container);
        //setLayout(mainLayout);
    }
private slots:
    /*void updateDropdowns() {
        QSet<QString> usedOptions;

        // Collect selected options
        for (auto *widget : subsectionWidgets) {
            QString selection = widget->getCurrentSelection();
            if (!selection.isEmpty()) {
                usedOptions.insert(selection);
            }
        }

        // Update all widgets
        for (auto *widget : subsectionWidgets) {
            widget->updateAvailableOptions(usedOptions);
        }
    }*/

    void handleSectionClick(int id) {
        static bool fullScreenMode = false;
        static int activeSectionId = -1;

        if (!fullScreenMode) {
            // Enter full-screen mode for the clicked section
            activeSectionId = id;
            for (int i = 0; i < sections.size(); ++i) {
                if (sections[i]->getId() == id) {
                    sections[i]->setFullScreenMode(true);
                    sections[i]->setParent(nullptr);
                    leftLayout->addWidget(sections[i], 0, 0, 2, 2); // Enlarge to fill left section
                } else {
                    sections[i]->hide();
                }
            }
            fullScreenMode = true;
        } else {
            // Exit full-screen mode and return to 2x2 grid
            for (int i = 0; i < sections.size(); ++i) {
                sections[i]->setFullScreenMode(false);
                sections[i]->show();
                leftLayout->addWidget(sections[i], i / 2, i % 2); // Return to grid
            }
            fullScreenMode = false;
            activeSectionId = -1;
        }
    }
private:
    QHBoxLayout *mainLayout;
    //QGridLayout *leftLayout;
    QVBoxLayout *rightLayout;
    QGridLayout *leftLayout;
    QList<ImageSection *> sections;
    Qt3DCore::QTransform *pivotTransform2;
    Qt3DCore::QTransform *pivotTransform3;
    QLabel *label;
    QLabel *label2;
    QLabel *label3;
    QList<SubsectionWidget *> subsectionWidgets;
};

/*QWidget mainWindow;
    //mainWindow.setLayout(screen);
    mainWindow.resize(1280, 720);
    mainWindow.setLayout(mainLayout);
    mainWindow.show();*/

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

#endif

#include "main_window.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    initUI();
    applyStyle();
    setWindowTitle("数字图像处理系统");
    resize(1100, 750);
}

void MainWindow::initUI() {
    auto *central = new QWidget();
    auto *layout = new QVBoxLayout(central);

    // --- 顶部工具栏 ---
    auto *topBar = new QHBoxLayout();
    auto *btnOpen = new QPushButton("📂 载入图片");
    auto *btnSave = new QPushButton("💾 保存图像");
    statusLabel = new QLabel("状态: 等待加载");
    topBar->addWidget(btnOpen);
    topBar->addWidget(btnSave);
    topBar->addStretch();
    topBar->addWidget(statusLabel);
    layout->addLayout(topBar);

    // --- 预览区 ---
    auto *viewLayout = new QHBoxLayout();
    srcView = new QGraphicsView();
    dstView = new QGraphicsView();
    srcScene = new QGraphicsScene(this);
    dstScene = new QGraphicsScene(this);

    // 配置视图属性
    for (auto *v : {srcView, dstView}) {
        v->setScene(v == srcView ? srcScene : dstScene);
        v->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        v->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        v->setBackgroundRole(QPalette::Dark);
    }
    viewLayout->addWidget(srcView);
    viewLayout->addWidget(dstView);
    layout->addLayout(viewLayout);

    // --- 控制面板 ---
    auto *ctrlGroup = new QGroupBox("滤镜调节面板");
    auto *ctrlLayout = new QHBoxLayout(ctrlGroup);
    filterGroup = new QButtonGroup(this);
    QStringList labels = {"原图", "灰度", "反色", "高斯模糊", "硬核浮雕", "艺术卡通"};

    for (int i = 0; i < labels.size(); ++i) {
        auto *rb = new QRadioButton(labels[i]);
        filterGroup->addButton(rb, i);
        ctrlLayout->addWidget(rb);
        if (i == 0) rb->setChecked(true);
    }

    ctrlLayout->addSpacing(30);
    ctrlLayout->addWidget(new QLabel("调节强度:"));
    paramSlider = new QSlider(Qt::Horizontal);
    paramSlider->setRange(1, 40);
    paramSlider->setValue(10);
    ctrlLayout->addWidget(paramSlider);
    layout->addWidget(ctrlGroup);

    setCentralWidget(central);

    // 绑定信号
    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::onOpenImage);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSaveImage);
    connect(filterGroup, &QButtonGroup::buttonClicked, this, &MainWindow::updateProcess);
    connect(paramSlider, &QSlider::valueChanged, this, &MainWindow::updateProcess);
}

void MainWindow::updateProcess() {
    if (srcMat.empty()) return;

    int mode = filterGroup->checkedId();
    int val = paramSlider->value();
    paramSlider->setEnabled(mode >= BLUR);

    switch(mode) {
    case ORIGINAL: dstMat = srcMat.clone(); break;
    case GRAY:     cv::cvtColor(srcMat, dstMat, cv::COLOR_BGR2GRAY); break;
    case INVERT:   cv::bitwise_not(srcMat, dstMat); break;
    case BLUR: {
        int k = (val / 2) * 2 + 1;
        cv::GaussianBlur(srcMat, dstMat, cv::Size(k, k), 0);
        break;
    }
    case EMBOSS: {
        cv::Mat kernel = (cv::Mat_<float>(3,3) << -1, -1, 0, -1, 0, 1, 0, 1, 1);
        cv::filter2D(srcMat, dstMat, CV_8U, kernel, cv::Point(-1,-1), 128 + val);
        break;
    }
    case CARTOON: {
        // 卡通化强度优化：滑块同时控制边缘阈值范围和双边滤波力度
        cv::Mat gray, edges, color;
        cv::cvtColor(srcMat, gray, cv::COLOR_BGR2GRAY);
        cv::medianBlur(gray, gray, 7);

        // 边缘提取随 val 动态变化
        int blockSize = (val % 2 == 0) ? val + 1 : val;
        if (blockSize < 3) blockSize = 3;
        cv::adaptiveThreshold(gray, edges, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, blockSize, 2);

        // 色彩平滑随 val 动态变化
        cv::bilateralFilter(srcMat, color, val/2 + 3, val * 4, val * 4);
        dstMat = cv::Mat::zeros(srcMat.size(), srcMat.type());
        color.copyTo(dstMat, edges);
        break;
    }
    }

    dstScene->clear();
    QPixmap pix = QPixmap::fromImage(matToQImage(dstMat));
    dstScene->addPixmap(pix);
    dstView->fitInView(pix.rect(), Qt::KeepAspectRatio);
    statusLabel->setText(QString("当前模式: %1 | 强度: %2").arg(filterGroup->checkedButton()->text()).arg(val));
}

void MainWindow::onOpenImage() {
    QString path = QFileDialog::getOpenFileName(this, "载入图片", "", "Images (*.jpg *.png *.bmp)");
    if (path.isEmpty()) return;
    srcMat = cv::imread(path.toLocal8Bit().toStdString());
    if (srcMat.empty()) return;

    srcScene->clear();
    QPixmap pix = QPixmap::fromImage(matToQImage(srcMat));
    srcScene->addPixmap(pix);
    srcView->fitInView(pix.rect(), Qt::KeepAspectRatio);
    updateProcess();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (!srcScene->items().isEmpty()) srcView->fitInView(srcScene->itemsBoundingRect(), Qt::KeepAspectRatio);
    if (!dstScene->items().isEmpty()) dstView->fitInView(dstScene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::onSaveImage() {
    if (dstMat.empty()) return;
    QString path = QFileDialog::getSaveFileName(this, "导出图片", "processed.png", "*.png;;*.jpg");
    if (!path.isEmpty()) cv::imwrite(path.toLocal8Bit().toStdString(), dstMat);
}

QImage MainWindow::matToQImage(const cv::Mat& mat) {
    if(mat.type() == CV_8UC3) return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped();
    if(mat.type() == CV_8UC1) return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
    return QImage();
}

void MainWindow::applyStyle() {
    this->setStyleSheet(R"(
        QMainWindow { background-color: #2c3e50; }
        QGroupBox { color: #ffffff; border: 1px solid #7f8c8d; border-radius: 8px; margin-top: 10px; padding: 10px; }
        QPushButton { background-color: #3498db; color: white; border-radius: 4px; padding: 10px; min-width: 100px; font-weight: bold; }
        QPushButton:hover { background-color: #2980b9; }
        QRadioButton { color: #ecf0f1; }
        QLabel { color: #ecf0f1; }
        QGraphicsView { background-color: #1e272e; border: none; }
        QSlider::groove:horizontal { border: 1px solid #7f8c8d; height: 6px; background: #95a5a6; border-radius: 3px; }
        QSlider::handle:horizontal { background: #3498db; width: 18px; height: 18px; margin: -6px 0; border-radius: 9px; }
    )");
}

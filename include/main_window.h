#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QButtonGroup>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>

class QSlider;
class QGraphicsView;
class QLabel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

protected:
    void resizeEvent(QResizeEvent *event) override; // 窗口缩放自适应

private slots:
    void onOpenImage();
    void onSaveImage();
    void updateProcess();

private:
    void initUI();
    void applyStyle();
    QImage matToQImage(const cv::Mat& mat);

    enum FilterMode { ORIGINAL = 0, GRAY, INVERT, BLUR, EMBOSS, CARTOON };

    QGraphicsScene *srcScene, *dstScene;
    QGraphicsView  *srcView,  *dstView;
    QSlider        *paramSlider;
    QButtonGroup   *filterGroup;
    QLabel         *statusLabel;

    cv::Mat srcMat, dstMat;
};

#endif

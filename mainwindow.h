#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "primitive.h"
#include <QMainWindow>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QColorDialog>
#include <QFileDialog>
#include <QPainter>
#include <QBrush>
#include <QString>
#include <QUrl>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *event);		// 绘制事件
    void mousePressEvent(QMouseEvent *event);	// 鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event);	// 鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event);	// 鼠标松开事件
    void resizeEvent(QResizeEvent *event);		// 窗口调整事件

private slots:
    // 程序按钮对应的槽函数，切换程序状态
    void on_action_open_triggered();
    void on_action_save_triggered();
    void on_action_line_triggered();
    void on_action_triangle_triggered();
    void on_action_rectangle_triggered();
    void on_action_circle_triggered();
    void on_action_ellipse_triggered();
    void on_action_polygon_triggered();
    void on_action_curve_triggered();
    void on_action_translate_triggered();
    void on_action_rotate_triggered();
    void on_action_palette_triggered();
    void on_action_trash_triggered();
    void on_action_zoomin_triggered();
    void on_action_zoomout_triggered();
    void on_action_clip_triggered();
    void on_action_addpoint_triggered();
    void on_action_deletepoint_triggered();
    void on_action_help_triggered();

private:
    Ui::MainWindow *ui;
    enum State {Line, Triangle, Rectangle, Circle, Ellipse, Polygon, Curve,
                Translate, Rotate, Clip, ZoomIn, ZoomOut, Trash} state;	// 程序状态
    QVector<QPoint> points;			// 记录鼠标点击位置
    QList<Primitive *> primitives;	// 已经绘制的图元列表
    Primitive *primitive;			// 当前操作的图元
    QImage image;					// 画布
    QPen pen;						// 点的颜色和大小
    QPainter painter;				// 画笔，用于绘制单个点
    QString s;
};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    state(Line),
    primitive(nullptr),
    pen(Qt::black, 3)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    foreach (Primitive *p, primitives)
        delete p;
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    image.fill(Qt::white);
    painter.begin(&image);
    if (!s.isEmpty())
    {
        QImage temp(s);
        painter.drawImage(QPoint((image.width() - temp.width()) / 2, (image.height() - temp.height()) / 2),temp);
    }
    foreach (Primitive *p, primitives)
    {
        painter.setPen(p->pen());
        painter.drawPoints(p->points());
    }
    if (state == Polygon || state == Curve)
    {
        painter.setBrush(QBrush(Qt::black));
        foreach (QPoint p, points)
            painter.drawEllipse(p, 4, 4);
    }
    if (state == Clip && primitive)
    {
        painter.setPen(primitive->pen());
        painter.drawPoints(primitive->points());
    }
    painter.end();
    ui->label->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    pos.rx() -= 11;
    pos.ry() -= 51;
    points.append(pos);
    switch (state)
    {
    case Line:
        primitive = new Primitive(pen, Primitive::Line,
        {pos, pos});
        primitives.append(primitive);
        break;
    case Triangle:
        primitive = new Primitive(pen, Primitive::Polygon,
        {pos, pos, pos});
        primitives.append(primitive);
        break;
    case Rectangle:
        primitive = new Primitive(pen, Primitive::Polygon,
        {pos, pos, pos, pos});
        primitives.append(primitive);
        break;
    case Circle:
        primitive = new Primitive(pen, Primitive::Circle,
        {pos, QPoint(0, 0)});
        primitives.append(primitive);
        break;
    case Ellipse:
        primitive = new Primitive(pen, Primitive::Ellipse,
        {pos, QPoint(0, 0)});
        primitives.append(primitive);
        break;
    case Polygon:
    case Curve:
        break;
    case Clip:
        primitive = new Primitive(QPen(Qt::black, 1), Primitive::Polygon,
        {pos, pos, pos, pos});
        break;
    case Translate:
    case Rotate:
    case ZoomIn:
    case ZoomOut:
    case Trash:
        primitive = nullptr;
        foreach (Primitive *p, primitives)
            if (p->contain(points[0]))
            {
                primitive = p;
                break;
            }
        break;
    }
    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    pos.rx() -= 11;
    pos.ry() -= 51;
    QVector<QPoint> args;
    switch (state)
    {
    case Line:
        primitive->setArgs({points[0], pos});
        break;
    case Triangle:
        primitive->setArgs({pos,
                            {(points[0].x() + pos.x()) / 2, points[0].y()},
                            {points[0].x(), pos.y()}});
        break;
    case Rectangle:
        primitive->setArgs({points[0],
                            {points[0].x(), pos.y()},
                            pos,
                            {pos.x(), points[0].y()}});
        break;
    case Circle:
        if (qAbs(pos.x() - points[0].x()) < qAbs(pos.y() - points[0].y()))
            if (pos.ry() > points[0].y())
                pos.ry() = points[0].y() + qAbs(pos.rx() - points[0].x());
            else
                pos.ry() = points[0].y() - qAbs(pos.rx() - points[0].x());
        else
            if (pos.rx() > points[0].x())
                pos.rx() = points[0].x() + qAbs(pos.ry() - points[0].y());
            else
                pos.rx() = points[0].x() - qAbs(pos.ry() - points[0].y());
        primitive->setArgs({(pos + points[0]) / 2,
                            (pos - points[0]) / 2});
        break;
    case Ellipse:
        primitive->setArgs({(pos + points[0]) / 2,
                            (pos - points[0]) / 2});
        break;
    case Polygon:
    case Curve:
    case ZoomIn:
    case ZoomOut:
    case Trash:
        break;
    case Translate:
        if (!primitive)
            break;
        args = primitive->translate(pos - points[0]);
        primitive->setPoints(args);
        break;
    case Clip:
        primitive->setArgs({points[0],
                            {points[0].x(), pos.y()},
                            pos,
                            {pos.x(), points[0].y()}});
        foreach (Primitive *p, primitives)
        {
            args = primitive->args();
            args = p->clip(args[0], args[2]);
            p->setPoints(args);
        }
        break;
    case Rotate:
        if (!primitive)
            break;
        QPoint a = points[0] - primitive->center(), b = pos - primitive->center();
        qreal product = a.x() * b.y() - a.y() * b.x();
        qreal aNorm = qSqrt(qreal(a.x() * a.x() + a.y() * a.y()));
        qreal bNorm = qSqrt(qreal(b.x() * b.x() + b.y() * b.y()));
        args = primitive->rotate(qAsin(product / aNorm / bNorm));
        primitive->setPoints(args);
        break;
    }
    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    pos.rx() -= 11;
    pos.ry() -= 51;
    QVector<QPoint> args;
    switch (state)
    {
    case Line:
        primitive->setArgs({points[0], pos});
        break;
    case Triangle:
        primitive->setArgs({pos,
                            {(points[0].x() + pos.x()) / 2, points[0].y()},
                            {points[0].x(), pos.y()}});
        break;
    case Rectangle:
        primitive->setArgs({points[0],
                            {points[0].x(), pos.y()},
                            pos,
                            {pos.x(), points[0].y()}});
        break;
    case Circle:
        if (qAbs(pos.x() - points[0].x()) < qAbs(pos.y() - points[0].y()))
            if (pos.ry() > points[0].y())
                pos.ry() = points[0].y() + qAbs(pos.rx() - points[0].x());
            else
                pos.ry() = points[0].y() - qAbs(pos.rx() - points[0].x());
        else
            if (pos.rx() > points[0].x())
                pos.rx() = points[0].x() + qAbs(pos.ry() - points[0].y());
            else
                pos.rx() = points[0].x() - qAbs(pos.ry() - points[0].y());
        primitive->setArgs({(pos + points[0]) / 2,
                            (pos - points[0]) / 2});
        break;
    case Ellipse:
        primitive->setArgs({(pos + points[0]) / 2,
                            (pos - points[0]) / 2});
        break;
    case Polygon:
        primitive->setArgs(points);
        break;
    case Curve:
        primitive->setArgs(points);
        break;
    case Translate:
        if (!primitive)
            break;
        args = primitive->translate(pos - points[0]);
        primitive->setArgs(args);
        break;
    case Clip:
        primitive->setArgs({points[0],
                            {points[0].x(), pos.y()},
                            pos,
                            {pos.x(), points[0].y()}});

        foreach (Primitive *p, primitives)
        {
            args = primitive->args();
            args = p->clip(args[0], args[2]);
            p->setArgs(args);
        }
        delete primitive;
        primitive = nullptr;
        break;
    case ZoomIn:
        if (!primitive)
            break;
        args = primitive->scale(1.1);
        primitive->setArgs(args);
        break;
    case ZoomOut:
        if (!primitive)
            break;
        args = primitive->scale(0.9);
        primitive->setArgs(args);
        break;
    case Trash:
        primitives.removeAll(primitive);
        delete primitive;
        break;
    case Rotate:
        if (!primitive)
            break;
        QPoint a = points[0] - primitive->center(), b = pos - primitive->center();
        qreal product = a.x() * b.y() - a.y() * b.x();
        qreal aNorm = qSqrt(qreal(a.x() * a.x() + a.y() * a.y()));
        qreal bNorm = qSqrt(qreal(b.x() * b.x() + b.y() * b.y()));
        args = primitive->rotate(qAsin(product / aNorm / bNorm));
        primitive->setArgs(args);
        break;
    }
    update();
    if (state != Curve && state != Polygon)
        points.clear();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    ui->label->resize(ui->centralWidget->size() - QSize(22, 22));
    image = QImage(ui->label->size(), QImage::Format_RGB32);
}

void MainWindow::on_action_open_triggered()
{
    s = (QFileDialog::getOpenFileName(this, QString(), QString(), "Image Files(*.bmp *.jpg *.png)"));
}

void MainWindow::on_action_save_triggered()
{
    image.save(QFileDialog::getSaveFileName(this, QString(), QString(), "Image Files(*.bmp *.jpg *.png)"));
}

void MainWindow::on_action_line_triggered()
{
    state = Line;
    points.clear();
}

void MainWindow::on_action_triangle_triggered()
{
    state = Triangle;
    points.clear();
}

void MainWindow::on_action_rectangle_triggered()
{
    state = Rectangle;
    points.clear();
}

void MainWindow::on_action_circle_triggered()
{
    state = Circle;
    points.clear();
}

void MainWindow::on_action_ellipse_triggered()
{
    state = Ellipse;
    points.clear();
}

void MainWindow::on_action_polygon_triggered()
{
    state = Polygon;
    points.clear();
    primitive = new Primitive(pen, Primitive::Polygon, points);
    primitives.append(primitive);
}

void MainWindow::on_action_curve_triggered()
{
    state = Curve;
    points.clear();
    primitive = new Primitive(pen, Primitive::Curve, points);
    primitives.append(primitive);
}
void MainWindow::on_action_translate_triggered()
{
    state = Translate;
    points.clear();
}

void MainWindow::on_action_rotate_triggered()
{
    state = Rotate;
    points.clear();
}

void MainWindow::on_action_palette_triggered()
{
    pen.setColor(QColorDialog::getColor(pen.color(), this));
}

void MainWindow::on_action_trash_triggered()
{
    state = Trash;
    points.clear();
}

void MainWindow::on_action_zoomin_triggered()
{
    state = ZoomIn;
    points.clear();
}

void MainWindow::on_action_zoomout_triggered()
{
    state = ZoomOut;
    points.clear();
}

void MainWindow::on_action_clip_triggered()
{
    state = Clip;
    points.clear();
}

void MainWindow::on_action_addpoint_triggered()
{
    pen.setWidth(pen.width() + 1);
}

void MainWindow::on_action_deletepoint_triggered()
{
    if (pen.width() > 1)
        pen.setWidth(pen.width() - 1);
}

void MainWindow::on_action_help_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/GeekEmperor/Paint/blob/master/README.md"));
}

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <QPen>
#include <QPoint>
#include <QVector>
#include <QPainter>
#include <QtMath>
#include <QtAlgorithms>
#include <QDebug>
#include <functional>

class Primitive
{
public:
    enum Type { Line, Polygon, Circle, Ellipse, Curve };
    Primitive();
    Primitive(QPen pen, Type type, QVector<QPoint> args);
    QPen pen();		// 获取图元的点的颜色和大小
    bool contain(QPoint p);	// 判断图元包含某点
    QPoint center() const;	// 获取图元中心
    Type type() const;	// 获取图元类型
    QVector<QPoint> args() const;	// 获取图元参数
    QVector<QPoint> points() const;	// 获取图元点集合
    void setArgs(QVector<QPoint> args);	// 设置图元参数
    void setPoints(QVector<QPoint> args);	// 设置图元点集合
    static QVector<QPoint> drawLine(QVector<QPoint> args);		// 绘制直线
    static QVector<QPoint> drawPolygon(QVector<QPoint> args);	// 绘制多边形
    static QVector<QPoint> drawCircle(QVector<QPoint> args);	// 绘制圆形
    static QVector<QPoint> drawEllipse(QVector<QPoint> args);	// 绘制椭圆
    static QVector<QPoint> drawCurve(QVector<QPoint> args);		// 绘制曲线
    QVector<QPoint> translate(QPoint pos);		// 平移
    QVector<QPoint> rotate(qreal r);			// 旋转
    QVector<QPoint> scale(qreal s);				// 缩放
    QVector<QPoint> clip(QPoint lt, QPoint rb);	// 裁剪
private:
    QPen _pen;	// 点的颜色和大小
    Type _type;	// 图元类型，属于直线、多边形、圆形、椭圆、曲线之一
    QPoint _center;	// 图元中心，用于旋转和缩放
    QVector<QPoint> _args;	// 图元参数
    QVector<QPoint> _points;	// 图元点集
};

#endif // PRIMITIVE_H

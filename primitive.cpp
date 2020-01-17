#include "primitive.h"

Primitive::Primitive()
{

}

Primitive::Primitive(QPen pen, Primitive::Type type, QVector<QPoint> args)
    : _pen(pen), _type(type)
{
    setArgs(args);
}

QPen Primitive::pen()
{
    return _pen;
}

bool Primitive::contain(QPoint pos)
{
    foreach (QPoint p, _points)
    {
        QPoint diff = p - pos;
        if (diff.x() * diff.x() + diff.y() * diff.y() < 25)
            return true;
    }
    return false;
}

QPoint Primitive::center() const
{
    return _center;
}

Primitive::Type Primitive::type() const
{
    return _type;
}

QVector<QPoint> Primitive::args() const
{
    return _args;
}

QVector<QPoint> Primitive::points() const
{
    return _points;
}

void Primitive::setArgs(QVector<QPoint> args)
{
    _args = args;
    setPoints(args);
    int x = 0, y = 0;
    foreach (QPoint p, args)
    {
        x += p.x();
        y += p.y();
    }
    _center = QPoint(x, y) / args.size();
}

void Primitive::setPoints(QVector<QPoint> args)
{
    switch (_type)
    {
    case Line:
        _points = drawLine(args); break;
    case Polygon:
        _points = drawPolygon(args); break;
    case Circle:
        _points = drawCircle(args); break;
    case Ellipse:
        _points = drawEllipse(args); break;
    case Curve:
        _points = drawCurve(args); break;
    }
}

QVector<QPoint> Primitive::drawLine(QVector<QPoint> args)
{
    int x1 = args[0].x(), y1 = args[0].y(), x2 = args[1].x(), y2 = args[1].y();
    int dx = qAbs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = qAbs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e;
    QVector<QPoint> points;
    while (x1 != x2 || y1 != y2)
    {
        points.append(QPoint(x1, y1));
        e = err;
        if (e > -dx) { err -= dy; x1 += sx; }
        if (e < dy) { err += dx; y1 += sy; }
    }
    return points;
}

QVector<QPoint> Primitive::drawPolygon(QVector<QPoint> args)
{
    QVector<QPoint> points;
    int n = args.size();
    for (int i = 0; i < n; ++i)
    {
        if (i == n - 1)
            points.append(drawLine({args[n - 1], args[0]}));
        else
            points.append(drawLine({args[i], args[i + 1]}));
    }
    return points;
}

QVector<QPoint> Primitive::drawCircle(QVector<QPoint> args)
{
    QVector<QPoint> points;
    int cx = args[0].x(), cy = args[0].y(), r = qMin(qAbs(args[1].x()), qAbs(args[1].y()));
    auto lambda = [&](int x, int y)
    {
        points.append(QPoint(cx + x, cy + y));
        points.append(QPoint(cx - x, cy + y));
        points.append(QPoint(cx - x, cy - y));
        points.append(QPoint(cx + x, cy - y));
        points.append(QPoint(cx + y, cy + x));
        points.append(QPoint(cx - y, cy + x));
        points.append(QPoint(cx - y, cy - x));
        points.append(QPoint(cx + y, cy - x));
    };
    int p = 1 - r, x = 0, y = r;
    lambda(x, y);
    while (x < y)
    {
        if (p < 0) {
            p += 2 * x + 3;
        }
        else {
            y--;
            p += 2 * (x - y) + 5;
        }
        ++x;
        lambda(x, y);
    }
    return points;
}

QVector<QPoint> Primitive::drawEllipse(QVector<QPoint> args)
{
    QVector<QPoint> points;
    int cx = args[0].x(), cy = args[0].y(), rx = qMax(qAbs(args[1].x()), 1), ry = qMax(qAbs(args[1].y()), 1);
    auto lambda = [&](int x, int y)
    {
        points.append(QPoint(cx + x, cy + y));
        points.append(QPoint(cx - x, cy + y));
        points.append(QPoint(cx - x, cy - y));
        points.append(QPoint(cx + x, cy - y));
    };
    if (rx >= ry)
    {
        int rx2 = rx * rx, ry2 = ry * ry;
        int p = ry2 - rx2 * ry + rx2, x = 0, y = ry;
        lambda(x, y);
        while (ry2 * x <= rx2 * y)
        {
            if (p < 0)
                p += ry2 * (3 + 2 * x);
            else
            {
                p += ry2 * (3 + 2 * x) + rx2 * (2 - 2 * y);
                --y;
            }
            ++x;
            lambda(x, y);
        }
        p = int(ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1.0) * (y - 1.0) - 1.0 * rx2 * ry2);
        while (y >= 0)
        {
            if (p < 0)
            {
                p += ry2 * (2 + 2 * x) + rx2 * (3 - 2 * y);
                ++x;
            }
            else
            {
                p += rx2 * (3 - 2 * y);
            }
            --y;
            lambda(x, y);
        }
    }
    else
    {
        qSwap(args[0].rx(), args[0].ry());
        qSwap(args[1].rx(), args[1].ry());
        points = drawEllipse(args);
        for (auto& p : points)
            qSwap(p.rx(), p.ry());
    }
    return points;
}

QVector<QPoint> Primitive::drawCurve(QVector<QPoint> args)
{
    QVector<QPoint> points;
    int n = args.size();
    if (!n)
        return points;
    --n;
    int k = 3, m = n + k + 1;
    QVector<qreal> v(m + 1);
    for (int i = 0; i <= m; ++i)
        v[i] = qreal(i) / m;
    std::function<qreal(int, int, qreal)> lambda;
    lambda = [&](int i, int k, qreal u) -> qreal
    {
        if (!k)
            return (v[i] <= u && u <= v[i + 1]) ? 1.0 : 0.0;
        return (lambda(i, k - 1, u) * (u - v[i]) / (v[i + k] - v[i])) +
                (lambda(i + 1, k - 1, u) * (v[i + k + 1] - u) / (v[i + k + 1] - v[i + 1]));
    };
    for (qreal u = v[k]; u <= v[n + 1]; u += 0.001)
    {
        qreal x = 0.0, y = 0.0;
        for (int i = 0; i <= n; ++i)
        {
            qreal p = lambda(i, k, u);
            x += p * args[i].x();
            y += p * args[i].y();
        }
        points.append({int(x), int(y)});
    }
    /*
    for (int i = 3; i < n; ++i)
        for (qreal u = 0.0; u <= 1.0; u += 0.001)
        {
            qreal a = -u * u * u + 3 * u * u - 3 * u + 1;
            qreal b = 3 * u * u * u - 6 * u * u + 4;
            qreal c = -3 * u * u * u + 3 * u * u + 3 * u + 1;
            qreal d = u * u * u;
            int x = 1.0 / 6 * (a * args[i - 3].x() + b * args[i - 2].x() + c * args[i - 1].x() + d * args[i].x());
            int y = 1.0 / 6 * (a * args[i - 3].y() + b * args[i - 2].y() + c * args[i - 1].y() + d * args[i].y());
            points.append({x, y});
        }*/
    /*
    std::function<qreal(int, int, qreal)> f;
    f = [&](int n, int i, qreal u) -> qreal
    {
        if (!n)
            return qreal(args[i].x());
        return (1 - u) * f(n - 1, i, u) + u * f(n - 1, i + 1, u);
    };
    std::function<qreal(int, int, qreal)> g;
    g = [&](int n, int i, qreal u) -> qreal
    {
        if (!n)
            return qreal(args[i].y());
        return (1 - u) * g(n - 1, i, u) + u * g(n - 1, i + 1, u);
    };
    for (qreal u = 0.0; u <= 1.0; u += 0.001)
        points.append(QPoint(int(f(n - 1, 0, u)), int(g(n - 1, 0, u))));
    */
    return points;
}

QVector<QPoint> Primitive::translate(QPoint pos)
{
    QVector<QPoint> args = _args;
    if (_type == Circle || _type == Ellipse)
        args[0] += pos;
    else
        for (int i = 0; i < args.size(); ++i)
            args[i] += pos;
    return args;
}

QVector<QPoint> Primitive::rotate(qreal r)
{
    QVector<QPoint> args = _args;
    qreal cosr = qCos(r), sinr = qSin(r);
    int dx, dy;
    if (_type != Circle && _type != Ellipse)
        for (auto& arg : args)
        {
            dx = arg.x() - _center.x();
            dy = arg.y() - _center.y();
            arg.rx() = int(_center.x() + dx * cosr - dy * sinr);
            arg.ry() = int(_center.y() + dx * sinr + dy * cosr);
        }
    return args;
}

QVector<QPoint> Primitive::scale(qreal s)
{
    QVector<QPoint> args = _args;
    if (_type == Circle || _type == Ellipse)
        args[1] *= s;
    else
        for (auto& arg : args)
            arg = _center + (arg - _center) * s;
    return args;
}

QVector<QPoint> Primitive::clip(QPoint lt, QPoint rb)
{
    int l = qMin(lt.x(), rb.x()), r = qMax(lt.x(), rb.x()), t = qMin(lt.y(), rb.y()), b = qMax(lt.y(), rb.y());
    QVector<QPoint> args = _args;
    auto lambda = [=](int x, int y)
    {
        uint8_t code = 0;
        if (y < t)
            code |= 8;
        if (y > b)
            code |= 4;
        if (x > r)
            code |= 2;
        if (x < l)
            code |= 1;
        return code;
    };
    if (_type == Line)
    {
        int x = 0, y = 0, x1 = args[0].x(), y1 = args[0].y(), x2 = args[1].x(), y2 = args[1].y();
        uint8_t code1 = lambda(x1, y1), code2 = lambda(x2, y2), code;
        while (code1 || code2)
        {
            if (code1 & code2)
                break;
            code = code1 ? code1 : code2;
            if (1 & code)
            {
                x = l;
                y = y1 + (y2 - y1) * (l - x1) / (x2 - x1);
            }
            else if (2 & code)
            {
                x = r;
                y = y1 + (y2 - y1) * (r - x1) / (x2 - x1);
            }
            else if (4 & code)
            {
                y = b;
                x = x1 + (x2 - x1) * (b - y1) / (y2 - y1);
            }
            else if (8 & code)
            {
                y = t;
                x = x1 + (x2 - x1) * (t - y1) / (y2 - y1);
            }
            if (code == code1)
            {
                x1 = x;
                y1 = y;
                code1 = lambda(x1, y1);
            }
            else
            {
                x2 = x;
                y2 = y;
                code2 = lambda(x2, y2);
            }
        }
        args = {{x1, y1}, {x2, y2}};
    }
    return args;
}

#include "point.h"

point::point(double x, double y) {
    _x = x;
    _y = y;
}

double point::distant(const point p) const {
    double xd = _x - p.x();
    double yd = _y - p.y();
    return sqrt(xd * xd + yd * yd);
}

void point::moveTo(const point p) {
    _x = p.x();
    _y = p.y();
}

double point::dist(const point p, const point q) {
    double xd = p.x() - q.x();
    double yd = p.y() - q.y();
    return sqrt(xd * xd + yd * yd);
}

double point::dist(const point *p, const point *q) {
    double xd = p->x() - q->x();
    double yd = p->y() - q->y();
    return sqrt(xd * xd + yd * yd);
}

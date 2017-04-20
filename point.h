#ifndef ALGORITHM_POINT_H
#define ALGORITHM_POINT_H

#include <cmath>

class point {
private:
    double _x;
    double _y;
public:
    point(double x = 0.0, double y = 0.0) {
        _x = x;
        _y = y;
    }

    double x() { return _x; }

    double y() { return _y; }

    double dist(point p) {
        double xd = _x - p._x;
        double yd = _y - p._y;
        return sqrt(xd * xd + yd * yd);
    }

    void move(double a, double b) {
        _x += a;
        _y += b;
    }

};


#endif //ALGORITHM_POINT_H

#ifndef ALGORITHM_POINT_H
#define ALGORITHM_POINT_H

#include <cmath>
#include <iomanip>
#include <ostream>

class point {
protected:
    double _x;
    double _y;
public:
    explicit point(double x = 0.0, double y = 0.0);

    double x() const { return _x; }

    double y() const { return _y; }

    double distant(const point p) const;

    void moveTo(const point p);

    static double dist(const point p, const point q);

    static double dist(const point *p, const point *q);

    virtual ~point() {};

    bool operator!=(const point &rhs) const {
        return _x != rhs._x || _y != rhs._y;
    }

    friend std::ostream &operator<<(std::ostream &os, const point &p) {
        os << std::fixed << std::setprecision(3) << "(" << p.x() << ", " << p.y() << ")";
        return os;
    }
};

#endif //ALGORITHM_POINT_H

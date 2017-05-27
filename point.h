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
    point(double x = 0.0, double y = 0.0) {
        _x = x;
        _y = y;
    }

	double x() const { return _x; }

	double y() const { return _y; }

	double distant(const point p) const {
		double xd = _x - p.x();
		double yd = _y - p.y();
		return sqrt(xd * xd + yd * yd);
	}

	static double dist(const point p, const point q) {
		double xd = p.x() - q.x();
		double yd = p.y() - q.y();
		return sqrt(xd * xd + yd * yd);
	}

	static double dist(const point * p, const point * q) {
		double xd = p->x() - q->x();
		double yd = p->y() - q->y();
		return sqrt(xd * xd + yd * yd);
	}

    virtual ~point() {};

	friend std::ostream& operator<<(std::ostream& os, const point &p) {
        os << std::fixed << std::setprecision(3) << "(" << p.x() << ", " << p.y() << ")";
        return os;
	}

protected:

};


#endif //ALGORITHM_POINT_H

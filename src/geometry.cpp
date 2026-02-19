#include "geometry.h"

#include <random>
#include <iostream>
#include <algorithm>
#include <cmath>

std::ostream& operator << (std::ostream& s, const Point& P) {
    return s << "(" << P.x << ", " << P.y << ")";
}

std::ostream& operator << (std::ostream& s, const Vector& V) {
    return s << "(" << V.x << ", " << V.y << ")";
}

const double eps = 0.001;
std::uniform_real_distribution<double> unif(0, 1);
std::default_random_engine re;

double Point::norm2() const {
    return x * x + y * y;
}

double Vector::norm2() const {
    return x * x + y * y;
}

double Vector::norm() const {
    return sqrt(norm2());
}

double Vector::orient() const {
    return atan2(-y, x);
}

Vector& Vector::normalize() {
    double norm = sqrt(norm2());
    x /= norm;
    y /= norm;
    return *this;    
}

Vector& Vector::operator+=(const Vector& V) {
    x += V.x;
    y += V.y;
    return *this;
}

Vector& Vector::operator-=(const Vector& V) {
    x -= V.x;
    y -= V.y;
    return *this;
}

Vector& Vector::operator*=(double k) {
    x *= k;
    y *= k;
    return *this;
} 

Vector& Vector::operator/=(double k) {
    x /= k;
    y /= k;
    return *this;
} 

Vector Vector::rot() const {
    return {y, -x};
}

Vector Vector::rot(double alpha) const {
    return {cos(alpha) * x + sin(alpha) * y, -sin(alpha) * x + cos(alpha) * y};
}

Vector operator-(const Point& A, const Point& B) {
    return {A.x - B.x, A.y - B.y};
}

Vector operator+(const Vector& U, const Vector& V) {
    return {U.x + V.x, U.y + V.y};
}

Vector operator-(const Vector& U, const Vector& V) {
    return {U.x - V.x, U.y - V.y};
}

Vector operator*(const Vector& V, double k) {
    return {k * V.x, k * V.y};
}

Vector operator/(const Vector& V, double k) {
    return {V.x / k, V.y / k};
}

Vector operator*(double k, const Vector& V) {
    return {k * V.x, k * V.y};
}

Vector operator-(const Vector& V) {
    return {-V.x, -V.y};
}

double operator*(const Vector& U, const Vector& V) {
    return U.x * V.x + U.y * V.y;
}

double operator^(const Vector& U, const Vector& V) {
    return U.x * V.y - U.y - V.x;
}

Point operator+(const Point& A, const Vector& V) {
    return {A.x + V.x, A.y + V.y};
}

Point operator-(const Point& A, const Vector& V) {
    return {A.x - V.x, A.y - V.y};
}

Point operator*(const Point& A, const Point& B) {
    return {(A.x + B.x) / 2, (A.y + B.y) / 2};
}

double dist(const Point& A, const Point& B) {
    return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}

double dist2(const Point& A, const Point& B) {
    return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}

double area(double a, double b, double c) {
    double p = (a + b + c) / 2;
    return sqrt(p * (p - a) * (p - b) * (p - c));
}

Point Barycentric(const Point& A, const Point& B, const Point& C, double x, double y, double z) {
    double s = x + y + z;
    
    x /= s;
    y /= s;
    z /= s;

    return {x * A.x + y * B.x + z * C.x, x * A.y + y * B.y + z * C.y};
}

Point Trilinear(const Point& A, const Point& B, const Point& C, double x, double y, double z) {
    double a = dist(B, C);
    double b = dist(A, C);
    double c = dist(A, B);

    return Barycentric(A, B, C, a * x, b * y, c * z);
}

Vector solve(const Vector& a1, const Vector& a2, const Vector& b) {
    // a1 and a2 are the rows of a 2x2 matrix A. Solve A*x = b
    double det = a1.x * a2.y - a1.y * a2.x;
    Vector res{b.x * a2.y - a1.y * b.y, a1.x * b.y - b.x * a2.x};
    return res / det;
}

Cycle::Cycle(const Point& A, double b, double c) : a(0), b(b), c(c), R(INFINITY) {
    d = -b * A.x - c * A.y;
}

Cycle::Cycle(const Point& A, const Vector& V) : Cycle(A, V.y, -V.x) {
}

Cycle::Cycle(const Point& O, double R) : a(1), R(R), O(O) {
    b = - 2 * O.x;
    c = - 2 * O.y;
    d = -R * R + O.x * O.x + O.y * O.y;
    if (R < 0) {
        a = -a;
        b = -b;
        c = -c;
        d = -d;
    }
}

Cycle::Cycle(double a, double b, double c, double d, double R) : a(a), b(b), c(c), d(d), R(R) {
    if (a != 0) {
        O.x = - b / 2 / a;
        O.y = - c / 2 / a;
    }
}

Cycle::Cycle(const Point& A, const Point& B, double alpha) {
    if (fabs(alpha) < eps) {
        // this is a line
        a = 0;
        b = A.y - B.y;
        c = B.x - A.x;
        d = A.x * B.y - B.x * A.y;
        R = INFINITY;
        return;
    }
    
	double dd = dist(A, B);
	double t = dd / 2 / tan(alpha);
	O.x = (A.x + B.x) / 2 + (B.y - A.y) / dd * t;
	O.y = (A.y + B.y) / 2 + (A.x - B.x) / dd * t;

	R = dd / 2 / sin(alpha);
	
    a = 1;
    b = -2 * O.x;
    c = -2 * O.y;
    d = -R * R + O.x * O.x + O.y * O.y;
    if (R < 0) {
        a = -a;
        b = -b;
        c = -c;
        d = -d;
    }
}

Cycle::Cycle(const Point& A, const Point& B, const Point& C) {
    Vector u = C - A;
    Vector v = B - A;
    
    u.normalize();
    v.normalize();
     
    if (fabs(u * v) < eps) {
        a = 0;
        b = A.y - B.y;
        c = B.x - A.x;
        d = A.x * B.y - B.x * A.y;
        R = INFINITY;
    }
    // this is circumcircle
    double a2 = dist2(B, C);
    double b2 = dist2(A, C);
    double c2 = dist2(A, B);
    
    double ba = a2 * (b2 + c2 - a2);
    double bb = b2 * (a2 + c2 - b2);
    double bc = c2 * (a2 + b2 - c2);
    double bsum = ba + bb + bc;
    
    ba /= bsum;
    bb /= bsum;
    bc /= bsum;
    
    O.x = ba * A.x + bb * B.x + bc * C.x;
    O.y = ba * A.y + bb * B.y + bc * C.y;
    
    R = dist(A, O);
    
    a = 1;
    b = -2 * O.x;
    c = -2 * O.y;
    d = -R * R + O.x * O.x + O.y * O.y;
}

Point Cycle::sample() const {
    double t = unif(re);
    
    if (a == 0) {
        // bx + cy + d = 0
        t *= 100;
        if (fabs(b) < fabs(c)) {
            return {t, (-d - b * t) / c};
        } else {
            return {(-d - c * t) / b, t};
        }    
    }
    // uniform on the circumcircle
    t *= 2 * M_PI;
    Vector V = R * Vector{cos(t), sin(t)};
    return O + V;
}

Point Cycle::inv(const Point& A) const {
    if (a == 1) {
        double d = dist(A, O);
        return O + ((R * R)/ (d * d)) * (A - O);
    }
    // here we are mirror imaging a point with respect to a line
    // bx + cy + d = 0
    double sign = b * A.x + c * A.y + d;
    double dd = fabs(b * A.x + c * A.y + d) / sqrt(b * b + c * c);
    
    Vector V = {b, c};
    V.normalize();

    // vector V always points from the negative half-plane to the positive one
    
    if (sign < 0) {
        return A + 2 * dd * V;
    } else {
        return A - 2 * dd * V;
    }
}

Cycle Cycle::inv(const Cycle& X) const {
    Point A = X.sample();
    Point B = X.sample();
    Point C = X.sample();
    
    return Cycle(inv(A), inv(B), inv(C));
}

int Cycle::side(const Point& A) const {
    double value = a * (A.x * A.x + A.y * A.y) + b * A.x + c * A.y + d;
    if (value < 0) {
        return -1;
    } else {
        return 1;
    }
}

void Cycle::invert() {
    if (a != 0) {
        R = -R;
    }
    a = -a;
    b = -b;
    c = -c;
    d = -d;
}

double Cycle::prod(const Point& A, const Vector& V) const {
    if (a == 0) {
        Vector U{b, c};
        U = U.rot().normalize();
        return V * U;
    }

    Vector U = (A - O).normalize().rot();
    return V * U;
}

Cycle hline(const Point& A, const Point& B) {
    if (A.x == B.x) {
        return Cycle(A, B);    
    }

    Point M = A * B;
    Vector v = (B - A).rot();
    
    double t = M.y / v.y;
    
    Point O = M - t * v;
    double R = dist(A, O);
    
    if (A.x > B.x) {
        R = -R;
    }
    return Cycle(O, R);
}

std::vector<Point> operator^(const Cycle& X, const Cycle& Y) {
    if (X.a == 0 && Y.a == 0) {
        // a pair of lines
        Vector u = {X.b, X.c};
        Vector v = {Y.b, Y.c};
        
        u.normalize();
        v.normalize();
        
        if (fabs(u * v) < eps) {
            // lines are parallel. We assume the intersection is empty
            return std::vector<Point>();
        }
        // intersection is a single point
        // bx + cy + d = 0
        double delta = X.b * Y.c - X.c * Y.b;        
        return {{ (-X.d * Y.c + X.c * Y.d) / delta, (-X.b * Y.d + X.d * Y.b) / delta}};  
    }
    if (X.a == 1 && Y.a == 0) {
        double d = fabs(Y.b * X.O.x + Y.c * X.O.y + Y.d) / sqrt(Y.b * Y.b + Y.c * Y.c);
        if (d > X.R + eps) {
            return {};
        }
       
        Vector V = {Y.b, Y.c};
        V.normalize();
        V *= -Y.side(X.O);
        
        if (fabs(X.R - d) <= eps) {
            return {X.O + X.R * V};
        }
        
        double t = sqrt(X.R * X.R - d * d);        
        Vector W = V.rot();
        return {X.O + d * V + t * W, X.O + d * V - t * W};
    }
    if (X.a == 0 && Y.a == 1) {
        return Y ^ X;
    }
    // intersection of two circles
    
    double R = fabs(X.R);
    double r = fabs(Y.R);
    double d = dist(X.O, Y.O);
    
    if (R + r < d + eps  || fabs(R - r) > d + eps) {
        return {};
    }
    
    Vector V = (Y.O - X.O).normalize();
    Vector W = V.rot();
      
    if (fabs(R + r - d) < eps || fabs(fabs(R - r) - d) < eps) {
        return {X.O + R * V};    
    }
    
    // two points
    double h = 2 * area(R, r, d) / d;
    //double h = sqrt(std::max[0, R * R - x * x));
    double x = (d * d - r * r + R * R) / 2 / d;    
    return {X.O + x * V + h * W, X.O + x * V - h * W};
}

std::vector<Cycle> Apollonius(const Cycle& X, const Cycle& Y, const Cycle& Z) {
    if (X.a == 0 && X.b == 0 && X.c == 0) {
        // three lines. Return two to four circles
        return {};
    }
    if (X.a == 1 && Y.a == 0 && Z.a == 0) {
        // two lines and a circle
        // a line and two circles
        // solve the system of equations
        Point O = (Y ^ Z)[0]; // intersection point
        
        Vector v = {Y.b, Y.c};
        Vector w = {Z.b, Z.c};
        v.normalize();
        w.normalize();        
        
        double coss = v * w;
        double cos_half = sqrt((coss + 1) / 2);
        double sin_half = sqrt((1 - coss) / 2);
        double tan_half = sqrt((1 - coss) / (coss + 1));
        
        Vector u = (v + w).normalize();
        
        std::vector<Cycle> res;
        std::vector<Vector> uu = {(v + w).normalize(), (v - w).normalize()};
        std::vector<int> ee = {1, -1};
        
        for (const auto& u : uu) {
            for (int e : ee) {		    
                // radius = t * cos_half 
                // solve some quadratic equations
                // (O - X.O + t * u) ^ 2 = (X.R +- t * cos_half) ^ 2 

                double A = 1 - cos_half * cos_half;
                double B = 2 * ((O - X.O) * u + e * X.R * cos_half); 
                double C = dist2(O, X.O) - X.R * X.R;
                double D = B * B - 4 * A * C;
                if (D < -eps) {
                    continue;
                }
                if (D < 0) {
                    D = 0;
                }
                D = sqrt(D);

                if (D < eps) {
                    double t = -B / 2 / A;
                    res.emplace_back(O + t * u, t * cos_half);
                }

                double t = (-B + D) / 2 / A;
                res.emplace_back(O + t * u, t * cos_half);

                t = (-B - D) / 2 / A;
                res.emplace_back(O + t * u, t * cos_half);
			}			
			cos_half = sin_half;
        }
        return res;
    }
    if (X.a == 0 && Y.a == 1 && Z.a == 0) {
        return Apollonius(Y, Z, X);
    }
    if (X.a == 0 && Y.a == 0 && Z.a == 1) {
        return Apollonius(Z, X, Y);
    }


    if (X.a == 1 && Y.a == 1 && Z.a == 0) {
        // a line and two circles
        return {};   
    }
    if (X.a == 1 && Y.a == 0 && Z.a == 1) {
        return Apollonius(Z, X, Y);
    }
    if (X.a == 0 && Y.a == 1 && Z.a == 1) {
        return Apollonius(Y, Z, X);
    }
    // three circles case
    const Point& Ia = X.O;
    const Point& Ib = Y.O;
    const Point& Ic = Z.O;
    const static std::vector<std::vector<int>> ee{{1,1,1}, {1,1,-1}, {1,-1,1}, {-1,1,1}, {1,-1,-1}, {-1,1,-1}, {-1,-1,1}, {-1,-1,-1}};
    
    std::vector<Cycle> res;
    for (const auto& e : ee) {
        // solve the system of equations
        // (X.O.x - x)^2 + (X.O.y - y)^2 = (R + e[0]* X.R)^2
        // (Y.O.x - x)^2 + (Y.O.y - y)^2 = (R + e[1]* Y.R)^2
        // (Z.O.x - x)^2 + (Z.O.y - y)^2 = (R + e[2]* Z.R)^2
        // subtract 1 from 2 and 3
        // Y.O.x^2 - X.O.x^2 - 2x(X.O.x - Y.O.x)    + (Y.O.y - y)^2 = (R + e[1]* Y.R)^2
        double ra = X.R * e[0];
        double rb = Y.R * e[1];
        double rc = Z.R * e[2];

        Vector a1 = 2 * (Ia - Ib);
        Vector a2 = 2 * (Ia - Ic);
        Vector b{2 * (-ra + rb), 2 * (-ra + rc)};
        Vector c{rb * rb - ra * ra - Ib.norm2() + Ia.norm2(), rc * rc - ra * ra - Ic.norm2() + Ia.norm2()}; 
    
        Vector n = solve(a1, a2, b);
        Vector m = solve(a1, a2, c);
        m -= Vector{Ia.x, Ia.y};

        double qa = 1 - n.norm2();
        double qb = 2 * ra - 2 * m * n;
        double qc = ra * ra - m.norm2();
        double disc = qb * qb - 4 * qa * qc;
        
        if (disc >= -eps) {
            double R = (-qb + sqrt(fmax(disc, 0))) / 2 / qa;
            Point E = Ia + m + R * n;
            res.emplace_back(E, fabs(R));
        }
    }
    return res;

}

std::vector<Cycle> Bisectors(const Cycle& X, const Cycle& Y) {
	if (X.a == 0 && Y.a == 0) {
        Vector V = {X.b, X.c};
        Vector W = {Y.b, Y.c};

        if (fabs(V * W) < eps) {
            return {Cycle(0, (X.b + Y.b) / 2, (X.c + Y.c) / 2, (X.d + Y.d) / 2, -1)};
        }

        Point A = (X ^ Y)[0];
        return {Cycle(A, V + W), Cycle(A, V - W)};
    }
    if (X.a == 1 && Y.a == 0) {
        // two cases 
        std::vector<Point> xy = X ^ Y;

        Vector V = {Y.b, Y.c};
        V.normalize();
        V *= -Y.side(X.O);

        double d = fabs(Y.b * X.O.x + Y.c * X.O.y + Y.d) / sqrt(Y.b * Y.b + Y.c * Y.c);       
        if (xy.size() <= 1) {
            return {Cycle(X.O - X.R * V, sqrt(2 * X.R * (X.R + d)))};
        }
        // two cicrles

        Point& A = xy[0];
        Point& B = xy[1];

        return {Cycle(X.O - X.R * V, dist(X.O - X.R * V, A)), Cycle(X.O + X.R * V, dist(X.O + X.R * V, A))};
    }
    if (X.a == 0 && Y.a == 1) {
        return Bisectors(Y, X);
    }
    // two circles
    double R = X.R;
    double r = Y.R;
    double d = dist(X.O, Y.O);
    Vector V = (Y.O - X.O).normalize();
   
    std::vector<Point> xy = X ^ Y;
   
    if (xy.size() <= 1) {
        // one cycle in the intersection
        if (fabs(R - r) < eps) {
            return {Cycle(X.O * Y.O, V.x, V.y)};
        }
       
        if (r + R <= d + eps) {
            // ok we have that the points R and -R are mapped into d - r and d + r respectively
            // and the map is x |--> rho^2 / (x+a) - a
            //            (-R                   R)                    (d - r    d + r)
            //              rho^2 / (a+-R) = a + d -+ r
            // rho^2 = (a + d - r)(a + R) = (a + d + r)(a - R) = a^2 + (R + d - r)a + dR - rR = a^2 + (-R + d + r)a - dR - rR
            //   dR  =  (-R + r)a
            double a = d * R / (r - R);
            Point S = X.O - a * V;
            double rho = sqrt(fmax((a + d - r) * (a + R), 0));
            //std::cout << "***" << rho << std::endl;
            return {Cycle(S, rho)};
        } else {
            // ok we have that the points R and -R are mapped into d + r and d - r respectively
            // and the map is x |--> rho^2 / (x+a) - a
            //              rho^2 / (a+-R) = a + d +- r
            // rho^2 = (a + d - r)(a - R) = (a + d + r)(a + R) = a^2 + (d - R - r)a - dR + rR = a^2 + (R + d + r)a + dR + rR
            //  (- R - r)a   =  dR
            double a = d * R / (-r - R);
            Point S = X.O - a * V;
            double rho = sqrt(fmax((a + d + r) * (a + R), 0));
            //std::cout << "###" << rho <<std::endl;
            return {Cycle(S, rho)};
        }
    }
   
    // two cycles
    Point A = xy[0];
    Point B = xy[1];   
    if (fabs(R - r) < eps) {
        // a circle and a line
        Point C = X.O * Y.O;
        return {Cycle(C, V.x, V.y), Cycle(C, dist(C, A))};
    }
    // two circles
   
    Point O = {(r * X.O.x + R * Y.O.x) / (R + r), (r * X.O.y + R * Y.O.y) / (R + r)};
    Point Q = {(r * X.O.x - R * Y.O.x) / (r - R), (r * X.O.y - R * Y.O.y) / (r - R)};
    
    return {Cycle(O, dist(O, A)), Cycle(Q, dist(Q, A))};
}

Cycle Orthogonal(const Cycle& X, const Point& A, const Point& B) {
    Point C = X.inv(A);
    Point D = X.inv(B);
    
    if (dist(A, C) > dist(B, D)) {
        return Cycle(A, B, C);
    }
    return Cycle(A, B, D);
}

double sin(const Cycle& X, const Cycle& Y) {
    double cs = cos(X, Y);
    if (fabs(cs) <= 1) {
        return sqrt(1.0 - cs * cs);
    } else {
        return sqrt(cs * cs - 1.0);
    }

    Point Z = (X ^ Y)[0];
    
    Vector v = (Z - X.O).normalize();
    Vector w = (Z - Y.O).normalize();
    
    double cos = v * w;
    
    return sqrt(1 - cos * cos);
}

double cos(const Cycle& X, const Cycle& Y) {
   if (X.a == 0 && Y.a == 0) {
       Vector u{X.b, X.c};
       Vector v{Y.b, Y.c};
       
       u.normalize();
       v.normalize();
       
       return u * v;
   }
   if (X.a != 0 && Y.a == 0) {
       Vector v{Y.b, Y.c};
       return (Y.b * X.O.x + Y.c * X.O.y + Y.d) / v.norm() / X.R;
   }
   if (X.a == 0 && Y.a != 0) {
       return cos(Y, X);
   }
   double d2 = dist2(X.O, Y.O);
   return (d2 - X.R * X.R - Y.R * Y.R) / 2 / X.R / Y.R;
}

double operator*(const Cycle& X, const Cycle& Y) {
    return cos(X, Y);
}

inline double sqr(double x) {
    return x*x;
}

Cycle Split(const Cycle& X, const Cycle& Y, double lambda) {
    double rx = X.R;
    double ry = Y.R;

    double u = -ry / (lambda * rx - ry);
    double v = lambda * rx / (lambda * rx - ry);
    
    double d = dist2(X.O, Y.O);
    double m = (sqr(sqr(rx) - sqr(ry)) / sqr(d) - 2 * (sqr(rx) + sqr(ry)) + sqr(d)) / 4;    
    
    Point O = {u * X.O.x + v * Y.O.x, u * X.O.y + v * Y.O.y};
    //double R = (1 + lambda) * X.R * Y.R / (lambda * X.R + Y.R);
    
    double x = (sqr(rx) - sqr(ry) + sqr(d)) / 2 / d;
    double y = (sqr(rx) - sqr(ry) - sqr(d)) / 2 / d;
    
    m = sqr(rx) - sqr(x);
    
    double z = u * x + v * y;
    
    //double R = sqrt(sqr(z) + m);
    
    //double R = sqrt(sqr(u * rx) + sqr(v * ry) + u * v * (sqr(rx) + sqr(ry) - sqr(d)));
    double R = sqrt(sqr(rx) * sqr(ry) * (1 + sqr(lambda))  - lambda * rx * ry * (sqr(rx) + sqr(ry) - d))/fabs(lambda * rx - ry);
    
    return Cycle(O, R);
}

Orientation operator!(const Orientation& orient) {
    return orient == ABC? ACB : ABC;
} 

Orientation Triangle::get_euclidean_orientation() const {
    if ((B.x - A.x) * (C.y - A.y) > (B.y - A.y) * (C.x - A.x)) {
        return ACB;
    }
    return ABC;
}

void Triangle::hyperbolic() {
    double a = dist(B, C);
    double b = dist(A, C);
    double c = dist(A, B);
    
    auto orient = get_euclidean_orientation();
    
    alpha0 = acos((b * b + c * c - a * a) / (2 * b * c));
    beta0 = acos((a * a + c * c - b * b) / (2 * a * c));
    gamma0 = acos((a * a + b * b - c * c) / (2 * a * b));
    
    aa = hline(B, C);
    bb = hline(C, A);
    cc = hline(A, B);
    
    //aa.draw();
   // bb.draw();
    //cc.draw();
    
    orientation = aa.side(A) == -1? ABC : ACB;  
    
    //std::cout << orientation << std::endl;


   /* if (aa.side(A) == -1) {
        aa.R = -aa.R;
    }
    if (bb.side(B) == -1) {
        bb.R = -bb.R;
    }
    if (cc.side(C) == -1) {
        cc.R = -cc.R;
    }*/
    
    alpha = acos(std::clamp(bb * cc, -1.0, 1.0));
    beta = acos(std::clamp(aa * cc, -1.0, 1.0));
    gamma = acos(std::clamp(aa * bb, -1.0, 1.0));
    
    if (orientation == ABC) {
		if (orient == ABC) {
			a0 = (beta - beta0 + gamma - gamma0 - alpha + alpha0) / 2;
			b0 = (alpha - alpha0 + gamma - gamma0 - beta + beta0) / 2;
			c0 = (alpha - alpha0 + beta - beta0 - gamma + gamma0) / 2;
		} else {
		    a0 = -M_PI + (4 * M_PI - beta - beta0 - gamma - gamma0 + alpha + alpha0) / 2;
		    b0 = -M_PI + (4 * M_PI - alpha - alpha0 - gamma - gamma0 + beta + beta0) / 2;
		    c0 = -M_PI + (4 * M_PI - alpha - alpha0 - beta - beta0 + gamma + gamma0) / 2;
		}    
    } else {
		if (orient == ACB) {
			a0 = (beta - beta0 + gamma - gamma0 - alpha + alpha0) / 2;
			b0 = (alpha - alpha0 + gamma - gamma0 - beta + beta0) / 2;
			c0 = (alpha - alpha0 + beta - beta0 - gamma + gamma0) / 2;
		} else {
		    a0 = -M_PI + (4 * M_PI - beta - beta0 - gamma - gamma0 + alpha + alpha0) / 2;
		    b0 = -M_PI + (4 * M_PI - alpha - alpha0 - gamma - gamma0 + beta + beta0) / 2;
		    c0 = -M_PI + (4 * M_PI - alpha - alpha0 - beta - beta0 + gamma + gamma0) / 2;
		}    
    }

    //std::cout << a0 << "  " << b0 << "  " << c0 << std::endl;
   // std::cout << alpha << "  " << beta << "  " << gamma << std::endl;
    //std::cout << alpha0 << "  " << beta0 << "  " << gamma0 << std::endl;
    
    //if (orientation == ACB) {
    //    a0 = 2 * M_PI - a0;
    //    b0 = 2 * M_PI - b0;
    //    c0 = 2 * M_PI - c0;
    //}

   // aa = orient > 0? Cycle(B, C, a0) : Cycle(C, B, a0);
  //  bb = orient > 0? Cycle(C, A, b0) : Cycle(A, C, b0);
   // cc = orient > 0? Cycle(A, B, c0) : Cycle(B, A, c0);


}

void Triangle::recompute() {
    double a = dist(B, C);
    double b = dist(A, C);
    double c = dist(A, B);
    
    auto orient = get_euclidean_orientation();    

    alpha0 = acos((b * b + c * c - a * a) / (2 * b * c));
    beta0 = acos((a * a + c * c - b * b) / (2 * a * c));
    gamma0 = acos((a * a + b * b - c * c) / (2 * a * b));
    
    // solving the system
    // b0 + alpha0 + c0 = alpha
    // a0 + beta0 + c0 = beta
    // a0 + gamma0 + b0 = gamma
    
    a0 = (beta - beta0 + gamma - gamma0 - alpha + alpha0) / 2;
    b0 = (alpha - alpha0 + gamma - gamma0 - beta + beta0) / 2;
    c0 = (alpha - alpha0 + beta - beta0 - gamma + gamma0) / 2;
   
    // solving the system
    // b0 + alpha0 + c0 = 2pi - alpha
    // a0 + beta0 + c0 = 2pi - beta
    // a0 + gamma0 + b0 = 2pi - gamma
    if (orient < 0) {
        a0 = -M_PI + (4 * M_PI - beta - beta0 - gamma - gamma0 + alpha + alpha0) / 2;
        b0 = -M_PI + (4 * M_PI - alpha - alpha0 - gamma - gamma0 + beta + beta0) / 2;
        c0 = -M_PI + (4 * M_PI - alpha - alpha0 - beta - beta0 + gamma + gamma0) / 2;
    }

    aa = orient > 0? Cycle(B, C, a0) : Cycle(C, B, a0);
    bb = orient > 0? Cycle(C, A, b0) : Cycle(A, C, b0);
    cc = orient > 0? Cycle(A, B, c0) : Cycle(B, A, c0);
    
    return;

    double delta = (M_PI + alpha + beta + gamma) / 4;

    omega_a = orient > 0? Cycle(B, C, -delta + a0) : Cycle(C, B, delta + a0);
    omega_b = orient > 0? Cycle(C, A, -delta + b0) : Cycle(A, C, delta + b0);
    omega_c = orient > 0? Cycle(A, B, -delta + c0) : Cycle(B, A, delta + c0);

    std::vector<Point> ha = omega_b ^ aa;
    std::vector<Point> hb = omega_c ^ bb;
    std::vector<Point> hc = omega_a ^ cc;

    if (ha.size() == 1) {
        ha.push_back(ha.back());
    }
    if (hb.size() == 1) {
        hb.push_back(hb.back());
    }
    if (hc.size() == 1) {
        hc.push_back(hc.back());
    }

    Ha = dist(C, ha[0]) > dist(C, ha[1])? ha[0] : ha[1];
    Hb = dist(A, hb[0]) > dist(A, hb[1])? hb[0] : hb[1];
    Hc = dist(B, hc[0]) > dist(B, hc[1])? hc[0] : hc[1];
    
    std::vector<Cycle> inexcircles = Apollonius(aa, bb, cc);
    
    int aa_side = aa.side(A);
    int bb_side = bb.side(B);
    int cc_side = cc.side(C);

    for (const Cycle& circle : inexcircles) {
        if (aa.side(circle.sample()) == aa_side && bb.side(circle.sample()) == bb_side && cc.side(circle.sample()) == cc_side) {
            inc = circle;
            break;
        }
    }
}

std::vector<Cycle> Triangle::get_sides() const {
    return {aa, bb, cc};
}

std::vector<Cycle> Triangle::get_bisectors() const {
    std::vector<Cycle> bs;
    
    auto orient = get_euclidean_orientation();
    
    Vector ab = orient == ABC ? (B - A).rot(-c0).normalize() : (B - A).rot(c0).normalize();
    Vector ba = orient == ABC ? (A - B).rot(c0).normalize() : (A - B).rot(-c0).normalize();
    Vector ac = orient == ABC ? (C - A).rot(b0).normalize() : (C - A).rot(-b0).normalize();
    Vector ca = orient == ABC ? (A - C).rot(-b0).normalize() : (A - C).rot(b0).normalize();       
    Vector bc = orient == ABC ? (C - B).rot(-a0).normalize() : (C - B).rot(a0).normalize();
    Vector cb = orient == ABC ? (B - C).rot(a0).normalize() : (B - C).rot(-a0).normalize();
    
    std::vector<Cycle> a_bisectors = Bisectors(bb, cc);
    std::vector<Cycle> b_bisectors = Bisectors(aa, cc);
    std::vector<Cycle> c_bisectors = Bisectors(aa, bb);
    
    if (fabs(alpha - M_PI) > eps) {
        if (a_bisectors.size() == 1) {
            a_bisectors[0].draw();
        } else {  // size() == 2
            Vector V = (ab + ac).normalize();
            if (fabs(a_bisectors[0].prod(A, V)) > fabs(a_bisectors[1].prod(A, V))) {
                bs.push_back(a_bisectors[0]);
            } else {
                bs.push_back(a_bisectors[1]);
            }
        }
    }
    if (fabs(beta - M_PI) > eps) {        
        if (b_bisectors.size() == 1) {
            b_bisectors[0].draw();
        } else { // size() == 2
            Vector V = (ba + bc).normalize();
            if (fabs(b_bisectors[0].prod(B, V)) > fabs(b_bisectors[1].prod(B, V))) {
                bs.push_back(b_bisectors[0]);
            } else {
                bs.push_back(b_bisectors[1]);
            }
        }
    }
    if (fabs(gamma - M_PI) > eps) {            
        if (c_bisectors.size() == 1) {
            c_bisectors[0].draw();
        } else { // size() == 2
            Vector V = (ca + cb).normalize();
            if (fabs(c_bisectors[0].prod(C, V)) > fabs(c_bisectors[1].prod(C, V))) {
                bs.push_back(c_bisectors[0]);
            } else {
                bs.push_back(c_bisectors[1]);
            }
        }
    }    

    return bs;
}

std::vector<Cycle> Triangle::get_cevians(const Point& X, const Point& Y, const Point& Z) const {
    std::vector<Point> AA = bb ^ cc;
    std::vector<Point> BB = aa ^ cc;
    std::vector<Point> CC = aa ^ bb;    
    return {Cycle(AA[0], X, AA[1]), Cycle(BB[0], Y, BB[1]), Cycle(CC[0], Z, CC[1])};
}

std::vector<Cycle> Triangle::get_cevians(const Point& P) const {
    return get_cevians(P, P, P);
}

std::vector<Cycle> Triangle::get_altitudes() const {
    std::vector<Point> AA = bb ^ cc;
    std::vector<Point> BB = aa ^ cc;
    std::vector<Point> CC = aa ^ bb;
    return {Orthogonal(aa, AA[0], AA[1]), Orthogonal(bb, BB[0], BB[1]), Orthogonal(cc, CC[0], CC[1])};
}

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

inline double sqr(double x) {
    return x*x;
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
    return U.x * V.y - U.y * V.x;
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

Cycle::Cycle(const Point& A, const Vector& V) : Cycle(A, A + V) {
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

Cycle::Cycle(double a, double b, double c, double d) : a(a), b(b), c(c), d(d) {
    if (a) {
        O.x = - b / 2 / a;
        O.y = - c / 2 / a;
        R = sqrt(fabs(b*b + c*c - 4*a*d)) / 2 / a;
        if (b*b + c*c - 4*a*d < 0) {
            virt = true;
        }
        if (fabs(R) > 1e6) {
            a = 0;
            R = INFINITY;
        }
        
    } else {
        R = INFINITY;
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

    if (fabs(u ^ v) < eps) {
        a = 0;
        b = A.y - B.y;
        c = B.x - A.x;
        d = A.x * B.y - B.x * A.y;
        R = INFINITY;
        return;        
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
    if (a) {
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

Cycle hperpendicular(const Cycle& X, const Cycle& Y) {
    double d = dist(X.O, Y.O);
    double t = 0.5 * (1.0 + (X.R * X.R - Y.R * Y.R ) / d / d);
    Point O = X.O + t * (Y.O - X.O);
    double R = sqrt(fabs(t * t * d * d  - X.R * X.R));
    return Cycle(O, R);
}

std::vector<Point> operator^(const Cycle& X, const Cycle& Y) {
    if (X.a == 0 && Y.a == 0) {
        // a pair of lines
        Vector u = {X.b, X.c};
        Vector v = {Y.b, Y.c};
        
        u.normalize();
        v.normalize();
        
        if (fabs(u ^ v) < eps) {
            // lines are parallel. We assume the intersection is empty
            return std::vector<Point>();
        }
        // intersection is a single point
        // bx + cy + d = 0
        double delta = X.b * Y.c - X.c * Y.b;        
        return {{ (-X.d * Y.c + X.c * Y.d) / delta, (-X.b * Y.d + X.d * Y.b) / delta}};  
    }
    if (X.a != 0 && Y.a == 0) {
        double d = fabs(Y.b * X.O.x + Y.c * X.O.y + Y.d) / sqrt(Y.b * Y.b + Y.c * Y.c);
        if (d > fabs(X.R) + eps) {
            return {};
        }
       
        Vector V = {Y.b, Y.c};
        V.normalize();
        V *= -Y.side(X.O);
        
        if (fabs(fabs(X.R) - d) <= eps) {
            return {X.O + fabs(X.R) * V};
        }
        
        double t = sqrt(X.R * X.R - d * d);        
        Vector W = V.rot();
        return {X.O + d * V + t * W, X.O + d * V - t * W};
    }
    if (X.a == 0 && Y.a != 0) {
        return Y ^ X;
    }
    // intersection of two circles
    
    double R = fabs(X.R);
    double r = fabs(Y.R);
    double d = dist(X.O, Y.O);
    
    if (R + r < d - eps || fabs(R - r) > d + eps) {
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


std::vector<double> solve_quadratic(std::vector<double> cc) {
    if (cc.size() != 3) {
        std::cout << "ERROR! Incorrect input in quadratic equation";
        exit(1);
    }

    double a = cc[0];
    double b = cc[1];
    double c = cc[2];
    double eps = 1e-7;
    
    if (fabs(a) < eps) {
        if (b) {
            return {-c / b};
        }
        if (c) {
            return {};
        } else {
            return {0};
        }
    }
    double D = b * b - 4 * a * c;
    if (D < -eps) {
        return {};
    }
    if (D < eps) {
        return {- b / 2 / a}; 
    }

    D = sqrt(D);
    return { (-b - D) / 2 / a, (-b + D) / 2 / a};
}

std::vector<double> solve_linear23(std::vector<double> a, std::vector<double> b, bool solve_homogeneous=false) {
    if (a.size() != 4 || b.size() != 4) {
        std::cout << "ERROR! Incorrect input in 2*3 system of linear equations" << std::endl;
        exit(1);
    }
    if (solve_homogeneous) {
        // ignore the fourth column; get the nonzero solution by taking cross product
        return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
    }
    // now get any solution to the full system
    double d0 = fabs(a[1] * b[2] - a[2] * b[1]);
    double d1 = fabs(a[2] * b[0] - a[0] * b[2]);
    double d2 = fabs(a[0] * b[1] - a[1] * b[0]);
    
    int i = 0;
    if (d1 > d0 && d1 >= d2) {
        i = 1;
    }
    if (d1 > d0 && d2 > d1) {
        i = 2;
    }
    
    a[3] -= a[i];
    b[3] -= b[i];
    
    int j = (i + 1) % 3;
    int k = (i + 2) % 3;
    
    double d = a[j] * b[k] - a[k] * b[j]; 
    
    std::vector<double> res(3);
    
    res[i] = 1.0;
    res[j] = (a[3] * b[k] - a[k] * b[3]) / d;
    res[k] = (a[j] * b[3] - a[3] * b[j]) / d;
    
    return res;
}


std::vector<Cycle> apollonius(const Cycle& X, const Cycle& Y, const Cycle& Z) {
    // the dot product is (2 * (a * T + X * d) - b * Y - c * Z) = 1 
    if (X.a == 0 && X.b == 0 && X.c == 0) {
        // three lines. Return two to four circles
        return {};
    }
    if (X.a && Y.a == 0 && Z.a == 0) {
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
    if (X.a == 0 && Y.a && Z.a == 0) {
        return apollonius(Y, Z, X);
    }
    if (X.a == 0 && Y.a == 0 && Z.a) {
        return apollonius(Z, X, Y);
    }


    if (X.a && Y.a && Z.a == 0) {
        // a line and two circles
        return {};   
    }
    if (X.a && Y.a == 0 && Z.a) {
        return apollonius(Z, X, Y);
    }
    if (X.a == 0 && Y.a && Z.a) {
        return apollonius(Y, Z, X);
    }
    // three circles case
    const static std::vector<std::vector<int>> ee{{1,1,1}, {1,1,-1}, {1,-1,1}, {-1,1,1}, {1,-1,-1}, {-1,1,-1}, {-1,-1,1}, {-1,-1,-1}};
    
    std::vector<Cycle> res;
    for (const auto& e : ee) {
        // solve the system of equations
        // (x - X.O.x)^2 + (y - X.O.y)^2 = (R + e[0]*X.R)^2
        // (x - Y.O.x)^2 + (y - Y.O.y)^2 = (R + e[1]*Y.R)^2
        // (x - Z.O.x)^2 + (y - Z.O.y)^2 = (R + e[2]*Z.R)^2
        
        // subtract 1 from 2 and 3
        //  x(X.O.x - Y.O.x) + y(X.O.y - Y.O.y) + R(e[1]*Y.R - e[0]*X.R) = (-Y.R^2 + X.R^2 + Y.O.x^2 - X.O.x^2 + Y.O.y^2 - X.O.y^2) / 2
        //  x(X.O.x - Z.O.x) + y(X.O.y - Z.O.y) + R(e[2]*Z.R - e[0]*X.R) = (-Z.R^2 + X.R^2 + Z.O.x^2 - X.O.x^2 + Z.O.y^2 - X.O.y^2) / 2
        
        
        std::vector<double> a = {X.O.x - Y.O.x, X.O.y - Y.O.y, -e[1] * Y.R + e[0] * X.R, (sqr(Y.R) - sqr(X.R) - sqr(Y.O.x) - sqr(Y.O.y) + sqr(X.O.x) + sqr(X.O.y)) / 2};
        std::vector<double> b = {X.O.x - Z.O.x, X.O.y - Z.O.y, -e[2] * Z.R + e[0] * X.R, (sqr(Z.R) - sqr(X.R) - sqr(Z.O.x) - sqr(Z.O.y) + sqr(X.O.x) + sqr(X.O.y)) / 2};
        
        std::vector<double> res0 = solve_linear23(a, b, true);
        std::vector<double> res1 = solve_linear23(a, b);
        
        // now solve the quadratic equation that remains after inserting this into 1
        // x = res1[0] + t * res0[0]
        // y = res1[1] + t * res0[1]
        // R = res1[2] + t * res0[2]
        
        std::vector cc = {sqr(res0[0]) + sqr(res0[1]) - sqr(res0[2]), 
                          2 * res0[0] * (res1[0] - X.O.x) + 2 * res0[1] * (res1[1] - X.O.y) - 2 * res0[2] * (res1[2] + e[0] * X.R),
                          sqr(res1[0] - X.O.x) + sqr(res1[1] - X.O.y) - sqr(res1[2] + e[0] * X.R)};
                          
        for (double t : solve_quadratic(cc)) {
            Point O = {res1[0] + t * res0[0], res1[1] + t * res0[1]};
            double R = res1[2] + t * res0[2];
            res.emplace_back(O, R);
        }
    }
    return res;

}

std::vector<Cycle> get_bisectors(const Cycle& X, const Cycle& Y) {
	if (X.a == 0 && Y.a == 0) {
        Vector V = {X.b, X.c};
        Vector W = {Y.b, Y.c};

        if (fabs(V * W) < eps) {
            return {Cycle(0, (X.b + Y.b) / 2, (X.c + Y.c) / 2, (X.d + Y.d) / 2)};
        }

        Point A = (X ^ Y)[0];
        return {Cycle(A, V + W), Cycle(A, V - W)};
    }
    if (X.a && Y.a == 0) {
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

        const Point& A = xy[0];

        return {Cycle(X.O - X.R * V, dist(X.O - X.R * V, A)), Cycle(X.O + X.R * V, dist(X.O + X.R * V, A))};
    }
    if (X.a == 0 && Y.a) {
        return get_bisectors(Y, X);
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
    const Point& A = xy[0];
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

Cycle orthogonal(const Cycle& X, const Point& A, const Point& B) {
    // a cycle passing through A and B and orthogonal to X
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
   if (X.a && Y.a == 0) {
       Vector v{Y.b, Y.c};
       return -(Y.b * X.O.x + Y.c * X.O.y + Y.d) / v.norm() / X.R;
   }
   if (X.a == 0 && Y.a) {
       return cos(Y, X);
   }
   double d2 = dist2(X.O, Y.O);
   return (X.R * X.R + Y.R * Y.R - d2) / 2 / X.R / Y.R;
}

double operator*(const Cycle& X, const Cycle& Y) {
    return cos(X, Y);
}

double splitting(const Cycle& X, const Cycle& Y, const Cycle& Z) {
    double norm_x = sqrt(-4 * X.a * X.d + X.b * X.b + X.c * X.c);
    double norm_y = sqrt(-4 * Y.a * Y.d + Y.b * Y.b + Y.c * Y.c);
    
    std::vector<double> xx = {X.a / norm_x, X.b / norm_x, X.c / norm_x, X.d / norm_x};
    std::vector<double> yy = {Y.a / norm_y, Y.b / norm_y, Y.c / norm_y, Y.d / norm_y};
    std::vector<double> zz = {Z.a, Z.b, Z.c, Z.d};
    std::vector<double> ll;
    
    for (int i = 0; i < 3; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            double det = zz[i] * yy[j] - zz[j] * yy[i];
            if (fabs(det) > 1e-9) {
                ll.push_back((zz[i] * xx[j] - zz[j] * xx[i]) / det);
            }
        }
    }
    
    if (ll.empty()) {
        return 0;
    }
    double lambda = 0;
    for (double l : ll) {
        lambda += l;
    }
    return lambda / static_cast<double>(ll.size());
}

Cycle split(const Cycle& X, const Cycle& Y, double lambda) {
    double norm_x = sqrt(-4 * X.a * X.d + X.b * X.b + X.c * X.c);
    double norm_y = sqrt(-4 * Y.a * Y.d + Y.b * Y.b + Y.c * Y.c);
    double a = X.a / norm_x - lambda * Y.a / norm_y;
    double b = X.b / norm_x - lambda * Y.b / norm_y;
    double c = X.c / norm_x - lambda * Y.c / norm_y;
    double d = X.d / norm_x - lambda * Y.d / norm_y;

    return {a, b, c, d};


  /*  double rx = X.R;
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
    
    return Cycle(O, R);*/
}

Cycle split(const Cycle& X, const Cycle& Y, double x, double y) {
    double norm_x = sqrt(-4 * X.a * X.d + X.b * X.b + X.c * X.c);
    double norm_y = sqrt(-4 * Y.a * Y.d + Y.b * Y.b + Y.c * Y.c);
    double a = x * X.a / norm_x + y * Y.a / norm_y;
    double b = x * X.b / norm_x + y * Y.b / norm_y;
    double c = x * X.c / norm_x + y * Y.c / norm_y;
    double d = x * X.d / norm_x + y * Y.d / norm_y;
    double norm = sqrt(-4 * a * d + b * b + c * c);
    
    a /= norm;
    b /= norm;
    c /= norm;
    d /= norm;
    
    if (fabs(a) < 1e-7) {
        a = 0;
    }

    return {a, b, c, d};
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


void Triangle::recompute() {
    double a = dist(B, C);
    double b = dist(A, C);
    double c = dist(A, B);
    
    auto orient = get_euclidean_orientation();    

    alpha0 = acos((b * b + c * c - a * a) / (2 * b * c));
    beta0 = acos((a * a + c * c - b * b) / (2 * a * c));
    gamma0 = acos((a * a + b * b - c * c) / (2 * a * b));
    
    if (hyperbolic) {
		aa = hline(B, C);
		bb = hline(C, A);
		cc = hline(A, B);

		orientation = aa.side(A) == -1? ABC : ACB;  

		alpha = acos(std::clamp(-(bb * cc), -1.0, 1.0));
		beta = acos(std::clamp(-(aa * cc), -1.0, 1.0));
		gamma = acos(std::clamp(-(aa * bb), -1.0, 1.0));
		//std::cout << alpha << " " << beta << " " << gamma << std::endl;

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
    } else {
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
    }

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
    
    compute_inexcircles();

}

Cycle Triangle::cycle(double x, double y, double z) const {
    double norm_a = sqrt(-4 * aa.a * aa.d + aa.b * aa.b + aa.c * aa.c);
    double norm_b = sqrt(-4 * bb.a * bb.d + bb.b * bb.b + bb.c * bb.c);
    double norm_c = sqrt(-4 * cc.a * cc.d + cc.b * cc.b + cc.c * cc.c);
    
    double a = x * aa.a / norm_a + y * bb.a / norm_b + z * cc.a / norm_c;
    double b = x * aa.b / norm_a + y * bb.b / norm_b + z * cc.b / norm_c;
    double c = x * aa.c / norm_a + y * bb.c / norm_b + z * cc.c / norm_c;
    double d = x * aa.d / norm_a + y * bb.d / norm_b + z * cc.d / norm_c;
    
    return {a, b, c, d};
}

std::vector<Cycle> Triangle::get_sides() const {
    return {aa, bb, cc};
}

std::vector<Cycle> Triangle::get_bisectors() const {
    return {split(bb, cc, 1), split(cc, aa, 1), split(aa, bb, 1)};
}

std::vector<Cycle> Triangle::get_external_bisectors() const {
    return {split(bb, cc, -1), split(cc, aa, -1), split(aa, bb, -1)};
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
    return {split(bb, cc, cos(beta), -cos(gamma)), split(cc, aa, cos(gamma), -cos(alpha)), split(aa, bb, cos(alpha), -cos(beta))};
}

void Triangle::compute_inexcircles() {
    std::vector<Cycle> all_tangent = apollonius(aa, bb, cc);
    std::vector<Cycle> bs = get_bisectors();
    std::vector<Cycle> exbs = get_external_bisectors();
    
    incircles.clear();
    excircles.clear();
    
    double eps = 1e-6;
    for (const Cycle& X : all_tangent) {
        if (fabs(X * bs[0]) < eps && fabs(X * bs[1]) < eps && fabs(X * bs[2]) < eps) {
            incircles.push_back(X);
        }
    }
    for (const Cycle& X : all_tangent) {
        if (fabs(X * bs[0]) < eps && fabs(X * exbs[1]) < eps && fabs(X * exbs[2]) < eps) {
            excircles.push_back(X);
        }
    }
    for (const Cycle& X : all_tangent) {
        if (fabs(X * exbs[0]) < eps && fabs(X * bs[1]) < eps && fabs(X * exbs[2]) < eps) {
            excircles.push_back(X);
        }
    }
    for (const Cycle& X : all_tangent) {
        if (fabs(X * exbs[0]) < eps && fabs(X * exbs[1]) < eps && fabs(X * bs[2]) < eps) {
            excircles.push_back(X);
        }
    }
}



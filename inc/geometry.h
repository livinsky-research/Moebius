#pragma once
#include <vector>
#include <cmath>
#include <ostream>

struct Point {
    double x;
    double y;  
    void draw() const;
    double norm2() const;
};


double dist(const Point& A, const Point& B);
double dist2(const Point& A, const Point& B);
double area(double a, double b, double c);

void line(const Point& A, const Point& B);
void circle(const Point& A, double r);
void arc(const Point& A, const Point& B, double alpha);

// midpoint
Point operator*(const Point& A, const Point& B);

struct Vector {
    double x;
    double y;
    
    double norm() const;
    double norm2() const;
    double orient() const;
    
    Vector& normalize();
    Vector& operator+=(const Vector& V);
    Vector& operator-=(const Vector& V);
    Vector& operator*=(double k);
    Vector& operator/=(double k);    
    Vector rot() const;
    Vector rot(double alpha) const;
    
    void draw(const Point& A) const;
};

std::ostream& operator << (std::ostream& s, const Point& P);
std::ostream& operator << (std::ostream& s, const Vector& V);

Vector operator-(const Point& A, const Point& B);
Vector operator+(const Vector& U, const Vector& V);
Vector operator-(const Vector& U, const Vector& V);
Vector operator*(const Vector& V, double k);
Vector operator/(const Vector& V, double k);
Vector operator*(double k, const Vector& V);

Vector operator-(const Vector& V);
double operator*(const Vector& U, const Vector& V);
double operator^(const Vector& U, const Vector& V);
Point operator+(const Point& A, const Vector& V);
Point operator-(const Point& A, const Vector& V);
Point Barycentric(const Point& A, const Point& B, const Point& C, double x, double y, double z);
Point Trilinear(const Point& A, const Point& B, const Point& C, double x, double y, double z);


// solve a 2x2 system of linear equations
Vector solve(const Vector& a1, const Vector& a2, const Vector& b);

struct Cycle {
    // line and circle in the form
    // a(x^2 + y^2) + bx + cy + d = 0
    // a = 0 iff we have a line
    // R is computed from R^2 = b^2/4a^a + c^2/4a^2 - d/a
    // sign R must be the equal to sign a
    double a, b, c, d;
    double R;
    bool virt = false;
    Point O;
    
    Cycle() = default;
    Cycle(const Point& A, double b, double c);
    Cycle(const Point& A, const Vector& V);
    Cycle(const Point& O, double R);
    Cycle(double a, double b, double c, double d);
    Cycle(const Point& A, const Point& B, double alpha = 0);
    Cycle(const Point& A, const Point& B, const Point& C);
    
    Point sample() const;
    Point inv(const Point& A) const;
    Cycle inv(const Cycle& X) const;
    int side(const Point& A) const;
    void invert();
    double prod(const Point& P, const Vector& V) const;
    
    void draw() const;
};


// hyperbolic geometry oriented line A --> B
Cycle hline(const Point& A, const Point& B);

// a common perperdicular to two lines on the hyperbolic plane
Cycle hperpendicular(const Cycle& X, const Cycle& Y);

// intersection of cycles. Returns 0, 1, or 2 points
std::vector<Point> operator^(const Cycle& X, const Cycle& Y);

// returns all cycles tangent to X, Y, and Z
std::vector<Cycle> Apollonius(const Cycle& X, const Cycle& Y, const Cycle& Z);

// returns 1 or 2 cycles that map X into Y
std::vector<Cycle> Bisectors(const Cycle& X, const Cycle& Y);

// draw a cycle passing through A and B that is orthogonal to X
Cycle Orthogonal(const Cycle& X, const Point& A, const Point& B);

//Cycle operator *(const Cycle& P, const Cycle& Q);
double sin(const Cycle& X, const Cycle& Y);
double cos(const Cycle& X, const Cycle& Y);

double operator*(const Cycle& X, const Cycle& Y);

Cycle Split(const Cycle& X, const Cycle& Y, double lambda);
Cycle Split(const Cycle& X, const Cycle& Y, double x, double y);

class Digon {
public:
    Digon(const Point& A, const Point& B, double alpha) : A(A), B(B), alpha(alpha), orientation(0.3), bisector(false) {
    }

    const Point& A;
    const Point& B;

    double alpha;
    double orientation;
    bool bisector;
    
    void draw() const;
};

enum Orientation {ABC = 1, ACB = -1};

Orientation operator!(const Orientation& orient);

class Triangle {
public:
    Triangle(const Point& A, const Point& B, const Point& C, double alpha, double beta, double gamma) : A(A), B(B), C(C), alpha(alpha), beta(beta), gamma(gamma) {
    }

    const Point& A;
    const Point& B;
    const Point& C;

    double alpha;
    double beta;
    double gamma;

    Orientation orientation = ABC;

    bool euler = false;
    bool circumcircle = false;
    bool incircle = false;
    bool excircles = false;
    bool bisectors = false;
    bool altitudes = false;
    bool pseudoaltitudes = false;
    bool omega = false;
    bool brocard = false;

    void recompute();
    void hyperbolic();
    double area() const;
    void draw() const;
    void draw_body() const;

    Orientation get_euclidean_orientation() const;

    Cycle cycle(double x, double y, double z) const;
    std::vector<Cycle> get_sides() const;
    std::vector<Cycle> get_bisectors() const;
    std::vector<Cycle> get_cevians(const Point& X, const Point& Y, const Point& Z) const;
    std::vector<Cycle> get_cevians(const Point& P) const;
    std::vector<Cycle> get_altitudes() const;

    Cycle aa;
    Cycle bb;
    Cycle cc;

    Point Ha;
    Point Hb;
    Point Hc;

    Cycle omega_a;
    Cycle omega_b;
    Cycle omega_c;

    Cycle inc;

    double a0;
    double b0;
    double c0;

    double alpha0;
    double beta0;
    double gamma0;

};


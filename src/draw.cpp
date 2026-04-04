#include <GL/glut.h>

#include "geometry.h"

#include <iostream>

const double eps = 0.0001;
double point_rad = 5;
int angle_step = 5;
const int n_segments = 150;

int sx = 800;
int sy = 600;

const double s = 0.015;
const double fx = 0;//10.0;
const double fy = 0;//-10.0;


void write_cycle(const Point& A, const Point& B, const Cycle& X) {
	std::cout << "\\draw ";
	std::cout.precision(6);
 	std::cout << "(" << std::fixed << fx + s * A.x << "," << fy + s * A.y << ") arc (";

 	double R = fabs(X.R); 	
 	double f1 = atan2(A.y - X.O.y, A.x - X.O.x) / M_PI * 180;
 	double f2 = atan2(B.y - X.O.y, B.x - X.O.x) / M_PI * 180;
 	
    std::cout << f1 << ":" << f2 << ":" << s * R << ");" << std::endl;
}

void write_cycle(const Point& A, const Cycle& X, double degs) {
	std::cout << "\\draw ";
	std::cout.precision(6);
 	std::cout << "(" << std::fixed << fx + s * A.x << "," << fy + s * A.y << ") arc (";

 	double R = fabs(X.R); 	
 	double f1 = atan2(A.y - X.O.y, A.x - X.O.x) / M_PI * 180;
 	double f2 = f1 + degs;
 	
    std::cout << f1 << ":" << f2 << ":" << s * R << ");" << std::endl;
}

void write_cycle(const Cycle& X, double f1, double f2) {
	std::cout << "\\draw ";
	std::cout.precision(6);
	
 	double R = fabs(X.R);
	Vector v = {cos(f1 / 180 * M_PI), sin(f1 / 180 * M_PI)};
	Point A = X.O + R * v;
	
 	std::cout << "(" << std::fixed << fx + s * A.x << "," << fy + s * A.y << ") arc (";
    std::cout << f1 << ":" << f2 << ":" << s * R << ");" << std::endl;
}


void write_point(const Point& A) {
    std::cout << "\\node [vertex] at (" << fx + s * A.x << "," << fy + s * A.y <<") {};" << std::endl;
}

void line(const Point& A, const Point& B) {
    glBegin(GL_LINE_LOOP);
    glVertex3d(A.x, A.y, 0);
    glVertex3d(B.x, B.y, 0);    
	glEnd();
}

void circle(const Point& A, double r) {
	static float angle_theta;
    glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 2 * n_segments; i++) {
		angle_theta = i * M_PI / n_segments;
		glVertex3d(A.x + r * cos(angle_theta), A.y + r * sin(angle_theta), 0);
	}
	glEnd();
}

void Vector::draw(const Point& X) const {
    glColor3d(0.5, 0.0, 0.0);
    line(X, X + 50 * *this);
}

void Cycle::draw() const {
    if (fabs(R) < 1e5) {
        circle(O, R);
    } else {
        glBegin(GL_LINE_STRIP);
        const double l = 5000;
        if (fabs(b) <= fabs(c)) {
            glVertex3d(0, - d / c, 0);
            glVertex3d(l, - (d + b * l) / c, 0);
        } else {
            glVertex3d(-d / b, 0, 0);
            glVertex3d((-d - c * l) / b, l, 0);
        }
        glEnd();
    }
}

void arc(const Point& A, const Point& B, double alpha) {
    //draws a circle arc from A to B that has oriented angle alpha with the straight segment AB
    if (alpha < 0) {
        arc(B, A, - alpha);
        return;
    }    
    if (alpha > M_PI) {
        arc(B, A, 2 * M_PI - alpha);
        return;
    }
    glVertex3d(A.x, A.y, 0);
    if (M_PI - alpha < eps) {
        double d = dist(A, B);
        double L = 10000;
        double cx = (A.x - B.x) / d;
        double cy = (A.y - B.y) / d;
        
        double vx = A.x + L * cx;
        double vy = A.y + L * cy;
        
	    glVertex3d(vx, vy, 0);
	    
	    vx -= L * cy;
	    vy += L * cx;
	    
	    glVertex3d(vx, vy, 0);
	    
	    vx -= (2 * L + d) * cx;
	    vy -= (2 * L + d) * cy;
	    
	    glVertex3d(vx, vy, 0);
	    	     
	    vx += L * cy;
	    vy -= L * cx;
	    
	    glVertex3d(vx, vy, 0);
    } else if (fabs(alpha) >= eps) {
		double d = dist(A, B);
		double t = d / 2 / tan(alpha);
		double cs = cos(2 * alpha / n_segments);
		double sn = sin(2 * alpha / n_segments);    
		double cx = (A.x + B.x) / 2 + (B.y - A.y) / d * t;
		double cy = (A.y + B.y) / 2 + (A.x - B.x) / d * t;
		double vx = A.x - cx;
		double vy = A.y - cy;

		for (int i = 0; i < n_segments; i++) {
			double vxx = cs * vx + sn * vy;
			double vyy = -sn * vx + cs * vy;
			glVertex3d(cx + vxx, cy + vyy, 0);
			vx = vxx;
			vy = vyy;
		}    
    }
	glVertex3d(B.x, B.y, 0);
	return;
	
	std::cout << "\\draw ";
	std::cout.precision(6);
 	std::cout << "(" << std::fixed << fx + s * A.x << "," << fy + s * A.y << ") arc (";

    double d = dist(A, B); 	
 	double R = d / 2 / sin(alpha);
 	double t = d / 2 / tan(alpha);
    double cx = (A.x + B.x) / 2 + (B.y - A.y) / d * t;
    double cy = (A.y + B.y) / 2 + (A.x - B.x) / d * t; 	
 	
 	Point O{cx, cy};
 	
 	double f1 = atan2(A.y - O.y, A.x - O.x) / M_PI * 180;
 	double f2 = atan2(B.y - O.y, B.x - O.x) / M_PI * 180;
 	
 	if (f2 > f1) {
 	    f1 += 360;
 	}
 	
    std::cout << f1 << ":" << f2 << ":" << s * R << ");" << std::endl;
    std::cout << "\\node [vertex] at (" << fx + s * A.x << "," << fy + s * A.y <<") {};" << std::endl;
    std::cout << "\\node [vertex] at (" << fx + s * B.x << "," << fy + s * B.y <<") {};" << std::endl;
}

void Point::draw() const {
	static float angle_theta;
	glBegin(GL_POLYGON);
	circle(*this, point_rad);
	glEnd();    
}
    
void Digon::draw() const {
    //std::cout << alpha / 2 + orientation << "  " << alpha / 2 - orientation << std::endl;
    if (alpha / 2 + orientation >= M_PI) {
        glClearColor(0.7, 0.7, 0.7, 0.0);        
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3d(0.9, 0.9, 0.9);
        glBegin(GL_POLYGON);
        arc(A, B, alpha / 2 + orientation);
        glEnd();
        if (alpha / 2 - orientation >= 0) {
            glColor3d(0.7, 0.7, 0.7);
        }        
        glBegin(GL_POLYGON);
        arc(B, A, alpha / 2 - orientation);        
        glEnd();
    } else if (alpha / 2 - orientation >= M_PI) {
        glClearColor(0.7, 0.7, 0.7, 0.0);        
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3d(0.9, 0.9, 0.9);
        glBegin(GL_POLYGON);
        arc(B, A, alpha / 2 - orientation);
        glEnd(); 
        if (alpha / 2 + orientation >= 0) {
            glColor3d(0.7, 0.7, 0.7);
        }
        glBegin(GL_POLYGON);
        arc(A, B, alpha / 2 + orientation);        
        glEnd();
    } else if (alpha / 2 + orientation >= 0 && alpha / 2 - orientation >= 0) {
        glColor3d(0.7, 0.7, 0.7);
        glBegin(GL_POLYGON);
        arc(A, B, alpha / 2 + orientation);
        arc(B, A, alpha / 2 - orientation);
        glEnd();
	} else if (alpha / 2 + orientation >= 0 && alpha / 2 - orientation < 0) {
        glColor3d(0.7, 0.7, 0.7);
        glBegin(GL_POLYGON);
        arc(A, B, alpha / 2 + orientation);
        glEnd();
        glColor3d(0.9, 0.9, 0.9);
        glBegin(GL_POLYGON);
        arc(B, A, alpha / 2 - orientation);        
        glEnd();
	} else if (alpha / 2 + orientation < 0 && alpha / 2 - orientation >= 0) {
        glColor3d(0.7, 0.7, 0.7);
        glBegin(GL_POLYGON);
        arc(B, A, alpha / 2 - orientation);
        glEnd();
        glColor3d(0.9, 0.9, 0.9);
        glBegin(GL_POLYGON);
        arc(A, B, alpha / 2 + orientation);        
        glEnd();
    }
    
    glColor3d(0.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
	arc(A, B, alpha / 2 + orientation);
	glEnd();
    glBegin(GL_LINE_STRIP);
	arc(B, A, alpha / 2 - orientation);
	glEnd();	
}

void Digon::draw_bisector() const {
    glColor3d(0.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
    arc(A, B, orientation);
    glEnd();
}

void Triangle::draw_body() const {
    // draw body
	auto euclid = get_euclidean_orientation();
	
	double outer_grey = orientation == ABC? 0.7 : 0.9;
	double inner_grey = orientation == ABC? 0.9 : 0.7;
	
	if (euclid == ABC) {
	    glClearColor(inner_grey, inner_grey, inner_grey, 0.0);
	    glClear(GL_COLOR_BUFFER_BIT);
	    if (a0 >= M_PI) {
	        glClearColor(outer_grey, outer_grey, outer_grey, 0.0);
	        glClear(GL_COLOR_BUFFER_BIT);
	        glColor3d(inner_grey, inner_grey, inner_grey);            
	        glBegin(GL_POLYGON);
	        arc(B, C, a0);
	        glEnd();
	    }
	    if (b0 >= M_PI) {
	        glClearColor(outer_grey, outer_grey, outer_grey, 0.0);
	        glClear(GL_COLOR_BUFFER_BIT);
	        glColor3d(inner_grey, inner_grey, inner_grey);            
	        glBegin(GL_POLYGON);
	        arc(C, A, b0);
	        glEnd();
	    }
	    if (c0 >= M_PI) {
	        glClearColor(outer_grey, outer_grey, outer_grey, 0.0);
	        glClear(GL_COLOR_BUFFER_BIT);      
	        glColor3d(inner_grey, inner_grey, inner_grey);            
	        glBegin(GL_POLYGON);
	        arc(A, B, c0);
	        glEnd();
	    }
	
	    glColor3d(outer_grey, outer_grey, outer_grey);
	    glBegin(GL_POLYGON);
	    arc(A, B, 0);
	    arc(B, C, 0);
	    arc(A, C, 0);
	    glEnd();
	    if (c0 < M_PI) {
	        glBegin(GL_POLYGON);
	        arc(A, B, c0);
	        glEnd();
	    }
	    if (a0 < M_PI) {
	        glBegin(GL_POLYGON);
	        arc(B, C, a0);
	        glEnd();
	    }
	    if (b0 < M_PI) {
	        glBegin(GL_POLYGON);
	        arc(C, A, b0);
	        glEnd();
	    }
	    glColor3d(inner_grey, inner_grey, inner_grey);
	    if (a0 < 0) {
	        glBegin(GL_POLYGON);
	        arc(B, C, a0);
	        glEnd();
	    }   
	    if (b0 < 0) {
	        glBegin(GL_POLYGON);
	        arc(C, A, b0);
	        glEnd();
	    }
	    if (c0 < 0) {
	        glBegin(GL_POLYGON);
	        arc(A, B, c0);
	        glEnd();
	    }                  
	} else {
	    glClearColor(outer_grey, outer_grey, outer_grey, 0.0);
	    glClear(GL_COLOR_BUFFER_BIT);	    
	    if (a0 >= M_PI) {
	        glClearColor(inner_grey, inner_grey, inner_grey, 0.0);
	        glClear(GL_COLOR_BUFFER_BIT);
	        glColor3d(outer_grey, outer_grey, outer_grey);
	  	    glBegin(GL_POLYGON);
	        arc(C, B, a0);
	        glEnd();
	    }
	    if (b0 >= M_PI) {
	        glClearColor(inner_grey, inner_grey, inner_grey, 0.0);
	        glClear(GL_COLOR_BUFFER_BIT);
	        glColor3d(outer_grey, outer_grey, outer_grey);
	        glBegin(GL_POLYGON);
	        arc(A, C, b0);
	        glEnd();
	    }
	    if (c0 >= M_PI) {
	        glClearColor(inner_grey, inner_grey, inner_grey, 0.0);
	        glClear(GL_COLOR_BUFFER_BIT);      
	        glColor3d(outer_grey, outer_grey, outer_grey);          
	        glBegin(GL_POLYGON);
	        arc(B, A, c0);
	        glEnd();
	    }
	
	    glColor3d(inner_grey, inner_grey, inner_grey);
	    glBegin(GL_POLYGON);
	    arc(A, B, 0);
	    arc(B, C, 0);
	    arc(A, C, 0);
	    glEnd();
	    
	   
	    if (c0 < M_PI) {
	        glBegin(GL_POLYGON);
	        arc(B, A, c0);
	        glEnd();
	    }
	    if (a0 < M_PI) {
	        glBegin(GL_POLYGON);
	        arc(C, B, a0);
	        glEnd();
	    }
	    if (b0 < M_PI) {
	        glBegin(GL_POLYGON);
	        arc(A, C, b0);
	        glEnd();
	    }
	    glColor3d(outer_grey, outer_grey, outer_grey);
	    if (a0 < 0) {
	        glBegin(GL_POLYGON);
	        arc(C, B, a0);
	        glEnd();
	    }   
	    if (b0 < 0) {
	        glBegin(GL_POLYGON);
	        arc(A, C, b0);
	        glEnd();
	    }
	    if (c0 < 0) {
	        glBegin(GL_POLYGON);
	        arc(B, A, c0);
	        glEnd();
	    } 
	}   
	// draw edges
	glColor3d(0.0, 0.0, 0.0);
	if (euclid > 0) {
	    glBegin(GL_LINE_STRIP);
	    arc(A, B, c0);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	    arc(B, C, a0);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	    arc(C, A, b0);
	    glEnd();
	} else {
	    glBegin(GL_LINE_STRIP);
	    arc(B, A, c0);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	    arc(C, B, a0);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	    arc(A, C, b0);
	    glEnd();
	}
}

void Triangle::draw() const {   
    draw_body();
}

void Triangle::draw_bisectors() const {
    const auto bs = get_bisectors();
    for (const auto& b: bs) {
        b.draw();
    }
}

void Triangle::draw_altitudes() const {
    const auto altitudes = get_altitudes();
    glColor3d(1.0, 0.5, 0.0);
    for (const auto& h : altitudes) {
         h.draw();             
    }
   
    if (hyperbolic && fabs(altitudes[0] * altitudes[1]) > 1) {
        Cycle hh = hperpendicular(altitudes[0], altitudes[2]);
        hh.draw();
    }
}  

void Triangle::draw_pseudoaltitudes() const {
    //Ha.draw();
    //Hb.draw();
    //Hc.draw();
    glColor3d(1.0, 0.0, 0.0);     
    const auto pseudoaltitudes = get_cevians(Ha, Hb, Hc);
    for (const auto& p : pseudoaltitudes) {
         p.draw();
    }
}

void Triangle::draw_circumcircle() const {
    Cycle circumcircle(A, B, C);
    glColor3d(0.0, 0.0, 1.0);
    circumcircle.draw();
}

void Triangle::draw_incircle() const {
    glColor3d(1.0, 0.5, 0.0);
    for (const Cycle& incircle : incircles) {
        incircle.draw();
    }
}

void Triangle::draw_excircles() const {
    glColor3d(1.0, 0.0, 0.0);
    for (const Cycle& excircle : excircles) {
        excircle.draw();
    }
    
    glLineWidth(1);
    glColor3d(0.0, 0.0, 0.0);
    aa.draw();
    bb.draw();
    cc.draw();
    glLineWidth(3);
}

void Triangle::draw_euler_circle() const {
    Cycle euler_circle(Ha, Hb, Hc);
    glColor3d(0.0, 0.7, 0.0);
    euler_circle.draw();
}

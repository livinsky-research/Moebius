#include <GL/glut.h>

#include "geometry.h"

#include <iostream>

const double eps = 0.0001;
double point_rad = 5;
int angle_step = 5;
const int n_segments = 150;

int sx = 800;
int sy = 600;


void line(const Point& A, const Point& B) {
    glBegin(GL_LINE_LOOP);
    glVertex3d(A.x, A.y, 0);
    glVertex3d(B.x, B.y, 0);    
	glEnd();
}

void circle(const Point& A, double r, int t = 1) {
	static float angle_theta;
    glBegin(GL_LINE_LOOP);
    for (int x = r + 1 - t; x <= r - 1 + t; ++x) {
		for (int i = 0; i < 2 * n_segments; i++) {
			angle_theta = i * M_PI / n_segments;
			glVertex3d(A.x + x * cos(angle_theta), A.y + x * sin(angle_theta), 0);
		}
	}
	glEnd();
}

void Cycle::draw() const {
    if (a == 1) {
        circle(O, R);
        if (R > 0) {            
            circle(O, R - 1);
        } else {
            circle(O, R + 1);
        }
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
	
	if (bisector) {
        glBegin(GL_LINE_STRIP);
        arc(A, B, orientation);
        glEnd();
	}	
}

void Triangle::draw_body() const {
    // draw body
	auto euclid = get_euclidean_orientation();
	
	double outer_grey = orientation == ABC? 0.7 : 0.9;
	double inner_grey = orientation == ABC? 0.9 : 0.7;
	
	if (euclid == ABC) {
	    glClearColor(inner_grey, inner_grey, inner_grey, 0.0);
	    glClear(GL_COLOR_BUFFER_BIT);
	    //std::cout << a0 << " - " << b0 << " - " << c0 << std::endl;
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
	    //std::cout << a0 << " - " << b0 << " - " << c0 << std::endl;
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
    int aa_side = -1;
    int bb_side = -1;
    int cc_side = -1;
    
    aa_side = aa.side(A);
    bb_side = bb.side(B);
    cc_side = cc.side(C);
    
    draw_body();
    
    if (bisectors) {
        const auto bs = get_bisectors();
        for (const auto b: bs) {
            b.draw();
        }
    }
    
    if (circumcircle) {
        Cycle circum_circle(A, B, C);
        glColor3d(0.0, 0.0, 1.0);
        circum_circle.draw();
    }
    
    Cycle euler_circle(Ha, Hb, Hc);
    
    if (pseudoaltitudes) {
        glColor3d(1.0, 0.0, 0.0);
        
        std::vector<Cycle> pseudoaltitudes = get_cevians(Ha, Hb, Hc);
        for (const Cycle& p : pseudoaltitudes) {
             p.draw();
        }
    }       

    if (omega) {
        glColor3d(1.0, 0.0, 0.0);
        
        omega_a.draw();
        omega_b.draw();
        omega_c.draw();
    }
    
    if (altitudes) {
        std::vector<Cycle> altitudes = get_altitudes();
        Ha.draw();
        Hb.draw();
        Hc.draw();
        
        glColor3d(1.0, 0.5, 0.0);
        for (const Cycle& h : altitudes) {
             h.draw();
        }
    }    
    
    if (incircle || excircles) {
        std::vector<Cycle> inexcircles = Apollonius(aa, bb, cc);
        glColor3d(1.0, 0.0, 0.0);
        
        //std::cout << orient << ": " << aa_side << bb_side << cc_side <<  std::endl;
        //std::cout << orient << ": " << inexcircles.size() << std::endl;
        
        std::vector<Cycle> ins;
        for (const Cycle& circle : inexcircles) {
            if (aa.side(circle.sample()) == aa_side && bb.side(circle.sample()) == bb_side && cc.side(circle.sample()) == cc_side) {
                ins.push_back(circle);
            }
        }
        
        std::vector<Cycle> exs;
        for (const Cycle& circle : inexcircles) {
            if (aa.side(circle.sample()) == aa_side && bb.side(circle.sample()) == bb_side && cc.side(circle.sample()) != cc_side) {
                exs.push_back(circle);
            }
            if (aa.side(circle.sample()) == aa_side && bb.side(circle.sample()) != bb_side && cc.side(circle.sample()) == cc_side) {
                exs.push_back(circle);
            }
            if (aa.side(circle.sample()) != aa_side && bb.side(circle.sample()) == bb_side && cc.side(circle.sample()) == cc_side) {
                exs.push_back(circle);
            }
        }        
        
        if (incircle) {
            glColor3d(1.0, 0.6, 0.0);        
            for (const Cycle& circle : ins) {
                circle.draw();
            }
        }
        if (excircles) {
            glColor3d(1.0, 0.0, 0.0);        
            for (const Cycle& circle : exs) {
                circle.draw();
            }

            glColor3d(0.0, 0.0, 0.0);        
            aa.draw();
            bb.draw();
            cc.draw();
        }
        
        /*if (euler) {
            std::vector<Cycle> euler_circles = Apollonius(exs[0], exs[1], exs[2]);
            glColor3d(0.0, 0.7, 0.0);
            for (const Cycle& circle : euler_circles) {
                circle.draw();
                break;
            }            
        }*/
    }
    
    if (euler) {
        glColor3d(0.0, 0.7, 0.0);
        euler_circle.draw();
    }

    if (brocard) {
        // c / b : a / c : b / a
        double x = sin(gamma) / sin(beta);
        double y = sin(alpha) / sin(gamma);
        double z = sin(beta) / sin(alpha);
        
        
        double lambda = y / x;
        double mu = z / y;
        double nu = x / z;
        
        Cycle bra = Split(aa, bb, lambda);
        Cycle brb = Split(bb, cc, mu);
        Cycle brc = Split(cc, aa, nu);
        
        glColor3d(0.0, 1.0, 0.0);              
        
        bra.draw();
        brb.draw();
        brc.draw();
    }

}


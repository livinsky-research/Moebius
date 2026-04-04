#include <string.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <GL/glui.h>
#include <GL/glut.h>

#include "geometry.h"


extern double point_rad;// = 5;
extern int sx; // = 800;
extern int sy; // = 600;

float lambdas[2]{1.0, 1.0};
const double lambda_step = 0.01;

double lambda = 1;
double mu = 1.0;
double nu = 1.0;

//float angles[3]{M_PI / 4, M_PI / 4, M_PI / 4};

std::vector<Point> points = {{300, 100}, {300, 500}, {600, 300}};
bool pressed[3] = {false, false, false};

int alpha_ = 90;
int beta_ = 90;
int gamma_ = 90;

Point& A = points[0];
Point& B = points[1];
Point& C = points[2];
Triangle T(A, B, C, M_PI / 2, M_PI / 2, M_PI / 2);
Point mouse;

int main_window;

extern int angle_step;

void alpha_trans(int x);
void beta_trans(int x);
void gamma_trans(int x);

/**************************************** myGlutKeyboard() **********/
void myGlutKeyboard(unsigned char Key, int x, int y) {
    switch(Key) {
        case 27:
        case 'q':
            exit(0);
        break;
    };
    glutPostRedisplay();
}


/***************************************** myGlutMenu() ***********/
void myGlutMenu(int value) {
    myGlutKeyboard(value, 0, 0);
}

/***************************************** myGlutMouse() **********/
void myGlutMouse(int button, int button_state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && button_state == GLUT_DOWN) {
        //Point Q = {mouse.x, sy - mouse.y};    
        for (int i = 0; i < points.size(); ++i) {
            if (dist(mouse, points[i]) <= point_rad) {
                pressed[i] = true;
            }       
        }
    }
    if (button == 3 && button_state == GLUT_DOWN) {
        if (dist(mouse, A) <= point_rad) {
            alpha_trans(angle_step);
        }
        if (dist(mouse, B) <= point_rad) {
            beta_trans(angle_step);
        }        
        if (dist(mouse, C) <= point_rad) {
            gamma_trans(angle_step);
        }
    }
    if (button == 4 && button_state == GLUT_DOWN) {
        if (dist(mouse, A) <= point_rad) {
            alpha_trans(-angle_step);
        }
        if (dist(mouse, B) <= point_rad) {
            beta_trans(-angle_step);
        }        
        if (dist(mouse, C) <= point_rad) {
            gamma_trans(-angle_step);
        }
    }    
}


/***************************************** myGlutMotion() **********/
void myGlutMotion(int x, int y) {
    for (int i = 0; i < points.size(); ++i) {
        if (pressed[i]) {
            points[i].x += x - mouse.x;
            points[i].y += sy - y - mouse.y;
            points[i].y = std::fmax(0, points[i].y);
        }
    }

    mouse.x = x;
    mouse.y = sy - y;

    glutPostRedisplay();
}

void myGlutPassiveMotion(int x, int y) {
    mouse.x = x;
    mouse.y = sy - y;
    
    for (int i = 0; i < points.size(); ++i) {
        pressed[i] = false;
    }

    glutPostRedisplay();
}

/**************************************** myGlutReshape() *************/
void myGlutReshape( int x, int y ) {
    glViewport(0, 0, x, y);
    
    mouse.y += y - sy;
    sx = x;
    sy = y; 
    
    glutPostRedisplay();
}

/***************************************** myGlutDisplay() *****************/

void myGlutDisplay() {    
    glClearColor( .9f, .9f, .9f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    // Given the coordinates
    //gluOrtho2D(0.0, 800.0, 0.0, 600.0);
    //gluOrtho2D(0.0, sx, 0.0, sy);
    gluOrtho2D(0, sx, 0, sy);    
    glViewport(0, 0, sx, sy);

    T.recompute();
    T.draw();
    
    const Cycle& aa = T.aa;
    const Cycle& bb = T.bb;
    const Cycle& cc = T.cc;     
    
    Cycle la = split(bb, cc, lambda);
    Cycle lb = split(cc, aa, mu);
    Cycle lc = split(aa, bb, nu);
    
    la.draw();
    lb.draw();
    lc.draw();
    
    for (const auto& P : la ^ lb) {
        //write_point(P);
    }   
    
    glColor3d(0.0, 0.7, 0.0);
    if (fabs(la * lb) < 1.0) {
        auto PP = la ^ lb;
        for (const Point& Q : PP) {
            Q.draw();        
        }
    } else {
		if (T.hyperbolic) {	    
		    Cycle nn = hperpendicular(la, lb);
		    nn.draw();
		}
    }
    for (const Point& P : points) {
        //Point Q = {mouse.x, sy - mouse.y};    
		if (dist(P, mouse) <= point_rad) {    
		    glColor3d(1.0, 0.3, 0.3);
		} else {
		    glColor3d(0.3, 0.3, 1.0);
		}
		P.draw();   
    }
    
    double cosa = -cos(bb, cc);
    double cosb = -cos(aa, cc);
    double cosc = -cos(aa, bb);
    
    double sina = sin(bb, cc);
    double sinb = sin(aa, cc);
    double sinc = sin(aa, bb);    
    
    double cosab = cosa*cosb - sina*sinb;
    double cosbc = cosb*cosc - sinb*sinc;
    double cosac = cosa*cosc - sina*sinc;

    double x = lambda * nu;
    double y = lambda;
    double z = 1;
    
    double A = x * y * sinc;
    double B = x * z * sinb;
    double C = y * z * sina;
    
    //double t = A*A + B*B + C*C + 2 *x*y*z*(x*cosa+y*cosb+z*cosc +x*cosb*cosc+y*cosa*cosc+z*cosa*cosb);
    
    auto sqr = [](double x) {return x*x;};
    
    double t = sqr(x*sina+y*sinb+z*sinc) + 2*x*y*(cosc + cosab) + 2*x*z*(cosb+cosac) + 2*y*z*(cosa+cosbc);
    double X = fabs(x) * sqrt(y*y+2*y*z*cosa+z*z);
    double Y = fabs(y) * sqrt(x*x+2*x*z*cosb+z*z);
    double Z = fabs(z) * sqrt(x*x+2*x*y*cosc+y*y);  
   
    //std::cout << X + Y - Z << std::endl;
    //std::cout << X - Y + Z << std::endl;
    //std::cout << -X + Y + Z << std::endl;
    
    for (const Point& P : points) {
        //Point Q = {mouse.x, sy - mouse.y};    
		if (dist(P, mouse) <= point_rad) {    
		    glColor3d(1.0, 0.3, 0.3);
		} else {
		    glColor3d(0.3, 0.3, 1.0);
		}
		P.draw();   
    }
    glColor3d(0.3, 0.3, 1.0);    
    
    glFlush();
    glutSwapBuffers();
}

void hyperbolic_cb(int control) {
    if (T.hyperbolic) {
        alpha_ = static_cast<int>(T.alpha / M_PI * 180);
        beta_ = static_cast<int>(T.beta / M_PI * 180);
        gamma_ = static_cast<int>(T.gamma / M_PI * 180);
    }
    T.hyperbolic = !T.hyperbolic;
    glutPostRedisplay();
}

void alpha_trans(int x) {
    alpha_ += x;
    
    int minn = std::max(std::max(0, beta_ + gamma_ - 540), std::abs(beta_ - gamma_) - 180);
    int maxx = std::min(std::min(360, beta_ + gamma_ + 180), -std::abs(beta_ - gamma_) + 540);
    if (alpha_ < minn) {
        alpha_ = minn;
    }    
    if (alpha_ > maxx) {
        alpha_ = maxx;
    }
    T.alpha = M_PI / 180 * alpha_;    
    glutPostRedisplay();
}

void beta_trans(int x) {
    beta_ += x;
    
    int minn = std::max(std::max(0, alpha_ + gamma_ - 540), std::abs(alpha_ - gamma_) - 180);
    int maxx = std::min(std::min(360, alpha_ + gamma_ + 180), -std::abs(alpha_ - gamma_) + 540);    
    if (beta_ < minn) {
        beta_ = minn;
    }    
    if (beta_ > maxx) {
        beta_ = maxx;
    } 
    
    T.beta = M_PI / 180 * beta_;    
    glutPostRedisplay();
}

void gamma_trans(int x) {
    gamma_ += x;
    
    int minn = std::max(std::max(0, alpha_ + beta_ - 540), std::abs(alpha_ - beta_) - 180);
    int maxx = std::min(std::min(360, alpha_ + beta_ + 180), -std::abs(alpha_ - beta_) + 540);
    if (gamma_ < minn) {
        gamma_ = minn;
    }    
    if (gamma_ > maxx) {
        gamma_ = maxx;
    }
    T.gamma = M_PI / 180 * gamma_;    
    glutPostRedisplay();
}

void lambda_trans(int x) {
    lambda = lambda_step * static_cast<int>(lambdas[0]);
    nu = 1.0 / lambda / mu;
    glutPostRedisplay();
}

void mu_trans(int x) {
    mu = lambda_step * static_cast<int>(lambdas[1]);
    nu = 1.0 / lambda / mu;
    glutPostRedisplay();
}


/**************************************** main() ********************/

int main(int argc, char* argv[]) {
    /****************************************/
    /*   Initialize GLUT and create window  */
    /****************************************/

    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE /*| GLUT_DEPTH */);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(sx, sy);

    main_window = glutCreateWindow("Ceva's Theorem");
    glutDisplayFunc( myGlutDisplay );
    glutReshapeFunc( myGlutReshape );
    glutKeyboardFunc( myGlutKeyboard );
    glutMotionFunc( myGlutMotion );
    glutPassiveMotionFunc( myGlutPassiveMotion );    
    glutMouseFunc( myGlutMouse );

    /****************************************/
    /*          Enable z-buferring          */
    /****************************************/

    //glEnable(GL_DEPTH_TEST);

    /****************************************/
    /*         Here's the GLUI code         */
    /****************************************/

    GLUI *glui = GLUI_Master.create_glui( "GLUI", 0, 800, 50 ); /* name, flags, x, and y */
    
    new GLUI_Checkbox(glui, "Hyperbolic geometry", 0, 0, hyperbolic_cb);
    new GLUI_Translation(glui, "Lambda", GLUI_TRANSLATION_X, &lambdas[0], 0, lambda_trans); 
    new GLUI_Translation(glui, "Mu", GLUI_TRANSLATION_X, &lambdas[1], 0, mu_trans); 

    new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

    glui->set_main_gfx_window(main_window);

    /* We register the idle callback with GLUI, *not* with GLUT */
    //GLUI_Master.set_glutIdleFunc( myGlutIdle );
    GLUI_Master.set_glutIdleFunc( NULL );

    glLineWidth(3);
    glutMainLoop();

    return EXIT_SUCCESS;
}

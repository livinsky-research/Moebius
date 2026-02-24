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

float angles[3]{M_PI / 2, M_PI / 2, M_PI / 2};

std::vector<Point> points = {{300, 100}, {300, 500}, {600, 300}, {400, 300}};
bool pressed[4] = {false, false, false, false};

int alpha_ = 90;
int beta_ = 90;
int gamma_ = 90;

Point& A = points[0];
Point& B = points[1];
Point& C = points[2];
Point& P = points[3];

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
        }
    }
    
    //std::cout << x << "  " << y << std::endl;

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
    
    const auto sides = T.get_sides();
    const auto bisectors = T.get_bisectors();
    
    std::vector<Point> AA = sides[1] ^ sides[2];
    std::vector<Point> BB = sides[0] ^ sides[2];
    std::vector<Point> CC = sides[0] ^ sides[1];
    
    const Cycle& a = sides[0];
    const Cycle& b = sides[1];
    const Cycle& c = sides[2];
    
    Cycle ca(AA[0], P, AA[1]);
    Cycle cb(BB[0], P, BB[1]);
    Cycle cc(CC[0], P, CC[1]);
    
    Cycle sca = bisectors[0].inv(ca);
    Cycle scb = bisectors[1].inv(cb);
    Cycle scc = bisectors[2].inv(cc);
    
    T.draw();
    for (const Point& P : points) {
        //Point Q = {mouse.x, sy - mouse.y};    
		if (dist(P, mouse) <= point_rad) {    
		    glColor3d(1.0, 0.3, 0.3);
		} else {
		    glColor3d(0.3, 0.3, 1.0);
		}
		P.draw();   
    }
    
    ca.draw();
    cb.draw();
    cc.draw();
    
    glColor3d(0.0, 0.7, 0.0);    
    
    sca.draw();
    scb.draw();
    scc.draw();
    
    double prod = sin(b, ca) * sin(c, cb) * sin(a, cc) / sin(c, ca) / sin(a, cb) / sin(b, cc);
   
    std::cout << prod << std::endl;
    
    for (const Point& Q: sca ^ scb) {
        Q.draw();
    }
    
    glColor3d(0.3, 0.3, 1.0);
    
    glFlush();
    glutSwapBuffers();
}


void bisectors_cb(int control) {
    T.bisectors = !T.bisectors;
    glutPostRedisplay();
}

void circumcircle_cb(int control) {
    T.circumcircle = !T.circumcircle;
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


/**************************************** main() ********************/

int main(int argc, char* argv[]) {
    /****************************************/
    /*   Initialize GLUT and create window  */
    /****************************************/

    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE /*| GLUT_DEPTH */);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(sx, sy);

    main_window = glutCreateWindow("Moebius isogonal conjugation");
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
    
    new GLUI_Checkbox(glui, "Bisectors", 0, 0, bisectors_cb);
    new GLUI_Checkbox(glui, "Circumcircle", 0, 0, circumcircle_cb);
    new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

    glui->set_main_gfx_window(main_window);

    /* We register the idle callback with GLUI, *not* with GLUT */
    //GLUI_Master.set_glutIdleFunc( myGlutIdle );
    GLUI_Master.set_glutIdleFunc( NULL );

    glLineWidth(3);
    glutMainLoop();

    return EXIT_SUCCESS;
}

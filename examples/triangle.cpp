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

//float angles[3]{M_PI / 4, M_PI / 4, M_PI / 4};

std::vector<Point> points = {{300, 100}, {300, 500}, {600, 300}};
bool pressed[3] = {false, false, false};

int alpha_ = 45;
int beta_ = 45;
int gamma_ = 45;

bool bisectors = false;
bool altitudes = false;
bool pseudoaltitudes = false;
bool circumcircle = false;
bool circumcenter = false;
bool incircle = false;
bool excircles = false;
bool euler = false;

Point& A = points[0];
Point& B = points[1];
Point& C = points[2];
Triangle T(A, B, C, M_PI / 4, M_PI / 4, M_PI / 4);
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
 
    gluOrtho2D(0, sx, 0, sy);    
    glViewport(0, 0, sx, sy);

    T.recompute();
    T.draw();
    if (bisectors) {
        T.draw_bisectors();
    } 
    if (altitudes) {
        T.draw_altitudes();
    } 
    if (pseudoaltitudes) {
        T.draw_pseudoaltitudes();
    } 
    if (circumcircle) {
        T.draw_circumcircle();
    }
    if (circumcenter) {
        T.draw_circumcenter();
    }
    if (incircle) {
        T.draw_incircle();
    } 
    if (excircles) {
        T.draw_excircles();
    }
    if (euler) {
        T.draw_euler_circle();
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
    glColor3d(0.3, 0.3, 1.0);
    
    glFlush();
    glutSwapBuffers();
}


void bisectors_cb(int control) {
    bisectors = !bisectors;
    glutPostRedisplay();
}

void altitudes_cb(int control) {
    altitudes = !altitudes;
    glutPostRedisplay();
}

void pseudoaltitudes_cb(int control) {
    pseudoaltitudes = !pseudoaltitudes;
    glutPostRedisplay();
}

void circumcircle_cb(int control) {
    circumcircle = !circumcircle;
    glutPostRedisplay();
}

void circumcenter_cb(int control) {
    circumcenter = !circumcenter;
    glutPostRedisplay();
}

void incircle_cb(int control) {
    incircle = !incircle;
    glutPostRedisplay();
}

void excircles_cb(int control) {
    excircles = !excircles;
    glutPostRedisplay();
}

void euler_cb(int control) {
    euler = !euler;
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

    main_window = glutCreateWindow("Moebius Triangle");
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
    new GLUI_Checkbox(glui, "Altitudes", 0, 0, altitudes_cb);
    new GLUI_Checkbox(glui, "Pseudoaltitudes", 0, 0, pseudoaltitudes_cb); 
    new GLUI_Checkbox(glui, "Circumcircle", 0, 0, circumcircle_cb);
    new GLUI_Checkbox(glui, "Circumcenter", 0, 0, circumcenter_cb);
    new GLUI_Checkbox(glui, "Incircle", 0, 0, incircle_cb);
    new GLUI_Checkbox(glui, "Excircles", 0, 0, excircles_cb);
    new GLUI_Checkbox(glui, "Euler circle", 0, 0, euler_cb);

    new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

    glui->set_main_gfx_window(main_window);

    /* We register the idle callback with GLUI, *not* with GLUT */
    //GLUI_Master.set_glutIdleFunc( myGlutIdle );
    GLUI_Master.set_glutIdleFunc( NULL );

    glLineWidth(3);
    glutMainLoop();

    return EXIT_SUCCESS;
}

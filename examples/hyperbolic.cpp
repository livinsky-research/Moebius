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

int alpha_ = 60;
int beta_ = 60;
int gamma_ = 60;

bool bisectors = false;
bool altitudes = false;
bool medians = false;
bool pseudoaltitudes = false;
bool pseudomedians = false;
bool circumcircle = false;
bool circumcenter = false;
bool incircle = false;
bool excircles = false;
bool euler = false;

Point& A = points[0];
Point& B = points[1];
Point& C = points[2];
Triangle T(A, B, C, M_PI / 3, M_PI / 3, M_PI / 3, true);
Point mouse;

int main_window;

extern int angle_step;

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


double coss(const Cycle& X, const Cycle& Y) {
	double norm_x = sqrt(-4 * X.a * X.d + X.b * X.b + X.c * X.c);
	double norm_y = sqrt(-4 * Y.a * Y.d + Y.b * Y.b + Y.c * Y.c);

	return (2 * (X.a * Y.d + Y.a * X.d) - X.b * Y.b - X.c * Y.c) / norm_x / norm_y;
}

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
    if (medians) {
        T.draw_medians();
    }
    if (pseudoaltitudes) {
        T.draw_pseudoaltitudes();
    }
    if (pseudomedians) {
        T.draw_pseudomedians();
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

void medians_cb(int control) {
    medians = !medians;
    glutPostRedisplay();
}

void pseudoaltitudes_cb(int control) {
    pseudoaltitudes = !pseudoaltitudes;
    glutPostRedisplay();
}

void pseudomedians_cb(int control) {
    pseudomedians = !pseudomedians;
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

/**************************************** main() ********************/

int main(int argc, char* argv[]) {
    /****************************************/
    /*   Initialize GLUT and create window  */
    /****************************************/

    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE /*| GLUT_DEPTH */);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(sx, sy);

    main_window = glutCreateWindow("Hyperbolic Triangle");
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
    new GLUI_Checkbox(glui, "Medians", 0, 0, medians_cb);    
    new GLUI_Checkbox(glui, "Pseudoaltitudes", 0, 0, pseudoaltitudes_cb);
    new GLUI_Checkbox(glui, "Pseudomedians", 0, 0, pseudomedians_cb);     
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

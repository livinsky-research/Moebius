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

float angles[2]{0.0, 0.0};
bool bisector = false;

std::vector<Point> points = {{300, 400}, {600, 400}};
bool pressed[2] = {false, false};

Point& A = points[0];
Point& B = points[1];
Digon D(A, B, M_PI / 4);
Point mouse;

int main_window;

extern int angle_step;

void alpha_trans(int x);

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
            alpha_trans(angle_step);
        }
    }
    if (button == 4 && button_state == GLUT_DOWN) {
        if (dist(mouse, A) <= point_rad) {
            alpha_trans(-angle_step);
        }
        if (dist(mouse, B) <= point_rad) {
            alpha_trans(-angle_step);
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

    D.draw();
    if (bisector) {
        D.draw_bisector();
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

void bisector_cb(int control) {
    bisector = !bisector;
    glutPostRedisplay();
}

void alpha_trans(int x) {
    static int digon = 45;
    digon += x;
    if (digon <= 0) {
        digon = 1;
    }
    if (digon >= 360) {
        digon = 359;
    }
    D.alpha = M_PI / 180 * digon;
    glutPostRedisplay();
}

void orientation_trans(int control) {
    static int prev = 0;
    static int angle = 0;
    static int orient = 0;
    
    angle = static_cast<int>(angles[1]);
    orient += angle - prev;
    prev = angle;    
    if (orient >= 180) {
        orient = 179;
    }    
    if (orient <= -180) {
        orient = -179;
    }
    D.orientation = M_PI / 180 * orient;    
    
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

    main_window = glutCreateWindow("Moebius Digon");
    glutDisplayFunc( myGlutDisplay );
    glutReshapeFunc( myGlutReshape );
    glutKeyboardFunc( myGlutKeyboard );
    glutMotionFunc( myGlutMotion );
    glutPassiveMotionFunc( myGlutPassiveMotion );    
    glutMouseFunc( myGlutMouse );

    GLUI *glui = GLUI_Master.create_glui( "GLUI", 0, 800, 50 ); /* name, flags, x, and y */
    
    new GLUI_Checkbox(glui, "Bisector", 0, 0, bisector_cb);
    new GLUI_Translation(glui, "Digon position", GLUI_TRANSLATION_X, &angles[1], 0, orientation_trans);
    new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

    glui->set_main_gfx_window(main_window);

    /* We register the idle callback with GLUI, *not* with GLUT */
    GLUI_Master.set_glutIdleFunc( NULL );

    glLineWidth(3);
    glutMainLoop();

    return EXIT_SUCCESS;
}

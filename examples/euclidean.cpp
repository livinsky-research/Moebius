#include <string.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <GL/glui.h>
#include <GL/glut.h>

#include "geometry.h"


extern double point_rad;// = 5;
extern int angle_step;// = 5;
extern int sx; // = 800;
extern int sy; // = 600;

std::vector<Point> points = {{300, 100}, {300, 500}, {600, 300}};

bool pressed[3] = {false, false, false};

bool bisectors = false;
bool altitudes = false;
bool medians = false;
bool euler_line = false;
bool euler_circle = false;
bool circumcircle = false;
bool incircle = false;
bool excircles = false;
bool lemoine = false;
bool toricelli = false;
bool kiepert = false;
bool apollo = false;
bool braxis = false;

double alpha_;
double beta_;
double gamma_;
double a, b, c;
double ha, hb, hc;
double ra, rb, rc;
double sina, sinb, sinc;
double cosa, cosb, cosc;

double p;
double R, r;
double S;


Point& A = points[0];
Point& B = points[1];
Point& C = points[2];

Point mouse;

int main_window;

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
    
    glColor3d(0.0, 0.0, 0.0);    
    
    line(A, B);
    line(B, C);
    line(C, A);
    
    a = dist(B, C);
    b = dist(A, C);
    c = dist(A, B);
    
    p = (a + b + c) / 2;
    
    S = sqrt(fabs(p * (p - a) * (p - b) * (p - c)));
    
    r = S / p;
    R = a * b * c / 4 / S; 

    sina = a / 2 / R;
    sinb = b / 2 / R;
    sinc = c / 2 / R;

    cosa = (b*b + c*c - a*a) / 2 / b / c;    
    cosb = (a*a + c*c - b*b) / 2 / a / c;    
    cosc = (a*a + b*b - c*c) / 2 / a / b;

    alpha_ = atan2(sina, cosa);
    beta_ = atan2(sinb, cosb);
    gamma_ = atan2(sinc, cosc);  

    ha = 2 * S / a;
    hb = 2 * S / b;
    hc = 2 * S / c;

    ra = S / (p - a);
    rb = S / (p - b);
    rc = S / (p - c); 

    // draw body
    glColor3d(0.7, 0.7, 0.7);
    glBegin(GL_POLYGON);
    glVertex3d(A.x, A.y, 0);
   	glVertex3d(B.x, B.y, 0);
    glVertex3d(C.x, C.y, 0);
    glEnd();
    
    glColor3d(0.3, 0.3, 0.3);
    line(A, B);  
    line(B, C);  
    line(A, C);  

    if (euler_circle) {
        glColor3d(0.0, 0.7, 0.0);    
        Point O = Barycentric(A, B, C, a * cosa, b * cosb, c * cosc);
        Point H = Barycentric(A, B, C, (a*a + c*c - b*b) * (a*a + b*b - c*c), (b*b + c*c - a*a) * (a*a + b*b - c*c), (b*b + c*c - a*a) * (a*a + c*c - b*b));
        Point E = O * H;
        circle(E, R / 2);
    }
    if (euler_line) {
        glColor3d(0.0, 0.7, 0.0);    
        Point O = Barycentric(A, B, C, a * cosa, b * cosb, c * cosc);
        Point H = Barycentric(A, B, C, (a*a + c*c - b*b) * (a*a + b*b - c*c), (b*b + c*c - a*a) * (a*a + b*b - c*c), (b*b + c*c - a*a) * (a*a + c*c - b*b));
        
        Vector v = (H - O).normalize();
        line(O - 2000 * v, O + 2000 * v);
    }
    if (medians) {
        Point M = Barycentric(A, B, C, 1, 1, 1);
        Point Ma = B * C;
        Point Mb = A * C;
        Point Mc = A * B;
        
        glColor3d(0.0, 1.0, 0.0);
        
        line(A, Ma);
        line(B, Mb);
        line(C, Mc);
        
        M.draw();
        Ma.draw();
        Mb.draw();
        Mc.draw();
    }
    if (bisectors) {
        glColor3d(0.4, 0.0, 0.0);
            
        Point La = Barycentric(A, B, C, 0, b, c);
        Point Lb = Barycentric(A, B, C, a, 0, c);
        Point Lc = Barycentric(A, B, C, a, b, 0);
        Point I = Barycentric(A, B, C, a, b, c);
        
        line(A, La);
        line(B, Lb);
        line(C, Lc);

        I.draw();
        La.draw();
        Lb.draw();
        Lc.draw();
    }
    if (altitudes) {
        glColor3d(1.0, 0.5, 0.0);
            
        Point Ha = cosa == 0? A : Barycentric(A, B, C, 0, cosc * sinb, cosb * sinc);
        Point Hb = cosb == 0? B : Barycentric(A, B, C, cosc * sina, 0, cosa * sinc);
        Point Hc = cosc == 0? C : Barycentric(A, B, C, cosb * sina, cosa * sinb, 0);
        Point H = cosa==0? A: cosb == 0? B : cosc == 0? C: Barycentric(A, B, C, sina / cosa, sinb / cosb,  sinc / cosc);
        
        line(A, Ha);
        line(B, Hb);
        line(C, Hc);
        line(H, Ha);
        line(H, Hb);
        line(H, Hc);        

        H.draw();
        Ha.draw();
        Hb.draw();
        Hc.draw();    
    }
    if (incircle) {
        glColor3d(1.0, 0.5, 0.0);  
        Point I = Barycentric(A, B, C, a, b, c);
        I.draw();
        circle(I, r);
    }
    if (excircles) {
        glColor3d(0.2, 0.0, 0.0);    
        Point Ia = Barycentric(A, B, C, -a, b, c);
        Point Ib = Barycentric(A, B, C, a, -b, c);
        Point Ic = Barycentric(A, B, C, a, b, -c);
        Ia.draw();
        Ib.draw();
        Ic.draw();
        circle(Ia, ra);
        circle(Ib, rb);
        circle(Ic, rc);
    }    
    if (circumcircle) {
        glColor3d(0.0, 0.0, 1.0);
        Point O = Barycentric(A, B, C, a * cosa, b * cosb, c * cosc);
        O.draw();
        circle(O, R);
    }
    if (lemoine) {
        glColor3d(0.0, 0.4, 0.4);
        Point L = Barycentric(A, B, C, a*a, b*b, c*c);
        L.draw();
    }
    if (kiepert) {
        glColor3d(0.0, 0.3, 0.0);
        glBegin(GL_LINE_STRIP);
        double cot_omega = (a * a + b * b + c * c) / 4 / S;
        double phi1 = atan(-cot_omega - sqrt(cot_omega * cot_omega - 3));
        double phi2 = atan(-cot_omega + sqrt(cot_omega * cot_omega - 3));
        double eps = 1e-5;
        for (int i = 0; i < 1000; i++) {
            double theta = phi1 + (phi2 - phi1 - eps) / 1000 * i + eps;
            Point Ti = Barycentric(A, B, C, a / sin(alpha_ + theta), b / sin(beta_ + theta), c / sin(gamma_ + theta));
            glVertex3d(Ti.x, sy - Ti.y, 0);
        }
        glEnd();
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < 1000; i++) {
            double theta = phi2 + (M_PI + phi1 - phi2 - eps) / 1000 * i + eps;
            Point Ti = Barycentric(A, B, C, a / sin(alpha_ + theta), b / sin(beta_ + theta), c / sin(gamma_ + theta));
            glVertex3d(Ti.x, sy - Ti.y, 0);
        }
        glEnd();
    }
    if (toricelli) {
        glColor3d(0.0, 0.3, 0.0);
        Point T1 = Barycentric(A, B, C, a / sin(alpha_ + M_PI / 3), b / sin(beta_ + M_PI / 3), c / sin(gamma_ + M_PI / 3));
        Point T2 = Barycentric(A, B, C, a / sin(alpha_ - M_PI / 3), b / sin(beta_ - M_PI / 3), c / sin(gamma_ - M_PI / 3));
        T1.draw();
        T2.draw();
    }
    if (braxis) {
        glColor3d(0.8, 0.8, 0.0);
        glBegin(GL_LINE_LOOP);        
        for (int i = 0; i < 1000; i++) {
            double theta = M_PI / 1000 * i;
            Point Ai = Barycentric(A, B, C, a * sin(alpha_ + theta), b * sin(beta_ + theta), c * sin(gamma_ + theta));
            glVertex3d(Ai.x, sy - Ai.y, 0);
        }
        glEnd();
    }
    if (apollo) {
        glColor3d(0.8, 0.8, 0.0);
        Point A1 = Barycentric(A, B, C, a * sin(alpha_ + M_PI / 3), b * sin(beta_ + M_PI / 3), c * sin(gamma_ + M_PI / 3));
        Point A2 = Barycentric(A, B, C, a * sin(alpha_ - M_PI / 3), b * sin(beta_ - M_PI / 3), c * sin(gamma_ - M_PI / 3));
        A1.draw();
        A2.draw();
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

void medians_cb(int control) {
    medians = !medians;
    glutPostRedisplay();
}

void altitudes_cb(int control) {
    altitudes = !altitudes;
    glutPostRedisplay();
}

void circumcircle_cb(int control) {
    circumcircle = !circumcircle;
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

void euler_line_cb(int control) {
    euler_line = !euler_line;
    glutPostRedisplay();
}

void euler_circle_cb(int control) {
    euler_circle = !euler_circle;
    glutPostRedisplay();
}

void lemoine_cb(int control) {
    lemoine = !lemoine;
    glutPostRedisplay();
}

void toricelli_cb(int control) {
    toricelli = !toricelli;
    glutPostRedisplay();
}

void kiepert_cb(int control) {
    kiepert = !kiepert;
    glutPostRedisplay();
}

void apollonius_cb(int control) {
    apollo = !apollo;
    glutPostRedisplay();
}

void braxis_cb(int control) {
    braxis = !braxis;
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

    main_window = glutCreateWindow("Euclidean Triangle");
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
    
    new GLUI_Checkbox(glui, "Medians", 0, 0, medians_cb);    
    new GLUI_Checkbox(glui, "Bisectors", 0, 0, bisectors_cb);
    new GLUI_Checkbox(glui, "Altitudes", 0, 0, altitudes_cb);
    new GLUI_Checkbox(glui, "Circumcircle", 0, 0, circumcircle_cb);    
    new GLUI_Checkbox(glui, "Incircle", 0, 0, incircle_cb);
    new GLUI_Checkbox(glui, "Excircles", 0, 0, excircles_cb);
    new GLUI_Checkbox(glui, "Euler line", 0, 0, euler_line_cb);    
    new GLUI_Checkbox(glui, "Euler circle", 0, 0, euler_circle_cb);
    new GLUI_Checkbox(glui, "Lemoine point", 0, 0, lemoine_cb);   
    new GLUI_Checkbox(glui, "Toricelli (isogonic) points", 0, 0, toricelli_cb);
    new GLUI_Checkbox(glui, "Kiepert hyperbola", 0, 0, kiepert_cb);
    new GLUI_Checkbox(glui, "Apollonius (isodynamic) points", 0, 0, apollonius_cb);   
    new GLUI_Checkbox(glui, "Brocard axis", 0, 0, braxis_cb);

    new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

    glui->set_main_gfx_window(main_window);

    /* We register the idle callback with GLUI, *not* with GLUT */
    //GLUI_Master.set_glutIdleFunc( myGlutIdle );
    GLUI_Master.set_glutIdleFunc( NULL );

    glLineWidth(3);
    glutMainLoop();

    return EXIT_SUCCESS;
}

#include <stdlib.h>
#include <glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#define MAX 7
#define PI 3.14159265358979323846

// Anna Papadopoulou 3136


// Structure that defines a point with x,y,z coordinates
typedef struct
{
    GLfloat x, y, z;
} POINT;

// Structure that defines a curve comprised of 4 control points
typedef struct
{
    POINT P1, P2, P3, P4;
} CURVE;

// Storage for the control points
POINT cpts[MAX];
// 4 curves used in Bezier Surface
CURVE c1, c2, c3, c4;

// Control points for the bezier surface
POINT cpts_surface[16];
// Holds temporarily the locations of control points
POINT temp[MAX];
// Storage for the points calculated for the first Hermite Curve
POINT pointsCubic[100000];
// Storage for the points calculated for the second Hermite Curve
POINT pointsCubic2[100000];
// Storage for the points calculated for the Bezier Curve with 7 control points
POINT pointsBezier_7[100000];
// Storage for the points calculated for the Bezier Curve with 4 control points
POINT pointsBezier_4[100000];
// Storage for the points calculated for the Polynomial Curves with 4 control points
POINT pointsPolynomial[100000];

// Index of selected point
int selectedPoint = -1;

// Counter for the number of control points
int ncpts = 0;
// Counter for amount of points calculated for the first Hermite Curve
int num_cube_points = 0;
// Counter for amount of points calculated for the second Hermite Curve
int num_cube_points2 = 0;
// Counter for amount of points calculated for Bezier Curve
int num_bez_points = 0;
// Counter for amount of points calculated for Polynomial Curve
int num_pol_points = 0;

// Used while moving the camera
float camera_angle = 0;

// Mode chosen in the menu
int mode = 0;

// Width and height of window
int W = 500, H = 500;



// Blending functions for Hermite Cubic Curve
float H0(float u) {

    return  float(2 * u * u * u - 3 * u * u + 1);
}

float H1(float u) {

    return float((-2 * u * u * u + 3 * u * u));
}

float H2(float u) {

    return float((u * u * u - 2 * u * u + u));
}

float H3(float u) {

    return float(u * u * u - u * u);
}

// Bezier Blending functions for Bezier Curve with 7 control points
float B0(float t) {

    return  float(pow(1 - t, 6));
}

float B1(float t) {

    return float((6 * t * pow(1 - t, 5)));
}

float B2(float t) {

    return float(15 * pow(t, 2) * pow(1 - t, 4));
}

float B3(float t) {

    return float(20 * pow(t, 3) * pow(1 - t, 3));
}

float B4(float t) {

    return float(15 * pow(t, 4) * pow(1 - t, 2));
}

float B5(float t) {

    return float(6 * pow(t, 5) * (1 - t));
}

float B6(float t) {

    return float(pow(t, 6));
}

// Bezier Blending functions for Bezier Curve with 4 control points
float C0(float t) {

    return  float(pow(1 - t, 3));
}

float C1(float t) {

    return float(3 * t * (1 - t) * (1 - t));
}

float C2(float t) {

    return float(3 * t * t * (1 - t));
}

float C3(float t) {

    return float(pow(t, 3));
}

// Blending functions for Polynomial Interpolating Curve with 4 control points
// In order to return a float value for division we cast operands to floats
float A0(float t) {

    return  float(-float(9)/float(2) * (t-float(1)/float(3)) * (t-float(2)/float(3)) * (t-1));
}

float A1(float t) {

    return float(float(27)/float(2) * t * (t-float(2)/float(3)) * (t-1));
}

float A2(float t) {

    return float(-float(27)/float(2) * t * (t-float(1)/float(3)) * (t-1));
}

float A3(float t) {

    return float(float(9)/float(2) * t * (t-float(1)/float(3)) * (t-float(2)/float(3)));
}


// Returns the Euclidean distance between two points
GLfloat distanceSquared(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2) 
{
    GLfloat d;

    d = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2);

    return d;
}

// Computes the points of a Hermite Curve
void computeHermiteCubicCurve(int n) {
    // Defines the density of the curve
    int MAX_STEPS = 250;

    
    for (int j = 0; j <= MAX_STEPS; j++)
    {
        // If we are computing the first hermite curve
        if (n == 0) {

            float u = float(j) / float(MAX_STEPS);

            // Compute Qx, Qy, Qz according to the blending functions
            float Qx = H0(u) * cpts[n].x + H1(u) * cpts[n + 1].x + H2(u) * cpts[n + 2].x + H3(u) * cpts[n + 3].x;
            float Qy = H0(u) * cpts[n].y + H1(u) * cpts[n + 1].y + H2(u) * cpts[n + 2].y + H3(u) * cpts[n + 3].y;
            float Qz = H0(u) * cpts[n].z + H1(u) * cpts[n + 1].z + H2(u) * cpts[n + 2].z + H3(u) * cpts[n + 3].z;

            // Add point to the array
            pointsCubic[num_cube_points].x = Qx;
            pointsCubic[num_cube_points].y = Qy;
            pointsCubic[num_cube_points].z = Qz;
            // Increase the counter for the first curve's points
            num_cube_points++;
        }
        // If we are computing the points for the second curve
        else {

            float u = float(j) / float(MAX_STEPS);

            float Qx = H0(u) * cpts[n].x + H1(u) * cpts[n + 3].x + H2(u) * cpts[n + 4].x + H3(u) * cpts[n + 5].x;
            float Qy = H0(u) * cpts[n].y + H1(u) * cpts[n + 3].y + H2(u) * cpts[n + 4].y + H3(u) * cpts[n + 5].y;
            float Qz = H0(u) * cpts[n].z + H1(u) * cpts[n + 3].z + H2(u) * cpts[n + 4].z + H3(u) * cpts[n + 5].z;

            // Add point to the array for the second curve
            pointsCubic2[num_cube_points2].x = Qx;
            pointsCubic2[num_cube_points2].y = Qy;
            pointsCubic2[num_cube_points2].z = Qz;
            // Increase the counter for the second curve's points
            num_cube_points2++;
        }
    }

}

// Computes the points for a closed Bezier Curve, where the first and the final control point are the same
void computeClosedBezierCurve(int n) {

    int MAX_STEPS = 1000;

    for (int j = 0; j <= MAX_STEPS; j++)
    {

        float t = float(j) / float(MAX_STEPS - 1);

         // Compute Qx, Qy, Qz according to the blending functions
         float Qx = B0(t) * cpts[n].x + B1(t) * cpts[n + 1].x + B2(t) * cpts[n + 2].x + B3(t) * cpts[n + 3].x + B4(t) * cpts[n + 4].x + B5(t) * cpts[n + 5].x + B6(t) * cpts[n].x;
         float Qy = B0(t) * cpts[n].y + B1(t) * cpts[n + 1].y + B2(t) * cpts[n + 2].y + B3(t) * cpts[n + 3].y + B4(t) * cpts[n + 4].y + B5(t) * cpts[n + 5].y + B6(t) * cpts[n].y;
         float Qz = B0(t) * cpts[n].z + B1(t) * cpts[n + 1].z + B2(t) * cpts[n + 2].z + B3(t) * cpts[n + 3].z + B4(t) * cpts[n + 4].z + B5(t) * cpts[n + 5].z + B6(t) * cpts[n].z;

         // Add point to the array 
         pointsBezier_7[num_bez_points].x = Qx;
         pointsBezier_7[num_bez_points].y = Qy;
         pointsBezier_7[num_bez_points].z = Qz;
         // Increase the counter for the bezier curve's points
         num_bez_points++;
        

    }

}

// Computes the points for a Bezier Curve with four control points
void computeBezierCurve(POINT points[], int n) {

    int MAX_STEPS = 500;

    for (int j = 0; j <= MAX_STEPS; j++)
    {
           float t = float(j) / float(MAX_STEPS - 1);

           // Compute Qx, Qy, Qz according to the blending functions
           float Qx = C0(t) * cpts[n].x + C1(t) * cpts[n+1].x + C2(t) * cpts[n+2].x + C3(t) * cpts[n+3].x;
           float Qy = C0(t) * cpts[n].y + C1(t) * cpts[n+1].y + C2(t) * cpts[n+2].y + C3(t) * cpts[n+3].y;
           float Qz = C0(t) * cpts[n].z + C1(t) * cpts[n+1].z + C2(t) * cpts[n+2].z + C3(t) * cpts[n+3].z;

           // Add point to the array 
           points[num_bez_points].x = Qx;
           points[num_bez_points].y = Qy;
           points[num_bez_points].z = Qz;

           // Increase the counter for the bezier curve's points
           num_bez_points++;
        
    }

}

// Computes the points for two Interpolating Polynomial Curves with four control points each
void computePolynomialCurve(POINT points[], int n) {

    int MAX_STEPS = 1000;

    for (int j = 0; j <= MAX_STEPS; j++)
    {
        float t = float(j) / float(MAX_STEPS - 1);

        // Compute Qx, Qy, Qz according to the blending functions
        float Qx = A0(t) * cpts[n].x + A1(t) * cpts[n + 1].x + A2(t) * cpts[n + 2].x + A3(t) * cpts[n + 3].x;
        float Qy = A0(t) * cpts[n].y + A1(t) * cpts[n + 1].y + A2(t) * cpts[n + 2].y + A3(t) * cpts[n + 3].y;
        float Qz = A0(t) * cpts[n].z + A1(t) * cpts[n + 1].z + A2(t) * cpts[n + 2].z + A3(t) * cpts[n + 3].z;

        // Add point to the array 
        points[num_pol_points].x = Qx;
        points[num_pol_points].y = Qy;
        points[num_pol_points].z = Qz;

        // Increase the counter for the polynomial curve's points
        num_pol_points++;

    }

}



// Function that draws the tangents for the hermite curves
void DrawTangents(int n) {

    // Draw the tangents for the first curve
    if (n == 0)
    {
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(cpts[0].x, cpts[0].y);
        glVertex2f(cpts[2].x, cpts[2].y);
        glEnd();

        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(cpts[1].x, cpts[1].y);
        glVertex2f(cpts[3].x, cpts[3].y);
        glEnd();
    }
    // Then for the second
    else
    {
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(cpts[1].x, cpts[1].y);
        glVertex2f(cpts[5].x, cpts[5].y);
        glEnd();

        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINES);
        glVertex2f(cpts[4].x, cpts[4].y);
        glVertex2f(cpts[6].x, cpts[6].y);
        glEnd();
    }

}

// Function that draws a curve according to a number of points
void DrawCurve(POINT points[], int num_points) {

    glPointSize(1.5);
    glColor3f(1.0, 0.0, 0.0);

    int i;
    glBegin(GL_POINTS);
    for (i = 0 ; i < num_points; i++)
    {
        glVertex3d(points[i].x, points[i].y, points[i].z);
    }
    glEnd();
    
}


// Function that draws a bezier surface
void DrawBezierSurface()
{
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &c1.P1.x);
    glEnable(GL_MAP2_VERTEX_3);
    glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
    for (int j = 0; j <= 8; j++)
    {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 30; i++)
            glEvalCoord2f((GLfloat)i / 30.0, (GLfloat)j / 8.0);
        glEnd();
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 30; i++)
            glEvalCoord2f((GLfloat)j / 8.0, (GLfloat)i / 30.0);
        glEnd();
    }
}

// Function that detects the closest point near the mouse
int closestPointTo(GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat d, min;
    int index;

    index = -1;

    if (ncpts > 0) {
        // define the distance from first control point as zero
        min = distanceSquared(cpts[0].x, cpts[0].y, cpts[0].z, x, y, z);
        index = 0;

        for (int i = 1; i < ncpts; i++)
        {
            // find euclidean distance between mouse and each point
            d = distanceSquared(cpts[i].x, cpts[i].y, cpts[i].z, x, y, z);

            // If it's lesser than the distance from the first point, replace it as the minimum distance
            if (d < min)
            {
                index = i;
                min = d;
            }
        }
    }
    // return the index of the control point chosen 
    return index;
}


// Function that defines the mouse drag motion
void motion(int x, int y)
{
    GLfloat wx, wy;

    // X,y via regularisation
    wx = (2.0 * x) / (float)(W - 1) - 1.0;
    wy = (2.0 * (H - 1 - y)) / (float)(H - 1) - 1.0;

    if (mode == 0 || mode == 1 || mode == 4)
    {
        if (selectedPoint >= 0)
        {
            cpts[selectedPoint].x = wx;
            cpts[selectedPoint].y = wy;
        }
    }
    else if (mode == 2)
    {
        // Add the previous control point address to the new coordinates
        if (selectedPoint >= 0)
        {
            cpts[selectedPoint].x = temp[selectedPoint].x + wx;
            cpts[selectedPoint].y = temp[selectedPoint].y + wy;
        }

        // Selecting the third control point will move the previous and next control points the same distance
        if (selectedPoint == 3)
        {
            cpts[selectedPoint - 1].x = temp[selectedPoint - 1].x + wx;
            cpts[selectedPoint - 1].y = temp[selectedPoint - 1].y + wy;
            cpts[selectedPoint + 1].x = temp[selectedPoint + 1].x + wx;
            cpts[selectedPoint + 1].y = temp[selectedPoint + 1].y + wy;

        }

        // In order to have C1 continuity:
        // P(4) = 2P(3) - P(2)
        else if (selectedPoint == 2)
        {
            cpts[selectedPoint + 2].x = (2 * cpts[selectedPoint + 1].x) - cpts[selectedPoint].x;
            cpts[selectedPoint + 2].y = (2 * cpts[selectedPoint + 1].y) - cpts[selectedPoint].y;
        }

        // In order to have C1 continuity: 
        // P(2) = 2P(3) - P(4)
        else if (selectedPoint == 4)
        {
            cpts[selectedPoint - 2].x = (2 * cpts[selectedPoint - 1].x) - cpts[selectedPoint].x;
            cpts[selectedPoint - 2].y = (2 * cpts[selectedPoint - 1].y) - cpts[selectedPoint].y;
        }

    }
    else if (mode == 3)
        return;

    glutPostRedisplay();
}


// Function for mouse clicks
static void mouse(int button, int state, int x, int y)
{
    float wx, wy;

    // Normalize x,y coordinates according to width and height of window
    wx = (2.0 * x) / (float)(W - 1) - 1.0;
    wy = (2.0 * (H - 1 - y)) / (float)(H - 1) - 1.0;
   
    // If right button is clicked
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        if (mode == 3)
            return;

        // Judge whether the number of control points exceed the maximum value we defined
        if (ncpts == MAX)
            return;

        // If menu choice is a closed curve, stop at 6 points
        if (mode == 1 && ncpts == 6)
            return;

      
        // Store a control point
        cpts[ncpts].x = wx;
        cpts[ncpts].y = wy;
        cpts[ncpts].z = 0;

        // Increase the number of control points
        ncpts++;

        glFlush();

        glutPostRedisplay();

    }
    // If left button is clicked
    else if (button == GLUT_LEFT_BUTTON)
    {
        // If it is pressed
        if (state == GLUT_DOWN) 
        {
            // Find the closest point to the mouse
            selectedPoint = closestPointTo(wx, wy, 0.0);
            // Store in a temporary struct the locations of all control points each time a new control point is selected
            for (int i = 0; i < 7; i++)
            {

                temp[i].x = cpts[i].x - wx;
                temp[i].y = cpts[i].y - wy;
                temp[i].z = cpts[i].z;

            }
        }
        else 
        {
            selectedPoint = -1;
            
        }
        glutPostRedisplay();

    }

}

// Sets the camera view to the default
void setViewNormal()
{

     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
     glMatrixMode(GL_MODELVIEW);
   
}

// Sets the camera view to the one specifically for the Bezier Curve
void setViewMode4()
{
    
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-30, 30, -30, 30, -550.0, 500.0);

      float angle_x = 70 * sin(camera_angle * PI / 180);
      float angle_z = -70 * cos(camera_angle * PI / 180);

      gluLookAt (0, 0, 0, angle_x, 0, -angle_z,  0, 1, 0); 
      glMatrixMode(GL_MODELVIEW);
    
}

// Function for special keys
void processSpecialKeys (int Key, int x, int y)
{
    if (mode == 3)
    {

        switch (Key)
        {
        // If left arrow is pressed
        case GLUT_KEY_LEFT: 
            // Turn left
            camera_angle = camera_angle - 5;
            break;
        // If right arrow is pressed
        case GLUT_KEY_RIGHT: 
            // Turn right
            camera_angle = camera_angle + 5;
            break;
        };
       
        setViewMode4();
        glutPostRedisplay();
    }
}


// Initialization
void myinit()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);
}


// Display function
void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0, 0.0, 0.0);

    glPointSize(5.0);

    // Draw all control points
    // For every other mode
    if (mode != 3) {
        glBegin(GL_POINTS);
        for (int i = 0; i < ncpts; i++)
            glVertex2f(cpts[i].x, cpts[i].y);
        glEnd();
    }
    // For mode 3
    else
    {
        glBegin(GL_POINTS);
        for (int i = 0; i < ncpts; i++)
            glVertex3f(cpts_surface[i].x, cpts_surface[i].y, cpts_surface[i].z);
        glEnd();
    }

    // Depending on the option picked at the menu
    switch (mode) {
    // Draw two hermite curves
    case 0:
        // First curve
        if (ncpts >= 4 && ncpts < 7)
        {
            computeHermiteCubicCurve(0);
            DrawCurve(pointsCubic, num_cube_points);
            DrawTangents(0);
            num_cube_points = 0;

        }
        // Second curve
        else if (ncpts == 7)
        {
            computeHermiteCubicCurve(0);
            DrawCurve(pointsCubic, num_cube_points);
            DrawTangents(0);

            computeHermiteCubicCurve(1);
            DrawCurve(pointsCubic2, num_cube_points2);
            DrawTangents(1);

            num_cube_points = 0;
            num_cube_points2 = 0;
        }
        break;
    // Draw a closed bezier curve
    case 1:
        if (ncpts == 6)
        {
            computeClosedBezierCurve(0);
            DrawCurve(pointsBezier_7, num_bez_points);

            num_bez_points = 0;
        }
        break;
    // Draw two bezier curves with C0/C1 continuity
    case 2:
        computeBezierCurve(pointsBezier_4, 0);
        computeBezierCurve(pointsBezier_4, 3);
        DrawCurve(pointsBezier_4, num_bez_points);

        num_bez_points = 0;
        break;
    // Draw a Bezier Surface
    case 3:
        DrawBezierSurface();
        break;
    // Draw two Polynomial Curves
    case 4:
        if (ncpts == 7)
        {
            computePolynomialCurve(pointsPolynomial, 0);
            computePolynomialCurve(pointsPolynomial, 3);
            DrawCurve(pointsPolynomial, num_pol_points);

            num_pol_points = 0;
        }
        break;
    }

    glFlush();

    glutSwapBuffers();

}

// Reshape function
void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, W, H); // adjust the view

    W = w;
    H = h;
}

// Menu 
void menu(int id)
{

    switch (id)
    {
    // Two Hermite Curves
    case 0:
        mode = 0;
        myinit();
        ncpts = 0;

        setViewNormal();
        glutPostRedisplay();
        break;
    // Closed Bezier Curve
    case 1:
        mode = 1;
        myinit();
        ncpts = 0;

        setViewNormal();
        glutPostRedisplay();
        break;
    // Two Bezier Curves
    // Includes the already defined control points
    case 2:
        mode = 2;
        myinit();
        ncpts = 7;

        cpts[0].x = -0.505271;
        cpts[0].y = 0;
        cpts[0].z = 0;

        cpts[1].x = -0.155070;
        cpts[1].y = 0.305230;
        cpts[1].z = 0;

        cpts[2].x = -0.303166;
        cpts[2].y = 0.352204;
        cpts[2].z = 0;

        cpts[3].x = 0;
        cpts[3].y = 0.352204;
        cpts[3].z = 0;

        cpts[4].x = 0.304188;
        cpts[4].y = 0.358597;
        cpts[4].z = 0;

        cpts[5].x = 0.158517;
        cpts[5].y = 0.306733;
        cpts[5].z = 0;

        cpts[6].x = 0.506413;
        cpts[6].y = 0;
        cpts[6].z = 0;

        setViewNormal();
        glutPostRedisplay();

        break;
    // Bezier Surface
    // Includes already defined control points (16 control points)
    case 3:
        mode = 3;
        myinit();
        ncpts = 16;

        cpts_surface[0].x = 0;
        cpts_surface[0].y = 10;
        cpts_surface[0].z = -5;

        cpts_surface[1].x = 2.5;
        cpts_surface[1].y = 10;
        cpts_surface[1].z = 0;

        cpts_surface[2].x = 2.5;
        cpts_surface[2].y = 10;
        cpts_surface[2].z = 5;

        cpts_surface[3].x = 0;
        cpts_surface[3].y = 10;
        cpts_surface[3].z = 10;

        cpts_surface[4].x = 0;
        cpts_surface[4].y = 5;
        cpts_surface[4].z = -5.5;

        cpts_surface[5].x = 5.0;
        cpts_surface[5].y = 5;
        cpts_surface[5].z = 0;

        cpts_surface[6].x = 5.0;
        cpts_surface[6].y = 5;
        cpts_surface[6].z = 5;

        cpts_surface[7].x = 0;
        cpts_surface[7].y = 5;
        cpts_surface[7].z = 10;

        cpts_surface[8].x = 0;
        cpts_surface[8].y = 0;
        cpts_surface[8].z = -5;

        cpts_surface[9].x = 5.0;
        cpts_surface[9].y = 0;
        cpts_surface[9].z = 0;

        cpts_surface[10].x = 5.0;
        cpts_surface[10].y = 0;
        cpts_surface[10].z = 5;

        cpts_surface[11].x = 0;
        cpts_surface[11].y = 0;
        cpts_surface[11].z = 10;

        cpts_surface[12].x = 0;
        cpts_surface[12].y = -5;
        cpts_surface[12].z = -5;

        cpts_surface[13].x = 0;
        cpts_surface[13].y = -5;
        cpts_surface[13].z = 0;

        cpts_surface[14].x = 0;
        cpts_surface[14].y = -5;
        cpts_surface[14].z = 5;

        cpts_surface[15].x = 0;
        cpts_surface[15].y = -5;
        cpts_surface[15].z = 10;

        // Add control points for curve 1
        c1.P1 = cpts_surface[0];
        c1.P2 = cpts_surface[1];
        c1.P3 = cpts_surface[2];
        c1.P4 = cpts_surface[3];

        // Add control points for curve 2
        c2.P1 = cpts_surface[4];
        c2.P2 = cpts_surface[5];
        c2.P3 = cpts_surface[6];
        c2.P4 = cpts_surface[7];

        // Add control points for curve 3
        c3.P1 = cpts_surface[8];
        c3.P2 = cpts_surface[9];
        c3.P3 = cpts_surface[10];
        c3.P4 = cpts_surface[11];

        // Add control points for curve 4
        c4.P1 = cpts_surface[12];
        c4.P2 = cpts_surface[13];
        c4.P3 = cpts_surface[14];
        c4.P4 = cpts_surface[15];

        setViewMode4();
        glutPostRedisplay();

        break;
     // Two Interpolating Polynomial Curves
    case 4:
        mode = 4;
        myinit();
        ncpts = 0;

        setViewNormal();
        glutPostRedisplay();
        break;
     // Termination
    case 5:
        exit(0);
        break;
    }
}




int main(int argc, char** argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(500, 200);
    glutCreateWindow("Bezier Curves");
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutSpecialFunc(processSpecialKeys);
    glutReshapeFunc(reshape);

    myinit();

    glutCreateMenu(menu);

    glutAddMenuEntry("Hermite Curves", 0);

    glutAddMenuEntry("Closed Bezier Curve", 1);

    glutAddMenuEntry("Two Bezier Curves", 2);

    glutAddMenuEntry("Bezier Surface", 3);

    glutAddMenuEntry("Two Cubic Polynomial Curves", 4);

    glutAddMenuEntry("Termination", 5);

    glutAttachMenu(GLUT_MIDDLE_BUTTON);


    glutMainLoop();

    return 0;
}


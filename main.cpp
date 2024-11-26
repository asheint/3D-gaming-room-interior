#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <SOIL2.h>
#include <iostream>
#include <vector>

constexpr float PI = 3.14159265358979323846;

std::vector<GLuint> textures;

GLboolean redFlag = true, fanSwitch = false, switchOne = false, switchTwo = false, switchLamp = false, amb1 = true, diff1 = true, spec1 = true, amb2 = true, diff2 = true, spec2 = true, amb3 = true, diff3 = true, spec3 = true;
double windowHeight = 800, windowWidth = 600;
double eyeX = 7.0, eyeY = 2.0, eyeZ = 15.0, refX = 0, refY = 0, refZ = 0;
double theta = 180.0, y = 1.36, z = 7.97888, a = 2;

int width;
int height;
unsigned char* image;
GLuint tex; //texture ID

//variables to move the camera
GLfloat camX = 0.0; GLfloat camY = 2.0; GLfloat camZ = 10.0; // Adjusted camera height and distance

//variables to move the scene
GLfloat sceRX = 0.0; GLfloat sceRY = 0.0; GLfloat sceRZ = 0.0;
GLfloat sceTX = 0.0; GLfloat sceTY = 0.0; GLfloat sceTZ = 0.0;

// Variable to control object rotation around the Y-axis
GLfloat objRY = 0.0;

// To on/off grids and axes
int gridOn = 0;
int axesOn = 0;

void drawGrid() {
    GLfloat step = 1.0f;
    GLint line;

    glBegin(GL_LINES);
    for (line = -20; line <= 20; line += step) {
        glVertex3f(line, -0.4, 20);
        glVertex3f(line, -0.4, -20);

        glVertex3f(20, -0.4, line);
        glVertex3f(-20, -0.4, line);
    }
    glEnd();
}

void drawAxes() {
    glBegin(GL_LINES);
    glLineWidth(1.5);

    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-200, 0, 0);
    glVertex3f(200, 0, 0);

    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, -200, 0);
    glVertex3f(0, 200, 0);

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, -200);
    glVertex3f(0, 0, 200);
    glEnd();
}

//Load textures
void loadTextures() {
    const char* textureFiles[] = {
        "floor.jpg", // Floor texture
        "wall.jpg", // Wall texture    
        "ceiling.jpg" // Ceiling texture
    };

    size_t numTextures = sizeof(textureFiles) / sizeof(textureFiles[0]);
    textures.resize(numTextures);

    for (size_t i = 0; i < numTextures; ++i) {
        image = SOIL_load_image(textureFiles[i], &width, &height, 0, SOIL_LOAD_RGB);
        if (image == NULL) {
            printf("Error loading texture %s: %s\n", textureFiles[i], SOIL_last_result());
            continue;
        }

        glGenTextures(1, &textures[i]);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        SOIL_free_image_data(image);

        printf("Loaded texture %s with ID %u\n", textureFiles[i], textures[i]);
    }
}



void init(void) {
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    loadTextures();
}

static GLfloat v_cube[8][3] =
{
    {0.0, 0.0, 0.0}, //0
    {0.0, 0.0, 3.0}, //1
    {3.0, 0.0, 3.0}, //2
    {3.0, 0.0, 0.0}, //3
    {0.0, 3.0, 0.0}, //4
    {0.0, 3.0, 3.0}, //5
    {3.0, 3.0, 3.0}, //6
    {3.0, 3.0, 0.0}  //7
};

static GLubyte quadIndices[6][4] =
{
    {0, 1, 2, 3}, //bottom
    {4, 5, 6, 7}, //top
    {5, 1, 2, 6}, //front
    {0, 4, 7, 3}, //back is clockwise
    {2, 3, 7, 6}, //right
    {1, 5, 4, 0}  //left is clockwise
};

static GLfloat colors[6][3] =
{
    {0.4, 0.1, 0.0}, //bottom
    {0.6, 0.0, 0.7}, //top
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 1.0},
    {0.8, 0.0, 0.0},
    {0.3, 0.6, 0.7}
};

void drawCube()
{
    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++)
    {
        glVertex3fv(&v_cube[quadIndices[i][0]][0]);
        glVertex3fv(&v_cube[quadIndices[i][1]][0]);
        glVertex3fv(&v_cube[quadIndices[i][2]][0]);
        glVertex3fv(&v_cube[quadIndices[i][3]][0]);
    }
    glEnd();
}

void base() {

    // right wall
    glColor3f(1, 0.8, 0.5);
    glPushMatrix();
    glTranslatef(-1.5, -1, .5);
    glScalef(5, 2, 0.1);
    drawCube();
    glPopMatrix();

    // left wall
    glColor3f(1, 0.8, 0.7);
    glPushMatrix();
    glTranslatef(-4.5, -1, 0);
    glScalef(1, 2, 5);
    drawCube();
    glPopMatrix();

    //ceiling
    glColor3f(1.0, 0.9, 0.8);
    glPushMatrix();
    //glScalef(5, 0.1, 7);
    glTranslatef(-2, 5.1, 0);
    glScalef(5, 0.1, 7);
    drawCube();
    glPopMatrix();

    // carpet
    glColor3f(0.4, 0.1, 0.0);
    glPushMatrix();
    //glScalef(5, 0.1, 7);
    glTranslatef(3, -0.2, 7);
    glScalef(1.3, 0.01, 1.7);
    drawCube();
    glPopMatrix();
}

void window() {

    //window white open
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(-0.9, 1, 8.9); //0.5,1,9.6
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, .6, .3);
    drawCube();
    glPopMatrix();

    //window side corner
    glColor3f(0.8, 0.6, 0.4);
    glPushMatrix();
    glTranslatef(-0.9, 1, 8.9);
    //glRotatef(22, 0,0,1);
    glScalef(0.04, 0.6, 0.0001);
    drawCube();
    glPopMatrix();

    //window left side corner
    glColor3f(0.8, 0.6, 0.4);
    glPushMatrix();
    glTranslatef(-0.9, 1, 9.8);
    glScalef(0.04, 0.6, 0.0001);
    drawCube();
    glPopMatrix();

    //window upper side corner
    glColor3f(0.7, 0.6, 0.5);
    glPushMatrix();
    glTranslatef(-0.7, 2.7, 8.9);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.05, 0.4);
    drawCube();
    glPopMatrix();


    //window lower side corner
    glColor3f(0.7, 0.6, 0.5);
    glPushMatrix();
    glTranslatef(-0.8, 1.02, 8.9);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.02, 0.34);
    drawCube();
    glPopMatrix();

    //window vertical bar 1
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(-0.87, 2.1, 8.9);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.02, 0.3);
    drawCube();
    glPopMatrix();

    //window vertical bar 2
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(-0.87, 1.6, 8.9);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.02, 0.3);
    drawCube();
    glPopMatrix();

    //window horizontal bar
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(-0.87, 1, 9.3);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.6, 0.02);
    drawCube();
    glPopMatrix();
}

void fan()
{
    glPushMatrix();
    glTranslatef(3, 4, 8);

    //stand
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(0.1, 0, 0.09);
    glScalef(0.01, 0.4, 0.01);
    //glTranslatef(-1.5,-1.5,-1.5);
    drawCube();
    glPopMatrix();

    //fan cube
    glColor3f(0.5, 0.2, 0.2);
    glPushMatrix();
    glScalef(0.1, 0.05, 0.1);
    glTranslatef(-1.5, -1.5, -1.5);
    drawCube();
    glPopMatrix();

    glPushMatrix();
    glRotatef(a, 0, 1, 0);

    //blade 1 blue
    glColor3f(0.8, 0.6, 0.4);
    glPushMatrix();
    //glRotatef(a, 0,1,0);
    glScalef(0.5, 0.01, 0.1);
    glTranslatef(-1.5, -1.5, -1.5);
    drawCube();
    glPopMatrix();

    //blade 2 purple
    glColor3f(0.8, 0.6, 0.4);
    glPushMatrix();
    //glRotatef(a, 0,1,0);
    glScalef(0.1, 0.01, 0.5);
    glTranslatef(-1.5, -1.5, -1.5);
    drawCube();
    glPopMatrix();

    glPopMatrix();
}

void lightBulb1() {
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
    GLfloat mat_diffuse[] = { 1.000, 0.843, 0.000, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat high_shininess[] = { 100.0 };
    GLfloat mat_emission[] = { 1.000, 1,1, 0.0 };

    glPushMatrix();
    glTranslatef(5, 5, 8);
    glScalef(0.2, 0.2, 0.2);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    if (switchOne == true) {
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    else {
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    }

    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}

void lightBulb2() {
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
    GLfloat mat_diffuse[] = { 1.000, 0.843, 0.000, 1.0 };
    GLfloat high_shininess[] = { 100.0 };
    GLfloat mat_emission[] = { 1,1,1, 1.0 };

    glPushMatrix();
    glTranslatef(0, 5, 8);
    glScalef(0.2, 0.2, 0.2);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    if (switchTwo == true) {
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    else {
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    }
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}

void lightBulb3() {
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
    GLfloat mat_diffuse[] = { 1.000, 0.843, 0.000, 1.0 };
    GLfloat high_shininess[] = { 100.0 };
    GLfloat mat_emission[] = { 1,1,1, 1.0 };

    glPushMatrix();
    glTranslatef(0.7, 1.5, 9.0);
    glScalef(0.2, 0.2, 0.2);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    if (switchLamp == true) {
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    else {
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    }
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}


void lightOne() {
    glPushMatrix();
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 5.0, 5.0, 8.0, 1.0 }; //5 5 10

    //glEnable( GL_LIGHT0);

    if (amb1 == true) { glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); }
    else { glLightfv(GL_LIGHT0, GL_AMBIENT, no_light); }

    if (diff1 == true) { glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); }
    else { glLightfv(GL_LIGHT0, GL_DIFFUSE, no_light); }

    if (spec1 == true) { glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); }
    else { glLightfv(GL_LIGHT0, GL_SPECULAR, no_light); }

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glPopMatrix();
}

void lightTwo() {
    glPushMatrix();
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 0.9, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 5.0, 8.0, 1.0 };

    //glEnable( GL_LIGHT1);

    if (amb2 == true) { glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient); }
    else { glLightfv(GL_LIGHT1, GL_AMBIENT, no_light); }

    if (diff2 == true) { glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse); }
    else { glLightfv(GL_LIGHT1, GL_DIFFUSE, no_light); }

    if (spec2 == true) { glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular); }
    else { glLightfv(GL_LIGHT1, GL_SPECULAR, no_light); }

    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glPopMatrix();
}

void lampLight() {
    glPushMatrix();
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.7, 1.5, 9.0, 1.0 };  //0.7, 4.5, 9.0

    //glEnable( GL_LIGHT2);

    if (amb3 == true) { glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient); }
    else { glLightfv(GL_LIGHT2, GL_AMBIENT, no_light); }

    if (diff3 == true) { glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse); }
    else { glLightfv(GL_LIGHT2, GL_DIFFUSE, no_light); }

    if (spec3 == true) { glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular); }
    else { glLightfv(GL_LIGHT2, GL_SPECULAR, no_light); }

    glLightfv(GL_LIGHT2, GL_POSITION, light_position);
    GLfloat spot_direction[] = { 0.3, -1, -0.8 };
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 35.0);
    glPopMatrix();
}


void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, windowWidth / windowHeight, 1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, refX, refY, refZ, 0, 1, 0);

    glEnable(GL_LIGHTING);
    lightOne();
    lightTwo();
    lampLight();
    //lamp();

    base();
    window();
    fan();

    lightBulb1();
    lightBulb2();
    //lightBulb3();
    glDisable(GL_LIGHTING);

    glFlush();
    glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
    // Prevent division by zero
    if (h == 0) h = 1;

    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);
    GLfloat aspect_ratio = (GLfloat)w / (GLfloat)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect_ratio, 1.0, 100.0);

    // Switch back to modelview matrix
    glMatrixMode(GL_MODELVIEW);
}

// Update the keyboardSpecial function to include boundary checks
void keyboardSpecial(int key, int x, int y) {
    if (key == GLUT_KEY_UP)
        camZ -= 1; // Move forward

    if (key == GLUT_KEY_DOWN)
        camZ += 1; // Move backward

    if (key == GLUT_KEY_RIGHT)
        camX += 1; // Move right

    if (key == GLUT_KEY_LEFT)
        camX -= 1; // Move left

    // Prevent camera from moving outside the room
    camX = std::max(-14.0f, std::min(camX, 14.0f));
    camY = std::max(-0.3f, std::min(camY, 5.0f)); // Assuming floor at -0.3 and ceiling at 5.0
    camZ = std::max(-14.0f, std::min(camZ, 14.0f));

    glutPostRedisplay();
}


void myKeyboardFunc(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w': // move eye point upwards along Y axis
        eyeY += 1.0;
        break;
    case 's': // move eye point downwards along Y axis
        eyeY -= 1.0;
        break;
    case 'a': // move eye point left along X axis
        eyeX -= 1.0;
        break;
    case 'd': // move eye point right along X axis
        eyeX += 1.0;
        break;
    case 'o':  //zoom out
        eyeZ += 1;
        break;
    case 'i': //zoom in
        eyeZ -= 1;
        break;
    case 'q': //back to default eye point and ref point
        eyeX = 7.0; eyeY = 2.0; eyeZ = 15.0;
        refX = 0.0; refY = 0.0; refZ = 0.0;
        break;
    case 'j': // move ref point upwards along Y axis
        refY += 1.0;
        break;
    case 'n': // move ref point downwards along Y axis
        refY -= 1.0;
        break;
    case 'b': // move ref point left along X axis
        refX -= 1.0;
        break;
    case 'm': // move eye point right along X axis
        refX += 1.0;
        break;
    case 'k':  //move ref point away from screen/ along z axis
        refZ += 1;
        break;
    case 'l': //move ref point towards screen/ along z axis
        refZ -= 1;
        break;
    case 'f': //turn on/off fan
        if (fanSwitch == false) {
            fanSwitch = true; break;
        }
        else {
            fanSwitch = false; break;
        }
    case '1': //to turn on and off light one
        if (switchOne == false)
        {
            switchOne = true; amb1 = true; diff1 = true; spec1 = true;
            glEnable(GL_LIGHT0); break;
        }
        else if (switchOne == true)
        {
            switchOne = false; amb1 = false; diff1 = false; spec1 = false; glDisable(GL_LIGHT0); break;
        }
    case '2': //to turn on and off light two
        if (switchTwo == false)
        {
            switchTwo = true; amb2 = true; diff2 = true; spec2 = true;
            glEnable(GL_LIGHT1); break;
        }
        else if (switchTwo == true)
        {
            switchTwo = false; amb2 = false; diff2 = false; spec2 = false;
            glDisable(GL_LIGHT1); break;
        }
    case '3': //to turn on and off light one
        if (switchLamp == false)
        {
            switchLamp = true; amb3 = true; diff3 = true; spec3 = true;
            glEnable(GL_LIGHT2); break;
        }
        else if (switchLamp == true)
        {
            switchLamp = false; amb3 = false; diff3 = false; spec3 = false;
            glDisable(GL_LIGHT2); break;
        }
    case'4': //turn on/off ambient light 1
        if (amb1 == false) { amb1 = true; break; }
        else { amb1 = false; break; }
    case'5':
        if (diff1 == false) { diff1 = true; break; }
        else { diff1 = false; break; }
    case'6':
        if (spec1 == false) { spec1 = true; break; }
        else { spec1 = false; break; }
    case'7': //turn on/off ambient light 2
        if (amb2 == false) { amb2 = true; break; }
        else { amb2 = false; break; }
    case'8':
        if (diff2 == false) { diff2 = true; break; }
        else { diff2 = false; break; }
    case'9':
        if (spec2 == false) { spec2 = true; break; }
        else { spec2 = false; break; }
    case'e': //turn on/off ambient lamp light
        if (amb3 == false) { amb3 = true; break; }
        else { amb3 = false; break; }
    case'r':
        if (diff3 == false) { diff3 = true; break; }
        else { diff3 = false; break; }
    case't':
        if (spec3 == false) { spec3 = true; break; }
        else { spec3 = false; break; }
    case 27:    // Escape key
        exit(1);
    }

    glutPostRedisplay();
}

void animate()
{
    if (redFlag)
    {
        theta += 2;
        z -= 0.02; //0.016667;
        if (theta >= 196 && theta <= 210)
        {
            y = 1.44;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.42;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.4;
        }
        else if (theta >= 164 && theta <= 178)
        {
            y = 1.42;
        }

        if (theta == 210)
        {
            redFlag = false;
        }
    }
    else
    {
        theta -= 2;
        z += 0.02;//0.016667;

        if (theta >= 196 && theta <= 210)
        {
            y = 1.44;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.42;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.4;
        }
        else if (theta >= 164 && theta <= 178)
        {
            y = 1.42;
        }

        if (theta == 150)
        {
            redFlag = true;
        }
    }

    if (fanSwitch) {
        a += 5;
        if (a > 360)
            a -= 360;
    }
    else {
        a = a;
    }

    glutPostRedisplay();

}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowHeight, windowWidth);
    glutCreateWindow("Gaming Room Interior");

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_NORMALIZE);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(myKeyboardFunc);
    //glutSpecialFunc(myKeyboardFunc);
    glutIdleFunc(animate);
    glutMainLoop();

    return 0;
}

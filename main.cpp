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
    {0, 4, 7, 3}, // back is clockwise
    {2, 3, 7, 6}, //right
    {1, 5, 4, 0}  //left is clockwise
};


static void getNormal3p
(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2 - x1;
    Uy = y2 - y1;
    Uz = z2 - z1;

    Vx = x3 - x1;
    Vy = y3 - y1;
    Vz = z3 - z1;

    Nx = Uy * Vz - Uz * Vy;
    Ny = Uz * Vx - Ux * Vz;
    Nz = Ux * Vy - Uy * Vx;

    glNormal3f(Nx, Ny, Nz);
}

void drawCube()
{
    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
            v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
            v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]);
        glVertex3fv(&v_cube[quadIndices[i][0]][0]);
        glVertex3fv(&v_cube[quadIndices[i][1]][0]);
        glVertex3fv(&v_cube[quadIndices[i][2]][0]);
        glVertex3fv(&v_cube[quadIndices[i][3]][0]);
    }
    glEnd();
}

void drawCube1(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX = 0, GLfloat ambY = 0, GLfloat ambZ = 0, GLfloat shine = 50)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0 };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glBegin(GL_QUADS);

    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
            v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
            v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]);
        glVertex3fv(&v_cube[quadIndices[i][0]][0]);
        glVertex3fv(&v_cube[quadIndices[i][1]][0]);
        glVertex3fv(&v_cube[quadIndices[i][2]][0]);
        glVertex3fv(&v_cube[quadIndices[i][3]][0]);
    }
    glEnd();
}

static GLfloat v_pyramid[5][3] =
{
    {0.0, 0.0, 0.0},
    {0.0, 0.0, 2.0},
    {2.0, 0.0, 2.0},
    {2.0, 0.0, 0.0},
    {1.0, 4.0, 1.0}
};

static GLubyte p_Indices[4][3] =
{
    {4, 1, 2},
    {4, 2, 3},
    {4, 3, 0},
    {4, 0, 1}
};

static GLubyte PquadIndices[1][4] =
{
    {0, 3, 2, 1}
};

void drawpyramid()
{
    glBegin(GL_TRIANGLES);
    for (GLint i = 0; i < 4; i++)
    {
        glVertex3fv(&v_pyramid[p_Indices[i][0]][0]);
        glVertex3fv(&v_pyramid[p_Indices[i][1]][0]);
        glVertex3fv(&v_pyramid[p_Indices[i][2]][0]);
    }
    glEnd();

    glBegin(GL_QUADS);
    for (GLint i = 0; i < 1; i++)
    {
        glVertex3fv(&v_pyramid[PquadIndices[i][0]][0]);
        glVertex3fv(&v_pyramid[PquadIndices[i][1]][0]);
        glVertex3fv(&v_pyramid[PquadIndices[i][2]][0]);
        glVertex3fv(&v_pyramid[PquadIndices[i][3]][0]);
    }
    glEnd();

}


void room()
{
    // carpet
    //glColor3f(0.4, 0.1, 0.0);
    glPushMatrix();
    glTranslatef(2, -0.8, 7);
    glScalef(1.3, 0.01, 1.7);
    drawCube1(0.4, 0.1, 0.0, 0.20, 0.05, 0.0);
    glPopMatrix();

    // front wall
    //glColor3f(1, 0.8, 0.5);
    glPushMatrix();
    glTranslatef(-1.5, -1, .5);
    glScalef(3.2, 2, 0.1);
    drawCube1(1, 0.8, 0.7, 0.5, 0.4, 0.25);
    glPopMatrix();

    // left wall
    //glColor3f(1, 0.8, 0.7);
    glPushMatrix();
    glTranslatef(-2, -1, 0);
    glScalef(0.2, 2, 5);
    drawCube1(1, 0.8, 0.7, 0.5, 0.4, 0.25);
    glPopMatrix();

    // right wall
    //glColor3f(1, 0.8, 0.7);
    glPushMatrix();
    glTranslatef(8, -1, 0);
    glScalef(0.2, 2, 5);
    drawCube1(1, 0.8, 0.7, 0.5, 0.4, 0.25);
    glPopMatrix();

    //ceiling
    //glColor3f(1.0, 0.9, 0.8);
    glPushMatrix();
    glTranslatef(-2, 5.1, 0);
    glScalef(3.54, 0.1, 6);
    drawCube1(1, 0.9, 0.8, 0.5, 0.45, 0.4);
    glPopMatrix();

    // floor
    glPushMatrix();
    glTranslatef(-2, -1.2, 0);
    glScalef(3.54, 0.1, 6);
    drawCube1(0.5, 0.1, 0.0, 0.25, 0.05, 0);
    glPopMatrix();
}


//Good for machine

//glColor3f(0.545, 0.271, 0.075);
//glPushMatrix();
//glTranslatef(0, -0.8, 2);
//glScalef(0.2, 0.7, 0.5);
//drawCube();
//glPopMatrix();

void computertable()
{
    // left body
    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(2, -0.8, 2);
    glScalef(0.4, 0.7, 0.5);
    drawCube();
    glPopMatrix();

    // up body
    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(2, 1.2, 2);
    glScalef(1, 0.05, 0.5);
    drawCube();
    glPopMatrix();

    // right body
    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(4.9, -0.8, 2);
    glScalef(0.02, 0.7, 0.5);
    drawCube();
    glPopMatrix();
}

void honeyCombCell()
{
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glRotatef(90, 0, 1, 0);
    glRotatef(-50, 1, 0, 0);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluCylinder(quad, .15, .15, .48, 30, 6);

    glPopMatrix();
}

void bed()
{
    //bed headboard
    glColor3f(0.5, 0.2, 0.2);
    glPushMatrix();
    glScalef(0.1, 0.5, 0.9);
    glTranslatef(-10.0, -1.6, 6);
    drawCube();
    glPopMatrix();

    //bed body
    glColor3f(0.824, 0.706, 0.549);
    glPushMatrix();
    glScalef(1, 0.2, 0.9);
    glTranslatef(-1, -4.0, 6.2);
    drawCube();
    glPopMatrix();

    //pillow right far
    glColor3f(0.627, 0.322, 0.176);
    glPushMatrix();
    glTranslatef(-0.5, -0.3, 6);
    glRotatef(20, 0, 0, 1);
    glScalef(0.1, 0.15, 0.28);
    drawCube();
    glPopMatrix();

    //pillow left near
    glColor3f(0.627, 0.322, 0.176);
    glPushMatrix();
    glTranslatef(-0.5, -0.3, 7.2);
    glRotatef(22, 0, 0, 1);
    glScalef(0.1, 0.15, 0.28);
    drawCube();
    glPopMatrix();

    //blanket
    glColor3f(0.627, 0.322, 0.176);
    glPushMatrix();
    glTranslatef(0.4, -0.3, 5.5);
    //glRotatef(22, 0,0,1);
    glScalef(0.5, 0.05, 0.95);
    drawCube();
    glPopMatrix();

    //blanket side left part
    glColor3f(0.627, 0.322, 0.176);
    glPushMatrix();
    glTranslatef(0.4, -0.9, 8.15);
    //glRotatef(22, 0,0,1);
    glScalef(0.5, 0.25, 0.05);
    drawCube();
    glPopMatrix();

}

void bedsideTable()
{
    //side drawer
    glColor3f(0.2, 0.1, 0.1); //0.5,0.2,0.2
    glPushMatrix();
    glTranslatef(-1.0, -0.9, 8.4); //0.5,-0.1,9
    //glRotatef(22, 0,0,1);
    glScalef(0.12, 0.2, 0.23);
    drawCube();
    glPopMatrix();

    //side drawer's drawer
    glColor3f(0.3, 0.2, 0.2);
    glPushMatrix();
    glTranslatef(-0.62, -0.8, 8.5);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.11, 0.18);
    drawCube();
    glPopMatrix();

    //side drawer's knob
    glColor3f(0.3, 0.1, 0.0);
    glPushMatrix();
    glTranslatef(-0.6, -0.7, 8.7);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.04, 0.04);
    drawCube();
    glPopMatrix();
}

void wallshelf()
{
    //Wall Shelf **********************************************

      //wall shelf one
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(1.5, 2.7, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.4, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //wall shelf two
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(1, 2.3, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.4, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //wall shelf three
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(0.5, 1.9, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.4, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //wall shelf four
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(1, 1.5, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.4, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //wall shelf five
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(1.5, 1.1, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.4, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //showpiece on the bottom shelf from left 1
    glColor3f(0.698, 0.133, 0.133);
    glPushMatrix();
    glTranslatef(1.5, 1.2, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.04, 0.06, 0.2);
    drawCube();
    glPopMatrix();

    //showpiece on the bottom shelf from left 2
    glColor3f(0.729, 0.333, 0.827);
    glPushMatrix();
    glTranslatef(2, 1.2, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.04, 0.06, 0.2);
    drawCube();
    glPopMatrix();

    //showpiece on the bottom shelf from left 3 lower portion
    glColor3f(0.098, 0.098, 0.439);
    glPushMatrix();
    glTranslatef(2.5, 1.2, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.04, 0.06, 0.2);
    drawCube();
    glPopMatrix();

    //showpiece on the bottom shelf from left 3 upper portion
    glColor3f(.529, 0.808, 0.980);
    glPushMatrix();
    glTranslatef(2.51, 1.35, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.01, 0.05, 0.2);
    drawCube();
    glPopMatrix();

    //showpiece on the top shelf  left 2
    glColor3f(0.502, 0.502, 0.000);
    glPushMatrix();
    glTranslatef(2.5, 2.71, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.05, 0.16, 0.01);
    drawCube();
    glPopMatrix();

    //showpiece on the top shelf left 1
    glColor3f(0, 0, .9);
    glPushMatrix();
    glTranslatef(1.8, 2.71, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.16, 0.1, 0.01);
    drawCube();
    glPopMatrix();

    //showpiece on 2nd shelf
    glColor3f(.416, 0.353, 0.804);
    glPushMatrix();
    glTranslatef(1.3, 2.4, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.16, 0.08, 0.01);
    drawCube();
    glPopMatrix();

    //showpiece on 3rd shelf left 1
    glColor3f(0.863, 0.078, 0.235);
    glPushMatrix();
    glTranslatef(0.4, 1.9, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.05, 0.16, 0.01);
    drawCube();
    glPopMatrix();

    //showpiece on 3rd shelf left 2
    glColor3f(0.780, 0.082, 0.522);
    glPushMatrix();
    glTranslatef(0.7, 1.9, 3);
    //glRotatef(22, 0,0,1);
    glScalef(0.05, 0.12, 0.01);
    drawCube();
    glPopMatrix();

    //showpiece on 3rd shelf left 3
    glColor3f(0.600, 0.196, 0.800);
    glPushMatrix();
    glTranslatef(1, 1.9, 3);
    glScalef(0.05, 0.09, 0.01);
    drawCube();
    glPopMatrix();

    //showpiece on 4th shelf
    glColor3f(0.282, 0.239, 0.545);
    glPushMatrix();
    glTranslatef(1.8, 1.5, 3);
    glScalef(0.2, 0.1, 0.2);
    drawpyramid();
    glPopMatrix();

    //showpiece on 4th shelf
    glColor3f(0.251, 0.878, 0.816);
    glPushMatrix();
    glTranslatef(1.4, 1.5, 3);
    glScalef(0.15, 0.1, 0.2);
    drawpyramid();
    glPopMatrix();



    // floor
    glColor3f(0.5, 0.1, 0.0);
    glPushMatrix();
    glScalef(5, 0.1, 7);
    glTranslatef(-1, -5, 0); //-1,-5,.5
    //glScalef(5, 0.1, 7);
    drawCube();
    glPopMatrix();
}

void window()
{
    //window white open
    glPushMatrix();
    glTranslatef(-1.3, 1, 8.9); //0.5,1,9.6
    glScalef(0.0001, .6, .3);
    drawCube1(1.0, 1.0, 1.0, 0.05, 0.05, 0.05);
    glPopMatrix();

    //window right side corner
    glPushMatrix();
    glTranslatef(-1.3, 1, 8.9);
    glScalef(0.04, 0.6, 0.0001);
    drawCube1(0.8, 0.6, 0.4, 0.4, 0.3, 0.2);
    glPopMatrix();

    //window left side corner
    glPushMatrix();
    glTranslatef(-1.3, 1, 9.8);
    glScalef(0.04, 0.6, 0.0001);
    drawCube1(0.8, 0.6, 0.4, 0.4, 0.3, 0.2);
    glPopMatrix();

    //window upper side corner
    glPushMatrix();
    glTranslatef(-1.1, 2.7, 8.9);
    glScalef(0.0001, 0.05, 0.4);
    drawCube1(0.7, 0.6, 0.5, 0.35, 0.3, 0.25);
    glPopMatrix();

    //window lower side corner
    glPushMatrix();
    glTranslatef(-1.2, 1.02, 8.9);
    glScalef(0.0001, 0.02, 0.34);
    drawCube1(0.7, 0.6, 0.5, 0.35, 0.3, 0.25);
    glPopMatrix();

    //window vertical bar 1
    glPushMatrix();
    glTranslatef(-1.27, 2.1, 8.9);
    glScalef(0.0001, 0.02, 0.3);
    drawCube1(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5);
    glPopMatrix();

    //window vertical bar 2
    glPushMatrix();
    glTranslatef(-1.27, 1.6, 8.9);
    glScalef(0.0001, 0.02, 0.3);
    drawCube1(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5);
    glPopMatrix();

    //window horizontal bar
    glPushMatrix();
    glTranslatef(-1.27, 1, 9.3);
    glScalef(0.0001, 0.6, 0.02);
    drawCube1(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5);
    glPopMatrix();
}

void fan()
{
    glPushMatrix();
    glTranslatef(3, 4, 8);

    //stand
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(0, 0, 0.09);
    glScalef(0.01, 0.4, 0.01);
    drawCube1(0.2, 0.1, 0.1, 0.2, 0.1, 0.1, 100);
    glPopMatrix();

    //fan cube
    glColor3f(0.5, 0.2, 0.2);
    glPushMatrix();
    glScalef(0.1, 0.05, 0.1);
    glTranslatef(-1.5, -1.5, -1.5);
    drawCube1(0.5, 0.2, 0.2, 0.5, 0.2, 0.2, 100);
    glPopMatrix();

    glPushMatrix();
    glRotatef(a, 0, 1, 0);

    //blade 1
    glColor3f(0.8, 0.6, 0.4);
    glPushMatrix();
    //glRotatef(a, 0,1,0);
    glScalef(0.5, 0.01, 0.1);
    glTranslatef(-1.5, -1.5, -1.5);
    drawCube1(0.8, 0.6, 0.4, 0.8, 0.6, 0.4, 100); 
    glPopMatrix();

    //blade 2
    glColor3f(0.8, 0.6, 0.4);
    glPushMatrix();
    //glRotatef(a, 0,1,0);
    glScalef(0.1, 0.01, 0.5);
    glTranslatef(-1.5, -1.5, -1.5);
    drawCube1(0.8, 0.6, 0.4, 0.8, 0.6, 0.4, 100); // Blade 2 with shininess
    glPopMatrix();

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
    glTranslatef(6, 5, 8);
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


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, refX, refY, refZ, 0, 1, 0); //7,2,15, 0,0,0, 0,1,0

    glEnable(GL_LIGHTING);
    lightOne();
    lightTwo();
    lampLight();
    room();
    fan();
    computertable();
    bed();
    window();
    bedsideTable();

    glPushMatrix();
    glTranslatef(-1.5,-0.8, -0.3);
    glPopMatrix();
    
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
            switchOne = true; amb1 = false; diff1 = true; spec1 = true;
            glEnable(GL_LIGHT0); break;
        }
        else if (switchOne == true)
        {
            switchOne = false; amb1 = false; diff1 = false; spec1 = false; glDisable(GL_LIGHT0); break;
        }
    case '2': //to turn on and off light two
        if (switchTwo == false)
        {
            switchTwo = true; amb2 = false; diff2 = true; spec2 = true;
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

void fullScreen(int w, int h)
{
    //Prevent a divide by zero, when window is too short;you cant make a window of zero width.
    if (h == 0)
        h = 1;
    float ratio = (GLfloat)w / (GLfloat)h;         //Calculate aspect ratio of the window

    //Set the perspective coordinate system
    glMatrixMode(GL_PROJECTION);                   //Use the Projection Matrix
    glLoadIdentity();                              //Reset Matrix

    glViewport(0, 0, w, h);                        //Set the viewport to be the entire window
    gluPerspective(60, ratio, 1, 500);             //Set the correct perspective.
    //glFrustum(-2.5,2.5,-2.5,2.5, ratio, 200);
    glMatrixMode(GL_MODELVIEW);                    //Get Back to the Modelview
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    std::cout << "To move Eye point:" << std::endl;
    std::cout << "w: up" << std::endl;
    std::cout << "s: down" << std::endl;
    std::cout << "a: left" << std::endl;
    std::cout << "d: right" << std::endl;
    std::cout << "i: zoom in" << std::endl;
    std::cout << "o: zoom out" << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "To move Camera point:" << std::endl;
    std::cout << "j: up" << std::endl;
    std::cout << "n: down" << std::endl;
    std::cout << "b: left" << std::endl;
    std::cout << "m: right" << std::endl;
    std::cout << "l: move nearer" << std::endl;
    std::cout << "k: move far" << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "Press q to move to default position" << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "f: to turn on/off fan     " << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "For lighting:      " << std::endl;
    std::cout << "Light source 1 [the light on the right on the screen      " << std::endl;
    std::cout << "1: to turn on/off light one     " << std::endl;
    std::cout << "4: to turn on/off ambient light one     " << std::endl;
    std::cout << "5: to turn on/off diffusion light one     " << std::endl;
    std::cout << "6: to turn on/off specular light one      " << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "Light source 2 [the light on the left on the screen " << std::endl;
    std::cout << "2: to turn on/off light two     " << std::endl;
    std::cout << "7: to turn on/off ambient light two     " << std::endl;
    std::cout << "8: to turn on/off diffusion light two     " << std::endl;
    std::cout << "9: to turn on/off specular light two      " << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "Lamp light (spot light)" << std::endl;
    std::cout << "3: to turn on/off lamp     " << std::endl;
    std::cout << "e: to turn on/off ambient lamp light    " << std::endl;
    std::cout << "r: to turn on/off diffusion lamp light      " << std::endl;
    std::cout << "t: to turn on/off specular lamp light      " << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "____________________" << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "      " << std::endl;

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowHeight, windowWidth);
    glutCreateWindow("Gaming Room Interior");

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glutReshapeFunc(fullScreen);
    glutDisplayFunc(display);
    glutKeyboardFunc(myKeyboardFunc);
    glutIdleFunc(animate);
    glutMainLoop();

    return 0;
}

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
GLfloat chairX = 4.0, chairY = -0.1, chairZ = 5.0;

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
    for (line = -20; line <= 20; line += static_cast<GLint>(step)) {
        glVertex3f(static_cast<GLfloat>(line), -0.4f, 20.0f);
        glVertex3f(static_cast<GLfloat>(line), -0.4f, -20.0f);

        glVertex3f(20.0f, -0.4f, static_cast<GLfloat>(line));
        glVertex3f(-20.0f, -0.4f, static_cast<GLfloat>(line));
    }
    glEnd();
}

void drawAxes() {
    glBegin(GL_LINES);
    glLineWidth(1.5f);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-200.0f, 0.0f, 0.0f);
    glVertex3f(200.0f, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -200.0f, 0.0f);
    glVertex3f(0.0f, 200.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -200.0f);
    glVertex3f(0.0f, 0.0f, 200.0f);
    glEnd();
}


// Number of textures
#define NUM_TEXTURES 12

// Global variables for texture IDs
GLuint textureIDs[NUM_TEXTURES];

// Paths to texture images
const char* textureFiles[NUM_TEXTURES] = {
    "Textures/floor.jpg", // 0
    "Textures/carpet.jpg", // 1
    "Textures/wallLR.jpg", // 2
    "Textures/ceilingW.jpeg", // 3
    "Textures/screenmc1.jpg", // 4
    "Textures/screenmc.jpg", // 5
    "Textures/screenmc2.jpg", // 6
    "Textures/pc.jpg", // 7
	"Textures/mousepad.jpg", // 8
	"Textures/keyboard.jpg", // 9
	"Textures/honeyCombGo.jpg", // 10
    "Textures/screen.jpeg", // 11
};

// Function to load textures
void loadTextures() {
    int width, height;
    unsigned char* image = nullptr;

    glGenTextures(NUM_TEXTURES, textureIDs); // Generate texture IDs

    for (int i = 0; i < NUM_TEXTURES; i++) {
        // Load image
        image = SOIL_load_image(textureFiles[i], &width, &height, 0, SOIL_LOAD_RGB);
        if (image == nullptr) {
            printf("Error loading texture %d: %s\n", i, SOIL_last_result());
            continue;
        }

        // Bind the texture
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        // Create the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Free the image memory
        SOIL_free_image_data(image);

        printf("Loaded texture %d from %s\n", i, textureFiles[i]);
    }

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
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


static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3) {
    GLfloat Ux = x2 - x1;
    GLfloat Uy = y2 - y1;
    GLfloat Uz = z2 - z1;

    GLfloat Vx = x3 - x1;
    GLfloat Vy = y3 - y1;
    GLfloat Vz = z3 - z1;

    GLfloat Nx = Uy * Vz - Uz * Vy;
    GLfloat Ny = Uz * Vx - Ux * Vz;
    GLfloat Nz = Ux * Vy - Uy * Vx;

    glNormal3f(Nx, Ny, Nz);
}


void drawCube() {
    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++) {
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


void drawCube1(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX = 0, GLfloat ambY = 0, GLfloat ambZ = 0, GLfloat shine = 50) {
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glBegin(GL_QUADS);

    for (GLint i = 0; i < 6; i++) {
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

void drawpyramid() {
    glBegin(GL_TRIANGLES);
    for (GLint i = 0; i < 4; i++) {
        glVertex3fv(&v_pyramid[p_Indices[i][0]][0]);
        glVertex3fv(&v_pyramid[p_Indices[i][1]][0]);
        glVertex3fv(&v_pyramid[p_Indices[i][2]][0]);
    }
    glEnd();

    glBegin(GL_QUADS);
    for (GLint i = 0; i < 1; i++) {
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
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0); // White
    glTranslatef(-0.6, -0.8, 2.0);
    glScalef(2.7, 0.01, 3.18);
    drawCube1(1.0, 1.0, 1.0, 1.0, 1.0, 1.0); // White
    glPopMatrix();

    // front wall
    glPushMatrix();
    glTranslatef(-1.5, -1, .5);
    glScalef(3.2, 2, 0.1);
    drawCube1(1, 0.8, 0.7, 0.5, 0.4, 0.25); // Original color
    glPopMatrix();

    // left wall
    glPushMatrix();
    glTranslatef(-2, -1, 0);
    glScalef(0.2, 2, 5);
    drawCube1(1, 0.8, 0.7, 0.5, 0.4, 0.25); // Original color
    glPopMatrix();

    // right wall
    glPushMatrix();
    glTranslatef(8, -1, 0);
    glScalef(0.2, 2, 5);
    drawCube1(1, 0.8, 0.7, 0.5, 0.4, 0.25); // Original color
    glPopMatrix();

    // ceiling
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0); // White
    glTranslatef(-2, 5.1, 0);
    glScalef(3.54, 0.1, 6);
    drawCube1(1.0, 1.0, 1.0, 1.0, 1.0, 1.0); // White
    glPopMatrix();

    // floor
    glPushMatrix();
    glColor3f(0.83, 0.83, 0.83); // Lighter gray
    glTranslatef(-2, -1.2, 0);
    glScalef(3.54, 0.1, 6);
    drawCube1(0.83, 0.83, 0.83, 0.83, 0.83, 0.83); // Lighter gray
    glPopMatrix();

    // ADDING TEXTURE LAYERS FROM INSIDE *************************

    // front wall with textures
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[10]); // Bind the texture for the front wall

    // Disable lighting temporarily
    //glDisable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(-1.5, -1, .9);
    glScalef(9.6, 6, 0.1);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // left wall
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[2]); // Bind the texture for the left wall

    glPushMatrix();
    glTranslatef(-1.3, -1, 0);
    glScalef(0.1, 6, 15);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // right wall
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[2]); // Bind the texture for the right wall

    glPushMatrix();
    glTranslatef(7.9, -1, 0);
    glScalef(0.1, 6, 15);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // carpet
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[1]); // Bind the texture for the carpet

    glPushMatrix();
    glTranslatef(-0.6, -0.7, 2.0);
    glScalef(8.1, 0.01, 9.72);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // ceiling
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[3]); // Bind the texture for the ceiling

    glPushMatrix();
    glTranslatef(-2, 5.0, 0);
    glScalef(10.62, 0.1, 18);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[0]); // Bind the texture for the floor

    glPushMatrix();
    glTranslatef(-2, -0.8, 0);
    glScalef(10.62, 0.1, 18);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
    glPopMatrix();

    // Re-enable lighting
    //glEnable(GL_LIGHTING);

    glDisable(GL_TEXTURE_2D);

    //***************************************************************************

    // PC Textures ****************************************************

    // mouse pad
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[11]); // Bind the texture for the mouse pad

    glPushMatrix();
    glTranslatef(3.00, 0.75, 2.7);
    glScalef(2.1, 0.01, 0.6);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // keyboard
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[9]); // Bind the texture for the keyboard

    glPushMatrix();
    glTranslatef(3.20, 0.8, 2.8);
    glScalef(0.9, 0.01, 0.3);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // monitor 1 screen
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[4]); // Bind the texture for the first screen
    glPushMatrix();
    glTranslatef(3.5, 1.0, 2.55);
    glRotatef(2, 1, 0, 0);
    glScalef(1.2, 0.9, 0.01);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // monitor 2 screen
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[5]); // Bind the texture for the second screen

    glPushMatrix();
    glTranslatef(2.25, 1.0, 2.6);
    glRotatef(2, 1, 0, 0);
    glRotatef(10, 0, 1, 0);
    glScalef(1.2, 0.9, 0.01);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // monitor 3 screen 
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[6]); // Bind the texture for the third screen

    glPushMatrix();
    glTranslatef(4.75, 1.0, 2.5);
    glRotatef(2, 1, 0, 0);
    glRotatef(-10, 0, 1, 0);
    glScalef(1.2, 0.9, 0.01);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // machine body
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[7]); // Bind the texture for the machine body

    glPushMatrix();
    glTranslatef(6.1, 0.62, 3.11);
    glScalef(0.45, 1.2, 0.9);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    //***************************************************************************
}

void computertable()
{
    // up body
    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(2.0, 0.6, 2);
    glScalef(1.65, 0.045, 0.5);
    drawCube();
    glPopMatrix();

    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(2.22, 0.45, 2.1);
    glScalef(1.52, 0.02, 0.4);
    drawCube();
    glPopMatrix();

    GLUquadric* quad = gluNewQuadric();

    // Legs
	glColor3f(0.545, 0.271, 0.075);
	glPushMatrix();
    glTranslatef(2.2, 0.7, 2.1);
	glRotatef(90, 1, 0, 0);
	glScalef(1.0, 1.0, 1.0);
    gluCylinder(quad, 0.05, 0.05, 1.5, 100, 100);
    glPopMatrix();

    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(6.8, 0.7, 2.1);
    glRotatef(90, 1, 0, 0);
    glScalef(1.0, 1.0, 1.0);
    gluCylinder(quad, 0.05, 0.05, 1.5, 100, 100);
    glPopMatrix();

    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(6.8, 0.7, 3.35);
    glRotatef(90, 1, 0, 0);
    glScalef(1.0, 1.0, 1.0);
    gluCylinder(quad, 0.05, 0.05, 1.5, 100, 100);
    glPopMatrix();

    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(2.2, 0.7, 3.35);
    glRotatef(90, 1, 0, 0);
    glScalef(1.0, 1.0, 1.0);
    gluCylinder(quad, 0.05, 0.05, 1.5, 100, 100);
    glPopMatrix();

    gluDeleteQuadric(quad);
}


void gamingChair()
{
    GLUquadric* quad = gluNewQuadric();

    // Chair leg bar
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(chairX, chairY+0.2, chairZ);
    glScalef(0.2, 0.2, 0.2);
    glRotatef(90, 1, 0, 0);
    gluCylinder(quad, 0.3, 0.3, 3, 100, 100);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(chairX, chairY - 0.4, chairZ - 0.3);
    glScalef(0.2, 0.2, 0.2);
    gluCylinder(quad, 0.3, 0.3, 3, 100, 100);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(chairX - 0.3, chairY - 0.4, chairZ);
    glRotatef(90, 0, 1, 0);
    glScalef(0.2, 0.2, 0.2);
    gluCylinder(quad, 0.3, 0.3, 3, 100, 100);
    glPopMatrix();

    // wheels
    glPushMatrix();
    glTranslatef(chairX - 0.3, chairY - 0.5, chairZ);
    glRotatef(90, 0, 1, 0);
    glScalef(0.4, 0.3, 0.3);
    glutSolidTorus(0.1, 0.2, 100, 100);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(chairX + 0.3, chairY - 0.5, chairZ);
    glRotatef(90, 0, 1, 0);
    glScalef(0.4, 0.3, 0.3);
    glutSolidTorus(0.1, 0.2, 100, 100);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(chairX, chairY - 0.5, chairZ + 0.3);
    glRotatef(90, 0, 1, 0);
    glScalef(0.4, 0.3, 0.3);
    glutSolidTorus(0.1, 0.2, 100, 100);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(chairX, chairY - 0.7, chairZ - 0.3);
    glRotatef(90, 0, 1, 0);
    glScalef(0.4, 0.3, 0.3);
    glutSolidTorus(0.1, 0.2, 100, 100);
    glPopMatrix();

    // Seat
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(chairX, chairY+0.2, chairZ);
    glRotatef(90, 1, 0, 0);
    glScalef(1.0, 1.2, 1.0);
    glutSolidTorus(0.1, 0.3, 100, 100);
    glTranslatef(0.0, 0.0, 0.0);
    gluDisk(quad, 0.0, 0.2, 100, 100);
    glPopMatrix();

    // back rest
    glPushMatrix();
    glTranslatef(chairX, chairY + 0.9, chairZ + 0.5);
    glScalef(0.2, 0.2, 0.2);
    glRotatef(90, 1, 0, 0);
    glRotatef(15, 1, 0, 0);
    gluCylinder(quad, 0.2, 0.3, 3, 100, 100);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(chairX, chairY + 1.3, chairZ + 0.52);
    glScalef(1.0, 1.5, 1.0);
    glRotatef(5, 1, 0, 0);
    glutSolidTorus(0.1, 0.3, 100, 100);
    gluDisk(quad, 0.0, 0.2, 100, 100);
    glPopMatrix();

    gluDeleteQuadric(quad);
}


void gamingPC() {
	// machine body
    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(6.1, 0.62, 2.2);
    glScalef(0.15, 0.4, 0.3);
    drawCube();
    glPopMatrix();

    // monitor 1
	// monitor 1 screen 
	glColor3f(0.545, 0.271, 0.075);
	glPushMatrix();
    glTranslatef(3.5, 1.0, 2.5);
	glRotatef(2, 1, 0, 0);
	glScalef(0.4, 0.3, 0.01);
	drawCube();
	glPopMatrix(); 

    // monitor 1 stand
    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(4.0, 0.7, 2.42);
    glScalef(0.05, 0.2, 0.02);
    drawCube();
    glPopMatrix();

    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(3.75, 0.75, 2.41);
    glScalef(0.2, 0.01, 0.06);
    drawCube();
    glPopMatrix();

    // monitor 2
    // monitor 2 screen 
    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(2.25, 1.0, 2.55);
    glRotatef(2, 1, 0, 0);
	glRotatef(10, 0, 1, 0);
    glScalef(0.4, 0.3, 0.01);
    drawCube();
    glPopMatrix();

    // monitor 2 stand
    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(2.75, 0.7, 2.43);
    glRotatef(10, 0, 1, 0);
    glScalef(0.05, 0.2, 0.02);
    drawCube();
    glPopMatrix();

    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(2.5, 0.75, 2.42);
    glRotatef(10, 0, 1, 0);
    glScalef(0.2, 0.01, 0.06);
    drawCube();
    glPopMatrix();

    // monitor 3
    // monitor 3 screen 
    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(4.75, 1.0, 2.45);
    glRotatef(2, 1, 0, 0);
    glRotatef(-10, 0, 1, 0);
    glScalef(0.4, 0.3, 0.01);
    drawCube();
    glPopMatrix();

    // monitor 3 stand
    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(5.25, 0.7, 2.33);
    glRotatef(-10, 0, 1, 0);
    glScalef(0.05, 0.2, 0.02);
    drawCube();
    glPopMatrix();

    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(5.0, 0.75, 2.32);
    glRotatef(-10, 0, 1, 0);
    glScalef(0.2, 0.01, 0.06);
    drawCube();
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

void lamp()
{

    //lamp base
    glColor3f(0, 0, 1);
    glPushMatrix();
    glTranslatef(-0.91, -0.28, 8.7);
    glScalef(0.07, 0.02, 0.07);
    drawCube();
    glPopMatrix();

    //lamp stand
    glColor3f(1, 0, 0);
    glPushMatrix();
    glTranslatef(-0.82, -0.2, 8.8);
    glScalef(0.01, 0.2, 0.01);
    drawCube();
    glPopMatrix();

    //lamp shade
    glColor3f(0.000, 0.000, 0.545);
    glPushMatrix();
    glTranslatef(-0.83, 0.4, 8.8);
    glScalef(0.08, 0.09, 0.08);
	glRotatef(90, 1, 0, 0);
	gluDisk(gluNewQuadric(), 0, 1, 20, 20);
	gluCylinder(gluNewQuadric(), 1, 2, 3, 20, 20); 
    //drawCube();
    glPopMatrix();

}

void Clock()
{
	glPopMatrix();
	glTranslatef(-0.3, 0.0, -3.5);

    //clock body
    glColor3f(0.545, 0.271, 0.075);
    glPushMatrix();
    glTranslatef(-0.9, 1.8, 7.87);
    //glRotatef(22, 0,0,1);
    glScalef(0.08, 0.25, 0.1);
    drawCube();
    glPopMatrix();

    //clock body white
    glColor3f(1.000, 0.894, 0.710);
    glPushMatrix();
    glTranslatef(-0.83, 1.9, 7.9);
    //glRotatef(22, 0,0,1);
    glScalef(0.06, 0.2, 0.08);
    drawCube();
    glPopMatrix();

    //clock hour handle
    glColor3f(0, 0, 0); //0.2,0.1,0.1
    glPushMatrix();
    glTranslatef(-0.65, 2.18, 8.01);
    glRotatef(45, 1, 0, 0);
    glScalef(0.0001, 0.01, 0.04);
    drawCube();
    glPopMatrix();

    //clock minute handle
    glColor3f(0, 0, 0); //0.2,0.1,0.1
    glPushMatrix();
    glTranslatef(-0.65, 2.18, 8.01);
    glRotatef(90, 1, 0, 0);
    glScalef(0.0001, 0.012, 0.08);
    drawCube();
    glPopMatrix();

    //clock body bottom strip
    glColor3f(0.2, 0.1, 0.1); //0.2,0.1,0.1
    glPushMatrix();
    glTranslatef(-0.66, 1.8, 7.89);
    //glRotatef(22, 0,0,1);
    glScalef(0.001, 0.01, 0.1);
    drawCube();
    glPopMatrix();

    //clock body right strip
    glColor3f(0.0, 0.0, 0.0); //0.2,0.1,0.1
    glPushMatrix();
    glTranslatef(-0.66, 1.8, 7.89);
    //glRotatef(22, 0,0,1);
    glScalef(0.005, 0.25, 0.01);
    drawCube();
    glPopMatrix();

    //clock body left strip
    glColor3f(0.2, 0.1, 0.1); //0.2,0.1,0.1
    glPushMatrix();
    glTranslatef(-0.65, 1.8, 8.2);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.25, 0.01);
    drawCube();
    glPopMatrix();

    //clock pendulum stick
    glColor3f(0.2, 0.1, 0.1); //0.2,0.1,0.1
    glPushMatrix();
    glTranslatef(-0.7, 2, 8.1);
    glRotatef(theta, 1, 0, 0);
    glScalef(0.0001, 0.2, 0.03);
    drawCube();
    glPopMatrix();

    //clock pendulum ball
    glColor3f(0.2, 0.1, 0.1); //0.2,0.1,0.1
    glPushMatrix();
    glTranslatef(-0.72, 1.42, z);
    //glRotatef(x, 1,0,0);
    glScalef(0.035, 0.035, 0.035);
    drawCube();
    glPopMatrix();

    //clock top pyramid
    glColor3f(0.5, 0.2, 0);
    glPushMatrix();
    glTranslatef(-0.9, 2.5, 7.87);
    //glRotatef(x, 1,0,0);
    glScalef(0.2, 0.1, 0.2);
    drawpyramid();
    glPopMatrix();

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

void windowOutside()
{
    //window white open
    glPushMatrix();
    glTranslatef(-2.3, 1, 8.9); //0.5,1,9.6
    glScalef(0.0001, .6, .3);
    drawCube1(1.0, 1.0, 1.0, 0.05, 0.05, 0.05);
    glPopMatrix();

    //window right side corner
    glPushMatrix();
    glTranslatef(-2.3, 1, 8.9);
    glScalef(0.04, 0.6, 0.0001);
    drawCube1(0.8, 0.6, 0.4, 0.4, 0.3, 0.2);
    glPopMatrix();

    //window left side corner
    glPushMatrix();
    glTranslatef(-2.3, 1, 9.8);
    glScalef(0.04, 0.6, 0.0001);
    drawCube1(0.8, 0.6, 0.4, 0.4, 0.3, 0.2);
    glPopMatrix();

    //window upper side corner
    glPushMatrix();
    glTranslatef(-2.1, 2.7, 8.9);
    glScalef(0.0001, 0.05, 0.4);
    drawCube1(0.7, 0.6, 0.5, 0.35, 0.3, 0.25);
    glPopMatrix();

    //window lower side corner
    glPushMatrix();
    glTranslatef(-2.2, 1.02, 8.9);
    glScalef(0.0001, 0.02, 0.34);
    drawCube1(0.7, 0.6, 0.5, 0.35, 0.3, 0.25);
    glPopMatrix();

    //window vertical bar 1
    glPushMatrix();
    glTranslatef(-2.27, 2.1, 8.9);
    glScalef(0.0001, 0.02, 0.3);
    drawCube1(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5);
    glPopMatrix();

    //window vertical bar 2
    glPushMatrix();
    glTranslatef(-2.27, 1.6, 8.9);
    glScalef(0.0001, 0.02, 0.3);
    drawCube1(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5);
    glPopMatrix();

    //window horizontal bar
    glPushMatrix();
    glTranslatef(-2.27, 1, 9.3);
    glScalef(0.0001, 0.6, 0.02);
    drawCube1(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5);
    glPopMatrix();
}


void wardrobe()
{
	glPushMatrix();
	glTranslatef(-0.9, -0.8, 2);

    //wardrobe
    glColor3f(0.3, 0.1, 0);
    glPushMatrix();
    glTranslatef(0, 0, 4);
    glScalef(0.12, 0.6, 0.4);
    drawCube();
    glPopMatrix();

    //wardrobe's 1st drawer
    glColor3f(0.5, 0.2, 0.2);
    glPushMatrix();
    glTranslatef(0.36, 1.4, 4.05);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.11, 0.38);
    drawCube();
    glPopMatrix();

    //wardrobe's 2nd drawer
    glColor3f(0.5, 0.2, 0.2);
    glPushMatrix();
    glTranslatef(0.36, 1, 4.05);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.11, 0.38);
    drawCube();
    glPopMatrix();

    //wardrobe's 3rd drawer
    glColor3f(0.5, 0.2, 0.2);
    glPushMatrix();
    glTranslatef(0.36, 0.6, 4.05);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.11, 0.38);
    drawCube();
    glPopMatrix();

    //wardrobe's 4th drawer
    glColor3f(0.5, 0.2, 0.2);
    glPushMatrix();
    glTranslatef(0.36, 0.2, 4.05);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.11, 0.38);
    drawCube();
    glPopMatrix();

    //wardrobe's 1st drawer handle
    glColor3f(0.3, 0.1, 0);
    glPushMatrix();
    glTranslatef(0.37, 1.5, 4.3);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //wardrobe's 2nd drawer handle
    glColor3f(0.3, 0.1, 0);
    glPushMatrix();
    glTranslatef(0.37, 1.1, 4.3);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //wardrobe's 3rd drawer handle
    glColor3f(0.3, 0.1, 0);
    glPushMatrix();
    glTranslatef(0.37, 0.7, 4.3);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //wardrobe's 4th drawer handle
    glColor3f(0.3, 0.1, 0);
    glPushMatrix();
    glTranslatef(0.37, 0.3, 4.3);
    //glRotatef(22, 0,0,1);
    glScalef(0.0001, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //wardrobe left stripline
    glColor3f(0.5, 0.2, 0.2);
    glPushMatrix();
    glTranslatef(0.35, 0, 5.3);
    //glRotatef(22, 0,0,1);
    glScalef(0.01, 0.6, 0.0001);
    drawCube();
    glPopMatrix();

	glPopMatrix();
}

void fan()
{
    glPushMatrix();
    glTranslatef(3, 4, 8);

    //stand
    glColor3f(0.2, 0.2, 0.2); // Dark gray
    glPushMatrix();
    glTranslatef(0, 0, 0.09);
    glScalef(0.01, 0.4, 0.01);
    drawCube1(0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 100); // Dark gray
    glPopMatrix();

    //fan cube
    glColor3f(0.2, 0.2, 0.2); // Dark gray
    glPushMatrix();
    glScalef(0.1, 0.05, 0.1);
    glTranslatef(-1.5, -1.5, -1.5);
    drawCube1(0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 100); // Dark gray
    glPopMatrix();

    glPushMatrix();
    glRotatef(a, 0, 1, 0);

    //blade 1
    glColor3f(0.2, 0.2, 0.2); // Dark gray
    glPushMatrix();
    //glRotatef(a, 0,1,0);
    glScalef(0.5, 0.01, 0.1);
    glTranslatef(-1.5, -1.5, -1.5);
    drawCube1(0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 100); // Dark gray
    glPopMatrix();

    //blade 2
    glColor3f(0.2, 0.2, 0.2); // Dark gray
    glPushMatrix();
    //glRotatef(a, 0,1,0);
    glScalef(0.1, 0.01, 0.5);
    glTranslatef(-1.5, -1.5, -1.5);
    drawCube1(0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 100); // Dark gray
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();
}

void cupboard()
{
    //Cupboard/Almari
 
    glColor3f(0.5, 0.2, 0.2); //0.3,0.1,0.0
    glPushMatrix();
	glTranslatef(-0.2, -0.8, 4.7); 
    //glRotatef(22, 0,0,1);
    glScalef(0.5, 1, 0.2);
    drawCube();
    glPopMatrix();

    //cupboard's 1st vertical stripline
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(-0.2, 0.2, 5.2);
    //glRotatef(22, 0,0,1);
    glScalef(0.5, 0.01, 0.0001);
    drawCube();
    glPopMatrix();

    //cupboard's 2nd vertical stripline
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(-0.2, -0.3, 5.2); 
    //glRotatef(22, 0,0,1);
    glScalef(0.5, 0.01, 0.0001);
    drawCube();
    glPopMatrix();

    //cupboard's last stripline
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(-0.2, -0.8, 5.2);
    //glRotatef(22, 0,0,1);
    glScalef(0.5, 0.01, 0.0001);
    drawCube();
    glPopMatrix();

    //cupboard's lst horizontal stripline
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(1.3, -0.8, 5.2); 
    //glRotatef(22, 0,0,1);
    glScalef(0.01, 1, 0.0001);
    drawCube();
    glPopMatrix();

    //cupboard's right side horizontal stripline
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(0.55, 0.2, 5.2);
    //glRotatef(22, 0,0,1);
    glScalef(0.01, 0.67, 0.0001);
    drawCube();
    glPopMatrix();

    //cupboard's left side horizontal stripline
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(-0.2, -0.8, 5.2);
    //glRotatef(22, 0,0,1);
    glScalef(0.01, 1, 0.0001);
    drawCube();
    glPopMatrix();

    //cupboard's handle right
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(0.8, 0.6, 5.3);
    //glRotatef(22, 0,0,1);
    glScalef(0.02, 0.18, 0.0001);
    drawCube();
    glPopMatrix();

    //cupboard's handle left
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(0.3, 0.6, 5.3); // -4.2 -0.8, +0.3
    //glRotatef(22, 0,0,1);
    glScalef(0.02, 0.18, 0.01);
    drawCube();
    glPopMatrix();

    //cupboard's drawer's 1st handle
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(0.3, -0.1, 5.3);
    //glRotatef(22, 0,0,1);
    glScalef(0.16, 0.02, 0.01);
    drawCube();
    glPopMatrix();

    //cupboard's drawer's 2nd handle
    glColor3f(0.2, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(0.3, -0.55, 5.3);
    //glRotatef(22, 0,0,1);
    glScalef(0.16, 0.02, 0.01);
    drawCube();
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
    GLfloat light_position[] = { -0.90, 0.3, 8.7, 1.0 };  //0.7, 4.5, 9.0

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
    windowOutside();
    bedsideTable();
    lamp();
	gamingChair();
    gamingPC();
    Clock();
    wardrobe();
	cupboard();

    glPushMatrix();
    glTranslatef(-0.25, 0, 15.0);
    //glScalef(10, 10, 10);

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
        camZ -= 1.0f; // Move forward

    if (key == GLUT_KEY_DOWN)
        camZ += 1.0f; // Move backward

    if (key == GLUT_KEY_RIGHT)
        camX += 1.0f; // Move right

    if (key == GLUT_KEY_LEFT)
        camX -= 1.0f; // Move left

    // Move chair
    if (key == GLUT_KEY_UP)
        chairZ -= 0.1f; // Move chair forward

    if (key == GLUT_KEY_DOWN)
        chairZ += 0.1f; // Move chair backward

    if (key == GLUT_KEY_RIGHT)
        chairX += 0.1f; // Move chair right

    if (key == GLUT_KEY_LEFT)
        chairX -= 0.1f; // Move chair left

    // Prevent camera from moving outside the room
    camX = std::max(-14.0f, std::min(camX, 14.0f));
    camY = std::max(-0.3f, std::min(camY, 5.0f)); // Assuming floor at -0.3 and ceiling at 5.0
    camZ = std::max(-14.0f, std::min(camZ, 14.0f));

    // Prevent chair from moving outside the room
    chairX = std::max(-14.0f, std::min(chairX, 14.0f));
    chairY = std::max(-0.3f, std::min(chairY, 5.0f));
    chairZ = std::max(-14.0f, std::min(chairZ, 14.0f));

    glutPostRedisplay();
}




void myKeyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
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
        eyeZ += 1.0;
        break;
    case 'p':  //zoom in
        eyeZ -= 1.0;
        break;
    case 'i': // move chair forward
        chairZ -= 0.1f;
        break;
    case 'k': // move chair backward
        chairZ += 0.1f;
        break;
    case 'j': // move chair left
        chairX -= 0.1f;
        break;
    case 'l': // move chair right
        chairX += 0.1f;
        break;
    case 'q': //back to default eye point and ref point
        eyeX = 7.0; eyeY = 2.0; eyeZ = 15.0;
        refX = 0.0; refY = 0.0; refZ = 0.0;
        break;
    case 'h': // move ref point upwards along Y axis
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

    case 'f': //turn on/off fan
        fanSwitch = ~fanSwitch;
        break;
    case '1': //to turn on and off light one
        switchOne = ~switchOne;
        if (switchOne) glEnable(GL_LIGHT0); else glDisable(GL_LIGHT0);
        break;
    case '2': //to turn on and off light two
        switchTwo = ~switchTwo;
        if (switchTwo) glEnable(GL_LIGHT1); else glDisable(GL_LIGHT1);
        break;
    case '3': //to turn on and off lamp
        switchLamp = ~switchLamp;
        if (switchLamp) glEnable(GL_LIGHT2); else glDisable(GL_LIGHT2);
        break;
    case '4': //turn on/off ambient light 1
        amb1 = ~amb1;
        break;
    case '5':
        diff1 = ~diff1;
        break;
    case '6':
        spec1 = ~spec1;
        break;
    case '7': //turn on/off ambient light 2
        amb2 = ~amb2;
        break;
    case '8':
        diff2 = ~diff2;
        break;
    case '9':
        spec2 = ~spec2;
        break;
    case 'e': //turn on/off ambient lamp light
        amb3 = ~amb3;
        break;
    case 'r':
        diff3 = ~diff3;
        break;
    case 't':
        spec3 = ~spec3;
        break;
    case 27:    // Escape key
        exit(1);
    }

    // Prevent chair from moving outside the room
    chairX = std::max(-14.0f, std::min(chairX, 14.0f));
    chairY = std::max(-0.3f, std::min(chairY, 5.0f));
    chairZ = std::max(-14.0f, std::min(chairZ, 14.0f));

    glutPostRedisplay();
}


void animate() {
    if (redFlag) {
        theta += 2.0;
        z -= 0.02; //0.016667;
        if (theta >= 196.0 && theta <= 210.0) {
            y = 1.44;
        }
        else if (theta >= 180.0 && theta <= 194.0) {
            y = 1.42;
        }
        else if (theta >= 180.0 && theta <= 194.0) {
            y = 1.4;
        }
        else if (theta >= 164.0 && theta <= 178.0) {
            y = 1.42;
        }

        if (theta == 210.0) {
            redFlag = false;
        }
    }
    else {
        theta -= 2.0;
        z += 0.02; //0.016667;

        if (theta >= 196.0 && theta <= 210.0) {
            y = 1.44;
        }
        else if (theta >= 180.0 && theta <= 194.0) {
            y = 1.42;
        }
        else if (theta >= 180.0 && theta <= 194.0) {
            y = 1.4;
        }
        else if (theta >= 164.0 && theta <= 178.0) {
            y = 1.42;
        }

        if (theta == 150.0) {
            redFlag = true;
        }
    }

    if (fanSwitch) {
        a += 5.0;
        if (a > 360.0)
            a -= 360.0;
    }

    glutPostRedisplay();
}


void fullScreen(int w, int h) {
    // Prevent a divide by zero, when window is too short; you can't make a window of zero width.
    if (h == 0)
        h = 1;
    float ratio = static_cast<GLfloat>(w) / static_cast<GLfloat>(h); // Calculate aspect ratio of the window

    // Set the perspective coordinate system
    glMatrixMode(GL_PROJECTION); // Use the Projection Matrix
    glLoadIdentity(); // Reset Matrix

    glViewport(0, 0, w, h); // Set the viewport to be the entire window
    gluPerspective(60.0, ratio, 1.0, 500.0); // Set the correct perspective.
    //glFrustum(-2.5,2.5,-2.5,2.5, ratio, 200);
    glMatrixMode(GL_MODELVIEW); // Get Back to the Modelview
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    std::cout << "To move Eye point:" << std::endl;
    std::cout << "w: up" << std::endl;
    std::cout << "s: down" << std::endl;
    std::cout << "a: left" << std::endl;
    std::cout << "d: right" << std::endl;
    std::cout << "p: zoom in" << std::endl;
    std::cout << "o: zoom out" << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "To move Camera point:" << std::endl;
    std::cout << "h: up" << std::endl;
    std::cout << "n: down" << std::endl;
    std::cout << "b: left" << std::endl;
    std::cout << "m: right" << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "Press q to move to default position" << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "f: to turn on/off fan     " << std::endl;
    std::cout << "      " << std::endl;
    std::cout << "i: move chair forward" << std::endl;
    std::cout << "k: move chair backward" << std::endl;
    std::cout << "j: move chair left" << std::endl;
    std::cout << "l: move chair right" << std::endl;
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
    init();
    glutReshapeFunc(fullScreen);
    glutDisplayFunc(display);
    glutKeyboardFunc(myKeyboardFunc);
    glutIdleFunc(animate);
    glutMainLoop();

    return 0;
}

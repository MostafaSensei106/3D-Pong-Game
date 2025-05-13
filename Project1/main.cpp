#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <string>
#include <sstream>
#include <iostream>

// Game variables
float ballPosX = 0.0f, ballPosY = 0.0f, ballPosZ = 0.0f;
float ballSpeedX = 0.02f, ballSpeedY = 0.02f, ballSpeedZ = 0.0f;
float paddle1Y = 0.0f, paddle2Y = 0.0f;  // Paddle positions
float paddle1Speed = 0.05f, paddle2Speed = 0.05f;  // Paddle movement speed
const float paddleHeight = 0.3f, paddleWidth = 0.05f, paddleDepth = 0.05f;
const float ballSize = 0.05f;
const float tableSizeX = 1.0f, tableSizeY = 0.8f;
const float boundaryHeight = 0.1f;

// Game state variables
int player1Score = 0, player2Score = 0;
bool gameStarted = false;
bool gameOver = false;
int winningScore = 5;

// Color definitions
GLfloat paddle1Color[3] = { 1.0f, 0.0f, 0.0f };  // Red
GLfloat paddle2Color[3] = { 0.0f, 0.0f, 1.0f };  // Blue
GLfloat ballColor[3] = { 1.0f, 1.0f, 1.0f };     // White
GLfloat tableColor[3] = { 0.0f, 0.5f, 0.0f };    // Green
GLfloat boundaryColor[3] = { 0.5f, 0.35f, 0.05f }; // Brown

// Lighting variables
bool lightingEnabled = true;
GLfloat lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };
GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

// Camera variables
float cameraAngleX = 0.0f;
float cameraAngleY = 30.0f;
float cameraDistance = 3.0f;

// Helper function to render text
void RenderString(float x, float y, void* font, const std::string& str, const GLfloat* color) {
    glDisable(GL_LIGHTING);  // Disable lighting for text rendering
    glColor3fv(color);
    glRasterPos2f(x, y);
    for (char c : str) {
        glutBitmapCharacter(font, c);
    }
    if (lightingEnabled) {
        glEnable(GL_LIGHTING);  // Re-enable lighting
    }
}

// Initialize OpenGL
void Init() {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);  // Dark blue background
    glEnable(GL_DEPTH_TEST);

    // Initialize lighting
    if (lightingEnabled) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

        // Set material properties
        GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat materialShininess[] = { 50.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
        glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
    }

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Reset the ball to the center with a random direction
void ResetBall() {
    // Forward declaration for ResetBall to resolve identifier not found errors
    void ResetBall();
    ballPosX = 0.0f;
    ballPosY = 0.0f;
    ballPosZ = 0.0f;

    // Random direction, but make sure it's not too vertical
    float angle = ((float)rand() / RAND_MAX) * 2 * 3.14159f;
    ballSpeedX = 0.02f * cos(angle);
    ballSpeedY = 0.02f * sin(angle);

    // Ensure the ball is moving horizontally enough
    if (fabs(ballSpeedX) < 0.01f) {
        ballSpeedX = (ballSpeedX >= 0) ? 0.01f : -0.01f;
    }
}

// Draw the paddle with the specified color
void DrawPaddle(float x, float y, const GLfloat* color) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    if (lightingEnabled) {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    }
    else {
        glColor3fv(color);
    }

    // Draw the paddle as a cuboid
    glBegin(GL_QUADS);
    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-paddleWidth / 2, -paddleHeight / 2, paddleDepth / 2);
    glVertex3f(paddleWidth / 2, -paddleHeight / 2, paddleDepth / 2);
    glVertex3f(paddleWidth / 2, paddleHeight / 2, paddleDepth / 2);
    glVertex3f(-paddleWidth / 2, paddleHeight / 2, paddleDepth / 2);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-paddleWidth / 2, -paddleHeight / 2, -paddleDepth / 2);
    glVertex3f(-paddleWidth / 2, paddleHeight / 2, -paddleDepth / 2);
    glVertex3f(paddleWidth / 2, paddleHeight / 2, -paddleDepth / 2);
    glVertex3f(paddleWidth / 2, -paddleHeight / 2, -paddleDepth / 2);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-paddleWidth / 2, paddleHeight / 2, -paddleDepth / 2);
    glVertex3f(-paddleWidth / 2, paddleHeight / 2, paddleDepth / 2);
    glVertex3f(paddleWidth / 2, paddleHeight / 2, paddleDepth / 2);
    glVertex3f(paddleWidth / 2, paddleHeight / 2, -paddleDepth / 2);

    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-paddleWidth / 2, -paddleHeight / 2, -paddleDepth / 2);
    glVertex3f(paddleWidth / 2, -paddleHeight / 2, -paddleDepth / 2);
    glVertex3f(paddleWidth / 2, -paddleHeight / 2, paddleDepth / 2);
    glVertex3f(-paddleWidth / 2, -paddleHeight / 2, paddleDepth / 2);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(paddleWidth / 2, -paddleHeight / 2, -paddleDepth / 2);
    glVertex3f(paddleWidth / 2, paddleHeight / 2, -paddleDepth / 2);
    glVertex3f(paddleWidth / 2, paddleHeight / 2, paddleDepth / 2);
    glVertex3f(paddleWidth / 2, -paddleHeight / 2, paddleDepth / 2);

    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-paddleWidth / 2, -paddleHeight / 2, -paddleDepth / 2);
    glVertex3f(-paddleWidth / 2, -paddleHeight / 2, paddleDepth / 2);
    glVertex3f(-paddleWidth / 2, paddleHeight / 2, paddleDepth / 2);
    glVertex3f(-paddleWidth / 2, paddleHeight / 2, -paddleDepth / 2);
    glEnd();

    glPopMatrix();
}

// Draw the ball
void DrawBall() {
    glPushMatrix();
    glTranslatef(ballPosX, ballPosY, ballPosZ);

    if (lightingEnabled) {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ballColor);
    }
    else {
        glColor3fv(ballColor);
    }

    glutSolidSphere(ballSize, 20, 20);  // Draw a sphere
    glPopMatrix();
}

// Draw the table and boundaries
void DrawTable() {
    glPushMatrix();

    // Draw table surface
    if (lightingEnabled) {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tableColor);
    }
    else {
        glColor3fv(tableColor);
    }

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-tableSizeX, -tableSizeY, -0.1f);
    glVertex3f(tableSizeX, -tableSizeY, -0.1f);
    glVertex3f(tableSizeX, tableSizeY, -0.1f);
    glVertex3f(-tableSizeX, tableSizeY, -0.1f);
    glEnd();

    // Draw top and bottom boundaries
    if (lightingEnabled) {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, boundaryColor);
    }
    else {
        glColor3fv(boundaryColor);
    }

    // Top boundary
    glBegin(GL_QUADS);
    // Front face
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-tableSizeX, tableSizeY, -0.1f);
    glVertex3f(tableSizeX, tableSizeY, -0.1f);
    glVertex3f(tableSizeX, tableSizeY, boundaryHeight);
    glVertex3f(-tableSizeX, tableSizeY, boundaryHeight);

    // Top face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-tableSizeX, tableSizeY, boundaryHeight);
    glVertex3f(tableSizeX, tableSizeY, boundaryHeight);
    glVertex3f(tableSizeX, tableSizeY + 0.05f, boundaryHeight);
    glVertex3f(-tableSizeX, tableSizeY + 0.05f, boundaryHeight);
    glEnd();

    // Bottom boundary
    glBegin(GL_QUADS);
    // Front face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-tableSizeX, -tableSizeY, -0.1f);
    glVertex3f(-tableSizeX, -tableSizeY, boundaryHeight);
    glVertex3f(tableSizeX, -tableSizeY, boundaryHeight);
    glVertex3f(tableSizeX, -tableSizeY, -0.1f);

    // Top face
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-tableSizeX, -tableSizeY - 0.05f, boundaryHeight);
    glVertex3f(-tableSizeX, -tableSizeY, boundaryHeight);
    glVertex3f(tableSizeX, -tableSizeY, boundaryHeight);
    glVertex3f(tableSizeX, -tableSizeY - 0.05f, boundaryHeight);
    glEnd();

    // Draw dashed line in the middle
    glLineStipple(3, 0xAAAA);  // Dashed line pattern
    glEnable(GL_LINE_STIPPLE);

    if (lightingEnabled) {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ballColor);
    }
    else {
        glColor3fv(ballColor);
    }

    glBegin(GL_LINES);
    glVertex3f(0.0f, -tableSizeY, 0.001f);
    glVertex3f(0.0f, tableSizeY, 0.001f);
    glEnd();

    glDisable(GL_LINE_STIPPLE);

    glPopMatrix();
}

// Update ball position and handle collisions
void UpdateBall() {
    if (!gameStarted || gameOver) return;

    // Update ball position
    ballPosX += ballSpeedX;
    ballPosY += ballSpeedY;

    // Collision with top and bottom boundaries
    if (ballPosY + ballSize > tableSizeY || ballPosY - ballSize < -tableSizeY) {
        ballSpeedY = -ballSpeedY;
        // Add a small random factor to make the game less predictable
        ballSpeedY += (((float)rand() / RAND_MAX) - 0.5f) * 0.005f;
    }

    // Collision with left paddle
    if (ballPosX - ballSize < -tableSizeX + paddleWidth &&
        ballPosY < paddle1Y + paddleHeight / 2 &&
        ballPosY > paddle1Y - paddleHeight / 2) {

        ballSpeedX = -ballSpeedX;

        // Adjust ball direction based on where it hits the paddle
        float relativePaddleCollision = (ballPosY - paddle1Y) / (paddleHeight / 2);
        ballSpeedY = relativePaddleCollision * 0.03f;

        // Increase speed slightly
        ballSpeedX *= 1.05f;
        if (ballSpeedX > 0.05f) ballSpeedX = 0.05f;
    }

    // Collision with right paddle
    if (ballPosX + ballSize > tableSizeX - paddleWidth &&
        ballPosY < paddle2Y + paddleHeight / 2 &&
        ballPosY > paddle2Y - paddleHeight / 2) {

        ballSpeedX = -ballSpeedX;

        // Adjust ball direction based on where it hits the paddle
        float relativePaddleCollision = (ballPosY - paddle2Y) / (paddleHeight / 2);
        ballSpeedY = relativePaddleCollision * 0.03f;

        // Increase speed slightly
        ballSpeedX *= 1.05f;
        if (ballSpeedX < -0.05f) ballSpeedX = -0.05f;
    }

    // Scoring
    if (ballPosX + ballSize > tableSizeX) {  // Player 1 scores
        player1Score++;
        ResetBall();

        if (player1Score >= winningScore) {
            gameOver = true;
        }
    }

    if (ballPosX - ballSize < -tableSizeX) {  // Player 2 scores
        player2Score++;
        ResetBall();

        if (player2Score >= winningScore) {
            gameOver = true;
        }
    }
}



// Reset the game
void ResetGame() {
    player1Score = 0;
    player2Score = 0;
    paddle1Y = 0.0f;
    paddle2Y = 0.0f;
    ResetBall();
    gameOver = false;
    gameStarted = false;
}

// Process keyboard input
void ProcessInput() {
    // Player 1 controls (W and S keys)
    if (GetAsyncKeyState(0x57)) { // W - Player 1 up
        paddle1Y += paddle1Speed;
        if (paddle1Y + paddleHeight / 2 > tableSizeY) {
            paddle1Y = tableSizeY - paddleHeight / 2;
        }
    }
    if (GetAsyncKeyState(0x53)) { // S - Player 1 down
        paddle1Y -= paddle1Speed;
        if (paddle1Y - paddleHeight / 2 < -tableSizeY) {
            paddle1Y = -tableSizeY + paddleHeight / 2;
        }
    }

    // Player 2 controls (Up and Down arrow keys)
    if (GetAsyncKeyState(VK_UP)) { // ↑ - Player 2 up
        paddle2Y += paddle2Speed;
        if (paddle2Y + paddleHeight / 2 > tableSizeY) {
            paddle2Y = tableSizeY - paddleHeight / 2;
        }
    }
    if (GetAsyncKeyState(VK_DOWN)) { // ↓ - Player 2 down
        paddle2Y -= paddle2Speed;
        if (paddle2Y - paddleHeight / 2 < -tableSizeY) {
            paddle2Y = -tableSizeY + paddleHeight / 2;
        }
    }

    // Space to start/restart game
    if (GetAsyncKeyState(VK_SPACE) & 0x1) {
        if (gameOver) {
            ResetGame();
        }
        gameStarted = true;
    }

    // L key to toggle lighting
    if (GetAsyncKeyState(0x4C) & 0x1) { // L key
        lightingEnabled = !lightingEnabled;
        if (lightingEnabled) {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
        }
        else {
            glDisable(GL_LIGHTING);
        }
    }

    // Camera controls
    if (GetAsyncKeyState(VK_LEFT)) {
        cameraAngleY -= 1.0f;
    }
    if (GetAsyncKeyState(VK_RIGHT)) {
        cameraAngleY += 1.0f;
    }
    if (GetAsyncKeyState(VK_PRIOR)) { // Page Up - zoom in
        cameraDistance -= 0.1f;
        if (cameraDistance < 1.0f) cameraDistance = 1.0f;
    }
    if (GetAsyncKeyState(VK_NEXT)) { // Page Down - zoom out
        cameraDistance += 0.1f;
        if (cameraDistance > 10.0f) cameraDistance = 10.0f;
    }
}

// Render the scene
void RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set camera position
    float camX = cameraDistance * sin(cameraAngleY * 3.14159f / 180.0f);
    float camZ = cameraDistance * cos(cameraAngleY * 3.14159f / 180.0f);
    gluLookAt(camX, 0.0f, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // Draw the table and boundaries
    DrawTable();

    // Draw paddles
    DrawPaddle(-tableSizeX + paddleWidth / 2, paddle1Y, paddle1Color);  // Player 1 paddle
    DrawPaddle(tableSizeX - paddleWidth / 2, paddle2Y, paddle2Color);   // Player 2 paddle

    // Draw the ball
    DrawBall();

    // Draw HUD (score and instructions)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw scores
    std::stringstream ss;
    ss << player1Score << " - " << player2Score;
    RenderString(-0.05f, 0.9f, GLUT_BITMAP_HELVETICA_18, ss.str(), ballColor);

    // Draw instructions
    if (!gameStarted) {
        RenderString(-0.3f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Press SPACE to start", ballColor);
    }
    else if (gameOver) {
        if (player1Score > player2Score) {
            RenderString(-0.3f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Player 1 wins!", ballColor);
        }
        else {
            RenderString(-0.3f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Player 2 wins!", ballColor);
        }
        RenderString(-0.3f, -0.1f, GLUT_BITMAP_HELVETICA_18, "Press SPACE to play again", ballColor);
    }

    // Display controls
    RenderString(-0.9f, -0.9f, GLUT_BITMAP_HELVETICA_12, "P1: W/S", paddle1Color);
    RenderString(0.7f, -0.9f, GLUT_BITMAP_HELVETICA_12, "P2: UP/DOWN", paddle2Color);
    RenderString(-0.2f, -0.9f, GLUT_BITMAP_HELVETICA_12, "L: Toggle Lighting", ballColor);

    // Restore projection matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Update game state
    UpdateBall();

    glutSwapBuffers();
}

// Timer function for updating
void Update(int value) {
    ProcessInput();  // Process keyboard input
    glutPostRedisplay();  // Request redisplay
    glutTimerFunc(16, Update, 0);  // Schedule next update (16ms = ~60 FPS)
}

// Setup camera and projection
void SetupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Window resize callback
void Reshape(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Setup everything
void Setup() {
    Init();
    SetupCamera();
    ResetBall();
    srand(GetTickCount());  // Initialize random seed
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Pong Game");

    glutDisplayFunc(RenderScene);
    glutReshapeFunc(Reshape);
    glutTimerFunc(25, Update, 0);

    Setup();

    glutMainLoop();
    return 0;
}
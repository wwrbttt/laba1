#include <math.h>       // ���. ������� (fabs)
#include <glut.h>    // OpenGL UT (�� Linux ����� ���� <GL/glut.h>)
#include <stdlib.h>     // rand, srand, exit
#include <time.h>       // time
#include <stdio.h>      // NULL
// ��������� ���� ����
float bgR = 0.0f, bgG = 0.0f, bgB = 0.0f;
// ������� ����
int winWidth = 800;
int winHeight = 600;

// ��������� ��������� �������
typedef struct {
    char ch;           // ������������ ������
    float x, y;        // ����������
    float velocity;
    float colorR, colorG, colorB; // ���� RGB
    int active;        // ���������� int ������ bool
} FallingChar;

// ������������ ������ �� ������ C
FallingChar* chars = NULL;
int charsCount = 0;       // ������� ����������
int charsCapacity = 0;    // ����������� �������

// ����� ��� ������ ��������
char text[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
int textLength;           // ����� ������ (����������� � main)
int currentIndex = 0;     // ��������� � ������

// ���������� ���������
const float GRAVITY = 0.05f;
const float MIN_SPEED = 1.0f;
const float MAX_SPEED = 3.0f;
const float DAMPING = 0.7f;

// ������� ��� ���������� ������ ������� � ������
void addChar() {
    if (textLength == 0) return;

    // ������� ����� ������
    FallingChar fc;
    fc.ch = text[currentIndex];
    fc.x = (float)(rand() % (winWidth - 20) + 10);
    fc.y = (float)winHeight;
    fc.velocity = MIN_SPEED + (float)rand() / RAND_MAX * (MAX_SPEED - MIN_SPEED);
    fc.colorR = (float)(rand() % 256) / 255.0f;
    fc.colorG = (float)(rand() % 256) / 255.0f;
    fc.colorB = (float)(rand() % 256) / 255.0f;
    fc.active = 1;  // true

    // ����������� ������ ��� �������������
    if (charsCount >= charsCapacity) {
        charsCapacity = charsCapacity == 0 ? 10 : charsCapacity * 2;
        chars = (FallingChar*)realloc(chars, charsCapacity * sizeof(FallingChar));
    }

    // ��������� � �����
    chars[charsCount++] = fc;
    currentIndex = (currentIndex + 1) % textLength;
}

// �������� ���������� ��������
void removeInactiveChars() {
    int newCount = 0;
    for (int i = 0; i < charsCount; i++) {
        if (chars[i].active || chars[i].velocity != 0) {
            // ��������� �������� ��� ����������
            if (newCount != i) {
                chars[newCount] = chars[i];
            }
            newCount++;
        }
    }
    charsCount = newCount;
}

// ������� ������� ��������� � ������
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // ���������� ������ ��� ���� ��������
    for (int i = 0; i < charsCount; ++i) {
        if (chars[i].active) {
            chars[i].velocity += GRAVITY;
            chars[i].y -= chars[i].velocity;

            // ������������ � ������ ��������
            if (chars[i].y < 0) {
                chars[i].y = 0;
                chars[i].velocity = -chars[i].velocity * DAMPING;
                if (fabs(chars[i].velocity) < 0.5f) {
                    chars[i].velocity = 0;
                    chars[i].active = 0;
                }
            }
        }
    }

    // ��������� ��������
    for (int i = 0; i < charsCount; ++i) {
        if (chars[i].active || chars[i].velocity == 0) {
            glColor3f(chars[i].colorR, chars[i].colorG, chars[i].colorB);
            glRasterPos2f(chars[i].x, chars[i].y);
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, chars[i].ch);
        }
    }

    // ��������� ���������� ������ �������
    if (rand() % 10 == 0) {
        addChar();
    }

    // �������� ���������� ��������
    removeInactiveChars();

    glutSwapBuffers();
}

// ��������� ��������� ������� ����
void Reshape(int width, int height) {
    if (height == 0) height = 1;
    winWidth = width;
    winHeight = height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ��������� ����������
void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27: // ESC - �����
        free(chars); // ����������� ������ ����� �������
        exit(0);
        break;
    case ' ': // ������ - �������� 5 ��������
        for (int i = 0; i < 5; ++i) {
            addChar();
        }
        break;
    }
}

// ������ ��������
void Timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}

// ����� ��������� ������ (��������� �� main.cpp)
void runSalute(int argc, char** argv) {
    // ����� ����������� ������ ���������
    if (chars) { free(chars); chars = NULL; }
    charsCount = 0;
    charsCapacity = 0;
    currentIndex = 0;

    // ��������� ����� ������
    textLength = sizeof(text) / sizeof(text[0]) - 1; // -1 ��� ����-�����������

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Falling Text (C version)");

    srand((unsigned int)time(NULL));

    // ��������� ��������� ����� ����
    bgR = (float)(rand() % 50) / 255.0f;
    bgG = (float)(rand() % 50) / 255.0f;
    bgB = (float)(rand() % 50) / 255.0f;

    // ��������� OpenGL
    glClearColor(bgR, bgG, bgB, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // �������� ��������� 20 ��������
    for (int i = 0; i < 20; ++i) {
        addChar();
    }

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(0, Timer, 0);

    glutMainLoop();

    // ���� ��� ���������� ������ ��� ������ �� glutMainLoop
    free(chars);

}

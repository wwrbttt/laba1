#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <glut.h>

using namespace std;

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const float PI = 3.14159265359f;

// ������� ���������
const int AQUARIUM_LEFT = 100;
const int AQUARIUM_RIGHT = WINDOW_WIDTH - 100;
const int AQUARIUM_TOP = WINDOW_HEIGHT - 100;
const int AQUARIUM_BOTTOM = 100;

// ��������� ��������
struct Bubble {
    float x, y;
    float radius;
    float speed;
    float wobble;
    float wobbleOffset;
    bool active;
};

// ��������� ����
struct Fish {
    float x, y;
    float speed;
    float size;
    float angle;
    float swimPhase;
    float r, g, b;
    bool movingRight;
    float tailAngle;
};

// ��������� ���������
struct Seaweed {
    float x;
    float height;
    float swayPhase;
    float swaySpeed;
    int segments;
};

vector<Bubble> bubbles;
vector<Fish> fishes;
vector<Seaweed> seaweeds;
int frameCount = 0;

// ��������� ���������� ����� � ���������
float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

// ��������� �����
void drawCircle(float x, float y, float radius, int segments = 30) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

// ��������� �������
void drawEllipse(float x, float y, float radiusX, float radiusY, int segments = 30) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        glVertex2f(x + cos(angle) * radiusX, y + sin(angle) * radiusY);
    }
    glEnd();
}

// ������������� ���������
void initBubbles() {
    for (int i = 0; i < 20; i++) {
        Bubble bubble;
        bubble.x = randomFloat(AQUARIUM_LEFT + 20, AQUARIUM_RIGHT - 20);
        bubble.y = randomFloat(AQUARIUM_BOTTOM, AQUARIUM_TOP);
        bubble.radius = randomFloat(3, 8);
        bubble.speed = randomFloat(0.5, 2.0);
        bubble.wobble = randomFloat(0.5, 1.5);
        bubble.wobbleOffset = randomFloat(0, 2 * PI);
        bubble.active = true;
        bubbles.push_back(bubble);
    }
}

// ������������� ���
void initFishes() {
    for (int i = 0; i < 10; i++) {
        Fish fish;
        fish.x = randomFloat(AQUARIUM_LEFT + 50, AQUARIUM_RIGHT - 50);
        fish.y = randomFloat(AQUARIUM_BOTTOM + 50, AQUARIUM_TOP - 50);
        fish.speed = randomFloat(0.5, 1.5);
        fish.size = randomFloat(25, 45);
        fish.angle = 0;
        fish.swimPhase = randomFloat(0, 2 * PI);
        fish.r = randomFloat(0.4f, 1.0f);
        fish.g = randomFloat(0.2f, 0.8f);
        fish.b = randomFloat(0.2f, 0.9f);
        fish.movingRight = (rand() % 2 == 0);
        fish.tailAngle = 0;
        fishes.push_back(fish);
    }
}

// ������������� ����������
void initSeaweeds() {
    for (int i = 0; i < 15; i++) {
        Seaweed seaweed;
        seaweed.x = randomFloat(AQUARIUM_LEFT + 20, AQUARIUM_RIGHT - 20);
        seaweed.height = randomFloat(100, 200);
        seaweed.swayPhase = randomFloat(0, 2 * PI);
        seaweed.swaySpeed = randomFloat(0.02f, 0.05f);
        seaweed.segments = 20;
        seaweeds.push_back(seaweed);
    }
}

// ��������� ����
void drawBackground() {
    // �����
    glBegin(GL_QUADS);
    glColor3f(0.9f, 0.85f, 0.8f);
    glVertex2f(0, WINDOW_HEIGHT);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glColor3f(0.8f, 0.75f, 0.7f);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(0, 0);
    glEnd();

    // ��������� ���������
    glBegin(GL_QUADS);
    glColor3f(0.4f, 0.2f, 0.1f);
    glVertex2f(AQUARIUM_LEFT - 20, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_RIGHT + 20, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_RIGHT + 20, 0);
    glVertex2f(AQUARIUM_LEFT - 20, 0);
    glEnd();

    // ���� � ���������
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.6f, 0.9f);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_TOP);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_TOP);
    glColor3f(0.0f, 0.1f, 0.3f);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_BOTTOM);
    glEnd();

    // ����� ���������
    glLineWidth(3.0f);
    glColor3f(0.8f, 0.9f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_TOP);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_TOP);
    glEnd();

    // ����� ����������� ����
    glLineWidth(1.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    glBegin(GL_LINES);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_TOP - 10);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_TOP - 10);
    glEnd();
}

// ��������� ���
void drawSeaFloor() {
    // �����
    glBegin(GL_QUADS);
    glColor3f(0.76f, 0.7f, 0.5f);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_BOTTOM + 60);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_BOTTOM + 60);
    glColor3f(0.6f, 0.55f, 0.4f);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_BOTTOM);
    glEnd();

    // �����
    glColor3f(0.5f, 0.5f, 0.55f);
    drawEllipse(AQUARIUM_LEFT + 100, AQUARIUM_BOTTOM + 40, 40, 25);
    drawEllipse(AQUARIUM_LEFT + 400, AQUARIUM_BOTTOM + 30, 50, 30);
    drawEllipse(AQUARIUM_RIGHT - 150, AQUARIUM_BOTTOM + 35, 45, 28);

    glColor3f(0.4f, 0.4f, 0.45f);
    drawEllipse(AQUARIUM_LEFT + 250, AQUARIUM_BOTTOM + 20, 30, 20);
    drawEllipse(AQUARIUM_LEFT + 600, AQUARIUM_BOTTOM + 25, 38, 24);
    drawEllipse(AQUARIUM_LEFT + 50, AQUARIUM_BOTTOM + 10, 42, 26);
}

// ��������� ����������
void drawSeaweeds() {
    for (auto& seaweed : seaweeds) {
        float sway = sin(seaweed.swayPhase) * 20;

        // �������
        glLineWidth(4.0f);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= seaweed.segments; i++) {
            float t = static_cast<float>(i) / seaweed.segments;
            float y = (AQUARIUM_BOTTOM + 40) + t * seaweed.height;
            float x = seaweed.x + sway * t * t;
            glColor3f(0.1f, 0.5f + t * 0.3f, 0.2f);
            glVertex2f(x, y);
        }
        glEnd();

        // ������
        for (int i = 3; i < seaweed.segments; i += 3) {
            float t = static_cast<float>(i) / seaweed.segments;
            float y = (AQUARIUM_BOTTOM + 40) + t * seaweed.height;
            float x = seaweed.x + sway * t * t;
            glColor3f(0.2f, 0.6f + t * 0.2f, 0.3f);
            drawEllipse(x - 8, y, 8, 4);
            drawEllipse(x + 8, y, 8, 4);
        }
    }
}

// ��������� ����
void drawFish(Fish& fish) {
    glPushMatrix();
    glTranslatef(fish.x, fish.y, 0);

    if (!fish.movingRight) {
        glScalef(-1, 1, 1);
    }

    float tailWag = sin(fish.swimPhase) * 15;

    // �����
    glColor3f(fish.r * 0.8f, fish.g * 0.8f, fish.b * 0.8f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-fish.size * 0.8f, 0);
    glVertex2f(-fish.size * 1.3f, fish.size * 0.5f + tailWag * 0.1f);
    glVertex2f(-fish.size * 1.3f, -fish.size * 0.5f + tailWag * 0.1f);
    glEnd();

    // ����
    glColor3f(fish.r, fish.g, fish.b);
    drawEllipse(0, 0, fish.size, fish.size * 0.6f);

    // ��������
    glColor3f(fish.r * 0.7f, fish.g * 0.7f, fish.b * 0.7f);
    // ������� �������
    glBegin(GL_TRIANGLES);
    glVertex2f(-fish.size * 0.2f, fish.size * 0.4f);
    glVertex2f(fish.size * 0.2f, fish.size * 0.9f);
    glVertex2f(fish.size * 0.4f, fish.size * 0.4f);
    glEnd();
    // ������ �������
    glBegin(GL_TRIANGLES);
    glVertex2f(-fish.size * 0.2f, -fish.size * 0.4f);
    glVertex2f(fish.size * 0.1f, -fish.size * 0.8f);
    glVertex2f(fish.size * 0.3f, -fish.size * 0.4f);
    glEnd();

    // ����
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(fish.size * 0.5f, fish.size * 0.15f, fish.size * 0.15f);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(fish.size * 0.55f, fish.size * 0.15f, fish.size * 0.07f);

    // �����
    glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(fish.size * 0.2f, fish.size * 0.3f);
    glVertex2f(fish.size * 0.2f, -fish.size * 0.3f);
    glEnd();
    glDisable(GL_BLEND);

    glPopMatrix();
}

// ��������� ��������
void drawBubble(Bubble& bubble) {
    if (!bubble.active) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ���� ��������
    glColor4f(0.8f, 0.9f, 1.0f, 0.4f);
    drawCircle(bubble.x, bubble.y, bubble.radius);

    // ����
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    drawCircle(bubble.x - bubble.radius * 0.3f, bubble.y + bubble.radius * 0.3f, bubble.radius * 0.25f);

    // �������
    glColor4f(0.9f, 0.9f, 1.0f, 0.6f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 30; i++) {
        float angle = 2.0f * PI * i / 30;
        glVertex2f(bubble.x + cos(angle) * bubble.radius, bubble.y + sin(angle) * bubble.radius);
    }
    glEnd();

    glDisable(GL_BLEND);
}

// ���������� ���������
void updateBubbles() {
    for (auto& bubble : bubbles) {
        if (bubble.active) {
            bubble.y += bubble.speed;
            bubble.x += sin(bubble.y * 0.05f + bubble.wobbleOffset) * bubble.wobble;

            if (bubble.y > AQUARIUM_TOP) {
                bubble.active = false;
            }
        }
    }

    // ��������� ����� ���������
    if (rand() % 20 == 0) {
        bool found = false;
        for (auto& bubble : bubbles) {
            if (!bubble.active) {
                bubble.x = randomFloat(AQUARIUM_LEFT + 50, AQUARIUM_RIGHT - 50);
                bubble.y = AQUARIUM_BOTTOM + 10;
                bubble.radius = randomFloat(3, 8);
                bubble.speed = randomFloat(1.0, 2.5);
                bubble.active = true;
                found = true;
                break;
            }
        }
        if (!found && bubbles.size() < 200) {
            Bubble b;
            b.x = randomFloat(AQUARIUM_LEFT + 50, AQUARIUM_RIGHT - 50);
            b.y = AQUARIUM_BOTTOM + 10;
            b.radius = randomFloat(3, 8);
            b.speed = randomFloat(1.0, 2.5);
            b.wobble = randomFloat(0.5, 1.5);
            b.wobbleOffset = randomFloat(0, 2 * PI);
            b.active = true;
            bubbles.push_back(b);
        }
    }
}

// ���������� ���
void updateFishes() {
    for (auto& fish : fishes) {
        fish.swimPhase += 0.1f;

        // �������� �� �����������
        if (fish.movingRight) {
            fish.x += fish.speed;
            if (fish.x > AQUARIUM_RIGHT - fish.size) {
                fish.movingRight = false;
                fish.speed = randomFloat(0.5, 1.5);
            }
        }
        else {
            fish.x -= fish.speed;
            if (fish.x < AQUARIUM_LEFT + fish.size) {
                fish.movingRight = true;
                fish.speed = randomFloat(0.5, 1.5);
            }
        }

        // ����������� �� ���������
        if (fish.y > AQUARIUM_TOP - fish.size) fish.y = AQUARIUM_TOP - fish.size;
        if (fish.y < AQUARIUM_BOTTOM + fish.size + 50) fish.y = AQUARIUM_BOTTOM + fish.size + 50;

        // ��������� ��������
        fish.y += sin(fish.swimPhase * 0.5f) * 0.5f;

        // �������� ��������� ������
        if (rand() % 100 < 5) {
            float mouthX = fish.movingRight ? fish.x + fish.size * 0.6f : fish.x - fish.size * 0.6f;
            float mouthY = fish.y;

            bool found = false;
            for (auto& bubble : bubbles) {
                if (!bubble.active) {
                    bubble.x = mouthX;
                    bubble.y = mouthY;
                    bubble.radius = randomFloat(2, 5);
                    bubble.speed = randomFloat(1.5, 3.0);
                    bubble.active = true;
                    found = true;
                    break;
                }
            }
            if (!found && bubbles.size() < 200) {
                Bubble b;
                b.x = mouthX;
                b.y = mouthY;
                b.radius = randomFloat(2, 5);
                b.speed = randomFloat(1.5, 3.0);
                b.wobble = randomFloat(0.5, 1.0);
                b.wobbleOffset = randomFloat(0, 2 * PI);
                b.active = true;
                bubbles.push_back(b);
            }
        }
    }
}

// ���������� ����������
void updateSeaweeds() {
    for (auto& seaweed : seaweeds) {
        seaweed.swayPhase += seaweed.swaySpeed;
    }
}

// ������� ���������
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawBackground();
    drawSeaFloor();
    drawSeaweeds();

    for (auto& fish : fishes) {
        drawFish(fish);
    }

    for (auto& bubble : bubbles) {
        drawBubble(bubble);
    }

    glutSwapBuffers();
}

// ������ ��������
void timer(int value) {
    updateBubbles();
    updateFishes();
    updateSeaweeds();
    frameCount++;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ������������� opengl
void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void runAquarium(int argc, char** argv) {
    // ����� ���������
    bubbles.clear();
    fishes.clear();
    seaweeds.clear();
    frameCount = 0;

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("Aquarium Simulation");

    initGL();
    initBubbles();
    initFishes();
    initSeaweeds();

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
}
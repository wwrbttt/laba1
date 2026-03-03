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

// границы аквариума
const int AQUARIUM_LEFT = 100;
const int AQUARIUM_RIGHT = WINDOW_WIDTH - 100;
const int AQUARIUM_TOP = WINDOW_HEIGHT - 100;
const int AQUARIUM_BOTTOM = 100;

// структура пузырька
struct Bubble {
    float x, y;
    float radius;
    float speed;
    float wobble;
    float wobbleOffset;
    bool active;
};

// структура рыбы
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

// структура водоросли
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

// генерация случайного числа в диапазоне
float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

// рисование круга
void drawCircle(float x, float y, float radius, int segments = 30) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

// рисование эллипса
void drawEllipse(float x, float y, float radiusX, float radiusY, int segments = 30) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * i / segments;
        glVertex2f(x + cos(angle) * radiusX, y + sin(angle) * radiusY);
    }
    glEnd();
}

// инициализация пузырьков
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

// инициализация рыб
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

// инициализация водорослей
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

// рисование фона
void drawBackground() {
    // стена
    glBegin(GL_QUADS);
    glColor3f(0.9f, 0.85f, 0.8f);
    glVertex2f(0, WINDOW_HEIGHT);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glColor3f(0.8f, 0.75f, 0.7f);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(0, 0);
    glEnd();

    // подставка аквариума
    glBegin(GL_QUADS);
    glColor3f(0.4f, 0.2f, 0.1f);
    glVertex2f(AQUARIUM_LEFT - 20, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_RIGHT + 20, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_RIGHT + 20, 0);
    glVertex2f(AQUARIUM_LEFT - 20, 0);
    glEnd();

    // вода в аквариуме
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.6f, 0.9f);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_TOP);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_TOP);
    glColor3f(0.0f, 0.1f, 0.3f);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_BOTTOM);
    glEnd();

    // рамка аквариума
    glLineWidth(3.0f);
    glColor3f(0.8f, 0.9f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_TOP);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_TOP);
    glEnd();

    // линия поверхности воды
    glLineWidth(1.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    glBegin(GL_LINES);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_TOP - 10);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_TOP - 10);
    glEnd();
}

// рисование дна
void drawSeaFloor() {
    // песок
    glBegin(GL_QUADS);
    glColor3f(0.76f, 0.7f, 0.5f);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_BOTTOM + 60);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_BOTTOM + 60);
    glColor3f(0.6f, 0.55f, 0.4f);
    glVertex2f(AQUARIUM_RIGHT, AQUARIUM_BOTTOM);
    glVertex2f(AQUARIUM_LEFT, AQUARIUM_BOTTOM);
    glEnd();

    // камни
    glColor3f(0.5f, 0.5f, 0.55f);
    drawEllipse(AQUARIUM_LEFT + 100, AQUARIUM_BOTTOM + 40, 40, 25);
    drawEllipse(AQUARIUM_LEFT + 400, AQUARIUM_BOTTOM + 30, 50, 30);
    drawEllipse(AQUARIUM_RIGHT - 150, AQUARIUM_BOTTOM + 35, 45, 28);

    glColor3f(0.4f, 0.4f, 0.45f);
    drawEllipse(AQUARIUM_LEFT + 250, AQUARIUM_BOTTOM + 20, 30, 20);
    drawEllipse(AQUARIUM_LEFT + 600, AQUARIUM_BOTTOM + 25, 38, 24);
    drawEllipse(AQUARIUM_LEFT + 50, AQUARIUM_BOTTOM + 10, 42, 26);
}

// рисование водорослей
void drawSeaweeds() {
    for (auto& seaweed : seaweeds) {
        float sway = sin(seaweed.swayPhase) * 20;

        // стебель
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

        // листья
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

// рисование рыбы
void drawFish(Fish& fish) {
    glPushMatrix();
    glTranslatef(fish.x, fish.y, 0);

    if (!fish.movingRight) {
        glScalef(-1, 1, 1);
    }

    float tailWag = sin(fish.swimPhase) * 15;

    // хвост
    glColor3f(fish.r * 0.8f, fish.g * 0.8f, fish.b * 0.8f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-fish.size * 0.8f, 0);
    glVertex2f(-fish.size * 1.3f, fish.size * 0.5f + tailWag * 0.1f);
    glVertex2f(-fish.size * 1.3f, -fish.size * 0.5f + tailWag * 0.1f);
    glEnd();

    // тело
    glColor3f(fish.r, fish.g, fish.b);
    drawEllipse(0, 0, fish.size, fish.size * 0.6f);

    // плавники
    glColor3f(fish.r * 0.7f, fish.g * 0.7f, fish.b * 0.7f);
    // верхний плавник
    glBegin(GL_TRIANGLES);
    glVertex2f(-fish.size * 0.2f, fish.size * 0.4f);
    glVertex2f(fish.size * 0.2f, fish.size * 0.9f);
    glVertex2f(fish.size * 0.4f, fish.size * 0.4f);
    glEnd();
    // нижний плавник
    glBegin(GL_TRIANGLES);
    glVertex2f(-fish.size * 0.2f, -fish.size * 0.4f);
    glVertex2f(fish.size * 0.1f, -fish.size * 0.8f);
    glVertex2f(fish.size * 0.3f, -fish.size * 0.4f);
    glEnd();

    // глаз
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(fish.size * 0.5f, fish.size * 0.15f, fish.size * 0.15f);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(fish.size * 0.55f, fish.size * 0.15f, fish.size * 0.07f);

    // жабры
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

// рисование пузырька
void drawBubble(Bubble& bubble) {
    if (!bubble.active) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // тело пузырька
    glColor4f(0.8f, 0.9f, 1.0f, 0.4f);
    drawCircle(bubble.x, bubble.y, bubble.radius);

    // блик
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    drawCircle(bubble.x - bubble.radius * 0.3f, bubble.y + bubble.radius * 0.3f, bubble.radius * 0.25f);

    // обводка
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

// обновление пузырьков
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

    // генерация новых пузырьков
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

// обновление рыб
void updateFishes() {
    for (auto& fish : fishes) {
        fish.swimPhase += 0.1f;

        // движение по горизонтали
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

        // ограничение по вертикали
        if (fish.y > AQUARIUM_TOP - fish.size) fish.y = AQUARIUM_TOP - fish.size;
        if (fish.y < AQUARIUM_BOTTOM + fish.size + 50) fish.y = AQUARIUM_BOTTOM + fish.size + 50;

        // плавающее движение
        fish.y += sin(fish.swimPhase * 0.5f) * 0.5f;

        // создание пузырьков рыбами
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

// обновление водорослей
void updateSeaweeds() {
    for (auto& seaweed : seaweeds) {
        seaweed.swayPhase += seaweed.swaySpeed;
    }
}

// функция отрисовки
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

// таймер анимации
void timer(int value) {
    updateBubbles();
    updateFishes();
    updateSeaweeds();
    frameCount++;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// инициализация opengl
void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));

    glutInit(&argc, argv);
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

    cout << "=== Aquarium Simulation Started ===" << endl;
    cout << "Fish count: " << fishes.size() << endl;
    cout << "Seaweed count: " << seaweeds.size() << endl;

    glutMainLoop();
    return 0;
}
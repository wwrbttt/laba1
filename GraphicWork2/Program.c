#include <math.h>       // Мат. функции (fabs)
#include <glut.h>    // OpenGL UT (на Linux может быть <GL/glut.h>)
#include <stdlib.h>     // rand, srand, exit
#include <time.h>       // time
#include <stdio.h>      // NULL

// Размеры окна
int winWidth = 800;
int winHeight = 600;

// Структура падающего символа
typedef struct {
    char ch;           // Отображаемый символ
    float x, y;        // координаты
    float velocity;
    float colorR, colorG, colorB; // Цвет RGB
    int active;        // Используем int вместо bool
} FallingChar;

// Динамический массив на чистом C
FallingChar* chars = NULL;
int charsCount = 0;       // Текущее количество
int charsCapacity = 0;    // Вместимость массива

// Текст для выбора символов
char text[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
int textLength;           // Длина строки (вычисляется в main)
int currentIndex = 0;     // Указатель в строке

// Физические константы
const float GRAVITY = 0.05f;
const float MIN_SPEED = 1.0f;
const float MAX_SPEED = 3.0f;
const float DAMPING = 0.7f;

// Функция для добавления нового символа в массив
void addChar() {
    if (textLength == 0) return;

    // Создаем новый символ
    FallingChar fc;
    fc.ch = text[currentIndex];
    fc.x = (float)(rand() % (winWidth - 20) + 10);
    fc.y = (float)winHeight;
    fc.velocity = MIN_SPEED + (float)rand() / RAND_MAX * (MAX_SPEED - MIN_SPEED);
    fc.colorR = (float)(rand() % 256) / 255.0f;
    fc.colorG = (float)(rand() % 256) / 255.0f;
    fc.colorB = (float)(rand() % 256) / 255.0f;
    fc.active = 1;  // true

    // Увеличиваем массив при необходимости
    if (charsCount >= charsCapacity) {
        charsCapacity = charsCapacity == 0 ? 10 : charsCapacity * 2;
        chars = (FallingChar*)realloc(chars, charsCapacity * sizeof(FallingChar));
    }

    // Добавляем в конец
    chars[charsCount++] = fc;
    currentIndex = (currentIndex + 1) % textLength;
}

// Удаление неактивных символов
void removeInactiveChars() {
    int newCount = 0;
    for (int i = 0; i < charsCount; i++) {
        if (chars[i].active || chars[i].velocity != 0) {
            // Оставляем активные или движущиеся
            if (newCount != i) {
                chars[newCount] = chars[i];
            }
            newCount++;
        }
    }
    charsCount = newCount;
}

// Главная функция отрисовки и физики
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Обновление физики для всех символов
    for (int i = 0; i < charsCount; ++i) {
        if (chars[i].active) {
            chars[i].velocity += GRAVITY;
            chars[i].y -= chars[i].velocity;

            // Столкновение с нижней границей
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

    // Отрисовка символов
    for (int i = 0; i < charsCount; ++i) {
        if (chars[i].active || chars[i].velocity == 0) {
            glColor3f(chars[i].colorR, chars[i].colorG, chars[i].colorB);
            glRasterPos2f(chars[i].x, chars[i].y);
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, chars[i].ch);
        }
    }

    // Случайное добавление нового символа
    if (rand() % 10 == 0) {
        addChar();
    }

    // Удаление неактивных символов
    removeInactiveChars();

    glutSwapBuffers();
}

// Обработка изменения размера окна
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

// Обработка клавиатуры
void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27: // ESC - выход
        free(chars); // Освобождаем память перед выходом
        exit(0);
        break;
    case ' ': // ПРОБЕЛ - добавить 5 символов
        for (int i = 0; i < 5; ++i) {
            addChar();
        }
        break;
    }
}

// Таймер анимации
void Timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}

// Точка входа
int main(int argc, char** argv) {
    // Вычисляем длину строки
    textLength = sizeof(text) / sizeof(text[0]) - 1; // -1 для нуль-терминатора

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Falling Text (C version)");

    srand((unsigned int)time(NULL));

    // Настройка OpenGL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Создание начальных 20 символов
    for (int i = 0; i < 20; ++i) {
        addChar();
    }

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(0, Timer, 0);

    glutMainLoop();

    // Этот код выполнится только при выходе из glutMainLoop
    free(chars);
    return 0;
}
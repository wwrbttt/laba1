#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <glut.h>

using namespace std;

void runAquarium(int argc, char** argv);
extern "C" void runSalute(int argc, char** argv);

int main(int argc, char** argv) {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    srand(static_cast<unsigned>(time(0)));
    glutInit(&argc, argv);

    int choice = -1;

    while (choice != 0) {
        cout << "\n=== Выберите РГР ===" << endl;
        cout << "1. Аквариум с рыбами" << endl;
        cout << "2. Падающие символы" << endl;
        cout << "0. Выход" << endl;
        cout << "Ваш выбор: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
        case 1:
            runAquarium(argc, argv);
            break;
        case 2:
            runSalute(argc, argv);
            break;
        case 0:
            cout << "Выход." << endl;
            break;
        default:
            cout << "Неверный выбор." << endl;
            break;
        }
    }

    return 0;
}

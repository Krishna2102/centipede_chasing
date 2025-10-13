#include <GL/glut.h>
#include <bits/stdc++.h>
using namespace std;

const int GRID_W = 40;   // no. of columns
const int GRID_H = 30;   // no. of rows 
const int CELL = 20;     // cell size
const int WIN_W = GRID_W * CELL; // total for our window
const int WIN_H = GRID_H * CELL;

int interval_ms = 100; 
enum Dir { UP, DOWN, LEFT, RIGHT };

deque<pair<int,int>> snake; 
Dir dir = RIGHT;
bool gameOver = false;

pair<int,int> apple;
int score = 0;  

void placeApple() {
    while (true) {
        int x = rand() % GRID_W;
        int y = rand() % GRID_H;
        bool onSnake = false;
        for (auto &s : snake) if (s.first == x && s.second == y) { onSnake = true; break; }
        if (!onSnake) { apple = {x,y}; return; }
    }
}

void restartGame() {
    snake.clear();
    int startX = GRID_W / 4;
    int startY = GRID_H / 2;
    snake.push_back({startX, startY});
    snake.push_back({startX-1, startY});
    snake.push_back({startX-2, startY});
    dir = RIGHT;
    gameOver = false;
    score = 0;  
    placeApple();
}

bool collidesWithSelf(int x, int y) {
    for (auto &s : snake) if (s.first == x && s.second == y) return true;
    return false;
}

void moveSnake() {
    if (gameOver) return;
    auto head = snake.front();
    int nx = head.first;
    int ny = head.second;
    switch (dir) {
        case UP: ny += 1; break;
        case DOWN: ny -= 1; break;
        case LEFT: nx -= 1; break;
        case RIGHT: nx += 1; break;
    }
    if (nx < 0 || nx >= GRID_W || ny < 0 || ny >= GRID_H) { gameOver = true; return; }
    if (collidesWithSelf(nx, ny)) { gameOver = true; return; }

    snake.push_front({nx, ny});

    if (nx == apple.first && ny == apple.second) {
        score += 10;  
        placeApple();
    } else {
        snake.pop_back();
    }
}

void timerFunc(int v) {
    moveSnake();
    glutPostRedisplay();
    glutTimerFunc(interval_ms, timerFunc, 0);
}

void drawCell(int x, int y) {
    int px = x * CELL;
    int py = y * CELL;
    glBegin(GL_QUADS);
      glVertex2i(px, py);
      glVertex2i(px + CELL, py);
      glVertex2i(px + CELL, py + CELL);
      glVertex2i(px, py + CELL);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);
    drawCell(apple.first, apple.second);

    bool isHead = true;
    for (auto &s : snake) {
        if (isHead) {
            glColor3f(0.1f, 0.8f, 0.1f);
            isHead = false;
        } else {
            glColor3f(0.0f, 0.6f, 0.0f);
        }
        drawCell(s.first, s.second);
    }

    if (gameOver) {
        // Semi-transparent black overlay
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
        glBegin(GL_QUADS);
          glVertex2i(0,0); glVertex2i(WIN_W,0); glVertex2i(WIN_W,WIN_H); glVertex2i(0,WIN_H);
        glEnd();
        glDisable(GL_BLEND);

        string msg1 = "Game Over!";
        string msg2 = "Score: " + to_string(score);
        string msg3 = "Press 'R' to restart";

        glColor3f(1,1,1);

        glRasterPos2i(WIN_W/2 - 8 * msg1.size()/2, WIN_H/2 + 40);
        for (char c : msg1) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

        glRasterPos2i(WIN_W/2 - 8 * msg2.size()/2, WIN_H/2);
        for (char c : msg2) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

        glRasterPos2i(WIN_W/2 - 8 * msg3.size()/2, WIN_H/2 - 40);
        for (char c : msg3) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
}

void specialKeys(int key, int x, int y) {
    if (gameOver) return;
    Dir newDir = dir;
    switch (key) {
        case GLUT_KEY_UP: newDir = UP; break;
        case GLUT_KEY_DOWN: newDir = DOWN; break;
        case GLUT_KEY_LEFT: newDir = LEFT; break;
        case GLUT_KEY_RIGHT: newDir = RIGHT; break;
    }
    // prevent reversing
    if ((dir == UP && newDir == DOWN) || (dir == DOWN && newDir == UP) ||
        (dir == LEFT && newDir == RIGHT) || (dir == RIGHT && newDir == LEFT)) {
        return;
    }
    dir = newDir;
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC
    if (key == 'r' || key == 'R') restartGame();
}

void chooseDifficulty() {
    cout << "Choose difficulty: (1) Easy  (2) Medium  (3) Hard\n";
    int choice;
    cin >> choice;
    if (choice == 1) interval_ms = 150;
    else if (choice == 2) interval_ms = 100;
    else if (choice == 3) interval_ms = 60;
    else interval_ms = 100;
}

int main(int argc, char** argv) {
    srand((unsigned)time(nullptr));

    chooseDifficulty(); 

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("Snake - OpenGL (freeglut)");

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    reshape(WIN_W, WIN_H);
    restartGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(interval_ms, timerFunc, 0);
    glutMainLoop();
    return 0;
}


// for 1 apple 10 score
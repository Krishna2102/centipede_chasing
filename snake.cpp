#include <GL/glut.h>
#include <cmath>
#include <vector>

struct Segment {
    float x, y;
};

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int SEGMENT_COUNT = 20;
const float SPEED = 3.0f;
const float SEGMENT_DISTANCE = 15.0f;

std::vector<Segment> centipede(SEGMENT_COUNT, {400.0f, 300.0f});
float mouseX = 400.0f, mouseY = 300.0f;

void drawSegment(float x, float y, float size, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 20; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / 20.0f;
        float dx = size * cosf(theta);
        float dy = size * sinf(theta);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw segments
    for (size_t i = 0; i < centipede.size(); ++i) {
        float r = 0.2f + 0.8f * (1.0f - (float)i / centipede.size());
        float g = 1.0f;
        float b = 0.2f;
        drawSegment(centipede[i].x, centipede[i].y, 10.0f, r, g, b);
    }

    glutSwapBuffers();
}

void idle() {
    // Move the head towards the mouse
    float dx = mouseX - centipede[0].x;
    float dy = mouseY - centipede[0].y;
    float dist = sqrt(dx * dx + dy * dy);
    if (dist > 1.0f) {
        centipede[0].x += dx / dist * SPEED;
        centipede[0].y += dy / dist * SPEED;
    }

    // Move each segment to follow the one ahead of it
    for (size_t i = 1; i < centipede.size(); ++i) {
        float dx = centipede[i - 1].x - centipede[i].x;
        float dy = centipede[i - 1].y - centipede[i].y;
        float d = sqrt(dx * dx + dy * dy);
        if (d > SEGMENT_DISTANCE) {
            centipede[i].x += dx / d * SPEED;
            centipede[i].y += dy / d * SPEED;
        }
    }

    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    mouseX = (float)x;
    mouseY = WINDOW_HEIGHT - (float)y;
}

void init() {
    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Centipede Follows Mouse - OpenGL");

    init();
    glutDisplayFunc(display);
    glutPassiveMotionFunc(mouseMotion);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}

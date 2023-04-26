#include "CGame.h"
#include "Vector2.h"
#include <cstdlib>
#include <iostream>
#include <fstream>

const struct Color Color::Black = { 0, 0, 0, 0 };
const struct Color Color::White = { 1, 1, 1, 1 };
const struct Color Color::AlphaGrey = { 0.5f, 0.5f, 0.5f, 0.3f };
const struct Color Color::Red = { 1, 0, 0, 1 };
const struct Color Color::AlphaRed = { 1, 0, 0, 0.3 };
const struct Color Color::Green = { 0, 1, 0, 1 };
const struct Color Color::AlphaGreen = { 0, 1, 0, 0.3 };

void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    CGame* game = static_cast<CGame*>(glfwGetWindowUserPointer(window));
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    game->inputBuffer.push_back({ button, action, Vector2(x, y) });
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    CGame* game = static_cast<CGame*>(glfwGetWindowUserPointer(window));
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    game->inputBuffer.push_back({ key, action, Vector2(x, -y) });
}

CGame::CGame(int frameT) :
    frame_time(frameT), current_frame(0), frame_count(frameT)
{}

CGame::~CGame()
{
    delete cities;
    cities = nullptr;
}

bool CGame::init(int windowW, int windowH, std::ofstream& archivo)
{
    /* Config */
    int numCities, popSize, mutChance;
    std::cout << "Enter number of cities: ";
    std::cin >> numCities;
    std::cout << "Enter population size: ";
    std::cin >> popSize;
    std::cout << "Enter mutation chance: ";
    std::cin >> mutChance;
    
    archivo << "number of cities: " << numCities << "\n";
    archivo << "population size: " << popSize << "\n";
    archivo << "mutation chance: " << mutChance << "\n";

    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    wWidth = windowW;
    wHeight = windowH;
    window = glfwCreateWindow(windowW, windowH, "Checkers", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Extra configurations */
    glClearColor(0, 0, 0, 1);
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Create cities */
    srand(time(0));
    auto c = createRandomCities(numCities);

    cities = new GeneticAlg(c, popSize, mutChance);

    return true;
}

void CGame::run(std::ofstream& archivo)
{
    archivo << "Epoch,MinAptitud\n";
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        if (frame_count == frame_time)
        {
            frame_count = 0;
            current_frame++;
            update();
            archivo << epoch << "," << cities->minAptitud << "\n";
        }
        frame_count++;
    }
    archivo.close();
    glfwTerminate();
}

const int CGame::getHeight() const
{
    return wHeight;
}

const int CGame::getWidth() const
{
    return wWidth;
}

void CGame::beginDrawing()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void CGame::endDrawing()
{
    glfwSwapBuffers(window);
    inputBuffer.clear();
    glfwPollEvents();
}

Vector2 CGame::getMouseLocation()
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return Vector2(x, y);
}

void CGame::drawLine(const Vector2& a, const Vector2& b, Color color)
{
    glBegin(GL_LINES);
        glColor4f(color.r, color.g, color.b, color.a);
        auto _a = a.toOpenGlCoor(this);
        auto _b = b.toOpenGlCoor(this);
        glVertex2f(_a.x, _a.y);
        glVertex2f(_b.x, _b.y);
    glEnd();
}

void CGame::drawLines(const std::vector<Vector2>& points, Color color)
{
    glBegin(GL_LINES);
        glColor4f(color.r, color.g, color.b, color.a);
        for (int i = 0; i < points.size(); i++)
        {
            auto p = points[i].toOpenGlCoor(this);
            glVertex2f(p.x, p.y);
        }
    glEnd();
}

void CGame::drawContLines(const std::vector<Vector2>& points, Color color)
{
    glBegin(GL_LINE_STRIP);
        glColor4f(color.r, color.g, color.b, color.a);
        for (int i = 0; i < points.size(); i++)
        {
            auto p = points[i].toOpenGlCoor(this);
            glVertex2f(p.x, p.y);
        }
    glEnd();
}

void CGame::drawLineLoop(const std::vector<Vector2>& points, Color color)
{
    glBegin(GL_LINE_LOOP);
    glColor4f(color.r, color.g, color.b, color.a);
    for (int i = 0; i < points.size(); i++)
    {
        auto p = points[i].toOpenGlCoor(this);
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

void CGame::drawRects(const std::vector<Vector2>& points, Color color)
{
    glBegin(GL_QUADS);
        glColor4f(color.r, color.g, color.b, color.a);
        for (int i = 0; i < points.size(); i++)
        {
            auto p = points[i].toOpenGlCoor(this);
            glVertex2f(p.x, p.y);
        }
    glEnd();
}

void CGame::drawCircle(const Vector2& pos, const Vector2& length, Color color)
{
    int i;
    int triangleAmount = 30; //# of triangles used to draw circle

    float twicePi = 2.0f * 3.1416f;
    auto newPos = pos.toOpenGlCoor(this);
    auto newLength = length / 2.0f;
    newLength = newLength.toOpenGlCoor(this) * Vector2(1, -1) + Vector2(1, 1);

    glBegin(GL_TRIANGLE_FAN);
        glColor4f(color.r, color.g, color.b, color.a);
        glVertex2f(newPos.x, newPos.y); // center of circle
        for (i = 0; i <= triangleAmount; i++)
        {
            glVertex2f(
                newPos.x + (newLength.x * cos(i * twicePi / triangleAmount)),
                newPos.y + (newLength.y * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();
}

void CGame::drawHollowCircle(const Vector2& pos, const Vector2& length, Color color)
{
    int i;
    int lineAmount = 30; //# of triangles used to draw circle

    float twicePi = 2.0f * 3.1416f;
    auto newPos = pos.toOpenGlCoor(this);
    auto newLength = length / 2.0f;
    newLength = newLength.toOpenGlCoor(this) * Vector2(1, -1) + Vector2(1, 1);

    glBegin(GL_LINE_LOOP);
        glColor4f(color.r, color.g, color.b, color.a);
        for (i = 0; i <= lineAmount; i++) {
            glVertex2f(
                newPos.x + (newLength.x * cos(i * twicePi / lineAmount)),
                newPos.y + (newLength.y * sin(i * twicePi / lineAmount))
            );
        }
    glEnd();
}

std::vector<Vector2> CGame::createRandomCities(int n)
{
    std::vector<Vector2> c(n);
    for (int i = 0; i < n; i++)
    {
        c[i] = (Vector2(wWidth, wHeight).randInt());
    }
    return c;
}

void CGame::update()
{
    bool process = !paused;
    for (auto& input : inputBuffer)
    {
        if (input.action == GLFW_PRESS && input.key == GLFW_KEY_RIGHT)
            process = true;
        if (input.action == GLFW_PRESS && input.key == GLFW_KEY_P)
            paused = !paused;
    }

    if (process)
        cities->update();

    beginDrawing();

    for (auto& city : cities->nodes)
    {
        drawCircle(city, Vector2(10, 10));
    }

    for (auto& path : cities->population)
    {
        drawLineLoop(cities->getNodesFromPath(path), Color::AlphaGrey);
    }

    drawLineLoop(cities->getNodesFromPath(cities->bestPath), Color::Green);

    if (process) {
        std::cout << epoch << " - " << cities->minAptitud << '\n';
        epoch++;
    }

    endDrawing();
}
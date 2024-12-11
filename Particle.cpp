#include "Particle.h"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace  sf;
using namespace std;
Particle::Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition)
    : m_A(2, numPoints) {

    m_ttl = TTL;

    m_numPoints = numPoints;

    // Initialize random angular velocity
    m_radiansPerSec = static_cast<float>(rand()) / RAND_MAX * M_PI;

    // Map mouse click position to Cartesian coordinates
    m_cartesianPlane = target.getDefaultView();
    m_cartesianPlane.setCenter(0, 0);
    m_cartesianPlane.setSize(target.getSize().x, (-1.0) * target.getSize().y);

    // Map mouse click position to the Cartesian coordinate system
    m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

    // Random pixel velocities
    m_vx = (rand() % 401 + 100) * (rand() % 2 == 0 ? 1 : -1); // Between 100 and 500
    m_vy = (rand() % 401 + 100);// *(rand() % 2 == 0 ? 1 : -1); // Between 100 and 500

    // Colors
    m_color1 = Color::White;
    m_color2 = Color(rand() % 256, rand() % 256, rand() % 256);

    // Generate vertices
    float theta = static_cast<float>(rand()) / RAND_MAX * (M_PI / 2);
    float dTheta = (numPoints > 1) ? (2 * M_PI / (numPoints - 1)) : 0; // Avoid division by zero
    for (int j = 0; j < numPoints; ++j)
    {
        float r, dx, dy;
        r = rand() % 60 + 20;
        dx = r * cos(theta);
        dy = r * sin(theta);
        m_A(0, j) = m_centerCoordinate.x + dx;
        m_A(1, j) = m_centerCoordinate.y + dy;
        theta += dTheta;
    }
}

void Particle::draw(RenderTarget& target, RenderStates states) const {
    // Ensure there are points to draw
    if (m_numPoints < 1)
        return; // Nothing to draw
  

    // Create a VertexArray for drawing the particle
    VertexArray lines(PrimitiveType::TriangleFan, m_numPoints + 1);

    // Map the center coordinate to pixel coordinates
    Vector2f center = Vector2f(target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane));
    lines[0].position = center; // Set the center position
    lines[0].color = m_color1;  // Set the center color

    // Loop through each vertex and set its position and color
    for (int j = 1; j <= m_numPoints; ++j) {
        // Access the vertex in the Matrix m_A
        Vector2f vertex = Vector2f(target.mapCoordsToPixel(Vector2f(m_A(0, j - 1), m_A(1, j - 1)), m_cartesianPlane));
        lines[j].position = vertex; // Set vertex position
        lines[j].color = m_color2;  // Set vertex color
    }

    // Draw the VertexArray to the target
    target.draw(lines, states);
}

void Particle::update(float dt) {
    m_ttl -= dt;

    rotate(dt * m_radiansPerSec);
    scale(SCALE);

    float dx = m_vx * dt;
    m_vy -= G * dt; // Apply gravity
    float dy = m_vy * dt;

    translate(dx, dy);
}


bool Particle::almostEqual(double a, double b, double eps)
{
    return fabs(a - b) < eps;
}

void Particle::unitTests()
{
    int score = 0;

    cout << "Testing RotationMatrix constructor...";
    double theta = M_PI / 4.0;
    RotationMatrix r(M_PI / 4);
    if (r.getRows() == 2 && r.getCols() == 2 && almostEqual(r(0, 0), cos(theta))
        && almostEqual(r(0, 1), -sin(theta))
        && almostEqual(r(1, 0), sin(theta))
        && almostEqual(r(1, 1), cos(theta)))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing ScalingMatrix constructor...";
    ScalingMatrix s(1.5);
    if (s.getRows() == 2 && s.getCols() == 2
        && almostEqual(s(0, 0), 1.5)
        && almostEqual(s(0, 1), 0)
        && almostEqual(s(1, 0), 0)
        && almostEqual(s(1, 1), 1.5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Testing TranslationMatrix constructor...";
    TranslationMatrix t(5, -5, 3);
    if (t.getRows() == 2 && t.getCols() == 3
        && almostEqual(t(0, 0), 5)
        && almostEqual(t(1, 0), -5)
        && almostEqual(t(0, 1), 5)
        && almostEqual(t(1, 1), -5)
        && almostEqual(t(0, 2), 5)
        && almostEqual(t(1, 2), -5))
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }


    cout << "Testing Particles..." << endl;
    cout << "Testing Particle mapping to Cartesian origin..." << endl;
    if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0)
    {
        cout << "Failed.  Expected (0,0).  Received: (" << m_centerCoordinate.x << "," << m_centerCoordinate.y << ")" << endl;
    }
    else
    {
        cout << "Passed.  +1" << endl;
        score++;
    }

    cout << "Applying one rotation of 90 degrees about the origin..." << endl;
    Matrix initialCoords = m_A;
    rotate(M_PI / 2.0);
    bool rotationPassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), -initialCoords(1, j)) || !almostEqual(m_A(1, j), initialCoords(0, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            rotationPassed = false;
        }
    }
    if (rotationPassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a scale of 0.5..." << endl;
    initialCoords = m_A;
    scale(0.5);
    bool scalePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0,j)) || !almostEqual(m_A(1, j), 0.5 * initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            scalePassed = false;
        }
    }
    if (scalePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Applying a translation of (10, 5)..." << endl;
    initialCoords = m_A;
    translate(10, 5);
    bool translatePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++)
    {
        if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) || !almostEqual(m_A(1, j), 5 + initialCoords(1, j)))
        {
            cout << "Failed mapping: ";
            cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << endl;
            translatePassed = false;
        }
    }
    if (translatePassed)
    {
        cout << "Passed.  +1" << endl;
        score++;
    }
    else
    {
        cout << "Failed." << endl;
    }

    cout << "Score: " << score << " / 7" << endl;
}

void Particle::rotate(double theta) {
    Vector2f temp = m_centerCoordinate; // Store the center coordinate
    translate(-temp.x, -temp.y); // Move to origin

    RotationMatrix R(theta); // Create rotation matrix
    m_A = R * m_A; // Apply rotation

    translate(temp.x, temp.y); // Move back to original position
}

void Particle::scale(double c) {
    Vector2f temp = m_centerCoordinate; // Store the center coordinate
    translate(-temp.x, -temp.y); // Move to origin

    ScalingMatrix S(c); // Create scaling matrix
    m_A = S * m_A; // Apply scaling

    translate(temp.x, temp.y); // Move back to original position
}

void Particle::translate(double xShift, double yShift) {
    TranslationMatrix T(xShift, yShift, m_A.getCols()); // Create translation matrix
    m_A = T + m_A; // Apply translation

    // Update the center coordinate
    m_centerCoordinate.x += xShift;
    m_centerCoordinate.y += yShift;
}
#include "Engine.h"


void Engine::input()
{
    Event event;
    while (m_Window.pollEvent(event))
    {
        if (event.type == Event::Closed)
            m_Window.close();

        if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            m_Window.close();

        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
        {
            // Create 5 particles on left mouse button press
            for (int i = 0; i < 5; ++i)
            {
                int numPoints = rand() % (50 - 25 + 1) + 25; // Random number between 25 and 50
                Vector2f position(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                m_particles.emplace_back(m_Window, numPoints, position);
            }
        }
    }
}

void Engine::update(float dtAsSeconds)
{
    for (auto it = m_particles.begin(); it != m_particles.end();)
    {
        if (it->getTTL() > 0.0f)
        {
            it->update(dtAsSeconds);
            ++it; // Increment the iterator
        }
        else
        {
            it = m_particles.erase(it); // Erase returns the next iterator
        }
    }
}

void Engine::draw()
{
    m_Window.clear(); // Clear the window

    // Draw each particle
    for (const auto& particle : m_particles)
    {
        m_Window.draw(particle); // Uses polymorphism to call Particle::draw()
    }

    m_Window.display(); // Display the window
}

Engine::Engine()
{
    // Create the window with desktop resolution
    m_Window.create(VideoMode::getDesktopMode(), "Particle Engine");
}

void Engine::run()
{
    Clock clock;

    // Output message for unit tests
    cout << "Starting Particle unit tests..." << endl;
    Particle p(m_Window, 4, { (int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2 });
    p.unitTests();
    cout << "Unit tests complete. Starting engine..." << endl;

    // Main game loop
    while (m_Window.isOpen())
    {
        // Restart the clock and get the time elapsed since the last frame
        Time dt = clock.restart();
        float dtAsSeconds = dt.asSeconds();

        input();
        update(dtAsSeconds);
        draw();
    }
}



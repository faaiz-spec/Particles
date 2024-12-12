#include "Engine.h"


Engine::Engine()
{
    // Create the window with desktop resolution
    m_Window.create(VideoMode::getDesktopMode(), "Particle Engine");

    m_color = 0;
    m_curr_sound = 0;

    for (int i = 0; i < AUDIO_COUNT; ++i)
        if (!m_audiofile[i].loadFromFile("audio\\firework_" + to_string(i) + ".mp3"))
            cout << "Failed to load audio\\firework_" << i << ".mp3 from file\n";

    for (int i = 0; i < CHANNEL_COUNT; ++i)
        if (i % AUDIO_COUNT == 0)
            m_firework[i].setBuffer(m_audiofile[0]);
        else if (i % AUDIO_COUNT == 1)
            m_firework[i].setBuffer(m_audiofile[1]);
        else if (i % AUDIO_COUNT == 2)
            m_firework[i].setBuffer(m_audiofile[2]);
        else if (i % AUDIO_COUNT == 3)
            m_firework[i].setBuffer(m_audiofile[3]);
        else
            m_firework[i].setBuffer(m_audiofile[4]);
}

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
            m_firework[m_curr_sound].play();
            if (++m_curr_sound == CHANNEL_COUNT) m_curr_sound = 0;
            // Create 5 particles on left mouse button press
            for (int i = 0; i < PARTICLE_COUNT; ++i)
            {
                if ((m_color += 30) >= SLIDER_MAX) m_color = 0;
                int numPoints = (rand() % 26) + 25; // Random number between 25 and 50
                Vector2i position((event.mouseButton.x), (event.mouseButton.y));
                m_particles.emplace_back(m_Window, numPoints, position, m_color);
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
        update(dtAsSeconds / 2);
        draw();
    }
}



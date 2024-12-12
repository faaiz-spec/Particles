#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Particle.h"
#include <vector>
#include <iostream>
#include <chrono>
using namespace sf;
using namespace std;

#define CHANNEL_COUNT 20
#define AUDIO_COUNT 5
const int PARTICLE_COUNT = 5;

class Engine
{
private:
	// A regular RenderWindow
	RenderWindow m_Window;
	SoundBuffer m_audiofile[AUDIO_COUNT];
	Sound m_firework[CHANNEL_COUNT];
	int m_curr_sound;
	int m_color;

	//vector for Particles
	vector<Particle> m_particles;

	// Private functions for internal use only
	void input();
	void update(float dtAsSeconds);
	void draw();

public:
	// The Engine constructor
	Engine();

	// Run will call all the private functions
	void run();

};
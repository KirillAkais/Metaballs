#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"

using namespace sf;
using namespace std;

#define WINDOW_W 2560
#define WINDOW_H 1440
#define ASP_RAT 1.7777777778
#define G 167
#define PART_COUNT 100

double rand_double()
{
	return ((double)rand() / (double)RAND_MAX);
}

Vector2f mouse_pos(RenderWindow &window)
{
	return Vector2f((float(Mouse::getPosition(window).x) / WINDOW_W - 0.5) * ASP_RAT, 0.5 - float(Mouse::getPosition(window).y) / WINDOW_H);
}

b2Vec2 operator* (b2Vec2 v, double x)
{
	return b2Vec2(v.x * x, v.y * x);
}

b2Vec2 operator/ (b2Vec2 v, double x)
{
	return b2Vec2(v.x / x, v.y / x);
}

void main()
{
	srand((unsigned)time(NULL));
	//Load the shader
	Shader shader;
	shader.loadFromFile("shader.frag", Shader::Fragment);
	RectangleShape rect;
	rect.setFillColor(Color::Green);
	rect.setSize(Vector2f(WINDOW_W, WINDOW_H));
	//Load background
	Texture background_texture;
	background_texture.loadFromFile("background.png");
	Sprite background;
	background.setTexture(background_texture);
	//Create world
	b2World *world;
	vector<b2Body*> particles;
	b2Vec2 gravity(0, 0);
	world = new b2World(gravity);
	//Create BodyDef
	b2BodyDef bd;
	bd.position.Set(0.0f, 0.0f);
	b2Body* ground = world->CreateBody(&bd);
	//Create box shape
	b2EdgeShape shape;
	//Create box FixtureDef
	b2FixtureDef sd;
	sd.shape = &shape;
	sd.density = 0.0f;
	sd.restitution = 1;
	//Create box
	shape.Set(b2Vec2(-50.0f * ASP_RAT, -50.0f), b2Vec2(-50.0f * ASP_RAT, 50.0f));
	ground->CreateFixture(&sd);

	shape.Set(b2Vec2(50.0f * ASP_RAT, -50.0f), b2Vec2(50.0f * ASP_RAT, 50.0f));
	ground->CreateFixture(&sd);

	shape.Set(b2Vec2(-50.0f * ASP_RAT, 50.0f), b2Vec2(50.0f * ASP_RAT, 50.0f));
	ground->CreateFixture(&sd);

	shape.Set(b2Vec2(-50.0f * ASP_RAT, -50.0f), b2Vec2(50.0f * ASP_RAT, -50.0f));
	ground->CreateFixture(&sd);

	//Create particle FixtureDef
	b2FixtureDef fd;
	fd.density = 3;
	fd.restitution = 1.0f;
	fd.friction = 0.0f;

	float *p_rad = new float[PART_COUNT];
	//Create all the particles
	for (int i = 0; i < PART_COUNT; i++)
	{
		//Set random position and direction
		double x = rand_double() * 100 - 50;
		double y = rand_double() * 100 - 50;
		double vx = rand_double() * 2 - 1;
		double vy = rand_double() * 2 - 1;
		//Add particle to the world
		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(x * ASP_RAT, y);
		particles.push_back(world->CreateBody(&bd));
		//Set random size
		b2CircleShape s;
		p_rad[i] = rand_double() * 2 + 0.5;
		s.m_radius = p_rad[i] / 5.0f;
		//p_rad[i] = 0.5f;
		//s.m_radius = 0.1f;
		fd.shape = &s;
		//Set equal mass
		particles[i]->CreateFixture(&fd);
		b2MassData mass;
		mass.mass = 1;
		mass.center = b2Vec2(0, 0);
		//particles[i]->SetMassData(&mass);
		particles[i]->SetBullet(true);
		//Set equal velocity
		b2Vec2 vel = b2Vec2(vx, vy);
		vel.Normalize();
		vel = b2Vec2(vel.x * 20, vel.y * 20);
		particles[i]->SetLinearVelocity(vel);
		particles[i]->SetLinearDamping(0.0f);
	}
	//Create window
	RenderWindow window(VideoMode(WINDOW_W, WINDOW_H), "window", Style::None);
	window.setVerticalSyncEnabled(true);
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case(Event::Closed):
				window.close();
				break;
			default:
				break;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
		world->Step(1.0 / 120.0, 8, 4);

		Vector2f *p_pos = new Vector2f[particles.size()];
		for (int i = 0; i < PART_COUNT; i++)
		{
			//Apply gravity to all particles
			for (int j = 0; j < PART_COUNT; j++)
			{
				if (j != i)
				{
					b2Vec2 dir = particles[i]->GetPosition() - particles[j]->GetPosition();
					double dist = dir.Length();
					dir.Normalize();
					particles[j]->ApplyForceToCenter(dir * G * particles[i]->GetMass() * particles[j]->GetMass() / (dist * dist), true);
				}
			}
			b2Vec2 pos = particles[i]->GetPosition();
			p_pos[i].x = pos.x / 50.0;
			p_pos[i].y = pos.y / 50.0;
		}
		//Set uniforms for the shader
		shader.setUniform("iResolution", Vector2f(WINDOW_W, WINDOW_H));
		shader.setUniformArray("iPartPos", p_pos, PART_COUNT);
		shader.setUniformArray("iPartRad", p_rad, PART_COUNT);
		//Draw everything
		window.clear(Color::White);
		window.draw(background);
		window.draw(rect, &shader);
		window.display();
		sleep(seconds(1.0 / 60.0));

	}
}
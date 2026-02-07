#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>



class GravitySource
{
    sf::Vector2f pos;
    float strength;
    sf::CircleShape s;

public:
    GravitySource(float pos_x, float pos_y, float strength)
    {
        pos.x = pos_x;
        pos.y = pos_y;
        this->strength = strength;
 
        s.setPosition(pos);
        s.setFillColor(sf::Color::White);
        s.setRadius(4);
    }

	void render(sf::RenderWindow& window) {
		window.draw(s);

    }
	sf::Vector2f getPos() {
        return pos;
	}

    void update_physics(){


    }

    float getStrength() {
        return strength;
	}
};

class Particle
{
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::CircleShape s;
public:
    Particle(float pos_x, float pos_y, float vel_x, float vel_y) {
        pos.x = pos_x;
        pos.y = pos_y;

        vel.x = vel_x;
        vel.y = vel_y;

        s.setPosition(pos);
        s.setFillColor(sf::Color::Red);
        s.setRadius(3);
    }
    void render(sf::RenderWindow& window) {
        s.setPosition(pos);
        window.draw(s);
    }
    void set_color(sf::Color color) {
        s.setFillColor(color);
	}
    void update_physics(GravitySource& s) {
        float distance_x = s.getPos().x - pos.x;
        float distance_y = s.getPos().y - pos.y;
        float distance = sqrt(distance_x * distance_x + distance_y * distance_y);

        float inverse_distance = 1.0f / distance;

        float normalized_x = distance_x * inverse_distance;
        float normalized_y = distance_y * inverse_distance;
        float inverse_distance_squared = inverse_distance * inverse_distance;
        float acceleration_x = normalized_x * s.getStrength() * inverse_distance_squared;
        float acceleration_y = normalized_y * s.getStrength() * inverse_distance_squared;
        vel.x += acceleration_x;
        vel.y += acceleration_y;

        pos.x += vel.x;
        pos.y += vel.y;

    }
};  

sf::Color map_val_to_color(float val) {
    if (val < 0.0) val = 0;
	if (val > 1.0) val = 1;
	int r = 0, g = 0, b = 0;
    if (val < 0.5) {
		b = 255 * (1.0f - 2 * val);
		g = 255 * 2 * val;
    } else {
		g = 255 * (2.0f - 2 * val);
		r = 255 * (2 * val - 1.0f);
	}
	return sf::Color(r, g, b);
}

/*
int main()
{
    sf::RenderWindow window(sf::VideoMode(1600, 1000), "SFML works!");
	window.setFramerateLimit(60);
	//GravitySource source(800, 500, 2000);
	//Particle particle(600, 300, 4, 0);

	std::vector<GravitySource> sources;
	sources.push_back(GravitySource(500, 500, 7000));
	sources.push_back(GravitySource(1200, 500, 7000));
	int num_particles = 1000;
	std::vector<Particle> particles;
    for (int i = 0; i < num_particles; i++) {
        particles.push_back(Particle(600, 700, 4, 0.2 + (0.1 / num_particles) * i));
		float val = (float)i / (float)num_particles;
		sf::Color col = map_val_to_color(val);
		particles[i].set_color(col);
    }
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
		for (int i = 0; i < sources.size(); i++) {
			for (int j = 0; j < particles.size(); j++) {
                particles[j].update_physics(sources[i]);
            }
		}
        for (int i = 0; i < sources.size(); i++) {
			sources[i].render(window);
            }
		for (int i = 0; i < particles.size(); i++) {
            particles[i].render(window);
		}
		window.display();
    }

    return 0;
}

*/
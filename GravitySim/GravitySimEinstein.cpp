#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm> 

const double G = 6.67430e-11;
const double SCALE = 1.0e6;
const double TIME_SPEED = 50500.0;
const double VISUAL_G_FACTOR = 1.5e13;

struct Body {
    double mass;
    double radius;
    sf::CircleShape s;
    sf::Vector2f position;
    sf::Vector2f velocity;

    Body(double mass, double radius, sf::Color color) {
        this->mass = mass;
        this->radius = radius;
        float visualRadius = (radius / SCALE) * 2.5f;
        if (visualRadius < 3.0f) visualRadius = 3.0f;

        s.setRadius(visualRadius);
        s.setOrigin(visualRadius, visualRadius);
        s.setFillColor(color);
        velocity = sf::Vector2f(0, 0);
    }

    void set_position(double x, double y) {
        position = sf::Vector2f(x, y);
        s.setPosition(position);
    }

    void render(sf::RenderWindow& window) {
        s.setPosition(position);
        window.draw(s);
    }
};

/*
struct Schwarzschild {
    double rs;

    Schwarzschild(double mass) {
        rs = (2.0 * G * mass) / std::pow(c, 2);
    }
    double get_g_tt(double r) {
        return -(1.0 - (rs / r));
    }
    double get_g_rr(double r) {
        return 1.0 / (1.0 - (rs / r));
    }
};
*/
class SpacetimeGrid {
    sf::VertexArray lines;
    std::vector<sf::Vector2f> original_positions;
    int width, height, spacing;

public:
    SpacetimeGrid(int w, int h, int space) : width(w), height(h), spacing(space) {
        lines.setPrimitiveType(sf::Lines);
        int buffer = 3000;

        for (int x = -buffer; x <= width + buffer; x += spacing) {
            for (int y = -buffer; y < height + buffer; y += spacing) {
                lines.append(sf::Vertex(sf::Vector2f(x, y), sf::Color(40, 40, 40)));
                original_positions.push_back(sf::Vector2f(x, y));
                lines.append(sf::Vertex(sf::Vector2f(x, y + spacing), sf::Color(40, 40, 40)));
                original_positions.push_back(sf::Vector2f(x, y + spacing));

                lines.append(sf::Vertex(sf::Vector2f(x, y), sf::Color(40, 40, 40)));
                original_positions.push_back(sf::Vector2f(x, y));
                lines.append(sf::Vertex(sf::Vector2f(x + spacing, y), sf::Color(40, 40, 40)));
                original_positions.push_back(sf::Vector2f(x + spacing, y));
            }
        }
    }
    sf::Vector2f getDisplacedPosition(sf::Vector2f p, const Body& b1, const Body& b2) const {
        float dist_min = 0.0f;
        float dist_max = 3000.0f;
        float strength_max = 0.5f;
        float strength_min = 0.0f;

        sf::Vector2f delta1 = b1.position - p;
        float dist1 = std::sqrt(delta1.x * delta1.x + delta1.y * delta1.y);
        float t1 = (std::clamp(dist1, dist_min, dist_max) - dist_min) / (dist_max - dist_min);
        float strength1 = strength_max + t1 * (strength_min - strength_max);

        sf::Vector2f delta2 = b2.position - p;
        float dist2 = std::sqrt(delta2.x * delta2.x + delta2.y * delta2.y);
        float t2 = (std::clamp(dist2, dist_min, dist_max) - dist_min) / (dist_max - dist_min);
        float strength2 = strength_max + t2 * (strength_min - strength_max);

        return p + (delta1 * strength1) + (delta2 * strength2);
    }
    void update(const Body& massiveBody, const Body& orbitBody ) {
       //sf::Vector2f center = massiveBody.position;
        //sf::Vector2f center = orbitBody.position;
        double mass = massiveBody.mass;
        double mass2 = orbitBody.mass;
        float dist_min = 0.0f;
        float dist_max = 3000.0f;
        float strength_max = 0.5f; 
        float strength_min = 0.0f;   

        for (size_t i = 0; i < lines.getVertexCount(); ++i) {
            sf::Vector2f p = original_positions[i];
            sf::Vector2f delta1 = massiveBody.position - p;
            double dist1 = std::sqrt(delta1.x * delta1.x + delta1.y * delta1.y);
            float t1 = (std::clamp((float)dist1, dist_min, dist_max) - dist_min) / (dist_max - dist_min);
            float strength1 = strength_max + t1 * (strength_min - strength_max);
            //double strength = (G * mass * VISUAL_G_FACTOR) / (effectiveDist * effectiveDist * effectiveDist); Shit too large not using.
           // printf("Delta: (%.2f, %.2f), Dist: %.2f, Strength: %.5e\n", delta.x, delta.y, dist, strength);
            sf::Vector2f delta2 = orbitBody.position - p;
            double dist2 = std::sqrt(delta2.x * delta2.x + delta2.y * delta2.y);
            float t2 = (std::clamp((float)dist2, dist_min, dist_max) - dist_min) / (dist_max - dist_min);
            float strength2 = strength_max + t2 * (strength_min - strength_max);
           //map the strength to a reasonable range for visualization
         

            lines[i].position.x = p.x + (delta1.x * strength1) + (delta2.x * strength2);
            lines[i].position.y = p.y + (delta1.y * strength1) + (delta2.y * strength2);
        }
    }




    void render(sf::RenderWindow& window) {
        window.draw(lines);
    }
};

struct LightRay {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::VertexArray path;
    float distanceTraveled;
    bool active = true;

    LightRay(float yStart) {
        position = sf::Vector2f(-800.0f, -yStart);
        velocity = sf::Vector2f(8.0f, 0.0f); // Speed of light (pixels per frame)
        path.setPrimitiveType(sf::LineStrip);
    }

    void update(const SpacetimeGrid& grid, const Body& b1, const Body& b2) {
        if (!active) return;
        position += velocity;
       
       // if (position.x == b1.position.x || position.y == b1.position.y);

  
        sf::Vector2f warpedPos = grid.getDisplacedPosition(position, b1, b2);
       // distanceTraveled += warpedPos.x;
       // float  deltaX = b1.position.x - warpedPos.x;
        //printf("deltaX %.2f Position X %.2f Raidus %.2f", deltaX,b1.position.x, b1.radius);

        sf::Vector2f delta = b1.position - warpedPos;
        sf::Vector2f delta2 = b2.position - warpedPos;
        float dist2 = std::sqrt(delta2.x * delta2.x + delta2.y * delta2.y);
        float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        float alpha = std::clamp(255.0f * (1.0f - (warpedPos.x+300.0f) / 2200.0f), 0.0f, 255.0f);
        if (alpha < 0) alpha = 0;
        sf::Color fadeColor(255, 255, 100, static_cast<sf::Uint8>(-alpha));
     
        path.append(sf::Vertex(warpedPos, fadeColor));
        if (dist < b1.radius/SCALE || dist2 < b2.radius/SCALE) active = false;
        if (warpedPos.x > 1600) active = false;

    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 1000), "Orbital Mechanics & Spacetime");
    window.setFramerateLimit(60);
    std::vector<LightRay> rays;
    // 1. Setup Bodies
    double mass_earth = 5.972e24;
    double radius_earth = 6.371e6;
    double mass_moon = 7.342e22;
    double radius_moon = 1.737e6;

    Body Earth(mass_earth, radius_earth, sf::Color(100, 149, 237));
    Body Moon(mass_moon, radius_moon, sf::Color(220, 220, 220));


    Earth.set_position(800, 500);


    double dist_moon_m = 3.844e8;
    double dist_moon_px = dist_moon_m / SCALE;
    Moon.set_position(800 + dist_moon_px, 500);

    double velocity_moon = std::sqrt(G * mass_earth / dist_moon_m);
    Moon.velocity = sf::Vector2f(0, velocity_moon); 


    SpacetimeGrid grid(1600, 1000, 50);
    sf::Clock clock;

    sf::VertexArray trail(sf::LineStrip);
    sf::VertexArray trailEarth(sf::LinesStrip);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::B) {
                    for (int i = -10; i < 10; ++i) {
                        rays.push_back(LightRay(i*80));
                    }
                }
            }
        }
        if (event.type == sf::Event::MouseMoved) {
         // Earth.set_position(event.mouseMove.x, event.mouseMove.y);
        }

        double dt_seconds = clock.restart().asSeconds();
  
        double dt = dt_seconds * TIME_SPEED;

   
        double dx = (Earth.position.x - Moon.position.x) * SCALE;
        double dy = (Earth.position.y - Moon.position.y) * SCALE;
        double distance_sq = dx * dx + dy * dy;
        double distance = std::sqrt(distance_sq);


        double force = (G * mass_earth * mass_moon) / distance_sq;

        double force_x = force * (dx / distance);
        double force_y = force * (dy / distance);

    
        Moon.velocity.x += (force_x / mass_moon) * dt;
        Moon.velocity.y += (force_y / mass_moon) * dt;

        Earth.velocity.x -= (force_x / mass_earth) * dt;
        Earth.velocity.y -= (force_y / mass_earth) * dt;


        Moon.position.x += (Moon.velocity.x * dt) / SCALE;
        Moon.position.y += (Moon.velocity.y * dt) / SCALE;

        Earth.position.x += (Earth.velocity.x * dt) / SCALE;
        Earth.position.y += (Earth.velocity.y * dt) / SCALE;
       
        for (auto& ray : rays) {
            ray.update(grid, Earth, Moon);
        }
        rays.erase(std::remove_if(rays.begin(), rays.end(),
            [](const LightRay& r) { return !r.active; }), rays.end());
  
    
        grid.update(Earth, Moon);


      //  grid.update(Earth);
        // if (trail.getVertexCount() > 500) trail.resize(0); 
        trail.append(sf::Vertex(Moon.position, sf::Color(255, 255, 255, 100)));
        trailEarth.append(sf::Vertex(Earth.position, sf::Color(255, 255, 255, 100))); //not sure but this visualises the actual path of spacetime in 2d inbetween the bodies? Not what i wanted but pretty cool.
        window.clear(sf::Color::Black);
        grid.render(window);
        window.draw(trail);
        window.draw(trailEarth);

        for (const auto& ray : rays) {
            if (ray.active) window.draw(ray.path);
        }

        Earth.render(window);
        Moon.render(window);
        window.display();
    }

    return 0;
};
#include <iostream>
#include <SFML/Graphics.hpp>
#include <math.h>

// Constant Variables
const short WINDOW_WIDTH = 1200;
const short WINDOW_HEIGHT = 950;
const short MAP_BORDER = 10;
const short MAP_WIDTH = 800 + MAP_BORDER;
const short MAP_HEIGHT = 800 + MAP_BORDER;
const short ATOM_WIDTH = 3;
const short FORCE_RANGE = 600;
const short number_of_particles = 150;
const short N_GREENS = 150;
const short N_REDS = 150;
const short N_YELLOWS = 150;
const short total_particles = N_GREENS+N_REDS+N_YELLOWS;
const int NUM_TYPES = 3;  // Number of different particle types
// RED      0
// GREEN    1
// YELLOW   2

// Interaction matrix: interaction[i][j] represents the interaction force between type i and type j particles
float interaction[NUM_TYPES][NUM_TYPES] = {
    { 2.0f, -1.0f,  0.5f},   // Type 0 interactions
    {-1.0f,  0.0f,  -2.7f},   // Type 1 interactions
    { 0.5f,  0.7f,  6.0f}    // Type 2 interactions
};

class Particle {
public:
    sf::Vector2f position;   // 2D vector representing the position (x, y)
    sf::Vector2f velocity;   // 2D vector representing the velocity (vx, vy)
    int type;                // Type of the particle (for interaction rules)

    Particle(float x, float y, int particle_type) {
        position = sf::Vector2f(x, y);       // Initialize position with input coordinates
        velocity = sf::Vector2f(0.0f, 0.0f); // Start with zero velocity
        type = particle_type;                // Assign the particle type
    }

    // Function to update the particle's position based on its velocity
    // auto yparxei san ksexwristo functions giati prepei prwta na ypologistoun oles oi dynameis
    // metaksei olwn twn particles kai meta na metakinithei to particle
    void update() {
        position += velocity;  // Add velocity to position to move the particle
        
       
        // the particles must always be on screen
        if (position.x > MAP_WIDTH)
            position.x = MAP_WIDTH - 1;
        else if (position.x < MAP_BORDER)
            position.x = MAP_BORDER;
        
        if (position.y > MAP_HEIGHT)
            position.y = MAP_HEIGHT - 1;
        else if (position.y < MAP_BORDER)
            position.y = MAP_BORDER;
    }

    // na koitaksw to dt
    // Apply a force to the particle to change its velocity
    void applyForce(sf::Vector2f force, float dt) {
        velocity += force * dt;  // Change velocity based on applied force and time step
    }
};

// Calculates forces for 2 specific particles
sf::Vector2f computeForce(const Particle& p1, const Particle& p2) {
    sf::Vector2f direction = p2.position - p1.position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance == 0 || distance > FORCE_RANGE) // if out of range dont apply any force
        return sf::Vector2f(0.0f, 0.0f);  // Avoid division by zero also set limit g

    float force_strength = interaction[p1.type][p2.type] / (distance*distance);
    direction /= distance;  // Normalize the direction giati mas noiazei mono to direction tou vector oxi to magnitude tou
    // isws na xreiazetai na dieresw me distance^2 gia normalization alla den nomizw
    return direction * force_strength;
    } 

// Creates a specifc number of every particle type and adds them to the argument vector of particles
// Every particle is initialized with random positions
void Create_particles(std::vector<Particle>& particles, const int number) {
    for (int i = 0; i < number; ++i) {
        for (int type=0; type < NUM_TYPES; type++) {
            float x = rand() % MAP_WIDTH;
            float y = rand() % MAP_HEIGHT;
            particles.emplace_back(x, y, type);
        }
    }
}

class Button {
public:
    Button(sf::Vector2f size, sf::Vector2f position, std::string text, sf::Font& font) {
        button.setSize(size);
        button.setFillColor(sf::Color(150,150,150));
        button.setPosition(position);
        buttonText.setFont(font);
        buttonText.setString(text);
        buttonText.setCharacterSize(24);
        buttonText.setFillColor(sf::Color::White);
        // Center the text relative to the button
        sf::FloatRect buttonBounds = button.getGlobalBounds();  // Get the button's bounds
        sf::FloatRect textBounds = buttonText.getLocalBounds(); // Get the text's bounds
        // Calculate the position to center the text
        buttonText.setPosition(
        buttonBounds.left + (buttonBounds.width / 2.0f) - (textBounds.width / 2.0f),
        buttonBounds.top + (buttonBounds.height / 2.0f) - (textBounds.height / 2.0f) - textBounds.top
        );
    }

    void draw(sf::RenderWindow& window) {
        window.draw(button);
        window.draw(buttonText);
    }

    bool isMouseOver(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect bounds = button.getGlobalBounds();
        return bounds.contains(static_cast<sf::Vector2f>(mousePos));
    }

    // void handleMouseHover(sf::RenderWindow& window) {
    //     if (isMouseOver(window)) {
    //         button.setFillColor(sf::Color::Red);
    //     } else {
    //         button.setFillColor(sf::Color::Blue);
    //     }
    // }

private:
    sf::RectangleShape button;
    sf::Text buttonText;
};

int main() {
// Create a window for visualization using SFML
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Particle Life Simulation");
     
    sf::Font font;
    if (!font.loadFromFile("fonts/Roboto-Regular.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }
    
    Button restart_button(sf::Vector2f(110, 50), sf::Vector2f(MAP_WIDTH+70, 20), "Restart",font);
    Button shuffle_button(sf::Vector2f(110, 50), sf::Vector2f(MAP_WIDTH+220,20),"Shuffle",font);
    std::vector<Particle> particles;
    Create_particles(particles,number_of_particles);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && restart_button.isMouseOver(window)) {
                window.clear(sf::Color::Black);
                particles.clear();
                Create_particles(particles,number_of_particles);

            }
        }

        // Update particle interactions
        float dt = 0.1f;  // Time step
        for (int i = 0; i < total_particles; ++i) {
            for (int j = 0; j < total_particles; ++j) {
                if (i != j) {
                    sf::Vector2f force = computeForce(particles[i], particles[j]);
                    particles[i].applyForce(force, dt);
                }
            }
        }
        // Update particle positions
        for (auto& particle : particles) {
            particle.update();
        }
        // Clear the screen
        window.clear(sf::Color::Black);

        // Draw particles
        for (const auto& particle : particles) {
            sf::CircleShape shape(ATOM_WIDTH);  // Particle size
            shape.setPosition(particle.position);
            if (particle.type == 0)
                shape.setFillColor(sf::Color::Red);
            else if (particle.type == 1)
                shape.setFillColor(sf::Color::Green);
            else
                shape.setFillColor(sf::Color::Yellow);
            window.draw(shape);
        }

        restart_button.draw(window);
        shuffle_button.draw(window);
        window.display();
    }
    return 0;
}
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "Values.hpp"
#include "ECS.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "ECS Rectangle and Ball Movement");

    // Creating rectangle entity
    Entity rectangle;
    rectangle.transform = { window_width / 2 - 50, window_height / 1.33f, 100.f, 30.f }; // Initial position and size
    rectangle.velocity = { 0.f, 0.f }; // Initial speed (dont move)

    // Creating ball entity
    Entity ball;
    ball.transform = { ball_initial_x, ball_initial_y , ball_size,ball_size };
    ball.velocity = { 200.f, 200.f }; // Initial ball speed

    //bricks
    std::vector<Entity> bricks;
    for (int i = 0; i < brick_rows; i++) {
        for (int j = 0; j < brick_cols; j++) {
            Entity brick;
            brick.transform = { brick_offset_x + j * (brick_width + brick_spacing),
                                 brick_offset_y + i * (brick_height + brick_spacing),
                                 brick_width, brick_height };
            brick.color = sf::Color::Red;
            bricks.push_back(brick);
        }
    }

    sf::Clock clock;

    // main loop
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (destroyed_bricks == 40 || fallen_balls == 4) {
            std::cout << "Game Over" << std::endl;
            if (destroyed_bricks == 40) {
                std::cout << "YOU WIN !" ;
            }
            window.close();
        }

        // Apply control system to adjust speed
        controllerSystem(rectangle, rectangle);

        // Apply movement system
        movementSystem(rectangle, dt);
        movementSystem(ball, dt);
            
        // Apply bouncing/collision system
        ColliderSystem(ball, rectangle, window_width, window_height, bricks);

        // Clear the window and draws entities by renderer system
        window.clear();
        renderSystem(window, rectangle, ball, bricks);
        window.display();
    }

    return 0;
}

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "Values.hpp"


//Transform  Composant : Position & dimensions
struct Transform {
    float x, y;
    float width, height;
};

//Velocity  Composant : speen on x & y axises
struct Velocity {
    float xSpeed, ySpeed;
};

// Entity : containTransform & Velocity composants 
struct Entity {
    Transform transform;
    Velocity velocity;
    sf::Color color;
    bool isVisible = true;
};

// movment system :upload position by speed
void movementSystem(Entity& entity, float dt) {
    entity.transform.x += entity.velocity.xSpeed * dt;
    entity.transform.y += entity.velocity.ySpeed * dt;
}

// render system : draw the entity to the screen
void renderSystem(sf::RenderWindow& window, const Entity& rectangle, const Entity& ball, const std::vector<Entity>& bricks) {
    // draw the rectangle
    sf::RectangleShape rectangleShape(sf::Vector2f(rectangle.transform.width, rectangle.transform.height));
    rectangleShape.setPosition(rectangle.transform.x, rectangle.transform.y);
    rectangleShape.setFillColor(sf::Color::Blue);
    window.draw(rectangleShape);

    // draw the ball
    sf::CircleShape ballShape(ball_size / 2); // the Ray is width's half
    ballShape.setPosition(ball.transform.x, ball.transform.y);
    ballShape.setFillColor(sf::Color::Yellow);
    window.draw(ballShape);

    // Draw bricks
    for (const auto& brick : bricks) {
        if (brick.isVisible) {  // shows only visibles bricks
            sf::RectangleShape brickShape(sf::Vector2f(brick.transform.width, brick.transform.height));
            brickShape.setPosition(brick.transform.x, brick.transform.y);
            brickShape.setFillColor(brick.color);
            window.draw(brickShape);
        }
    }
}

// controller system : change the speed of the rectangle by the user's inputs
void controllerSystem(Entity& entity, Entity& rectangle) {
    float speed = 280.f; // rectangle speed

    // speed reset
    entity.velocity.xSpeed = 0.f;
    entity.velocity.ySpeed = 0.f;

    // controls to change the speed (axis x ) by the key
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        if (rectangle.transform.x + rectangle.transform.width < window_width) {
            entity.velocity.xSpeed = speed;
        }

    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        if (rectangle.transform.x > 0)
        {
            entity.velocity.xSpeed = -speed;
        }
    }
}

// bouncing system for the ball
void ColliderSystem(Entity& ball, Entity& rectangle, float window_width, float window_height, std::vector<Entity>& bricks) {
    // Bounce on the window's edges
    if (ball.transform.x < 0 || ball.transform.x + ball_size > window_width) {
        ball.velocity.xSpeed = -ball.velocity.xSpeed; // Inverse x direction
    }
    if (ball.transform.y < 0 || ball.transform.y + ball_size > window_height) {
        ball.velocity.ySpeed = -ball.velocity.ySpeed; // Inverse y direction
    }
    if (ball.transform.y > rectangle.transform.y + rectangle.transform.height) {
        ball.transform.x = ball_initial_x;
        ball.transform.y = ball_initial_y;
        ball.velocity.ySpeed = 200.f;
        fallen_balls++;
        std::cout << "fallen balls: " << fallen_balls << std::endl;
    }
    if (ball.transform.y + ball_size >= rectangle.transform.y &&
        ball.transform.y <= rectangle.transform.y + rectangle.transform.height &&
        ball.transform.x + ball_size >= rectangle.transform.x &&
        ball.transform.x <= rectangle.transform.x + rectangle.transform.width) {
        ball.velocity.ySpeed = -ball.velocity.ySpeed * 1.05;
        ball.transform.y = rectangle.transform.y - ball_size;
    }
    // Collider with the bricks
    for (auto& brick : bricks) {
        if (brick.isVisible &&
            ball.transform.x + ball.transform.width >= brick.transform.x &&
            ball.transform.x <= brick.transform.x + brick.transform.width &&
            ball.transform.y + ball.transform.width >= brick.transform.y &&
            ball.transform.y <= brick.transform.y + brick.transform.height) {
            ball.velocity.ySpeed = -ball.velocity.ySpeed;
            brick.isVisible = false; // mask the brick when it's collide by the ball
            destroyed_bricks++;
            std::cout << "destroyed bricks: " << destroyed_bricks << std::endl;
            break;
        }
    }
}



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

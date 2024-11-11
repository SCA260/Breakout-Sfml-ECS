#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "Values.hpp"


// Composant Transform : Position et dimensions
struct Transform {
    float x, y;
    float width, height;
};

// Composant Velocity : Vitesse sur les axes x et y
struct Velocity {
    float xSpeed, ySpeed;
};

// Entité : Contient les composants Transform et Velocity
struct Entity {
    Transform transform;
    Velocity velocity;
    sf::Color color;
    bool isVisible = true;
};

// Système de mouvement : Met à jour la position en fonction de la vitesse
void movementSystem(Entity& entity, float dt) {
    entity.transform.x += entity.velocity.xSpeed * dt;
    entity.transform.y += entity.velocity.ySpeed * dt;
}

// Système de rendu : Dessine l'entité à l'écran
void renderSystem(sf::RenderWindow& window, const Entity& rectangle, const Entity& ball, const std::vector<Entity>& bricks) {
    // Dessiner le rectangle
    sf::RectangleShape rectangleShape(sf::Vector2f(rectangle.transform.width, rectangle.transform.height));
    rectangleShape.setPosition(rectangle.transform.x, rectangle.transform.y);
    rectangleShape.setFillColor(sf::Color::Blue);
    window.draw(rectangleShape);

    // Dessiner la balle
    sf::CircleShape ballShape(ball_size / 2); // Le rayon est la moitié de la largeur
    ballShape.setPosition(ball.transform.x, ball.transform.y);
    ballShape.setFillColor(sf::Color::Yellow);
    window.draw(ballShape);

    // Dessiner les briques
    for (const auto& brick : bricks) {
        if (brick.isVisible) {  // Affiche uniquement les briques visibles
            sf::RectangleShape brickShape(sf::Vector2f(brick.transform.width, brick.transform.height));
            brickShape.setPosition(brick.transform.x, brick.transform.y);
            brickShape.setFillColor(brick.color);
            window.draw(brickShape);
        }
    }
}

// Système de contrôle : Modifie la vitesse du rectangle selon les touches appuyées
void controllerSystem(Entity& entity, Entity& rectangle) {
    float speed = 280.f; // Vitesse de déplacement du rectangle

    // Réinitialiser la vitesse
    entity.velocity.xSpeed = 0.f;
    entity.velocity.ySpeed = 0.f;

    // Contrôles pour changer la vitesse selon les touches
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

// Système de rebond pour la balle
void ColliderSystem(Entity& ball, Entity& rectangle, float window_width, float window_height, std::vector<Entity>& bricks) {
    // Rebonds sur les bords de la fenêtre
    if (ball.transform.x < 0 || ball.transform.x + ball_size > window_width) {
        ball.velocity.xSpeed = -ball.velocity.xSpeed; // Inverse la direction x
    }
    if (ball.transform.y < 0 || ball.transform.y + ball_size > window_height) {
        ball.velocity.ySpeed = -ball.velocity.ySpeed; // Inverse la direction y
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
    // Collision avec les briques
    for (auto& brick : bricks) {
        if (brick.isVisible &&
            ball.transform.x + ball.transform.width >= brick.transform.x &&
            ball.transform.x <= brick.transform.x + brick.transform.width &&
            ball.transform.y + ball.transform.width >= brick.transform.y &&
            ball.transform.y <= brick.transform.y + brick.transform.height) {
            ball.velocity.ySpeed = -ball.velocity.ySpeed;
            brick.isVisible = false; // Masque la brique lorsqu'elle est touchée
            destroyed_bricks++;
            std::cout << "destroyed bricks: " << destroyed_bricks << std::endl;
            break;
        }
    }
}



int main() {
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "ECS Rectangle and Ball Movement");

    // Créer l'entité rectangle
    Entity rectangle;
    rectangle.transform = { window_width / 2 - 50, window_height / 1.33f, 100.f, 30.f }; // Position initiale et taille
    rectangle.velocity = { 0.f, 0.f }; // Vitesse initiale (au repos)

    // Créer l'entité balle
    Entity ball;
    ball.transform = { ball_initial_x, ball_initial_y , ball_size,ball_size };
    ball.velocity = { 200.f, 200.f }; // Vitesse initiale de la balle

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

    // Boucle principale
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // Gérer les événements
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (destroyed_bricks == 40 || fallen_balls == 4) {
            std::cout << "Game Over" << std::endl;
            window.close();
        }

        // Appliquer le système de contrôle pour ajuster la vitesse du rectangle
        controllerSystem(rectangle, rectangle);

        // Appliquer les systèmes de mouvement
        movementSystem(rectangle, dt);
        movementSystem(ball, dt);

        // Appliquer le système de rebond pour la balle
        ColliderSystem(ball, rectangle, window_width, window_height, bricks);

        // Effacer la fenêtre et dessiner les entités avec le système de rendu
        window.clear();
        renderSystem(window, rectangle, ball, bricks);
        window.display();
    }

    return 0;
}

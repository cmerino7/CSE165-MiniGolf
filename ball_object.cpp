#include "ball_object.h"

BallObject::BallObject()
    : GameObject(), Radius(12.5f), Stuck(true) { }

BallObject::BallObject(glm::vec2 pos, float radius, float mass, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), mass, sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true) { }

glm::vec2 BallObject::Move(float dt, unsigned int window_width, unsigned int window_height)
{

    // if not stuck to player board
    if (!this->Stuck)
    {
        const float FRICTION = 5500.5f;
        // move the ball
        
        this->Position += this->Velocity * dt;
        // add friction to slow down
        glm::vec2 momentumVector = this->Velocity * this->Mass;
        if (momentumVector.x != 0 || momentumVector.y != 0) {
            if (FRICTION < glm::length(momentumVector)) {
                this->Velocity -= dt * FRICTION * glm::normalize(momentumVector) / this->Mass;
            }
            else {
                this->Velocity = glm::vec2(0.0f);
            }
        }
        // then check if outside window bounds and if so, reverse velocity and restore at correct position
        if (this->Position.x <= 0.0f)  //left wall
        {
            this->Velocity -= dt * glm::normalize(momentumVector) / this->Mass;
            this->Velocity.x = -this->Velocity.x;
            this->Position.x = 0.0f;
        }
        else if (this->Position.x + this->Size.x >= window_width) //right wall
        {
            this->Velocity -= dt * glm::normalize(momentumVector) / this->Mass;
            this->Velocity.x = -this->Velocity.x;
            this->Position.x = window_width - this->Size.x;
        }
        if (this->Position.y <= 0.0f) //top wall
        {
            this->Velocity -= dt * glm::normalize(momentumVector) / this->Mass;
            this->Velocity.y = -this->Velocity.y;
            this->Position.y = 0.0f;
        }
        else if (this->Position.y + this->Size.y >= window_height) //bottom wall
        {
            this->Velocity -= dt * glm::normalize(momentumVector) / this->Mass;
            this->Velocity.y = -this->Velocity.y;
            this->Position.y = window_height - this->Size.y;
        }
    }
    return this->Position;
}

// resets the ball to initial Stuck Position (if ball is outside window bounds)
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    this->Position = position;
    this->Velocity = velocity;
    this->Stuck = true;
}


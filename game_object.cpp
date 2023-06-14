#include "game_object.h"


GameObject::GameObject()
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Mass(45.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, float mass, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Size(size), Mass(mass), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

glm::vec2 GameObject::Move(float dt, unsigned int window_width, unsigned int window_height)
{
    this->Position += this->Velocity * dt; //start moving obstacle object
    if (this->Position.x <= 30.0)  //left wall
    {
        this->Velocity.x = -this->Velocity.x;
        this->Position.x = 30.0f;
    }
    else if (this->Position.x + this->Size.x >= window_width-30.0f) //right wall
    {
        this->Velocity.x = -this->Velocity.x;
        this->Position.x = window_width - this->Size.x-30.0f;
    }
    return this->Position;
}

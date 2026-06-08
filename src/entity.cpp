#include "entity.h"

/* TODO: evaluate whether functions should be made into contract
 * ( none require impl )
 */

Entity::Entity()
    : _posX(0)
    , _posY(0)
    , _velX(0)
    , _velY(0)
{
}

Entity::~Entity() {}

void Entity::init() {}

void Entity::update() {}

void Entity::render(double delta) {}

void Entity::render(double delta, double deltaTime) {}

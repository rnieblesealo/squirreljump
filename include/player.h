#pragma once

#include "entity.h"
#include "input-controller.h"
#include "spritesheet-renderer.h"

enum STATE
{
  STATE_IDLE,
  STATE_RUNNING,
  STATE_SHOOTING
};

class Player final : Entity
{
public:
  Player(SpritesheetRenderer &playerSprite);
  ~Player();

  void HandleInput();
  void update() override;
  void render(double delta, double deltaTime) override;

private:
  STATE                _state;
  InputController      _input;
  SpritesheetRenderer &_sprite;

  float _speed;
};

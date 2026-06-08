#include "player.h"
#include "spritesheet-renderer.h"
#include <raylib.h>

Player::Player(SpritesheetRenderer &playerSprite)
    : _sprite(playerSprite)
    , _state(STATE_IDLE)
    , _speed(2.25)
{
}

Player::~Player() {}

void Player::update()
{
  HandleInput();

  _posX += _velX;
  _posY += _velY;
}

void Player::render(double delta, double deltaTime)
{
  _sprite.flip(deltaTime);
  _sprite.render(_posX + _velX * delta, _posY + _velY * delta);
}

void Player::HandleInput()
{
  switch (_state)
  {
  case STATE_IDLE:
  {
    _velX = 0;
    _velY = 0;

    if (_input.GetKeyDown(KEY_UP) || _input.GetKeyDown(KEY_DOWN) ||
        _input.GetKeyDown(KEY_LEFT) || _input.GetKeyDown(KEY_RIGHT))
    {
      _sprite.switchTo("run");
      _state = STATE_RUNNING;
    }

    break;
  }
  case STATE_RUNNING:
  {
    _velX = 0;
    _velY = 0;

    if (_input.GetKeyDown(KEY_UP))
      _velY = -1;
    else if (_input.GetKeyDown(KEY_DOWN))
      _velY = 1;

    if (_input.GetKeyDown(KEY_LEFT))
      _velX = -1;
    else if (_input.GetKeyDown(KEY_RIGHT))
      _velX = 1;

    if (_velX == 0 && _velY == 0)
    {
      _sprite.switchTo("idle");
      _state = STATE_IDLE;
    }

    _velX *= _speed;
    _velY *= _speed;

    break;
  }
  case STATE_SHOOTING:
  {
    break;
  }
  }
}

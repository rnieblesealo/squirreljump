#pragma once

class Entity
{
public:
  Entity();

  virtual void init();
  virtual void update();
  virtual void render(double delta); // delta = compensation for inbetween rendering
  virtual void render(double delta,
                      double deltaTime); // delta = compensation for inbetween rendering
                                         // deltaTime used for sprite animations, mainly

protected:
  virtual ~Entity(); // prot. virt destructor prevents deleting Entity via direct podoubleer to it

  double _posX;
  double _posY;

  double _velX;
  double _velY;
};

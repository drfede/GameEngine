#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "../EntityManager.h"
#include "../../lib/glm/glm.hpp"
#include <SDL2/SDL.h>
#include "../Game.h"
#include "../Constants.h"
#include <iostream>


class TransformComponent: public Component {
public:
  glm::vec2 position;
  glm::vec2 velocity;
  int width;
  int height;
  int scale;

  TransformComponent(int posX, int posY, int velX, int velY,int w,
    int h, int s){
    position = glm::vec2(posX,posY);
    velocity = glm::vec2(velX,velY);
    width = w;
    height = h;
    scale = s;
  }

  void Initialize() override {

  }

  void Update(float deltaTime) override {
    if ((position.x < Game::camera.x+WINDOW_WIDTH-31 && velocity.x>0)||(position.x > 0 && velocity.x<0 ))
    position.x += velocity.x * deltaTime;
    if ((position.y < Game::camera.y+WINDOW_HEIGHT-31 && velocity.y>0)||(position.y > 0 && velocity.y<0))
    position.y += velocity.y * deltaTime;

    //std::cout << owner->name << " X: " << position.x << " Y: " << position.y << std::endl;
  }

  void Render() override {

  }

};


#endif

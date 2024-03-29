#ifndef KEYBOARDCONTROLCOMPONENT_H
#define KEYBOARDCONTROLCOMPONENT_H

#include "../Game.h"
#include "../EntityManager.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/ColliderComponent.h"
#include "../Constants.h"

class KeyboardControlComponent : public Component {
public:
  std::string upKey;
  std::string downKey;
  std::string leftKey;
  std::string rightKey;
  std::string shootKey;
  TransformComponent *transform;
  SpriteComponent *sprite;
  DirectionType direction  = RIGHT;

  KeyboardControlComponent(){

  }


  KeyboardControlComponent(std::string upKey,
      std::string rightKey,
      std::string downKey,
      std::string leftKey,
      std::string shootKey){
      this->upKey = GetSDLKeyStringCode(upKey);
      this->rightKey = GetSDLKeyStringCode(rightKey);
      this->downKey = GetSDLKeyStringCode(downKey);
      this->leftKey = GetSDLKeyStringCode(leftKey);
      this->shootKey = GetSDLKeyStringCode(shootKey);
    }

    std::string GetSDLKeyStringCode(std::string key){
      if (key.compare("up") == 0) return "1073741906";
      if (key.compare("down") == 0) return "1073741905";
      if (key.compare("left") == 0) return "1073741904";
      if (key.compare("right") == 0) return "1073741903";
      if (key.compare("space") == 0) return "32";

      return std::to_string(static_cast<int>(key[0]));
    }

    int getDirection(){
      return static_cast<int>(direction)*90;
    }

    void Initialize() override {
      transform = owner -> GetComponent<TransformComponent>();
      sprite = owner -> GetComponent<SpriteComponent>();
    }

    void Update(float deltaTime) override {
      std::string keyCode = std::to_string(Game::event.key.keysym.sym);
      if (Game::event.type == SDL_KEYDOWN){

        if (keyCode.compare(upKey) == 0){
          transform->velocity.y = -50;
          transform->velocity.x = 0;
          direction = UP;
          sprite->Play("UpAnimation");
        }
        if (keyCode.compare(rightKey) == 0){
          transform->velocity.y = 0;
          transform->velocity.x = 50;
          direction = RIGHT;
          sprite->Play("RightAnimation");
        }
        if (keyCode.compare(downKey) == 0){
          transform->velocity.y = 50;
          transform->velocity.x = 0;
          direction = DOWN;
          sprite->Play("DownAnimation");
        }
        if (keyCode.compare(leftKey) == 0){
          transform->velocity.y = 0;
          transform->velocity.x = -50;
          direction = LEFT;
          sprite->Play("LeftAnimation");
        }
        if (keyCode.compare(shootKey) == 0){
          Game::playerShoot();
        }
      }

    if (Game::event.type == SDL_KEYUP) {

        if (keyCode.compare(upKey) == 0){
          transform->velocity.y = 0;
        }
        if (keyCode.compare(rightKey) == 0){
          transform->velocity.x = 0;
        }
        if (keyCode.compare(downKey) == 0){
          transform->velocity.y = 0;
        }
        if (keyCode.compare(leftKey) == 0){
          transform->velocity.x = 0;
        }
        if (keyCode.compare(shootKey) == 0){

        }
    }


    }


private:
};

#endif

#include <iostream>
#include "./Constants.h"
#include "./Game.h"
#include "./AssetManager.h"
#include "./Animation.h"
#include "./Map.h"
#include "./Components/TransformComponent.h"
#include "./Components/SpriteComponent.h"
#include "./Components/KeyboardControlComponent.h"
#include "../lib/glm/glm.hpp"


EntityManager manager;
SDL_Renderer* Game::renderer;
AssetManager* Game::assetManager = new AssetManager(&manager);
SDL_Event Game::event;
Map* map;

Game::Game() {
  this -> m_isRunning = false;
}

Game::~Game(){
}

bool Game::isRunning() const {
  return this -> m_isRunning;
}

void Game::Initialize(int width, int height){
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "Error with SDL" << std::endl;
    return;
  }
  window = SDL_CreateWindow(
    NULL,   //TITLE
    SDL_WINDOWPOS_CENTERED, //X POSITION
    SDL_WINDOWPOS_CENTERED, //Y POSITION
    width,                  //WINDOW WIDTH
    height,                 //WINDOW HEIGHT
    SDL_WINDOW_BORDERLESS
  );
  if (!window){
    std::cerr << "Error with SDL creating window" << std::endl;
    return;
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer){
    std::cerr << "Error creating renderer" << std::endl;
    return;
  }

  LoadLevel(0);

  manager.listAllEntities();

  m_isRunning = true;
  return;
}

void Game::LoadLevel(int levelNumber){
  // First include assets
  assetManager->AddTexture("tank-image", std::string("./assets/images/tank-big-right.png").c_str());
  assetManager->AddTexture("chopper-image", std::string("./assets/images/chopper-spritesheet.png").c_str());
  assetManager->AddTexture("radar-image", std::string("./assets/images/radar.png").c_str());
  assetManager->AddTexture("jungle-tiletexture", std::string("./assets/tilemaps/jungle.png").c_str());

  map = new Map("jungle-tiletexture", 1, 32);
  map->LoadMap("./assets/tilemaps/jungle.map", 25, 20);

  Entity& chopperEntity(manager.AddEntity("chopper", PLAYER_LAYER));
  chopperEntity.AddComponent<TransformComponent>(240, 106, 0, 0, 32, 32, 1);
  chopperEntity.AddComponent<SpriteComponent>("chopper-image", 2, 90, true, false);
  chopperEntity.AddComponent<KeyboardControlComponent>("w","d","s","a","space");

  Entity& radarEntity(manager.AddEntity("radar", UI_LAYER));
  radarEntity.AddComponent<TransformComponent>(720, 15, 0, 0, 64, 64, 1);
  radarEntity.AddComponent<SpriteComponent>("radar-image", 8, 150, false, true);
/*
  //Then include entities
    Entity& tankEntity(manager.AddEntity("tank"));
    tankEntity.AddComponent<TransformComponent>(0 , 0, 20, 20, 32, 32, 1);
    tankEntity.AddComponent<SpriteComponent>("tank-image");
*/


}

void Game::ProcessInput(){
  SDL_PollEvent(&event);

  switch (event.type) {
    case SDL_QUIT:{m_isRunning = false; break;}
    case SDL_KEYDOWN: {
      if (event.key.keysym.sym == SDLK_ESCAPE)
        m_isRunning = false;
    }
    default: {break;}
  }
}

void Game::Update(){
  // if render is too fast we should wait some ms before Update
  while (!SDL_TICKS_PASSED(SDL_GetTicks(),ticksLastFrame + FRAME_TARGET_TIME));

  // DeltaTime is diff from last frame in seconds
  float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

  deltaTime = (deltaTime > 0.05f) ? 0.05f : deltaTime;

  //ticks for current frame
  ticksLastFrame = SDL_GetTicks();

  manager.Update(deltaTime);

}

void Game::Render(){
  SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
  SDL_RenderClear(renderer);

  if (manager.hasNoEntities()){
    return;
  }

  manager.Render();

  SDL_RenderPresent(renderer);
}

void Game::Destroy(){
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

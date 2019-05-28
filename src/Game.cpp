#include <iostream>
#include "./Constants.h"
#include "./Game.h"


Game::Game() {
  this -> m_isRunning = false;
}

Game::~Game(){

}

bool Game::isRunning() const {
  return this -> m_isRunning;
}

float projectilePosX = 0.0f;
float projectilePosY = 0.0f;
float projectileVelX = 40.0f;
float projectileVelY = 20.0f;


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
  m_isRunning = true;
  return;
}

void Game::ProcessInput(){
  SDL_Event event;
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

  projectilePosX += projectileVelX * deltaTime;
  projectilePosY += projectileVelY * deltaTime;
}

void Game::Render(){
  SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
  SDL_RenderClear(renderer);

  SDL_Rect projectile {
    (int) projectilePosX,
    (int) projectilePosY,
    10,
    10
  };

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &projectile);
  SDL_RenderPresent(renderer);
}

void Game::Destroy(){
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

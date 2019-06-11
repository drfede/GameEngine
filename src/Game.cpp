#include <iostream>
#include "./Constants.h"
#include "./Game.h"
#include "./AssetManager.h"
#include "./Animation.h"
#include "./Map.h"
#include "./Components/TransformComponent.h"
#include "./Components/SpriteComponent.h"
#include "./Components/TextLabelComponent.h"
#include "./Components/KeyboardControlComponent.h"
#include "./Components/ProjectileEmitterComponent.h"
#include "../lib/glm/glm.hpp"
#include "./Components/ColliderComponent.h"

EntityManager manager;
SDL_Renderer* Game::renderer;
AssetManager* Game::assetManager = new AssetManager(&manager);
SDL_Event Game::event;
SDL_Rect Game::camera = {0,0,WINDOW_WIDTH, WINDOW_HEIGHT};
Entity* mainPlayer = NULL;
Entity* mainPlayerProjectile = NULL;
Map* map;
bool Game::collidersOn = true;

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
    std::cerr << "Error initializing SDL" << std::endl;
    return;
  }
  if (TTF_Init() != 0){
    std::cerr << "Error initializing SDL_TTF" << std::endl;
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

  LoadLevel(1);

  m_isRunning = true;
  return;
}

void Game::LoadLevel(int levelNumber){
  sol::state lua;
  lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::math);




  std::string levelName = "Level" + std::to_string(levelNumber);
  lua.script_file("./assets/scripts/" + levelName + ".lua");

  sol::table levelData = lua[levelName];

  /*************************************/
  /*LOAD ASSETS FROM lua               */
  /*************************************/


  sol::table levelAssets = levelData["assets"];

  unsigned int assetIndex = 0;
  while (true){
    sol::optional<sol::table> existsAssetIndexNode = levelAssets[assetIndex];
    if (existsAssetIndexNode == sol::nullopt){
      break;
    } else {
      sol::table asset = levelAssets[assetIndex];
      std::string assetType = asset["type"];
      if (assetType.compare("texture") == 0){
        std::string assetId = asset["id"];
        std::string assetFile = asset["file"];
        assetManager->AddTexture(assetId, assetFile.c_str());
      }
      if (assetType.compare("font") == 0){
        std::string assetId = asset["id"];
        std::string assetFile = asset["file"];
        int fontSize = asset["fontSize"];
        assetManager->AddFont(assetId, assetFile.c_str(), fontSize);
      }
    }
    assetIndex++;
  }

  /*************************************/
  /*LOAD MAP FROM lua                  */
  /*************************************/

  sol::table levelMap = levelData["map"];
  std::string mapTextureId = levelMap["textureAssetId"];
  std::string mapFile = levelMap["file"];

  map = new Map(
    mapTextureId,
    static_cast<int>(levelMap["scale"]),
    static_cast<int>(levelMap["tileSize"])
  );

  map->LoadMap(
    mapFile,
    static_cast<int>(levelMap["mapSizeX"]),
    static_cast<int>(levelMap["mapSizeY"])
  );

  /*************************************/
  /*LOAD ENTITIES FROM lua             */
  /*************************************/


  sol::table levelEntities = levelData["entities"];
  unsigned int entitiesIndex = 0;
  while (true) {
    sol::optional<sol::table> existEntityIndexNode = levelEntities[entitiesIndex];
    if (existEntityIndexNode == sol::nullopt){
      break;
    } else {
      sol::table entity = levelEntities[entitiesIndex];
      std::string entityName = entity["name"];
      LayerType layer = static_cast<LayerType>(static_cast<int>(entity["layer"]));
      Entity& newEntity(manager.AddEntity(entityName, layer));

      sol::optional<sol::table> existsTransformComponent = entity["components"]["transform"];
      // Check Transform Component
      if (existsTransformComponent != sol::nullopt){
        newEntity.AddComponent<TransformComponent>(
          static_cast<int>(entity["components"]["transform"]["position"]["x"]),
          static_cast<int>(entity["components"]["transform"]["position"]["y"]),
          static_cast<int>(entity["components"]["transform"]["velocity"]["x"]),
          static_cast<int>(entity["components"]["transform"]["velocity"]["y"]),
          static_cast<int>(entity["components"]["transform"]["width"]),
          static_cast<int>(entity["components"]["transform"]["height"]),
          static_cast<int>(entity["components"]["transform"]["scale"])
        );
      }
      //Check Sprite Component
      sol::optional<sol::table> existsSpriteComponent = entity["components"]["sprite"];
      if (existsSpriteComponent != sol::nullopt){
        std::string textureId = entity["components"]["sprite"]["textureAssetId"];
        bool isAnimated = entity["components"]["sprite"]["animated"];
        if (isAnimated){
          newEntity.AddComponent<SpriteComponent>(
            textureId,
            static_cast<int>(entity["components"]["sprite"]["frameCount"]),
            static_cast<int>(entity["components"]["sprite"]["animationSpeed"]),
            static_cast<bool>(entity["components"]["sprite"]["hasDirections"]),
            static_cast<bool>(entity["components"]["sprite"]["fixed"])
          );
        } else {
          newEntity.AddComponent<SpriteComponent>(textureId);
        }
      }
      //Check Input Control Component
      sol::optional<sol::table> existsInputComponent = entity["components"]["input"];
      if (existsInputComponent != sol::nullopt){
        sol::optional<sol::table> existsKeyboardInputComponent = entity["components"]["input"]["keyboard"];
        if (existsKeyboardInputComponent != sol::nullopt){
          std::string upKey =     entity["components"]["input"]["keyboard"]["up"];
          std::string rightKey =  entity["components"]["input"]["keyboard"]["right"];
          std::string downKey =   entity["components"]["input"]["keyboard"]["down"];
          std::string leftKey =   entity["components"]["input"]["keyboard"]["left"];
          std::string shootKey =  entity["components"]["input"]["keyboard"]["shoot"];
          newEntity.AddComponent<KeyboardControlComponent>(upKey,rightKey,downKey,leftKey,shootKey);
        }
      }
      //Check Collider Component
      sol::optional<sol::table> existsColliderComponent = entity["components"]["collider"];
      if (existsColliderComponent != sol::nullopt){
        std::string colliderTag = entity["components"]["collider"]["tag"];
        newEntity.AddComponent<ColliderComponent>(
          colliderTag,
          static_cast<int>(entity["components"]["transform"]["position"]["x"]),
          static_cast<int>(entity["components"]["transform"]["position"]["y"]),
          static_cast<int>(entity["components"]["transform"]["width"]),
          static_cast<int>(entity["components"]["transform"]["height"])
        );
      }

      sol::optional<sol::table> existsLabelComponent = entity["components"]["label"];
      if (existsLabelComponent != sol::nullopt){
        std::string labelName = entity["components"]["label"]["name"];
        int r = entity["components"]["label"]["color"]["r"];
        int g = entity["components"]["label"]["color"]["g"];
        int b = entity["components"]["label"]["color"]["b"];
        int a = entity["components"]["label"]["color"]["a"];
        std::string text = entity["components"]["label"]["text"];
        std::string font = entity["components"]["label"]["font"];
        SDL_Color color = {r,g,b,a};
        newEntity.AddComponent<TextLabelComponent>(
          static_cast<int>(entity["components"]["label"]["x"]),
          static_cast<int>(entity["components"]["label"]["y"]),
          text,
          font,
          color
        );
      }


      //Check Projectile Emitter Component
      sol::optional<sol::table> existsProjectileEmitterComponent = entity["components"]["projectileEmitter"];
            if (existsProjectileEmitterComponent != sol::nullopt) {
                int parentEntityXPos = entity["components"]["transform"]["position"]["x"];
                int parentEntityYPos = entity["components"]["transform"]["position"]["y"];
                int parentEntityWidth = entity["components"]["transform"]["width"];
                int parentEntityHeight = entity["components"]["transform"]["height"];
                int projectileWidth = entity["components"]["projectileEmitter"]["width"];
                int projectileHeight = entity["components"]["projectileEmitter"]["height"];
                int projectileSpeed = entity["components"]["projectileEmitter"]["speed"];
                int projectileRange = entity["components"]["projectileEmitter"]["range"];
                int projectileAngle = entity["components"]["projectileEmitter"]["angle"];
                bool projectileShouldLoop = entity["components"]["projectileEmitter"]["shouldLoop"];
                std::string textureAssetId = entity["components"]["projectileEmitter"]["textureAssetId"];

                std::string tempName = entity["name"];
                if (tempName.compare("player") != 0){
                  Entity& projectile(manager.AddEntity("projectile", PROJECTILE_LAYER));
                  projectile.AddComponent<TransformComponent>(
                      parentEntityXPos + (parentEntityWidth / 2),
                      parentEntityYPos + (parentEntityHeight / 2),
                      0,
                      0,
                      projectileWidth,
                      projectileHeight,
                      1
                  );
                  projectile.AddComponent<SpriteComponent>(textureAssetId);
                  projectile.AddComponent<ProjectileEmitterComponent>(
                      projectileSpeed,
                      projectileAngle,
                      projectileRange,
                      projectileShouldLoop
                  );
                  projectile.AddComponent<ColliderComponent>(
                      ENEMY_COLLIDER_PROJECTILE,
                      parentEntityXPos,
                      parentEntityYPos,
                      projectileWidth,
                      projectileHeight
                  );
                } else {
                  Entity& projectile(manager.AddEntity("friendly_projectile", PROJECTILE_LAYER));
                  projectile.AddComponent<TransformComponent>(
                      parentEntityXPos + (parentEntityWidth / 2),
                      parentEntityYPos + (parentEntityHeight / 2),
                      0,
                      0,
                      projectileWidth,
                      projectileHeight,
                      1
                  );
                  projectile.AddComponent<SpriteComponent>(textureAssetId);
                  projectile.AddComponent<ProjectileEmitterComponent>(
                      projectileSpeed,
                      projectileAngle,
                      projectileRange,
                      false
                  );
                  projectile.AddComponent<ColliderComponent>(
                      PLAYER_COLLIDER_PROJECTILE,
                      parentEntityXPos,
                      parentEntityYPos,
                      projectileWidth,
                      projectileHeight
                  );
                }
            }


        }
    entitiesIndex++;
  }
  mainPlayer = manager.GetEntityByName("player");
  mainPlayerProjectile = manager.GetEntityByName("friendly_projectile");
}

void Game::ProcessInput(){
  SDL_PollEvent(&event);

  switch (event.type) {
    case SDL_QUIT:{m_isRunning = false; break;}
    case SDL_KEYDOWN: {
      if (event.key.keysym.sym == SDLK_ESCAPE)
        m_isRunning = false;
      if (event.key.keysym.sym == SDLK_F1)
        collidersOn = collidersOn ? false : true;
      if (event.key.keysym.sym == SDLK_SPACE){
      }
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

  HandleCameraMovement();
  CheckCollisions();

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

void Game::HandleCameraMovement(){
  if (mainPlayer){
    TransformComponent* mainPlayerTransform = mainPlayer->GetComponent<TransformComponent>();
    camera.x = mainPlayerTransform -> position.x - (WINDOW_WIDTH / 2);
    camera.y = mainPlayerTransform -> position.y - (WINDOW_HEIGHT / 2);

    // to avoid negative values
    camera.x = camera.x < 0 ? 0 : camera.x;
    camera.y = camera.y < 0 ? 0 : camera.y;
    camera.x = camera.x > camera.w ? camera.w : camera.x;
    camera.y = camera.y > camera.h ? camera.h : camera.y;

  }

}

void Game::playerShoot(){
  TransformComponent* mainPlayerTransform = mainPlayer->GetComponent<TransformComponent>();
TransformComponent* mainPlayerProjectileTransform = mainPlayerProjectile->GetComponent<TransformComponent>();
  Entity& friendly_projectile(manager.AddEntity("friendly_projectile", PROJECTILE_LAYER));
  friendly_projectile.AddComponent<TransformComponent>(
    mainPlayerTransform -> position.x + (mainPlayerTransform->width/2),
    mainPlayerTransform -> position.y + (mainPlayerTransform->width/2),
    0,
    0,
    mainPlayerProjectileTransform ->width,
    mainPlayerProjectileTransform ->height,1);
  friendly_projectile.AddComponent<SpriteComponent>("bullet-friendly-texture");
  friendly_projectile.AddComponent<ColliderComponent>(
    PLAYER_COLLIDER_PROJECTILE,
    mainPlayerTransform -> position.x+16,
    mainPlayerTransform -> position.y+16,
    mainPlayerProjectileTransform -> width,
    mainPlayerProjectileTransform -> height);

KeyboardControlComponent* mainPlayerKeyboardControlComponent = mainPlayer->GetComponent<KeyboardControlComponent>();
ProjectileEmitterComponent* mainPlayerProjectileEmitterComponent = mainPlayerProjectile->GetComponent<ProjectileEmitterComponent>();
friendly_projectile.AddComponent<ProjectileEmitterComponent>(
  mainPlayerProjectileEmitterComponent->getSpeed(),
  mainPlayerKeyboardControlComponent->getDirection(),
  mainPlayerProjectileEmitterComponent->getRange(),
  false);
}



void Game::CheckCollisions(){
  CollisionType collisionType = manager.CheckCollisions();

  switch (collisionType) {
    case PLAYER_ENEMY_COLLISION:
      ProcessGameOver();
      break;
    case PLAYER_PROJECTILE_COLLISION:
      ProcessGameOver();
      break;
    case ENEMY_PROJECTILE_COLLISION:
      break;
    case PLAYER_LEVEL_COMPLETE_COLLISION:
      ProcessNextLevel(1);
      break;
  }
}

  void Game::ProcessNextLevel(int levelNumber){
    std::cout << "Next level" << std::endl;
    m_isRunning = false;
  }

  void Game::ProcessGameOver() {
    std::cout << "Game Over" << std::endl;
    m_isRunning = false;
  }


void Game::Destroy(){
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

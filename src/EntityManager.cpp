#include "./EntityManager.h"
#include <iostream>
#include <string>
#include "./Collision.h"
#include "./Components/ColliderComponent.h"

void EntityManager::ClearData() {
  for (auto& entity: entities) {
    entity -> Destroy();
  }
}

bool EntityManager::hasNoEntities(){
  return entities.size() == 0;
}

void EntityManager::Update(float deltaTime) {

for (int i=0; i< entities.size();i++){
  entities[i]->Update(deltaTime);
    if (!entities[i] -> isActive())
      entities.erase(entities.begin() + i);
  }
}


void EntityManager::Render() {

  for (int layerNum = 0; layerNum < NUM_LAYERS; layerNum++){
    for (auto& entity: getEntitiesByLayer(static_cast<LayerType>(layerNum))){
      entity->Render();
    }
  }
}

Entity& EntityManager::AddEntity(std::string entityName, LayerType layer) {
  Entity *entity = new Entity(*this, entityName, layer);
  entities.emplace_back(entity);
  return *entity;
}

std::vector<Entity*> EntityManager::getEntities() const {
  return entities;
}

std::vector<Entity*> EntityManager::getEntitiesByLayer(LayerType layer) const {
  std::vector<Entity*> selectedEntities;
  for (auto& entity: entities){
    if (entity-> layer == layer){
      selectedEntities.emplace_back(entity);
    }
  }
  return selectedEntities;
}

unsigned int EntityManager::getEntityCount() {
  return entities.size();
}

void EntityManager::listAllEntities(){
  for (auto& entity: entities){
    //std::cout << "Entity Name: " << entity->name << std::endl;
    entity->listAllComponents();
  }
}

CollisionType EntityManager::CheckCollisions() const {
  for (int i=0; i< entities.size() - 1; i++){
    auto& thisEntity = entities[i];
    if (thisEntity -> HasComponent<ColliderComponent>()){
      ColliderComponent* thisCollider = thisEntity -> GetComponent<ColliderComponent>();
      for (int j= i + 1; j <entities.size(); j++){
        auto& thatEntity = entities[j];
        if (thisEntity->name.compare(thatEntity->name) != 0 && thatEntity->HasComponent<ColliderComponent>()){
          ColliderComponent* thatCollider = thatEntity->GetComponent<ColliderComponent>();
          if (Collision::CheckRectangleCollision(thisCollider->collider, thatCollider->collider)) {
            if (thisCollider->colliderTag.compare("PLAYER") == 0 &&
                thatCollider->colliderTag.compare("ENEMY") == 0){
              return PLAYER_ENEMY_COLLISION;
            }
            if (thisCollider->colliderTag.compare("PLAYER") == 0 &&
                thatCollider->colliderTag.compare("PROJECTILE") == 0){
              return PLAYER_PROJECTILE_COLLISION;
            }
            if (thisCollider->colliderTag.compare("ENEMY") == 0 &&
                thatCollider->colliderTag.compare("FRIENDLY_PROJECTILE") == 0){
              return ENEMY_PROJECTILE_COLLISION;
            }
            if (thisCollider->colliderTag.compare("PLAYER") == 0 &&
                thatCollider->colliderTag.compare("LEVEL_COMPLETE") == 0){
              return PLAYER_LEVEL_COMPLETE_COLLISION;
            }
          }
        }
      }
    }
  }
  return NO_COLLISION;
}

std::string EntityManager::CheckEntityCollisions(Entity& myEntity) const{
  ColliderComponent* myCollider = myEntity.GetComponent<ColliderComponent>();
  for (auto& entity: entities){
    if (entity->name.compare(myEntity.name) != 0 && entity->name.compare("Tile") != 0){
      if (entity->HasComponent<ColliderComponent>()){
        ColliderComponent* otherEntityCollider = entity->GetComponent<ColliderComponent>();
        if (Collision::CheckRectangleCollision(myCollider->collider, otherEntityCollider->collider)){
          return otherEntityCollider->colliderTag;
        }
      }
    }
  }
  return std::string();
}

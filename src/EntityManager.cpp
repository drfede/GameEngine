#include "./EntityManager.h"
#include <iostream>

void EntityManager::ClearData() {
  for (auto& entity: entities) {
    entity -> Destroy();
  }
}

bool EntityManager::hasNoEntities(){
  return entities.size() == 0;
}

void EntityManager::Update(float deltaTime) {
  for (auto& entity: entities){
    entity->Update(deltaTime);
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

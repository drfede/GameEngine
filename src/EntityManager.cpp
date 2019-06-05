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
  for (auto& entity: entities){
    entity -> Render();
  }
}

Entity& EntityManager::AddEntity(std::string entityName) {
  Entity *entity = new Entity(*this, entityName);
  entities.emplace_back(entity);
  return *entity;
}

std::vector<Entity*> EntityManager::getEntities() const {
  return entities;
}

unsigned int EntityManager::getEntityCount() {
  return entities.size();
}

void EntityManager::listAllEntities(){
  for (auto& entity: entities){
    std::cout << "Entity Name: " << entity->name << std::endl;
    entity->listAllComponents();
  }
}

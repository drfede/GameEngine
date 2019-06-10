#include "./Entity.h"
#include "./Components/TransformComponent.h"
#include <iostream>
#include <string>

Entity::Entity(EntityManager& manager) : manager(manager){
  this->m_isActive = true;

}

Entity::Entity(EntityManager& manager, std::string name, LayerType layer) : manager(manager), name(name), layer(layer){
  this->m_isActive = true;

}

void Entity::Update(float deltaTime) {
  for (auto& component : components) {
    component->Update(deltaTime);
  }
}

void Entity::listAllComponents(){
  for (auto& component : components){
    if (typeid(component) == typeid(TransformComponent))
    std::cout << "Component<TransformComponent>" << std::endl;
  }
}

void Entity::Render() {
  for (auto& component : components) {
    component -> Render();
  }
}

void Entity::Destroy() {
  m_isActive = false;
}

bool Entity::isActive() const {
  return this->m_isActive;
}

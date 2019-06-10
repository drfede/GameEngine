#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "./Entity.h"
#include "./Component.h"
#include "Constants.h"
#include <vector>
#include <string>


class EntityManager {
private:
  std::vector<Entity*> entities;
public:
  void ClearData();
  void Update(float deltaTime);
  void Render();
  void listAllEntities();
  bool hasNoEntities();
  Entity& AddEntity(std::string entityName, LayerType layer);
  std::vector<Entity*> getEntities() const;
  std::vector<Entity*> getEntitiesByLayer(LayerType layer) const;
  Entity* GetEntityByName(std::string entityName) const;
  unsigned int getEntityCount();
  std::string CheckEntityCollisions(Entity& entity) const;
  CollisionType CheckCollisions() const;
};

#endif

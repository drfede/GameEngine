#include "./Game.h"
#include "./Map.h"
#include "./EntityManager.h"
#include "./Components/TileComponent.h"
#include <fstream>

extern EntityManager manager;

Map::Map(std::string textureId, int scale, int tileSize){
  this -> textureId = textureId;
  this -> scale = scale;
  this -> tileSize = tileSize;
}

void Map::LoadMap(std::string filePath, int mapSizeX, int mapSizeY){
  // read the map tile definitions from .map files
  std::fstream mapFile;
  mapFile.open(filePath);

  for (int y = 0; y < mapSizeY; y++){
    for (int x = 0; x < mapSizeX; x++){
      char ch;
      mapFile.get(ch);
      int sourceRectY = atoi(&ch) * tileSize;
      mapFile.get(ch);
      int sourceRectX = atoi(&ch) * tileSize;
      AddTile(sourceRectX, sourceRectY, x * (scale * tileSize), y * (scale * tileSize));
      mapFile.ignore(); //there's a comma to avoid
    }
  }

}

void Map::AddTile(int sourceRectX, int sourceRectY, int x, int y){
  Entity& newTile(manager.AddEntity("Tile", TILEMAP_LAYER));   //might add the x and y to name it better
  newTile.AddComponent<TileComponent>(sourceRectX, sourceRectY, x, y, tileSize, scale, textureId);

}

#include <iostream>
#include "./Game.h"
#include "./Constants.h"

using namespace std;

int main(int argc, char const *argv[]) {
  Game *game = new Game();

  game -> Initialize(WINDOW_WIDTH,WINDOW_HEIGHT);

  while (game-> isRunning()){

    game -> ProcessInput();
    game -> Update();
    game -> Render();
  }

  game -> Destroy();


  return 0;
}

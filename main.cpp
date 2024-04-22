//
//  main.cpp
//  game_engine
//
//  Created by 蕭翔群 on 2024/1/25.
//

#include <iostream>
#include "Engine.hpp"
#include "SDL2/SDL.h"

using namespace std;

int main(int argc, char* argv[]) {

    Engine engine;
    engine.GameLoop();

    return 0;
}


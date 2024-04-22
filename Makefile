main: *.cpp *.hpp *.h
	clang++ -std=c++17 -I./glm -I./rapidjson -I./box2d -I./box2d/src -I./box2d -I./lua -ILuaBridge -I./LuaBridge/detail -I./LuaBridge -ISDL2 -ISDL_image -ISDL_mixer -ISDL_ttf -Llib -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2main -lSDL2_ttf -llua5.4 -o game_engine_linux -O3 *.cpp

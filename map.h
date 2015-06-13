#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <ctime>

class Tile;

#ifndef MAP_H
#define MAP_H

//------------------------------------------------------------------------------

class Room{
public:
    Room(int map_w, int map_h);
    Room(int _x, int _y, int _w, int _h);
    void createBuilding(std::vector<std::vector<int> > &map);
    void createRoad(std::vector<std::vector<int> > &map);
private:
    int x, y, w, h;
};

//------------------------------------------------------------------------------

class Tileset{
public:
    //Constructors and Deconstructors
    Tileset() {}
    Tileset(SDL_Renderer* renderer, std::string path, int w, int h);
    ~Tileset() {eraseTileset();}

    //Accessors
    int returnWidth() const {return width;}
    int returnHeight() const {return height;}

    //Render
    void render(SDL_Renderer* renderer, Tile &tile1, int x_modifier=0, int y_modifier=0);

    void eraseTileset();

private:
    SDL_Texture* texture;
    int width, height;

};

//------------------------------------------------------------------------------

class Tile{
public:
    //Constructor
    Tile(int i, int x, int y, int w, int h, std::string n, bool a);

    //Accessors
    int getIndex() {return index;}

    SDL_Rect* returnRect() {return &tileBox;}

    void setLocation(int _x, int _y) {x=_x; y=_y;}
    void setHidden(bool h) {hidden=h;}
    void setSeen(bool s) {seen=s;}

    int returnX() {return x;}
    int returnY() {return y;}

    std::string returnName() {return name;}

    bool isWall() const {return wall;}
    bool isHidden() const {return hidden;}
    bool isSeen() const {return seen;}

private:
    SDL_Rect tileBox;
    int index;
    std::string name;
    int x, y;
    bool wall, hidden, seen;
};

#endif // MAP_H

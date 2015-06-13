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

#include "map.h"

bool loadfromfile(SDL_Renderer* renderer, SDL_Texture* &texture, std::string path);
void map_algorithm(int w, int h, std::ofstream &map);

//Room Class--------------------------------------------------------------------

Room::Room(int map_w, int map_h) {
    w=rand() % (map_w)/4 + 2;
    h=rand() % (map_w)/4 + 2;
    x=rand() % (map_w-1-w) + 1;
    y=rand() % (map_h-1-h) + 1;
}

Room::Room(int _x, int _y, int _w, int _h) {
    x=_x;
    y=_y;
    w=_w;
    h=_h;
}

void Room::createBuilding(std::vector<std::vector<int> > &map) {
    for(int i=x+1; i<(x+w-1); i++) {
        for(int j=y+1; j<(y+h-1); j++) {
            map[i][j]=1;
        }
    }
}

void Room::createRoad(std::vector<std::vector<int> > &map) {
    for(int i=x; i<(x+w); i++) {
        for(int j=y; j<(y+h); j++) {
            map[j][i]=0;
        }
    }
}

//Tileset Class-----------------------------------------------------------------

Tileset::Tileset(SDL_Renderer* renderer, std::string path, int w, int h) {
    width=w; height=h;
    if(!loadfromfile(renderer, texture, path)) {
        printf("Failed to create texture for panel from %s!\n", path.c_str());
    }
}

void Tileset::render(SDL_Renderer* renderer, Tile &tile1, int x_modifier, int y_modifier) {
    if(!tile1.isSeen()) {
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }
    else if(tile1.isHidden()) {
        SDL_SetTextureColorMod(texture, 128, 128, 128);
    }
    else if(!tile1.isHidden()) {
        SDL_SetTextureColorMod(texture, 0, 0, 0);
    }
    SDL_Rect renderQuad = {tile1.returnX()+x_modifier*24,tile1.returnY()+y_modifier*24,width,height};
    if(tile1.returnRect()!=NULL) {
        renderQuad.h=tile1.returnRect()->h;
        renderQuad.w=tile1.returnRect()->w;
    }
    if(SDL_RenderCopy(renderer, texture, tile1.returnRect(), &renderQuad)) {
        printf("Unable to render texture! SDL Error: %s\n", SDL_GetError() );
    }
}

void Tileset::eraseTileset() {
    if(texture!=NULL) {
        texture=NULL;
        width=0; height=0;
    }
}

//Tile Class--------------------------------------------------------------------

Tile::Tile(int i, int x, int y, int w, int h, std::string n, bool a) {
    tileBox.x=x;
    tileBox.y=y;
    tileBox.w=w;
    tileBox.h=h;
    index=i;
    name=n;
    wall=a;
    seen=false;
    hidden=false;
}

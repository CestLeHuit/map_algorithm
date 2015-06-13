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
#include <sstream>

#include "map.h"

//Basic requirements for rendering and creating window
SDL_Window* window;
SDL_Renderer* renderer_main;
SDL_Surface* screenSurface;

//Window parameters
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

Tileset tileSheet;
std::vector<Tile> tiles;

//Helper Function-------------------------------------------------------------------------

bool loadfromfile(SDL_Renderer* renderer, SDL_Texture* &texture, std::string path) {
    SDL_Surface* loadedSurface=IMG_Load(path.c_str());
    if(loadedSurface==NULL) {
        printf("Unable to load image: %s! SDL_Image Error: %s!\n", path.c_str(), IMG_GetError());
    }
    else{
       SDL_SetColorKey(loadedSurface,SDL_TRUE,SDL_MapRGB(loadedSurface->format,0,0xFF,0xFF));
       texture=SDL_CreateTextureFromSurface(renderer,loadedSurface);
       if(texture==NULL) {
           printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
       }
       SDL_FreeSurface(loadedSurface);
       return texture!=NULL;
    }
}

std::vector<Tile> parse_map(std::string path, int &w, int &h) {
    std::ifstream map(path.c_str());
    std::string reader;
    std::vector<Tile> tilelist;
    std::vector<std::vector<int> > info;
    std::vector<int> tmp;
    map >> reader;
    h=std::atoi(reader.c_str());
    map >> reader;
    w=std::atoi(reader.c_str());
    int counter=0;
    while(!map.eof()) {
        map >> reader;
        tmp.push_back(std::atoi(reader.c_str()));
        counter++;
        if(counter==w) {
            counter=0;
            info.push_back(tmp);
            tmp.clear();
        }
    }
    int p=0;
    for(int j=0; j<h; j++) {
        for(int i=0; i<w; i++) {
            tilelist.push_back(tiles[info[j][i]]);
            tilelist[p].setLocation(i*24,j*24);
            p++;
        }
    }
    return tilelist;
}

bool CONFIG_INITIALIZER() {
    bool SUCCESS=true;
    std::ifstream CONFIG("../map_algorithm/config.txt");
    std::pair<std::string, bool> check[3];
    check[0]= std::make_pair("SCREEN_WIDTH", false);
    check[1]= std::make_pair("SCREEN_HEIGHT", false);
    if(!CONFIG.good()) {
        printf("Failed to initialize config.txt!\n");
    }
    std::string reader;
    while(!CONFIG.eof()) {
        CONFIG >> reader;
        if(reader=="SCREEN_WIDTH") {
            check[0].second=true;
            CONFIG >> reader;
            SCREEN_WIDTH=std::atoi(reader.c_str());
        }
        if(reader=="SCREEN_HEIGHT") {
            check[1].second=true;
            CONFIG >> reader;
            SCREEN_HEIGHT=std::atoi(reader.c_str());
        }
    }
    for(int i=0;i<2;i++) {
        if(check[i].second==false) {
            printf("The %s is not specified in the config!\n",check[i].first.c_str());
            SUCCESS=false;
        }
    }
    return SUCCESS;
}

/*Starts up SDL2 and SDL_Image and creates the window and renderer*/

bool SDL_INITIALIZER() {
    bool SUCCESS=true;
    const std::string SCREEN_NAME="SDLGE Window";
    if(SDL_Init(SDL_INIT_VIDEO)<0) {
        printf("SDL could not initialize! SDL_ERROR: %s\n", SDL_GetError());
        SUCCESS=false;
    }
    else {
        window=SDL_CreateWindow(SCREEN_NAME.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window==NULL) {
            printf("Window could not be created! SDL_ERROR: %s\n", SDL_GetError());
            SUCCESS=false;
        }
        else {
            int imgFlags=IMG_INIT_PNG;
            if(!(IMG_Init( imgFlags )==imgFlags)){
                printf("SDL_Image could not initialize! SDL_ERROR: %s\n",SDL_GetError());
                SUCCESS=false;
            }
            if(TTF_Init()==-1) {
                printf("SDL_TTF could not initialize! SDL_ERROR: %s\n",SDL_GetError());
            }
            else {
                renderer_main=SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                if(renderer_main==NULL) {
                    printf("renderer_main could not be created! SDL_ERROR: %s\n", SDL_GetError());
                    SUCCESS=false;
                }
                else {
                    SDL_SetRenderDrawColor(renderer_main, 0xFF, 0xFF, 0xFF, 0xFF);
                }
                screenSurface=SDL_GetWindowSurface(window);
            }
        }
    }
    return SUCCESS;
}

bool TILE_INITIALIZER() {
    bool SUCCESS=true;
    std::ifstream TILES("../map_algorithm/tile_config.txt");
    if(!TILES.good()) {
        printf("Failed to initialize tile_config.txt!\n");
    }
    std::string reader;
    TILES >> reader;
    tileSheet = Tileset(renderer_main,reader.c_str(),48,24);
    while(!TILES.eof()) {
        TILES >> reader;
        int index=std::atoi(reader.c_str());
        TILES >> reader;
        std::string name=reader;
        TILES >> reader;
        int x=std::atoi(reader.c_str());
        TILES >> reader;
        int y=std::atoi(reader.c_str());
        TILES >> reader;
        int w=std::atoi(reader.c_str());
        TILES >> reader;
        int h=std::atoi(reader.c_str());
        TILES >> reader;
        bool wall=std::atoi(reader.c_str());
        tiles.push_back(Tile(index,x,y,w,h,name,wall));
    }
    return SUCCESS;
}

void CLOSE() {
    SDL_DestroyRenderer(renderer_main);
    SDL_DestroyWindow(window);
    renderer_main=NULL;
    window=NULL;
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}

int main(int argc, char* args[]) {
    if(!CONFIG_INITIALIZER()) {
        printf("CONFIG could not be initialized\n");
    }
    else {
        if(!SDL_INITIALIZER()) {
            printf("SDL could not be initialized!\n");
        }
        else {
            if(!TILE_INITIALIZER()) {
                printf("TILES could not be initialized!\n");
            }
            else {
                SDL_Event Event;

                bool QUIT=false;
                int map_w; int map_h;
                std::vector<Tile> tile_layout=parse_map("../map_algorithm/map.txt", map_w, map_h);
                while(!QUIT) {
                    while(SDL_PollEvent(&Event)!=0) {
                        //exit statement
                        if(Event.type==SDL_QUIT) {
                            QUIT=true;
                        }
                    }
                    //Rendering
                    SDL_SetRenderDrawColor(renderer_main, 255, 255, 255, 255);
                    SDL_RenderClear(renderer_main);
                    for(int i=0;i<tile_layout.size();i++) {
                        tileSheet.render(renderer_main,tile_layout[i]);
                    }
                    SDL_RenderPresent(renderer_main);
                }
                CLOSE();
            }
            }
        }
    }

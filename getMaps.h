#pragma once
#include "render3d.h"



class Map {
    filesystem::path pathToMaps{"Data/Maps/"};
    public:
//        vector<Block> blocks;
//        vector<int> colors;
        void setMapData(int idx);
};
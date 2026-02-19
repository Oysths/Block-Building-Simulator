#include "std_lib_facilities.h"




struct WorldPoint { //a three-dimensional point
    int x;
    int y;
    int z;
    WorldPoint(int x, int y, int z);
    bool operator== (WorldPoint rhs) {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
};

struct Block
{
    array<int, 8> indexes; //this is an array of 8 indexes, which are indexes to points in the World class. The array is ordered, meaning the first index points to (0, 0, 0) relative to the blocks coordinate system (the first point). The second point will for example be (0, 0, 1) and the eight point will be (1, 1, 1). These eight points make up a block
    Block(int x, int y, int z, World& world); //this is the constructor which will
};

class World {
    public:
        vector<WorldPoint> referencePoints; //these points will never change and be the reference when we calculate the transformation and rotation every frame
        vector<WorldPoint> transformedPoitns; //these points are post transformation AND rotation (not only transformation)
        vector<Block> blocks;
};

struct Player {
    array<double, 3> angles; //should be in following order: yaw, pitch, roll, same order as rotations matricies
    array<double, 3> deltaAngles; //used to change angles

};
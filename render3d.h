#include "std_lib_facilities.h"
#include "AnimationWindow.h"

class World;
class Player; //forward declaring these so the compiler doesn't raise an error
extern const int frameScaling;
extern const int fov;
extern const int windowWidth;
extern const int windowHeight;

struct TrigValues {
    double cXZ;
    double sXZ;
    double cYZ;
    double sYZ;
    double cXY;
    double sXY;
};

struct WorldPointDouble { //a three-dimensional point (relative coordinates)
    double x;
    double y;
    double z;
    bool operator== (WorldPointDouble& rhs) {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    WorldPointDouble operator+ (const WorldPointDouble& rhs){
        return WorldPointDouble {x + rhs.x, y + rhs.y, z + rhs.z};
    }
    WorldPointDouble operator- (const WorldPointDouble& rhs){
        return WorldPointDouble {x - rhs.x, y - rhs.y, z - rhs.z};
    }

    void printPoint();
};

struct WorldPointInt { //a three-dimensional point (absolute coordinates are always integers)
    int x;
    int y;
    int z;
    bool operator== (WorldPointInt rhs) {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    bool operator< (WorldPointDouble& rhs) {
        return double(x) < rhs.x && double(y) < rhs.y && double(z) < rhs.z;
    }
    bool operator> (WorldPointDouble& rhs) {
        return double(x) > rhs.x && double(y) > rhs.y && double(z) > rhs.z;
    }
};


struct Block
{
    array<int, 8> pointIndexes; //this is an array of 8 indexes, which are pointers to points in the transformedPoints in world class. The array is ordered, meaning the first index points to (0, 0, 0) relative to the blocks coordinate system (the first point). The second point will for example be (0, 0, 1) and the eight point will be (1, 1, 1). These eight points make up a block
    Block(int x, int y, int z, World& world); //this is the constructor which will
};

struct World {
    vector<WorldPointInt> referencePoints; //these points will never change and be the reference when we calculate the transformation and rotation every frame
    vector<WorldPointDouble> transformedPoints; //these points are post transformation AND rotation (not only transformation)
    vector<Block> blocks;
    void transformCoords(Player player);
    void renderPoints(AnimationWindow& window);
    void renderLine(AnimationWindow& window);
    void renderLines(AnimationWindow& window);
    void addBlock(int x, int y, int z);
    void placeBlock (Player player);
    void breakBlock (Player player);
};

struct Player {
    array<double, 3> angles {0, 0, 0}; //should be in following order: yaw, pitch, roll, same order as rotations matricies
    array<double, 3> deltaAngles {0, 0, 0}; //used to change angles
    TrigValues trigValues;
    WorldPointDouble coords {0, 2, 0};
    void move(string button);
    void getTrigValues();
    //void move(const string& button);
};
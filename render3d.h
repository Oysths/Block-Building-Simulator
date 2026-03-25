#include "std_lib_facilities.h"
#include "AnimationWindow.h"
#include <chrono>

struct World;
struct Player; //forward declaring these so the compiler doesn't raise an error
extern const int frameScaling;
extern double fov;
extern const int windowWidth;
extern const int windowHeight;
//extern auto starttid;
//extern auto sluttid;
//extern auto varighet;

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


//struct Surface

struct Block
{
    array<int, 8> pointIndexes; //this is an array of 8 indexes, which are pointers to points in the transformedPoints in world class. The array is ordered, meaning the first index points to (0, 0, 0) relative to the blocks coordinate system (the first point). The second point will for example be (0, 0, 1) and the eight point will be (1, 1, 1). These eight points make up a block
    World* world;
    Block(int x, int y, int z, World& world); //this is the constructor which will

    bool operator<(const Block& rhs);
};

struct World {
    vector<WorldPointInt> referencePoints; //these points will never change and be the reference when we calculate the transformation and rotation every frame
    vector<WorldPointDouble> transformedPoints; //these points are post transformation AND rotation (not only transformation)
    vector<Block> blocks;
    void transformCoords(Player player);
    void renderPoints(AnimationWindow& window); //inactive
    void renderLines(AnimationWindow& window); //inactive
    void renderSurfaces(AnimationWindow& window); //inactive
    void renderBlockSide(AnimationWindow& window, Point corner1, Point corner2, Point corner3, Point corner4);
    void renderBlocks(AnimationWindow& window);
    void addBlock(int x, int y, int z);
    void placeBlock(Player player);
    void breakBlock(Player player);
    void sortBlocks();
    vector<WorldPointDouble>& getTransformedPoints();
};

inline bool Block::operator<(const Block& rhs) {
    vector<WorldPointDouble>& transformedPoints = world->getTransformedPoints();

    WorldPointDouble p1 = transformedPoints.at(pointIndexes[0]); //takes the middle point of the blocks for reference, as any random point (even though they are the same index) leads to the possibility of the point in the cube furthest away being closer than that same point in the cube closer to us
    WorldPointDouble p11 = transformedPoints.at(pointIndexes[7]);

    double p1x = (p11.x+p1.x);
    double p1y = (p11.y+p1.y);
    double p1z = (p11.z+p1.z);

    WorldPointDouble p2 = transformedPoints.at(rhs.pointIndexes[0]);
    WorldPointDouble p22 = transformedPoints.at(rhs.pointIndexes[7]);

    double p2x = (p22.x+p2.x);
    double p2y = (p22.y+p2.y);
    double p2z = (p22.z+p2.z);

    return p1z*p1z + p1x*p1x + p1y*p1y < p2z*p2z + p2x*p2x + p2y*p2y; //since both blocks are in the transformed system, we only need to check a random z for both blocks and compare them (but they have to be the same index)
    //return p1.z < p2.z;
}

struct Player {
    int droneIdx = 0;
    array<double, 3> angles {0, 0, 0}; //should be in following order: yaw, pitch, roll, same order as rotations matricies
    array<double, 3> deltaAngles {0, 0, 0}; //used to change angles
    TrigValues trigValues;
    WorldPointDouble coords {0, 2, 0};
    void move(string button);
    void getTrigValues();
    std::vector<std::unordered_map<std::string, int>> Drones;
    void droneIdxCheck();
    //void move(const string& button);
};
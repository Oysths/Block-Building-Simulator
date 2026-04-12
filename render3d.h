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

//colors available in-game
enum class BlockColors {red, orange, yellow, green, blue, purple, black, white, grey, brown, burly_wood,  pink}; //if you add more colors - make sure that pink is the last one! This is because the game tracks the number of entries in the enum class by pinks numerical value

//converts BlockColors-class to Color-class
Color getColor(BlockColors& color);

//trigvalues should only be calculated once per frame and not for every point (saves a lot of porcessing time)
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


struct DefaultBlock //standard 1x1x1 cube
{
    array<int, 8> pointIndexes; //this is an array of 8 indexes, which are pointers to points in the referencePoints in world class. The array is ordered, meaning the first index points to (0, 0, 0) relative to the blocks coordinate system (the first point). The second point will for example be (0, 0, 1) and the eight point will be (1, 1, 1). These eight points make up a block
    World* world;
    DefaultBlock(int x, int y, int z, World& world);

};

struct Block : public DefaultBlock { //same as DefaultBlock with the addition of color
    BlockColors color;
    Block(int x, int y, int z, World& world, BlockColors& color); //this is the constructor which will make a block with color
    bool operator<(const Block& rhs);
};

//basically the same as a map, a 3-dimensional world
struct World {
    vector<WorldPointInt> referencePoints; //these points will never change and be the reference when we calculate the transformation and rotation every frame
    vector<WorldPointDouble> transformedPoints; //these points are post transformation AND rotation (not only transformation)
    vector<Block> blocks;
    string mapName; //unknown at compiletime
    void transformCoords(Player player);
    void renderPoints(AnimationWindow& window); //inactive
    void renderLines(AnimationWindow& window); //inactive
    void renderSurfaces(AnimationWindow& window); //inactive
    void renderBlockSide(AnimationWindow& window, Point corner1, Point corner2, Point corner3, Point corner4, Color& color);
    void renderBlocks(AnimationWindow& window);
    void addBlock(int x, int y, int z, BlockColors& color);
    void placeBlock(Player player);
    void breakBlock(Player player);
    void sortBlocks();
    vector<WorldPointDouble>& getTransformedPoints();
    void saveMapData();
    void loadMap(string mapNameString);
};

//information about the player
struct Player {
    array<double, 3> angles; //should be in following order: yaw, pitch, roll, same order as rotations matricies
    array<double, 3> deltaAngles; //used to change angles
    TrigValues trigValues;
    WorldPointDouble coords;
    BlockColors activeColor;
    Color activeColorColor; //this is the type that can actually be rendered and is being passed as an argument
    Player();
    void updateColor(); //updates activecolorcolor
    void nextColor();
    void previousColor();
    void move(string button);
    void getTrigValues();
    void resetPlayer(); //default player values
    //void move(const string& button);
};
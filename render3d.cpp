#include "render3d.h"

static const array<array<int, 2>, 12> cubeEdgePairs = {{ //these are the index pairs for every cornerpair with distance = 1
    {0,1}, {0,2}, {0,4},
    {1,3}, {1,5},
    {2,3}, {2,6},
    {3,7},
    {4,5}, {4,6},
    {5,7},
    {6,7}
}};

array<double, 2> yawRotation(double x, double z, double& c, double& s) { //roterer pointsa langs xz-planet gitt en vinkel (parametere er absolutte størrelser)
    double newX = x*c + z*s;
    double newZ = -x*s + z*c;
    return {newX, newZ};
}

array<double, 2> pitchRotation(double y, double z, double& c, double& s) {
    double newY = y*c - z*s;
    double newZ = y*s + z*c;
    return {newY, newZ};
}

array<double, 2> rollRotation(double x, double y, double& c, double& s) {
    double newX = x*c - y*s;
    double newY = x*s + y*c;
    return {newY, newY};
}

array<int, 8> getBlockIndexes(int x, int y, int z, World& world) {
    //this function return the corresponding indexes for all 8 points in the block in the 
    //referencepoints-vector in world. It checks if theres any points in there already matching
    //the coordinates of the block, if not it creates a new point in world's points.
    //This is way better than blindly creating points as this process is only run once
    //for every block and in an infinite world with only blocks, the size of world's points
    //will be 8x smaller

    int worldPointSize = world.referencePoints.size();
    array<int, 8> returnArray = {0, 0, 0, 0, 0, 0, 0, 0};       //this is the array being returned
    int iteration = -1;                                         //this tracks which iteration # we're on. this will also be the index of the corresponding point in returnarray
    bool found = false;                                         //trakcs if the block is found or not

    for (int i = 0; i < 2; i++) {                                //iterates over every corner
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                iteration += 1;

                WorldPointInt p0 {x + i, y + j, z + k};               //this is the point we're looking for, if not in world's referencepoints, it should be added to worldreferencepoints
                found = false;
                for (int l = 0; l < worldPointSize; l++) {         //iterates through every worldpoint in world to see if it exists already
                    WorldPointInt& p = world.referencePoints.at(l);
                    if (p0 == p) {
                        returnArray.at(iteration) = l;
                        found = true;
                        break;                                      //iterates over the next corner once it has found a corresponding worldpoint
                    }
                }
                if (!found) {
                    world.referencePoints.push_back(p0);            //if the program reaches this line of code without breaking the point doesn't exist in world ant should be added
                    WorldPointDouble p00 {p0.x, p0.y, p0.z};
                    world.transformedPoints.push_back(p00);
                    returnArray.at(iteration) = world.referencePoints.size()-1;
                }

            }
        }
    }

    return returnArray;
}

//Block-class-------------------------------------------------------
Block::Block(int x, int y, int z, World& world): pointPointers(getBlockIndexes(x, y, z, world))
{}

//World-class-------------------------------------------------------
void World::transformCoords(Player player) {
    transformedPoints.clear();

    WorldPointDouble placeHolderPoint;
    //first we have to offset the coords by the player position
    for (auto& p : referencePoints) {
        placeHolderPoint = WorldPointDouble(
            p.x - player.coords.x,
            p.y - player.coords.y,
            p.z - player.coords.z
        );
        transformedPoints.push_back(placeHolderPoint);
    }
    
    //applyer rotasjonsmatriser etter offsettet
    //først XZ-rotatasjonsmatrisa (yaw)
    double c = cos(player.angles[0]); //calculates sin and cosine in advance because it is resource intensive to do so for every block
    double s = sin(player.angles[0]);
    for (int i = 0; i < transformedPoints.size(); ++i) {
        const array<double, 2>& transformedCoordsXZ = yawRotation(transformedPoints[i].x, transformedPoints[i].z, c, s);
        transformedPoints[i].x = transformedCoordsXZ[0];
        transformedPoints[i].z = transformedCoordsXZ[1];
    }

    c = cos(player.angles[1]);
    s = sin(player.angles[1]);
    //applyer så YZ-rotasjonsmatrisa (pitch)
    for (int i = 0; i < transformedPoints.size(); ++i) {
        const array<double, 2>& transformedCoordsYZ = pitchRotation(transformedPoints[i].y, transformedPoints[i].z, c, s);
        transformedPoints[i].y = transformedCoordsYZ[0];
        transformedPoints[i].z = transformedCoordsYZ[1];
    }

    c = cos(player.angles[2]);
    s = sin(player.angles[2]);
    //finally applies roll (XY)
    for (int i = 0; i < transformedPoints.size(); ++i) {
        const array<double, 2>& transformedCoordsXY = rollRotation(transformedPoints[i].x, transformedPoints[i].y, c, s);
        transformedPoints[i].x = transformedCoordsXY[0];
        transformedPoints[i].y = transformedCoordsXY[1];
    }
}
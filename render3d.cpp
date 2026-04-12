#include "render3d.h"

auto starttidFrame = chrono::steady_clock::now();

void WorldPointDouble::printPoint() {
    cout << "x: " << x << ", y: " << y << ", z: " << z << endl;
    cout << "z: " << z << endl;
}

static const array<array<int, 2>, 12> cubeEdgePairs = {{ //these are the index pairs for every cornerpair with distance = 1
    {0,1}, {0,2}, {0,4},
    {1,3}, {1,5},
    {2,3}, {2,6},
    {3,7},
    {4,5}, {4,6},
    {5,7},
    {6,7}
}};

static const array<array<int, 4>, 6> cubeSurfaces = {{ //these are the index "pairs" (of four) for every surface om the cube
    {0, 1, 2, 3},
    {0, 1, 4, 5},
    {0, 2, 4, 6},
    {4, 5, 6, 7},
    {1, 3, 5, 7},
    {2, 3, 6, 7}
}};

array<int, 2> screenCoords(double x, double y, double z) {
    double screenX = frameScaling*(fov*x/z);
    double screenY = frameScaling*(fov*y/z);
    screenY *= -1; //fordi opp er oppover, ikke nedover slik som det er for skjermen
    screenX += windowWidth/2; //Dette fordi (0, 0) er øverst til venstre, men vi vil forskyve det til midten av skjermen
    screenY += windowHeight/2;
    return {static_cast<int>(round(screenX)), static_cast<int>(round(screenY))};
}

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
    return {newX, newY};
}

array<int, 8> getBlockIndexes(int x, int y, int z, World& world) { //this function may need improvement, I think this is the reason for the long waiting screen in the start
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

    //for (auto& p : world.transformedPoints) {
    //    p.printPoint();
    //}
    return returnArray;
}

//DefaultBlock-class------------------------------------------------
DefaultBlock::DefaultBlock(int x, int y, int z, World& world): pointIndexes(getBlockIndexes(x, y, z, world)), world{&world}
{}

//Block-class-------------------------------------------------------
Block::Block(int x, int y, int z, World& world, BlockColors& color): DefaultBlock(x, y, z, world), color{color}
{}

//need to define this overloaded operator in the cpp file in order to avoid a multiple definitions error
bool Block::operator<(const Block& rhs) {
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
    
    //for (auto& p : transformedPoints) {
    //    p.printPoint();
    //}
    //applyer rotasjonsmatriser etter offsettet
    //først XZ-rotatasjonsmatrisa (yaw)
    for (int i = 0; i < transformedPoints.size(); ++i) {
        const array<double, 2>& transformedCoordsXZ = yawRotation(transformedPoints[i].x, transformedPoints[i].z, player.trigValues.cXZ, player.trigValues.sXZ);
        transformedPoints[i].x = transformedCoordsXZ[0];
        transformedPoints[i].z = transformedCoordsXZ[1];
    }

    //applyer så YZ-rotasjonsmatrisa (pitch)
    for (int i = 0; i < transformedPoints.size(); ++i) {
        const array<double, 2>& transformedCoordsYZ = pitchRotation(transformedPoints[i].y, transformedPoints[i].z, player.trigValues.cYZ, player.trigValues.sYZ);
        transformedPoints[i].y = transformedCoordsYZ[0];
        transformedPoints[i].z = transformedCoordsYZ[1];
    }

    //finally applies roll (XY)
    for (int i = 0; i < transformedPoints.size(); ++i) {
        const array<double, 2>& transformedCoordsXY = rollRotation(transformedPoints[i].x, transformedPoints[i].y, player.trigValues.cXY, player.trigValues.sXY);
        transformedPoints[i].x = transformedCoordsXY[0];
        transformedPoints[i].y = transformedCoordsXY[1];
    }

}

void World::renderPoints(AnimationWindow& window) {
    //cout << transformedPoints.size() << endl;
    double x;
    double y;
    double z;
    int radius = 3;
    //cout << transformedPoints.size() << endl;
    for (auto& p : transformedPoints) {
        //p.printPoint();
        x = p.x;
        y = p.y;
        z = p.z;
        if (z > fov) {
            //konverterer de relative koordinatene til koordinater på skjermen ved hjelp av basic geometri   
            const array<int, 2>& sCoords = screenCoords(x, y, z);
            int sX = sCoords[0];
            int sY = sCoords[1];
            if (0 <= sX && sX <= windowWidth) {
                if (0 <= sY && sY <= windowHeight) {
                    Point referencePoint {static_cast<int>(round(sX)), static_cast<int>(round(sY))};
                    window.draw_triangle(referencePoint, {referencePoint.x + radius, referencePoint.y + radius}, {referencePoint.x - radius, referencePoint.y + radius}, Color::dark_violet);
                }
            }
        }

    }
}

void World::renderLines(AnimationWindow& window) {
    WorldPointDouble p1 {}; //p1 and p2 are the points which make up a line in a cube (two corners)
    WorldPointDouble p2 {};
    for (auto& b : blocks) {
        for (const array pair : cubeEdgePairs) {
            p1 = transformedPoints.at(b.pointIndexes.at(pair.at(0)));
            p2 = transformedPoints.at(b.pointIndexes.at(pair.at(1)));

            //p2.printPoint();
            //p2.printPoint();

            if (p1.z < 1e-2 || p2.z < 1e-2) { //screenCoords-funksjonen deler på z, men vet ikke hvor mye dette faktisk har å si
                continue;
            }
            
            const array<int, 2>& sCoords1 = screenCoords(p1.x, p1.y, p1.z);
            int sX1 = sCoords1.at(0);
            int sY1 = sCoords1.at(1);
            Point lineStart = {sX1, sY1};
            

            const array<int, 2> sCoords2 = screenCoords(p2.x, p2.y, p2.z);
            int sX2 = sCoords2.at(0);
            int sY2 = sCoords2.at(1);
            Point lineEnd = {sX2, sY2};
            if (p1.z > fov || p2.z > fov) { 
                if (0 <= sX1 && sX1 <= windowWidth) {
                    if (0 <= sY1 && sY1 <= windowHeight) {
                        window.draw_line(lineStart, lineEnd); 
                        continue;
                    }
                }
                if (0 <= sX2 && sX2 <= windowWidth) {
                    if (0 <= sY2 && sY2 <= windowHeight) {
                        window.draw_line(lineStart, lineEnd);
                    }
                }

            }
        }
    }
}

void World::renderSurfaces(AnimationWindow& window) {
    WorldPointDouble p1 {}; //p1 and p2 are the points which make up a line in a cube (two corners)
    WorldPointDouble p2 {};
    WorldPointDouble p3 {};
    WorldPointDouble p4 {};
    for (auto& b : blocks) {
        for (const array surfaceIndexes : cubeSurfaces) {
            p1 = transformedPoints.at(b.pointIndexes.at(surfaceIndexes.at(0)));
            p2 = transformedPoints.at(b.pointIndexes.at(surfaceIndexes.at(1)));
            p3 = transformedPoints.at(b.pointIndexes.at(surfaceIndexes.at(2)));
            p4 = transformedPoints.at(b.pointIndexes.at(surfaceIndexes.at(3)));


            //p2.printPoint();
            //p2.printPoint();

            if (p1.z < 1e-2 || p2.z < 1e-2 || p3.z < 1e-2 || p4.z < 1e-2 ) { //screenCoords-funksjonen deler på z, men vet ikke hvor mye dette faktisk har å si
                continue;
            }
            
            const array<int, 2>& sCoords1 = screenCoords(p1.x, p1.y, p1.z);
            int sX1 = sCoords1.at(0);
            int sY1 = sCoords1.at(1);
            Point corner1 = {sX1, sY1};


            const array<int, 2> sCoords2 = screenCoords(p2.x, p2.y, p2.z);
            int sX2 = sCoords2.at(0);
            int sY2 = sCoords2.at(1);
            Point corner2 = {sX2, sY2};


            const array<int, 2> sCoords3 = screenCoords(p3.x, p3.y, p3.z);
            int sX3 = sCoords3.at(0);
            int sY3 = sCoords3.at(1);
            Point corner3 = {sX3, sY3};

            
            const array<int, 2> sCoords4 = screenCoords(p4.x, p4.y, p4.z);
            int sX4 = sCoords4.at(0);
            int sY4 = sCoords4.at(1);
            Point corner4 = {sX4, sY4};

            //cout << "renderer surface" << endl;
            window.draw_triangle(corner1, corner2, corner3, Color::green);
            window.draw_triangle(corner2, corner3, corner4, Color::green); 


            //if (p1.z > fov || p2.z > fov) { 
            //    if (0 <= sX1 && sX1 <= windowWidth) {
            //        if (0 <= sY1 && sY1 <= windowHeight) {
            //            window.draw_triangle(corner1, corner2, corner2);
            //            window.draw_triangle(corner2, corner3, corner4); 
            //            continue;
            //        }
            //    }
            //    if (0 <= sX2 && sX2 <= windowWidth) {
            //        if (0 <= sY2 && sY2 <= windowHeight) {
            //            window.draw_line(lineStart, lineEnd);
            //        }
            //    }
//
            //}
        }
    }
}

void World::addBlock(int x, int y, int z, BlockColors& color) {
    try { //tries to add block to map
        blocks.push_back(Block {x, y, z, *this, color}); //sender også objektet det ble kalt fra som reference
    } catch (...) { //cathes all exeptions
        cout << "Could not add block to map." << endl;
    }
}

void World::placeBlock (Player player) {
    //cout << "Prøver å plassere" << endl;
    double placementRange = 20.0;
    double deltaRange = 0.01;
    double x = -cos(player.angles[1])*sin(player.angles[0])*deltaRange;
    double z = cos(player.angles[0])*cos(player.angles[1])*deltaRange;
    double y = sin(player.angles[1])*deltaRange; 
    //cout << "x: " << x << ", y: " << y << ", z: " << z << endl;

    WorldPointDouble p = player.coords; //this is the vector we will iterate
    WorldPointDouble deltaRangeVector {x, y, z}; //direction vector to add to p

    WorldPointInt p1; //first (x + 0, y + 0, z + 0) and last (x + 1, y +1 , z + 1) element of every block
    WorldPointInt p2;

    bool breakOutOfLoop = false;
    for (double i = 0; i < placementRange; i += deltaRange) {
        p = p + deltaRangeVector; //have defined addition for this type
        
        for (auto& b : blocks) { //iterates blocks to check the first iteration inside a block, when it has hit a block, it goes one iteration backwards and places a block there
            p1 = referencePoints.at(b.pointIndexes.front());
            p2 = referencePoints.at(b.pointIndexes.back());
            if (p1 < p && p2 > p) {
                //cout << "Fant en passende blokk" << endl;
                p = p - deltaRangeVector; //go one back to get the coordinates of that block
                breakOutOfLoop = true;
                break;
            }
        }
        if (breakOutOfLoop) {
            addBlock(floor(p.x), floor(p.y), floor(p.z), player.activeColor);
            //cout << "Blokk plassert" << endl;
            break;
        }
    }
}

void World::breakBlock(Player player) {
    double breakRange = 20.0;
    double deltaRange = 0.01;
    double x = -cos(player.angles[1])*sin(player.angles[0])*deltaRange;
    double z = cos(player.angles[0])*cos(player.angles[1])*deltaRange;
    double y = sin(player.angles[1])*deltaRange; 
    //cout << "x: " << x << ", y: " << y << ", z: " << z << endl;

    WorldPointDouble p = player.coords; //this is the vector we will iterate
    WorldPointDouble deltaRangeVector {x, y, z}; //direction vector to add to p

    WorldPointInt p1; //first (x + 0, y + 0, z + 0) and last (x + 1, y +1 , z + 1) element of every block
    WorldPointInt p2;

    bool breakOutOfLoop = false;
    for (double i = 0; i < breakRange; i += deltaRange) {
        p = p + deltaRangeVector; //have defined addition for this type
        
        for (int j = 0; j < blocks.size(); j++) { //iterates blocks to check the first iteration inside a block, when it has hit a block, it goes one iteration backwards and places a block there
            Block b = blocks.at(j);
            p1 = referencePoints.at(b.pointIndexes.front());
            p2 = referencePoints.at(b.pointIndexes.back());
            if (p1 < p && p2 > p) {
                blocks.erase(blocks.begin() + j);
                breakOutOfLoop = true;
                break;
            }
        }
        if (breakOutOfLoop) {
            break;
        }
    }
}

void World::sortBlocks() {
    sort(blocks); //from the algorithm-library std_lib_facilities provides. It automatically uses the overloaded less than operator we defined for the block class
}

vector<WorldPointDouble>& World::getTransformedPoints() {
    return transformedPoints;
}

bool compareSurfacesDescending(const array<WorldPointDouble, 4>& a, const array<WorldPointDouble, 4>& b) {
    //we don't divide by four to find average and we don't square root in return because the return value is a bool and it is mathematecally identical to not do so and it saves time
    double avgAz = a[0].z + a[1].z + a[2].z + a[3].z;
    double avgAx = a[0].x + a[1].x + a[2].x + a[3].x;
    double avgAy = a[0].y + a[1].y + a[2].y + a[3].y;
    double avgBz = b[0].z + b[1].z + b[2].z + b[3].z;
    double avgBx = b[0].x + b[1].x + b[2].x + b[3].x;    
    double avgBy = b[0].y + b[1].y + b[2].y + b[3].y;

    return avgAz*avgAz + avgAx*avgAx + avgAy*avgAy > avgBz*avgBz + avgBx*avgBx + avgBy*avgBy; //return whether or not the euclidian distance is greater. No square root because it saves time and returns a boolean value anyways and has property f(x2) > f(x1) => x2 > x1. 

    //double avgA = a[0].z + a[1].z + a[2].z + a[3].z; //tried painter's algorithm, but it falls short for real 3d rendering
    //double avgB = b[0].z + b[1].z + b[2].z + b[3].z;
    //return avgA > avgB;
}

void World::renderBlockSide(AnimationWindow& window, Point corner1, Point corner2, Point corner3, Point corner4, Color& color) {
    window.draw_triangle(corner1, corner2, corner3, color); //draw two triangles to make the side of the cube which have four sides
    window.draw_triangle(corner2, corner3, corner4, color);
    window.draw_line(corner1, corner2);
    window.draw_line(corner2, corner4);
    window.draw_line(corner3, corner4);
    window.draw_line(corner1, corner3);
}

void World::renderBlocks(AnimationWindow& window) {
    //cout << "Skal rendere: " << blocks.size() << " blocks" << endl;
    //cout << "Skal rendere: " << referencePoints.size() << " points" << endl;
    starttidFrame = chrono::steady_clock::now();
    vector<array<WorldPointDouble, 4>> surfaces; //list with every surface in it (the four points), the plan is to sort it and then render in the sorted order
    WorldPointDouble p1 {}; //p1 and p2 are the points which make up a line in a cube (two corners)
    WorldPointDouble p2 {};
    WorldPointDouble p3 {};
    WorldPointDouble p4 {};
    for (int i = blocks.size()-1; i >= 0; i--) {
        Block b = blocks.at(i);
        BlockColors color {b.color};
        Color realColor = getColor(color);
        surfaces = {};
        for (const array surfaceIndexes : cubeSurfaces) {
            p1 = transformedPoints[b.pointIndexes[surfaceIndexes[0]]];
            p2 = transformedPoints[b.pointIndexes[surfaceIndexes[1]]];
            p3 = transformedPoints[b.pointIndexes[surfaceIndexes[2]]];
            p4 = transformedPoints[b.pointIndexes[surfaceIndexes[3]]];
            surfaces.push_back({p1, p2, p3, p4});
        }
        sort(surfaces.begin(), surfaces.end(), compareSurfacesDescending);
        for (int j = 3; j < 6; j++) { //a maximum of three surfaces could possibly be visible in three dimensions per cube anyways, so starting at j = 3 saves time
            p1 = surfaces[j][0];
            p2 = surfaces[j][1];
            p3 = surfaces[j][2];
            p4 = surfaces[j][3];


            //if (p1.z < 1e-2 || p2.z < 1e-2 || p3.z < 1e-2 || p4.z < 1e-2 ) { //screenCoords-funksjonen deler på z, men vet ikke hvor mye dette faktisk har å si
            //    continue;
            //}
            
            const array<int, 2>& sCoords1 = screenCoords(p1.x, p1.y, p1.z);
            int sX1 = sCoords1[0];
            int sY1 = sCoords1[1];
            Point corner1 = {sX1, sY1};

            const array<int, 2> sCoords2 = screenCoords(p2.x, p2.y, p2.z);
            int sX2 = sCoords2[0];
            int sY2 = sCoords2[1];
            Point corner2 = {sX2, sY2};

            const array<int, 2> sCoords3 = screenCoords(p3.x, p3.y, p3.z);
            int sX3 = sCoords3[0];
            int sY3 = sCoords3[1];
            Point corner3 = {sX3, sY3};
            
            const array<int, 2> sCoords4 = screenCoords(p4.x, p4.y, p4.z);
            int sX4 = sCoords4[0];
            int sY4 = sCoords4[1];
            Point corner4 = {sX4, sY4};

            if (p1.z > fov || p2.z > fov || p3.z > fov || p4.z > fov) {
                if (0 <= sX1 && sX1 <= windowWidth) {
                    if (0 <= sY1 && sY1 <= windowHeight) {
                        renderBlockSide(window, corner1, corner2, corner3, corner4, realColor);
                        continue;
                    }
                }
                if (0 <= sX2 && sX2 <= windowWidth) {
                    if (0 <= sY2 && sY2 <= windowHeight) {
                        renderBlockSide(window, corner1, corner2, corner3, corner4, realColor);
                        continue;
                    }
                }
                if (0 <= sX3 && sX3 <= windowWidth) {
                    if (0 <= sY3 && sY3 <= windowHeight) {
                        renderBlockSide(window, corner1, corner2, corner3, corner4, realColor);
                        continue;
                    }
                }
                if (0 <= sX4 && sX4 <= windowWidth) {
                    if (0 <= sY4 && sY4 <= windowHeight) {
                        renderBlockSide(window, corner1, corner2, corner3, corner4, realColor);
                    }
                }

            }
        }
    }
}

void World::saveMapData() {
    filesystem::path fileName{"Data/Maps/"};
    fileName += mapName + ".txt";
    ofstream outputStream{fileName};
    outputStream << blocks.size() << endl;
    for (auto& b : blocks) {
        outputStream << referencePoints.at(b.pointIndexes[0]).x << " ";
        outputStream << referencePoints.at(b.pointIndexes[0]).y << " ";
        outputStream << referencePoints.at(b.pointIndexes[0]).z << " ";
        outputStream << static_cast<int>(b.color) << endl;
    }
}

void World::loadMap(string mapNameString) { //loads the map (which consists of the blocks-vector and the referencepoints-vector) by calling the addBlock-function which initializes every block
    mapName = mapNameString; 
    filesystem::path fileName{"Data/Maps/"};
    fileName += mapName + ".txt";
    ifstream inputStream{fileName};
    blocks.clear();
    referencePoints.clear();

    if (filesystem::is_empty(fileName)) { //add a single block to map if empty
        BlockColors green {BlockColors::green};
        addBlock(0, 0, 4, green);
    } else { //initialize map normally
        int blockCount;
        inputStream >> blockCount;
        string nextWord;
        int x, y, z, colorInt;
        BlockColors color;
        for (int i = 0; i < blockCount; ++i) {
            inputStream >> x;
            inputStream >> y;
            inputStream >> z;
            inputStream >> colorInt;
            color = static_cast<BlockColors>(colorInt);
            addBlock(x, y, z, color);
        }
    }
}

//Player-class--------------------------------------------------------------
Player::Player(): activeColor(BlockColors::green), activeColorColor(getColor(activeColor))
{}

Color getColor(BlockColors& color) {
    Color defaultReturn {Color::green};
    switch (color) { //returns the correct corresponing color
        case BlockColors::red:
            return Color::red;
        case BlockColors::orange:
            return Color::orange;
        case BlockColors::yellow:
            return Color::yellow;
        case BlockColors::green:
            return Color::green;
        case BlockColors::blue:
            return Color::blue;
        case BlockColors::purple:
            return Color::purple;
        case BlockColors::black:
            return Color::black;
        case BlockColors::white:
            return Color::white;
        case BlockColors::grey:
            return Color::grey;
        case BlockColors::brown:
            return Color::brown;
        case BlockColors::burly_wood:
            return Color::burly_wood;
        case BlockColors::pink:
            return Color::pink;
    }
    return defaultReturn;
}

void Player::updateColor() {
    switch (activeColor) { //makes sure the correct color is rendered on the block
        case BlockColors::red:
            activeColorColor = Color::red;
            break;
        case BlockColors::orange:
            activeColorColor = Color::orange;
            break;
        case BlockColors::yellow:
            activeColorColor = Color::yellow;
            break;
        case BlockColors::green:
            activeColorColor = Color::green;
            break;
        case BlockColors::blue:
            activeColorColor = Color::blue;
            break;
        case BlockColors::purple:
            activeColorColor = Color::purple;
            break;
        case BlockColors::black:
            activeColorColor = Color::black;
            break;
        case BlockColors::white:
            activeColorColor = Color::white;
            break;
        case BlockColors::grey:
            activeColorColor = Color::grey;
            break;
        case BlockColors::brown:
            activeColorColor = Color::brown;
            break;
        case BlockColors::burly_wood:
            activeColorColor = Color::burly_wood;
            break;
        case BlockColors::pink:
            activeColorColor = Color::pink;
            break;
    }
}

void Player::nextColor() { //goes to the next available color, gets the index of the color, increases it by one and finds the new color
    int colorIdx = static_cast<int>(activeColor); 
    int colorLength = static_cast<int>(BlockColors::pink);
    if (colorIdx < colorLength) {
        colorIdx++;
    } else {
        colorIdx = 0;
    }
    activeColor = static_cast<BlockColors>(colorIdx);
    updateColor();
}

void Player::previousColor() { //goes to the next available color, gets the index of the color, decreases it by one and finds the new color
    int colorIdx = static_cast<int>(activeColor);
    int colorLength = static_cast<int>(BlockColors::pink);
    if (colorIdx > 0) {
        colorIdx--;
    } else {
        colorIdx = colorLength;
    }
    activeColor = static_cast<BlockColors>(colorIdx);
    updateColor();
}

void Player::move(string button) {
    auto sluttid = std::chrono::steady_clock::now();
    auto varighet = chrono::duration<double>(sluttid-starttidFrame).count();
    varighet *= 500; //scaler varighet
    //cout << varighet << endl;
    if (button == "W") {
        coords.z += (varighet/60.0)*trigValues.cXZ; //2 blocks i sekundet (60hz)
        coords.x -= (varighet/60.0)*trigValues.sXZ;
    } else if (button == "S") {
        coords.z -= (varighet/60.0)*trigValues.cXZ; //2 blocks i sekundet (60hz)
        coords.x += (varighet/60.0)*trigValues.sXZ;
    } else if (button == "A") {
        coords.x -= (varighet/60.0)*trigValues.cXZ; //2 blocks i sekundet (60hz)
        coords.z -= (varighet/60.0)*trigValues.sXZ;
    } else if (button == "D") {
        coords.x += (varighet/60.0)*trigValues.cXZ; //2 blocks i sekundet (60hz)
        coords.z += (varighet/60.0)*trigValues.sXZ;
    } else if (button == "SPACE") {
        coords.y += varighet/60.0;
    } else if (button == "LSHIFT") {
        coords.y -= varighet/60.0;
    } else if (button == "Q") {
        angles[2] -= 0.02;
    } else if (button == "E") {
        angles[2] += 0.02;
    }
}

void Player::resetPlayer() {
    angles = {0, 0, 0}; //should be in following order: yaw, pitch, roll, same order as rotations matricies
    deltaAngles = {0, 0, 0}; //used to change angles
    coords = {0, 2, 0};
}

void Player::getTrigValues() {
    //calculates sin and cosine values in advance because it is resource intensive to do so for every block
    double c = cos(angles[0]);
    double s = sin(angles[0]);
    trigValues.cXZ = c;
    trigValues.sXZ = s;
    c = cos(angles[1]);
    s = sin(angles[1]);
    trigValues.cYZ = c;
    trigValues.sYZ = s;
    c = cos(angles[2]);
    s = sin(angles[2]);
    trigValues.cXY = c;
    trigValues.sXY = s;
}
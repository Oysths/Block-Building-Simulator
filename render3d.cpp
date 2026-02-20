#include "render3d.h"

void WorldPointDouble::printPoint() {
    cout << "x: " << x << ", y: " << y << ", z: " << z << endl;
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

//Block-class-------------------------------------------------------
Block::Block(int x, int y, int z, World& world): pointIndexes(getBlockIndexes(x, y, z, world))
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

void World::renderLine(AnimationWindow& window) {

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

            if (p1.z < 1e-2) { //screenCoords-funksjonen deler på z, men vet ikke hvor mye dette faktisk har å si
                continue;
            }
            
            const array<int, 2>& sCoords1 = screenCoords(p1.x, p1.y, p1.z);
            int sX1 = sCoords1.at(0);
            int sY1 = sCoords1.at(1);
            Point lineStart = {sX1, sY1};
            if (p2.z < 1e-2) { //screenCoords-funksjonen deler på z, men vet ikke hvor mye dette faktisk har å si
                continue;
            } 
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

void World::addBlock(int x, int y, int z) {
    blocks.push_back(Block {x, y, z, *this}); //sender også objektet det ble kalt fra som reference
}


//Player-class--------------------------------------------------------------
void Player::move(string button) {
    if (button == "W") {
        coords.z += (2.0/60.0)*trigValues.cXZ; //2 blocks i sekundet (60hz)
        coords.x -= (2.0/60.0)*trigValues.sXZ;
    } else if (button == "S") {
        coords.z -= (2.0/60.0)*trigValues.cXZ; //2 blocks i sekundet (60hz)
        coords.x += (2.0/60.0)*trigValues.sXZ;
    } else if (button == "A") {
        coords.x -= (2.0/60.0)*trigValues.cXZ; //2 blocks i sekundet (60hz)
        coords.z -= (2.0/60.0)*trigValues.sXZ;
    } else if (button == "D") {
        coords.x += (2.0/60.0)*trigValues.cXZ; //2 blocks i sekundet (60hz)
        coords.z += (2.0/60.0)*trigValues.sXZ;
    } else if (button == "SPACE") {
        coords.y += 2.0/60.0;
    } else if (button == "LSHIFT") {
        coords.y -= 2.0/60.0;
    } else if (button == "Q") {
        angles[2] -= 0.02;
    } else if (button == "E") {
        angles[2] += 0.02;
    }
}

void Player::getTrigValues() {
    //calculates sin and cosine values in advance because it is resource intensive to do so for every block
    double c = cos(angles.at(0));
    double s = sin(angles.at(0));
    trigValues.cXZ = c;
    trigValues.sXZ = s;
    c = cos(angles.at(1));
    s = sin(angles.at(1));
    trigValues.cYZ = c;
    trigValues.sYZ = s;
    c = cos(angles.at(2));
    s = sin(angles.at(2));
    trigValues.cXY = c;
    trigValues.sXY = s;
}

void World::placeBlock (Player player) {
    //cout << "Prøver å plassere" << endl;
    double placementRange = 3.0;
    double deltaRange = 0.003;
    double x = -cos(player.angles[1])*sin(player.angles[0]);
    double z = cos(player.angles[0])*cos(player.angles[1]);
    double y = sin(player.angles[1]); 
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
            addBlock(floor(p.x), floor(p.y), floor(p.z));
            //cout << "Blokk plassert" << endl;
            break;
        }
    }
}

void World::breakBlock(Player player) {
    double breakRange = 3.0;
    double deltaRange = 0.003;
    double x = -cos(player.angles[1])*sin(player.angles[0]);
    double z = cos(player.angles[0])*cos(player.angles[1]);
    double y = sin(player.angles[1]); 
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
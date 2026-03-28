#pragma once

#include <array>
#include <vector>

#include "Feature.h"

class Level;

class BetaBigTreeFeature : public Feature
{
private:
    static byte axisConversionArray[6];

    Random betaRandom;
    Level *level;
    int origin[3];
    int heightLimit;
    int trunkTopY;
    double heightAttenuation;
    double branchSlope;
    double widthScale;
    double foliageDensity;
    int trunkWidth;
    int heightVariance;
    int foliageHeight;
    std::vector<std::array<int, 4>> foliageCoords;

    void generateLeafNodeList();
    void placeLeafLayer(int x, int y, int z, float radius, byte axis, int tile, int data = 0);
    float treeShape(int localY) const;
    float foliageShape(int localY) const;
    void placeLeafNode(int x, int y, int z);
    void placeLine(const int *start, const int *end, int tile, int data = 0);
    void placeLeafNodes();
    bool leafNodeNeedsBase(int localY) const;
    void placeTrunk();
    void placeBranches();
    int checkLine(const int *start, const int *end) const;
    bool validTreeLocation();

public:
    BetaBigTreeFeature(bool doUpdate = false);

    virtual void init(double scaleX, double scaleY, double scaleZ);
    virtual bool place(Level *level, Random *random, int x, int y, int z);
};

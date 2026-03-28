#include "stdafx.h"

#include <cmath>

#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "Mth.h"
#include "BetaBigTreeFeature.h"
#include "BetaTreeSupport.h"

byte BetaBigTreeFeature::axisConversionArray[6] = { 2, 0, 0, 1, 2, 1 };

BetaBigTreeFeature::BetaBigTreeFeature(bool doUpdate) : Feature(doUpdate), level(nullptr), heightLimit(0), trunkTopY(0),
    heightAttenuation(0.618), branchSlope(0.381), widthScale(1.0), foliageDensity(1.0),
    trunkWidth(1), heightVariance(12), foliageHeight(4)
{
    origin[0] = 0;
    origin[1] = 0;
    origin[2] = 0;
}

void BetaBigTreeFeature::generateLeafNodeList()
{
    trunkTopY = static_cast<int>(static_cast<double>(heightLimit) * heightAttenuation);
    if (trunkTopY >= heightLimit)
    {
        trunkTopY = heightLimit - 1;
    }

    int clustersPerY = static_cast<int>(1.382 + std::pow(foliageDensity * static_cast<double>(heightLimit) / 13.0, 2.0));
    if (clustersPerY < 1)
    {
        clustersPerY = 1;
    }

    foliageCoords.clear();
    foliageCoords.reserve(clustersPerY * heightLimit);

    int y = origin[1] + heightLimit - foliageHeight;
    int trunkTop = origin[1] + trunkTopY;
    int relativeY = y - origin[1];

    foliageCoords.push_back({ origin[0], y, origin[2], trunkTop });
    --y;

    while (relativeY >= 0)
    {
        float shape = treeShape(relativeY);
        if (shape < 0.0f)
        {
            --y;
            --relativeY;
            continue;
        }

        for (int cluster = 0; cluster < clustersPerY; ++cluster)
        {
            double radius = widthScale * static_cast<double>(shape) * (static_cast<double>(betaRandom.nextFloat()) + 0.328);
            double angle = static_cast<double>(betaRandom.nextFloat()) * 2.0 * 3.14159;
            int x = Mth::floor(radius * std::sin(angle) + static_cast<double>(origin[0]) + 0.5);
            int z = Mth::floor(radius * std::cos(angle) + static_cast<double>(origin[2]) + 0.5);
            int start[3] = { x, y, z };
            int end[3] = { x, y + foliageHeight, z };

            if (checkLine(start, end) != -1)
            {
                continue;
            }

            int branchBase[3] = { origin[0], origin[1], origin[2] };
            double distance = std::sqrt(
                std::pow(static_cast<double>(std::abs(origin[0] - start[0])), 2.0) +
                std::pow(static_cast<double>(std::abs(origin[2] - start[2])), 2.0));
            double branchHeight = distance * branchSlope;

            if (static_cast<double>(start[1]) - branchHeight > static_cast<double>(trunkTop))
            {
                branchBase[1] = trunkTop;
            }
            else
            {
                branchBase[1] = static_cast<int>(static_cast<double>(start[1]) - branchHeight);
            }

            if (checkLine(branchBase, start) == -1)
            {
                foliageCoords.push_back({ x, y, z, branchBase[1] });
            }
        }

        --y;
        --relativeY;
    }
}

void BetaBigTreeFeature::placeLeafLayer(int x, int y, int z, float radius, byte axis, int tile, int data)
{
    int rad = static_cast<int>(static_cast<double>(radius) + 0.618);
    byte axis1 = axisConversionArray[axis];
    byte axis2 = axisConversionArray[axis + 3];
    int center[3] = { x, y, z };
    int position[3] = { 0, 0, 0 };
    position[axis] = center[axis];

    for (int offset1 = -rad; offset1 <= rad; ++offset1)
    {
        position[axis1] = center[axis1] + offset1;

        for (int offset2 = -rad; offset2 <= rad; ++offset2)
        {
            double distance = std::sqrt(
                std::pow(static_cast<double>(std::abs(offset1)) + 0.5, 2.0) +
                std::pow(static_cast<double>(std::abs(offset2)) + 0.5, 2.0));

            if (distance > static_cast<double>(radius))
            {
                continue;
            }

            position[axis2] = center[axis2] + offset2;
            int existing = level->getTile(position[0], position[1], position[2]);
            if (existing != 0 && existing != Tile::leaves_Id)
            {
                continue;
            }

            PlaceBetaWorldgenBlock(level, position[0], position[1], position[2], tile, data);
        }
    }
}

float BetaBigTreeFeature::treeShape(int localY) const
{
    if (static_cast<double>(localY) < static_cast<double>(static_cast<float>(heightLimit) * 0.3f))
    {
        return -1.618f;
    }

    float radius = static_cast<float>(heightLimit) / 2.0f;
    float adjacent = static_cast<float>(heightLimit) / 2.0f - static_cast<float>(localY);
    float distance;

    if (adjacent == 0.0f)
    {
        distance = radius;
    }
    else if (std::abs(adjacent) >= radius)
    {
        distance = 0.0f;
    }
    else
    {
        distance = static_cast<float>(std::sqrt(
            std::pow(static_cast<double>(std::abs(radius)), 2.0) -
            std::pow(static_cast<double>(std::abs(adjacent)), 2.0)));
    }

    distance *= 0.5f;
    return distance;
}

float BetaBigTreeFeature::foliageShape(int localY) const
{
    if (localY < 0 || localY >= foliageHeight)
    {
        return -1.0f;
    }

    return (localY != 0 && localY != foliageHeight - 1) ? 3.0f : 2.0f;
}

void BetaBigTreeFeature::placeLeafNode(int x, int y, int z)
{
    for (int leafY = y; leafY < y + foliageHeight; ++leafY)
    {
        float radius = foliageShape(leafY - y);
        placeLeafLayer(x, leafY, z, radius, 1, Tile::leaves_Id, 0);
    }
}

void BetaBigTreeFeature::placeLine(const int *start, const int *end, int tile, int data)
{
    int delta[3] = { 0, 0, 0 };
    byte primary = 0;
    byte index = 0;

    for (primary = 0; index < 3; ++index)
    {
        delta[index] = end[index] - start[index];
        if (std::abs(delta[index]) > std::abs(delta[primary]))
        {
            primary = index;
        }
    }

    if (delta[primary] == 0)
    {
        return;
    }

    byte axis1 = axisConversionArray[primary];
    byte axis2 = axisConversionArray[primary + 3];
    int direction = delta[primary] > 0 ? 1 : -1;
    double step1 = static_cast<double>(delta[axis1]) / static_cast<double>(delta[primary]);
    double step2 = static_cast<double>(delta[axis2]) / static_cast<double>(delta[primary]);
    int position[3] = { 0, 0, 0 };

    for (int offset = 0, finish = delta[primary] + direction; offset != finish; offset += direction)
    {
        position[primary] = Mth::floor(static_cast<double>(start[primary] + offset) + 0.5);
        position[axis1] = Mth::floor(static_cast<double>(start[axis1]) + static_cast<double>(offset) * step1 + 0.5);
        position[axis2] = Mth::floor(static_cast<double>(start[axis2]) + static_cast<double>(offset) * step2 + 0.5);
        PlaceBetaWorldgenBlock(level, position[0], position[1], position[2], tile, data);
    }
}

void BetaBigTreeFeature::placeLeafNodes()
{
    for (size_t i = 0; i < foliageCoords.size(); ++i)
    {
        const std::array<int, 4> &coord = foliageCoords[i];
        placeLeafNode(coord[0], coord[1], coord[2]);
    }
}

bool BetaBigTreeFeature::leafNodeNeedsBase(int localY) const
{
    return static_cast<double>(localY) >= static_cast<double>(heightLimit) * 0.2;
}

void BetaBigTreeFeature::placeTrunk()
{
    int start[3] = { origin[0], origin[1], origin[2] };
    int end[3] = { origin[0], origin[1] + trunkTopY, origin[2] };

    placeLine(start, end, Tile::treeTrunk_Id, 0);

    if (trunkWidth == 2)
    {
        ++start[0];
        ++end[0];
        placeLine(start, end, Tile::treeTrunk_Id, 0);
        ++start[2];
        ++end[2];
        placeLine(start, end, Tile::treeTrunk_Id, 0);
        --start[0];
        --end[0];
        placeLine(start, end, Tile::treeTrunk_Id, 0);
    }
}

void BetaBigTreeFeature::placeBranches()
{
    int base[3] = { origin[0], origin[1], origin[2] };

    for (size_t i = 0; i < foliageCoords.size(); ++i)
    {
        const std::array<int, 4> &coord = foliageCoords[i];
        int end[3] = { coord[0], coord[1], coord[2] };
        base[1] = coord[3];

        if (leafNodeNeedsBase(base[1] - origin[1]))
        {
            placeLine(base, end, Tile::treeTrunk_Id, 0);
        }
    }
}

int BetaBigTreeFeature::checkLine(const int *start, const int *end) const
{
    int delta[3] = { 0, 0, 0 };
    byte primary = 0;
    byte index = 0;

    for (primary = 0; index < 3; ++index)
    {
        delta[index] = end[index] - start[index];
        if (std::abs(delta[index]) > std::abs(delta[primary]))
        {
            primary = index;
        }
    }

    if (delta[primary] == 0)
    {
        return -1;
    }

    byte axis1 = axisConversionArray[primary];
    byte axis2 = axisConversionArray[primary + 3];
    int direction = delta[primary] > 0 ? 1 : -1;
    double step1 = static_cast<double>(delta[axis1]) / static_cast<double>(delta[primary]);
    double step2 = static_cast<double>(delta[axis2]) / static_cast<double>(delta[primary]);
    int position[3] = { 0, 0, 0 };
    int offset = 0;
    int finish = delta[primary] + direction;

    while (offset != finish)
    {
        position[primary] = start[primary] + offset;
        position[axis1] = Mth::floor(static_cast<double>(start[axis1]) + static_cast<double>(offset) * step1);
        position[axis2] = Mth::floor(static_cast<double>(start[axis2]) + static_cast<double>(offset) * step2);
        int tile = level->getTile(position[0], position[1], position[2]);
        if (tile != 0 && tile != Tile::leaves_Id)
        {
            break;
        }
        offset += direction;
    }

    return offset == finish ? -1 : std::abs(offset);
}

bool BetaBigTreeFeature::validTreeLocation()
{
    int start[3] = { origin[0], origin[1], origin[2] };
    int end[3] = { origin[0], origin[1] + heightLimit - 1, origin[2] };
    int below = level->getTile(origin[0], origin[1] - 1, origin[2]);

    if (below != Tile::grass_Id && below != Tile::dirt_Id)
    {
        return false;
    }

    int heightCheck = checkLine(start, end);
    if (heightCheck == -1)
    {
        return true;
    }

    if (heightCheck < 6)
    {
        return false;
    }

    heightLimit = heightCheck;
    return true;
}

void BetaBigTreeFeature::init(double scaleX, double scaleY, double scaleZ)
{
    heightVariance = static_cast<int>(scaleX * 12.0);
    if (scaleX > 0.5)
    {
        foliageHeight = 5;
    }
    widthScale = scaleY;
    foliageDensity = scaleZ;
}

bool BetaBigTreeFeature::place(Level *level, Random *random, int x, int y, int z)
{
    this->level = level;
    betaRandom.setSeed(random->nextLong());
    origin[0] = x;
    origin[1] = y;
    origin[2] = z;

    if (heightLimit == 0)
    {
        heightLimit = 5 + betaRandom.nextInt(heightVariance);
    }

    if (!validTreeLocation())
    {
        return false;
    }

    generateLeafNodeList();
    placeLeafNodes();
    placeTrunk();
    placeBranches();
    return true;
}

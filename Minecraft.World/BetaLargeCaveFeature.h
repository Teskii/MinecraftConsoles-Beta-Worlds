#pragma once

#include "LargeFeature.h"
#include "javaRnd.h"

class BetaLargeCaveFeature : public LargeFeature
{
public:
    virtual void apply(ChunkSource *chunkSource, Level *level, int xOffs, int zOffs, byteArray blocks);

protected:
    virtual void addFeature(Level *level, int x, int z, int xOffs, int zOffs, byteArray blocks);

private:
    JavaRand m_betaRandom;
    void addRoom(int xOffs, int zOffs, byteArray blocks, double xRoom, double yRoom, double zRoom);
    void addTunnel(int xOffs, int zOffs, byteArray blocks, double xCave, double yCave, double zCave,
                   float thickness, float yRot, float xRot, int step, int dist, double yScale);
};

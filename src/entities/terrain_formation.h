#ifndef TERRAIN_FAULT_FORMATION
#define TERRAIN_FAULT_FORMATION

#include <random>
#include <iostream>
#include <assert.h>

class TerrainFaultFormation{
public:
    TerrainFaultFormation(unsigned int sizeX, unsigned int sizeZ);
    ~TerrainFaultFormation();
    
    void generate(unsigned int iterations, float scale);
    float **map;
private:
    int m_randomPoints[2][2];
    int m_sizeX, m_sizeZ;
    void generateInternal(float _scale);
    std::minstd_rand0 randomGen;
};

TerrainFaultFormation::TerrainFaultFormation(unsigned int sizeX, unsigned int sizeZ) : m_sizeX(sizeX), m_sizeZ(sizeZ) {
    std::random_device rd;
    randomGen = std::minstd_rand0(rd());

}
TerrainFaultFormation::~TerrainFaultFormation(){
}

void TerrainFaultFormation::generate(unsigned int iterations, float scale){
    for(unsigned int i = 0; i < iterations; i++){
        generateInternal(scale * (1 - i / (float)iterations));
    }
}

void TerrainFaultFormation::generateInternal(float _scale){
    static std::uniform_int_distribution<int> randomCoordX(0,m_sizeX);
    static std::uniform_int_distribution<int> randomCoordZ(0,m_sizeZ);
    bool isDifferent = false;
    unsigned int generateFailCount = 0;
    while(!isDifferent && generateFailCount < 100){
        m_randomPoints[0][0] = randomCoordX(randomGen);
        m_randomPoints[0][1] = randomCoordZ(randomGen);
        m_randomPoints[1][0] = randomCoordX(randomGen);
        m_randomPoints[1][1] = randomCoordZ(randomGen);
        
        if(m_randomPoints[0][0] != m_randomPoints[1][0])
            isDifferent = true;
        if(m_randomPoints[0][1] != m_randomPoints[1][1])
            isDifferent = true;
    }

    if(!isDifferent){
        std::cout << "TerrainFaultFormation::Failed to generate random points" << std::endl;
        assert(isDifferent);
    }
    
    int p0[2] = {m_randomPoints[0][0], m_randomPoints[0][1]};
    int p1[2] = {m_randomPoints[1][0], m_randomPoints[1][1]};

    int cutLineDirX = p1[0] - p0[0];
    int cutLineDirZ = p1[1] - p0[1];

    for(unsigned int z = 0; z < m_sizeZ; z++){
        for(unsigned int x = 0; x < m_sizeX; x++){
            int dirX = x - p0[0];
            int dirZ = z - p0[1];

            int crossProduct = cutLineDirX * dirZ - cutLineDirZ * dirX;

            float deltaHeight;
            if(crossProduct > 0){
                deltaHeight = _scale;
            }
            else{
                deltaHeight = (-1)*_scale;
            }

            map[x][z] += deltaHeight;
            // addHeight(x,z, deltaHeight);
        }
    }
}

#endif
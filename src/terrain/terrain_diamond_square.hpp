#ifndef TERRAIN_DIAMOND_SQUARE
#define TERRAIN_DIAMOND_SQUARE
// unsigned int 떼기
#include "terrain.hpp"

bool is_power_of_two(int n);
unsigned int wrapCoord(int coord, unsigned int size);

class TerrainDiaSq : public Terrain{
public:
    TerrainDiaSq(unsigned int _size, std::string texturePath);
    void generate(float roughness, float initialDisplacement);
private:
    unsigned int m_size;
    void diamondStep(unsigned int squareSize, float randomDisplacementInterval);
    void squareStep(unsigned int squareSize, float randomDisplacementInterval);
    void diamondStepSampling(unsigned int squareSize, int x, int z, float randomDisplacementInterval);
    void squareStepSampling(unsigned int squareSize, int x, int z, float randomDisplacementInterval);
};

#endif
#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <vector>
#include <random>
#include <utils/glPrerequisites.h>
#include <utils/learnopenglPrerequisites.h>
#include <string>

/**
 * Terrain has x coord of 0 ~ sizeX - 1.
 * The heightmap only has integer coordinate
 * Hence, heightmap has (sizeX) * (sizeZ) heights.
 * But length is (sizeX - 1) * (sizeZ - 1)
*/
class Terrain{
public:
    Terrain(unsigned int sizeX, unsigned int sizeZ, std::string texturePath);
    // ~Terrain();

    // void operator delete(void* p);

    void Draw(Shader *shader);

    void clearHeightMap();

    void Randomize(unsigned int blockSize, float blockScale, float uniformScale);
    void ScaleHeight(float scale);
    
    inline float getHeight(int _x, int _z) const { return m_heightMap[_x       + _z*m_sizeX]; }
    inline float getHeight(glm::ivec2 _pos) const { return m_heightMap[_pos.x   + _pos.y*m_sizeX]; }
    inline float* getHeightMap() const { return m_heightMap; }
    float* getVertices() const { return m_vertices; }
    inline unsigned int getSizeX() const { return m_sizeX; }
    inline unsigned int getSizeZ() const { return m_sizeX; }
    inline unsigned int getSizeXZ() const { return m_sizeX * m_sizeZ; }
    
    // size of m_vertices is 6 times.
    inline unsigned int getNumOfVertices() const { return m_sizeX * m_sizeZ; };
    inline unsigned int getNumOfIndicies() const { return (2*m_sizeX+2)*(m_sizeZ-1)-2; }
    inline unsigned int getNumOfTriangles() const { return getNumOfIndicies() - 2; }
    inline unsigned int getNumOfHeightMap() const { return getSizeXZ(); }
    
    inline void setHeight(int _x, int _z, float value)  { m_heightMap[_x + _z*m_sizeX] = value; }
    inline void setHeight(glm::ivec2 _pos, float value) { m_heightMap[_pos.x + _pos.y*m_sizeX] = value; }
    inline void addHeight(int _x, int _z, float value)  { m_heightMap[_x + _z*m_sizeX] += value; }
    inline void addHeight(glm::ivec2 _pos, float value) { m_heightMap[_pos.x + _pos.y*m_sizeX] += value; }

    
    // utility functions
    void smooth(unsigned int iterTimes);
    void standardize();
    void scale(float scale);
    
    void printHeightMap() const;
    
protected:
    const unsigned int m_sizeX, m_sizeZ;
    float *m_heightMap;
    float *m_vertices;

    unsigned int *m_indices;
    GLuint m_VAO, m_VBO, m_EBO;
    GLuint texture;

    void initHeightMap();
    void initVertices();
    void initIndices();
    void initDraw();
    void getTexture(std::string texturePath);

    void clearAll();
    void clearVertices();
    void clearIndices();

    void updateVertices();
    void randomizeBlock(unsigned int blockSize, float blockScale);
    std::minstd_rand0 randomGen;
};

#endif
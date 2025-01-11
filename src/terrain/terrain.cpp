#include "terrain.hpp"

#include <assert.h>
#include <iostream>


// Constructor : make flat terrain
Terrain::Terrain(unsigned int sizeX, unsigned int sizeZ, std::string texturePath) : m_sizeX(sizeX), m_sizeZ(sizeZ){
    m_heightMap = new float[m_sizeX*m_sizeZ];
    m_vertices = new float[getNumOfVertices() * 6];
    m_indices = new unsigned int[getNumOfIndicies()];
    initHeightMap();
    initVertices();
    initIndices();
    getTexture(texturePath);
    initDraw();
    std::random_device rd;
    randomGen = std::minstd_rand0(rd());
}
// Terrain::~Terrain(){
//     // delete m_heightMap;
//     // delete m_vertices;
//     // delete m_indices;
//     // m_heightMap = nullptr;
//     // m_vertices = nullptr;
//     // m_indices = nullptr;
// }

void Terrain::Draw(Shader *shader){
    shader->use();
    shader->setInt("texture_diffuse1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture);

    // updateVertices();
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(float) * getNumOfVertices() * 6, m_vertices, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glDrawElements(GL_TRIANGLE_STRIP, getNumOfIndicies(), GL_UNSIGNED_INT, 0);
}


inline void Terrain::clearHeightMap(){
    for(unsigned int i = 0; i < getNumOfHeightMap(); i++)
        m_heightMap[i] = 0.0f;
}

// get normalized (-1.0~1.0) heights
void Terrain::Randomize(unsigned int blockSize = 1, float blockScale = 1, float uniformScale = 1){
    static std::uniform_real_distribution<float> randomFloat(-1, 1);
    // float sizeReduceTimes = (float)(m_sizeX > m_sizeZ ? m_sizeX : m_sizeZ) / blockSize;
    if(blockSize > 1 && blockScale > 1){
        // random height per block
        for (unsigned int i = blockSize; i > 1; i /= 2){
            std::cout << "i : " << i << std::endl;
            randomizeBlock(i, blockScale * i / blockSize);
        }
    }
    // add random height uniformly
    for (unsigned int i = 0; i < m_sizeX*m_sizeZ; i++){
        // m_heightMap[i] += uniformScale * ((float)(gen()) / gen.max() * 2 - 1.0f);
        m_heightMap[i] += uniformScale * randomFloat(randomGen);
    }
}

void Terrain::ScaleHeight(float scale){
    for(unsigned int i = 0; i < getNumOfVertices(); i++)
        m_heightMap[i] *= scale;
}

void Terrain::randomizeBlock(unsigned int blockSize, float blockScale){
    static std::uniform_real_distribution<float> randomFloat(-1, 1);
    for (unsigned int i = 0; i < m_sizeZ; i += blockSize){
        for (unsigned int j = 0; j < m_sizeX; j+= blockSize){
            float randomPerBlock = randomFloat(randomGen);
            for(unsigned int k = i; (k < i + blockSize) && (k < m_sizeZ); k++){
                for(unsigned int l = j; (l < j + blockSize) && (l < m_sizeX); l++){
                    // m_heightMap[i*m_sizeX + j + k*m_sizeZ + l] += randomPerBlock * blockScale;
                    m_heightMap[k*m_sizeX + l] += randomPerBlock * blockScale;
                }
            }
        }
    }
}
///////////
// utils
///////////

void Terrain::smooth(unsigned int iterTimes){
    // copy original one
    float originalHeightMap[m_sizeX*m_sizeZ];
    for(unsigned int x=0;x<m_sizeX;x++)
        for(unsigned int z=0;z<m_sizeZ;z++)
            originalHeightMap[x+z*m_sizeX] = getHeight(x,z);

    for(unsigned i = 0; i < iterTimes; i++){
        for(int x = 0; x < m_sizeX; x++){
            for(int z = 0; z < m_sizeZ; z++){
                // sample adjacent points
                float samplingSum = 0;
                unsigned int numOfSamplePoints = 0;
                for(int _x = x-1; _x < x+2; _x++){
                    for(int _z = z-1; _z < z+2; _z++){
                        if(0 <= _x && _x < m_sizeX && 0 <= _z && _z < m_sizeZ){
                            samplingSum += originalHeightMap[_x+_z*m_sizeX];
                            numOfSamplePoints++;
                        }
                    }
                }
                setHeight(x, z, samplingSum / numOfSamplePoints);
            }
        }
    }
}

void Terrain::standardize(){
    float maxHeight = 0;
    float minHeight = 0;
    for(unsigned int i = 0; i < getNumOfHeightMap(); i++){
        maxHeight = maxHeight > m_heightMap[i]? maxHeight : m_heightMap[i];
        minHeight = minHeight < m_heightMap[i]? minHeight : m_heightMap[i];
    }

    float displacement = maxHeight - minHeight;
    if(displacement != 0)
        for(unsigned int i = 0; i < getNumOfHeightMap(); i++){
            m_heightMap[i] /= displacement;
        }
}

inline void Terrain::scale(float scale){
    for(unsigned int i = 0; i < m_sizeX * m_sizeZ; i++){
        m_heightMap[i] *= scale;
    }
}

void Terrain::printHeightMap() const{
    printf("--- heightmap ---\n");
    for(unsigned int z = 0; z < m_sizeZ; z++){
        for(unsigned int x = 0; x < m_sizeX; x++){
            printf("%5.3f   ", getHeight(x,z));
        }
        printf("\n");
    }
}

//////////
// init
//////////

void Terrain::initHeightMap(){
    for(unsigned int i = 0; i < getNumOfVertices(); i++){
        m_heightMap[i] = 0;
    }
}

// fill y value with 0
void Terrain::initVertices(){
    /**
     * set vertices ( X = m_sizeX )
     *  ----> x-axis
     *  0 --- 1 --- 2 --- ... ---  X-2 --- X-1
     *  |  /  |  /  |  /       /    |   /   |  
     *  X ---X+1---X+2--- ... --- 2X-2 --- 2X-1
     *  |  /  |  /  |  /       /    |   /   |  
     * ...
     *  |  /  |  /  |  /       /    |   /   |  
     *X(Z-1) ...                          XZ-1
    */
    int i = 0;
    for(int z = 0; z < m_sizeZ; z++){
        for(int x = 0; x < m_sizeX; x++){
            m_vertices[i++] = (float)x;
            m_vertices[i++] = 0;
            m_vertices[i++] = (float)z;
            i += 3;
        }
    }
    assert(i == getNumOfVertices() * 6);
}

void Terrain::initIndices(){
    /**
     * init indices
     * 
     *  0 --- 2 --- 4 --- ... --- 2X-4 --- 2X-2          z = 0
     *  |  /  |  /  |  /       /    |   /   |  
     *  1 --- 3 --- 5 --- ... --- 2X-3 --- 2X-1          z = 1
     *(2X+1)                               (2X)
     * 2X+2  2X+4  2X+6           4X-2      4X
     *  |  /  |  /  |  /       /    |   /   |  
     * ...
     *  |  /  |  /  |  /       /    |   /   |  
     * 
    */
    unsigned int j = 0;
    for(unsigned int z = 0; z < m_sizeZ - 2; z++){ // except last row triangles
        for(unsigned int x = 0; x < m_sizeX; x++){
            m_indices[j++] = z*m_sizeX + x;
            m_indices[j++] = (z+1)*m_sizeX + x;
        }
        // degenerated triangles
        m_indices[j++] = z*m_sizeX + (m_sizeX-1) + m_sizeX;
        m_indices[j++] = (z+1)*m_sizeX;
    }
    // last row; no degeneration
    unsigned int z = m_sizeZ - 2;
    for(unsigned int x = 0; x < m_sizeX; x++){
        m_indices[j++] = z*m_sizeX + x;
        m_indices[j++] = (z+1)*m_sizeX + x;
    }

    assert(j == getNumOfIndicies());
}

// void Terrain::initNormals(){
//     unsigned int i = 0;
//     for(unsigned int z = 0; z < m_sizeZ; z++){
//         for(unsigned int x = 0; x < m_sizeX; x++){
//             m_vertices[i++] = (float)x;
//             m_vertices[i++] = 0;
//             m_vertices[i++] = (float)z;
//         }
//     }
// }


void Terrain::getTexture(std::string texturePath){
    // Create texture ids.
    glGenTextures(1, &texture);
    
    // All upcomming GL_TEXTURE_2D operations now on "texture" object
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Set texture parameters for wrapping.
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    unsigned char *image = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (!image) {
        printf("texture %s loading error ... \n", texturePath.c_str());
    }
    else printf("texture %s loaded\n", texturePath.c_str());
    
    GLenum format;
    if (nrChannels == 1) format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;
    
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Terrain::initDraw(){
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * getNumOfVertices() * 6, m_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*getNumOfIndicies(), m_indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Terrain::clearAll(){
    clearVertices();
    clearHeightMap();
    clearIndices();
}

inline void Terrain::clearVertices(){
    for(unsigned int i = 0; i < getNumOfTriangles() * 6; i++){
        m_vertices[i] = 0.f;
    }
}


inline void Terrain::clearIndices(){
    for(unsigned int i = 0; i < getNumOfIndicies(); i++)
        m_indices[i] = 0.0f;
}


void Terrain::updateVertices(){
    int i = 1;
    for(unsigned int z = 0; z < m_sizeZ; z++){
        for(unsigned int x = 0; x < m_sizeX; x++){
            m_vertices[i] = getHeight(x,z);
            i += 6;
        }
    }
    
    
    /**
     * 
     *  0 --- 2 --- 4 --- ... --- 2X-4 --- 2X-2          z = 0
     *  |  /  |  /  |  /       /    |   /   |  
     *  1 --- 3 --- 5 --- ... --- 2X-3 --- 2X-1          z = 1
     *(2X+1)                               (2X)
     * 2X+2  2X+4  2X+6           4X-2      4X
     *  |  /  |  /  |  /       /    |   /   |  
     * ...
     *  |  /  |  /  |  /       /    |   /   |  
     * 
     * 0  ---  1   ---  2
     * |   /   |    /   |
     * X  --- X+1 --- X+2
     * |   /   |    /   |
     * 2X --- 2X+1 --- 2X+2
     * 
     * two vertices of triangles that surrounding X+1 (other than X+1)
     * 1, X
     * 1, 2
     * 2, X+2
     * X, 2X
     * 2X, 2X+1
     * 2X+1, X+2
     * 
     * surrounding A
     * A-X, A-1
     * A-X, A-X+1
     * A-X+1, A+1
     * A-1, A+X-1
     * A+X-1, A+X
     * A+X, A+1
     * 
     * 
     * 
     * 0 1 2
     * 3 x 4
     * 5 6 7
     * 
     * 13
     * 12
     * 24
     * 35
     * 56
     * 64
    */

    i = 3;
    for(int z = 0; z < getSizeZ(); z++){
        for(int x = 0; x < getSizeX(); x++){
            int xMinus = glm::max<int>(x-1, 0);
            int xPlus = glm::min<int>(x+1, getSizeX()-1);
            int zMinus = glm::max<int>(z-1, 0);
            int zPlus = glm::min<int>(z+1, getSizeZ()-1);
            float h = getHeight(x,z);

            // vector x -> n
            glm::vec3 v1 = glm::vec3(0.f, getHeight(x, zMinus) - h, -1.f);
            glm::vec3 v2 = glm::vec3(+1.f, getHeight(xPlus, zMinus) - h, -1.f);
            glm::vec3 v3 = glm::vec3(-1.f, getHeight(xMinus, z) - h, 0.f);
            glm::vec3 v4 = glm::vec3(+1.f, getHeight(xPlus, z) - h, 0.f);
            glm::vec3 v5 = glm::vec3(-1.f, getHeight(xMinus, zPlus) - h, +1.f);
            glm::vec3 v6 = glm::vec3(0.f, getHeight(x, zPlus) - h, +1.f);
            
            // normals made with 13, 21, 42, 35, 56, 64
            glm::vec3 n1 = glm::normalize(glm::cross(v1, v3));
            glm::vec3 n2 = glm::normalize(glm::cross(v2, v1));
            glm::vec3 n3 = glm::normalize(glm::cross(v4, v2));
            glm::vec3 n4 = glm::normalize(glm::cross(v3, v5));
            glm::vec3 n5 = glm::normalize(glm::cross(v5, v6));
            glm::vec3 n6 = glm::normalize(glm::cross(v6, v4));

            glm::vec3 normal = (n1+n2+n3+n4+n5+n6)/6.f;

            // std::cerr << x << ", " << z << " : " << i;
            m_vertices[i++] = normal.x;
            m_vertices[i++] = normal.y;
            m_vertices[i++] = normal.z;
            i += 3;
        }
    }

    // assert(i-1 == getNumOfVertices() * 6);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * getNumOfVertices() * 6, m_vertices);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) 0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) (3*sizeof(float)));
    // glEnableVertexAttribArray(1);

}
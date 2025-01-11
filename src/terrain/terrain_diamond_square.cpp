#include "terrain_diamond_square.hpp"

TerrainDiaSq::TerrainDiaSq(unsigned int _size, std::string texturePath) : Terrain(_size, _size, texturePath) {m_size = _size;}

void TerrainDiaSq::generate(float roughness, float initialDisplacement){
    if(!is_power_of_two(m_sizeX - 1)){
        std::cout << "Invalid size for diamond square step" << std::endl;
    }
    else{
        // @note static 떼기
        std::uniform_real_distribution<float> randomDisplacement(-initialDisplacement, +initialDisplacement);
        setHeight(glm::ivec2(0,0), randomDisplacement(randomGen));
        setHeight(glm::ivec2(m_size - 1,0), randomDisplacement(randomGen));
        setHeight(glm::ivec2(m_size - 1,m_size - 1), randomDisplacement(randomGen));
        setHeight(glm::ivec2(0, m_size - 1), randomDisplacement(randomGen));

        // std::cout << getHeight(glm::ivec2(0,0)) << getHeight(glm::ivec2(m_size - 1,0)) << getHeight(glm::ivec2(m_size - 1,m_size - 1)) << getHeight(glm::ivec2(0,m_size - 1)) <<std::endl;
        int squareSize = m_sizeX - 1;
        float randomDisplacementInterval = initialDisplacement;
        float displacementReduceRate = 1 - pow(2.0f, -roughness);

        while(squareSize > 1){
            diamondStep(squareSize, randomDisplacementInterval);
            squareStep(squareSize, randomDisplacementInterval);

            randomDisplacementInterval *= displacementReduceRate;
            // std::cout << randomDisplacementInterval << std::endl;
            squareSize /= 2;
        }
    }
    updateVertices();
}

// step per one coord
/**
 * <-------> : length == squareSize
 * X . . . X
 * . . . . .
 * . . O . . <-- centerPos
 * . . . . .
 * X . . . X
 * 
 * X = sampling point 
 * O = average of 4 sampling points + random value
*/
void TerrainDiaSq::diamondStep(unsigned int squareSize, float randomDisplacementInterval){
    // std::cout << "diamondStep(" << squareSize << ", " << randomDisplacementInterval << ")" << std::endl;
    unsigned int halfSquareSize = squareSize / 2;
    std::uniform_real_distribution<float> randomDisplacement(-randomDisplacementInterval, +randomDisplacementInterval);
    for(unsigned int centerPosZ = halfSquareSize; centerPosZ < m_sizeZ; centerPosZ += squareSize){
        for(unsigned int centerPosX = halfSquareSize; centerPosX < m_sizeX; centerPosX += squareSize){
            diamondStepSampling(squareSize, centerPosX, centerPosZ, randomDisplacementInterval);
            // float upperLeftSampleHeight = getHeight(centerPosX - halfSquareSize, centerPosZ - halfSquareSize);
            // float upperRightSampleHeight = getHeight(centerPosX + halfSquareSize, centerPosZ - halfSquareSize);
            // float bottomLeftSampleHeight = getHeight(centerPosX - halfSquareSize, centerPosZ + halfSquareSize);
            // float bottomRightSampleHeight = getHeight(centerPosX + halfSquareSize, centerPosZ + halfSquareSize);

            // float randomVal = randomDisplacement(randomGen);
            // float average = (upperLeftSampleHeight + upperRightSampleHeight + bottomLeftSampleHeight + bottomRightSampleHeight) / 4.0f;

            // setHeight(centerPosX, centerPosZ, average + randomVal);
            // std::cout << centerPosX << ", " << centerPosZ << " : " << average + randomVal << std::endl;
        }
    }
}

void TerrainDiaSq::diamondStepSampling(unsigned int squareSize, int x, int z, float randomDisplacementInterval){
    std::uniform_real_distribution<float> randomDisplacement(-randomDisplacementInterval, +randomDisplacementInterval);
    int halfSquareSize = squareSize / 2;
    float topLeft       = getHeight(x - halfSquareSize, z - halfSquareSize);
    float topRight      = getHeight(x + halfSquareSize, z - halfSquareSize);
    float bottomLeft    = getHeight(x - halfSquareSize, z + halfSquareSize);
    float bottomRight   = getHeight(x + halfSquareSize, z + halfSquareSize);

    float randomVal = randomDisplacement(randomGen);
    float average = (topLeft + topRight + bottomLeft + bottomRight) / 4.f;

    setHeight(x,z, average + randomVal);
}

/**
 * <-------> : length == squareSize
 * X . O . X <- first centerPosX loop  <= centerPosZ = 0
 * . . . . .
 * O . X . O <- second centerPosX loop
 * . . . . .
 * X . O . X                           <= centerPosZ = squareSize
 * 
 * 
*/
void TerrainDiaSq::squareStep(unsigned int squareSize, float randomDisplacementInterval){
    unsigned int halfSquareSize = squareSize / 2;
    // odd row
    for(int centerPosZ = 0; centerPosZ < m_sizeZ; centerPosZ += squareSize){
        for(int centerPosX = halfSquareSize; centerPosX < m_sizeX; centerPosX += squareSize){
            squareStepSampling(squareSize, centerPosX, centerPosZ, randomDisplacementInterval);
        }
    }
    // even row
    for(int centerPosZ = halfSquareSize; centerPosZ < m_sizeZ; centerPosZ += squareSize){
        for(int centerPosX = 0; centerPosX < m_sizeX; centerPosX += squareSize){
            squareStepSampling(squareSize, centerPosX, centerPosZ, randomDisplacementInterval);
        }
    }
}

void TerrainDiaSq::squareStepSampling(unsigned int squareSize, int x, int z, float randomDisplacementInterval){    
    std::uniform_real_distribution<float> randomDisplacement(-randomDisplacementInterval, +randomDisplacementInterval);
    int halfSquareSize = squareSize / 2;
    // if sampling point is outside of the map, do not include in the average
    float top = 0.f, left = 0.f, right = 0.f, bottom = 0.f;
    int T = 0, L = 0, R = 0, B = 0; // flag whether sampled
    // top
    if(z - halfSquareSize >= 0){
        top = getHeight(x, z - halfSquareSize);
        T++;
    }
    // left
    if(x - halfSquareSize >= 0){
        left = getHeight(x - halfSquareSize, z);
        L++;
    }
    // right
    if(x + halfSquareSize < m_sizeX){
        right = getHeight(x + halfSquareSize, z);
        R++;
    }
    // bottom
    if(z + halfSquareSize < m_sizeZ){
        bottom = getHeight(x, z + halfSquareSize);
        B++;
    }

    float randomVal = randomDisplacement(randomGen);
    float average = (top + right + left + bottom) / (float)(T+R+L+B);

    // std::cout << "Sampling " << x << " " << z << " | " << top << " " << right << " " << left << " " << bottom << std::endl;
    setHeight(x,z, average + randomVal);
}

// wrapping is not used
// void TerrainDiaSq::squareStep(unsigned int squareSize, float randomDisplacementInterval){
//     // std::cout << "squareStep(" << squareSize << ", " << randomDisplacementInterval << ")" << std::endl;
//     int halfSquareSize = squareSize / 2;
//     std::uniform_real_distribution<float> randomDisplacement(-randomDisplacementInterval, +randomDisplacementInterval);
//     for(unsigned int centerPosZ = 0; centerPosZ < m_size - 1; centerPosZ += squareSize){ // except last row
//         unsigned int centerPosZOddRow = centerPosZ;
//         for(unsigned int centerPosX = halfSquareSize; centerPosX < m_size; centerPosX += squareSize){ // odd row
//             float upperSampleHeight = getHeight(wrapCoord(centerPosX, m_size), wrapCoord(centerPosZOddRow - halfSquareSize, m_size));
//             float downSampleHeight = getHeight(wrapCoord(centerPosX, m_size), wrapCoord(centerPosZOddRow + halfSquareSize, m_size));
//             float leftSampleHeight = getHeight(wrapCoord(centerPosX - halfSquareSize, m_size), wrapCoord(centerPosZOddRow, m_size));
//             float rightSampleHeight = getHeight(wrapCoord(centerPosX + halfSquareSize, m_size), wrapCoord(centerPosZOddRow, m_size));

//             float randomVal = randomDisplacement(randomGen);
//             float average = (upperSampleHeight + downSampleHeight + leftSampleHeight + rightSampleHeight) / 4.0f;
            
//             setHeight(centerPosX, centerPosZOddRow, average + randomVal);
//             // std::cout << centerPosX << ", " << centerPosZOddRow << " : " << average + randomVal << std::endl;
//         }
//         unsigned int centerPosZEvenRow = centerPosZ + halfSquareSize;
//         for(unsigned int centerPosX = 0; centerPosX < m_size; centerPosX += squareSize){ // even row
//             float upperSampleHeight = getHeight(wrapCoord(centerPosX, m_size), wrapCoord(centerPosZEvenRow - halfSquareSize, m_size));
//             float downSampleHeight = getHeight(wrapCoord(centerPosX, m_size), wrapCoord(centerPosZEvenRow + halfSquareSize, m_size));
//             float leftSampleHeight = getHeight(wrapCoord(centerPosX - halfSquareSize, m_size), wrapCoord(centerPosZEvenRow, m_size));
//             float rightSampleHeight = getHeight(wrapCoord(centerPosX + halfSquareSize, m_size), wrapCoord(centerPosZEvenRow, m_size));

//             float randomVal = randomDisplacement(randomGen);
//             float average = (upperSampleHeight + downSampleHeight + leftSampleHeight + rightSampleHeight) / 4.0f;
            
//             setHeight(centerPosX, centerPosZEvenRow, average + randomVal);
//             // std::cout << centerPosX << ", " << centerPosZEvenRow << " : " << average + randomVal << std::endl;
//         }
//     }
//     unsigned int centerPosZ = m_size - 1;
//     for(unsigned int centerPosX = halfSquareSize; centerPosX < m_size; centerPosX += squareSize){ // last row
//         float upperSampleHeight = getHeight(wrapCoord(centerPosX, m_size), wrapCoord(centerPosZ - halfSquareSize, m_size));
//         float downSampleHeight = getHeight(wrapCoord(centerPosX, m_size), wrapCoord(centerPosZ + halfSquareSize, m_size));
//         float leftSampleHeight = getHeight(wrapCoord(centerPosX - halfSquareSize, m_size), wrapCoord(centerPosZ, m_size));
//         float rightSampleHeight = getHeight(wrapCoord(centerPosX + halfSquareSize, m_size), wrapCoord(centerPosZ, m_size));

//         float randomVal = randomDisplacement(randomGen);
//         float average = (upperSampleHeight + downSampleHeight + leftSampleHeight + rightSampleHeight) / 4.0f;
        
//         setHeight(centerPosX, centerPosZ, average + randomVal);
//         // std::cout << centerPosX << ", " << centerPosZ << " : " << average + randomVal << std::endl;
//     }
// }

bool is_power_of_two(int n){
    if(n <= 0)
        return false;
    else
        return (n & (n-1)) == 0;
}

inline unsigned int wrapCoord(int coord, unsigned int size){
    /**
     *  . . . . . . . . . 
     *  . . . . . . . . . 
     *  . . . . . . . . X
     *  . . . . . . . . . 
     *  . . X . . . X . O (.) (X) 
     *  . . . . . . . . . 
     *  . . . . . . . . X 
     *  . . . . . . . . . 
     *  . . . . . . . . . 
    */
    // size - 1 : length of the map
    if(coord < 0)
        return coord + size - 1;
    else if(coord > size)
        return coord - size + 1;
    else
        return coord;
}

#ifndef __GROUND_H__
#define __GROUND_H__

#include <random>
#include <iostream>
#include <string>

#include <math.h>

#include <utils/glPrerequisites.h>
#include <utils/learnopenglPrerequisites.h>


// const int MAP_SIZE_X = 20;
// const int MAP_SIZE_Y = 20;
// const int MAP_RADIUS = 20;
// const int MAP_LENGTH = 50;


struct GroundVertex{
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 tex;
};

/**
 * map에서 x방향 : 카메라 기준 좌우, y방향 : 카메라 기준 앞뒤
 * xxxGL() 함수는 GL 관련 데이터가 변경되었을 때만 실행되야 함
 * */ 

class Ground{
private:
    // map
    void randomize(float scale, int seed);
    void deleteMap();
    const int mapSizeX, mapSizeY;
    const float mapRadius, mapLength;
    float** map;
    std::minstd_rand0 randomGen;

    // OpenGL
    void initGL(); // constructor에서만 사용
    void getTexture(std::string path); // constructor에서만 사용
    void updateGL(); // Map 업데이트 되었을 때 사용
    GroundVertex *vertices;
    int numVertices;
    unsigned int *indices;
    GLuint VAO, VBO, texture;

    // utils
    void mapToVertices();
public:
    Ground(int x, int y, int r, int l, std::string texturePath);
    // ~Ground();
    void clearMap();
    void draw(Shader* shader);
    void printMap();

    // void resize(int newX, int newY);
    void smooth();
    // void standardize();
};

// Ground::Ground() : mapSizeX(MAP_SIZE_X), mapSizeY(MAP_SIZE_Y), mapRadius(MAP_RADIUS), mapLength(MAP_LENGTH){
//     numVertices = mapSizeX*mapSizeY*2*3;
//     clearMap();
//     // randomize(100.0,1);
//     initGL();
// }

// void Ground::clearMap(){
//     map = new float*[mapSizeX];
//     for(int x=0; x < mapSizeX; x++){
//         map[x] = new float[mapSizeY];
//         for(int y=0;y<mapSizeY;y++){
//             map[x][y] = 0.f;
//         }
//     };
// }

// void Ground::Draw(Shader* shader){
//     shader->use();
//     glBindVertexArray(VAO);
//     glDrawArrays(GL_TRIANGLES, 0, numVertices);
// }

// /**
//  * vertices 초기화
//  * map[x][y] -> 삼각형 x*y*2개, vertex x*y*2*3개
//  * 한 vertex당 들어가는 float 개수 : 3(position) + 3(normal) + 2(texcoord) 
// */
// void Ground::initGL(){
//     vertices = new GroundVertex[numVertices];
//     for(int i=0;i<numVertices;i++){
//         vertices[i] = GroundVertex();
//     }

//     // (x,y), (x+1,y), (x,y+1), (x,y)에 해당하는 삼각형과 vertex 생성
//     // @todo: 현재 position만 설정됨. normal과 tex추가 필요
//     GroundVertex v;
//     float unitDegree = 1/(float)(mapSizeX) * 2 * M_PI;
//     for(int x=0; x < mapSizeX; x++){
//         float degree = x * unitDegree;
//         for(int y=0; y<(mapSizeY-1); y++){
//             float height = map[x][y];
//             // (x,y) (x+1, y), (x+1,y+1) 삼각형 : vertex(offset) + 0, 1, 2
//             // (x,y) (x, y+1), (x+1,y+1) 삼각형 : vertex(offset) + 3, 4, 5
//             // (x,y)
//             v.pos.x = (mapRadius+height) * cos(degree);
//             v.pos.y = mapLength * y/(float)mapSizeY;
//             v.pos.z = (mapRadius+height) * sin(degree);
//             vertices[(x*(mapSizeY-1) + y)*2*3 + 0] = v;
//             vertices[(x*(mapSizeY-1) + y)*2*3 + 3] = v;
            
//             // (x+1, y)
//             height = map[(x+1)%mapSizeX][y];
//             v.pos.x = (mapRadius+height) * cos(degree + unitDegree);
//             v.pos.y = mapLength * y/(float)mapSizeY;
//             v.pos.z = (mapRadius+height) * sin(degree + unitDegree);
//             vertices[(x*(mapSizeY-1) + y)*2*3 + 1] = v;

//             // (x, y+1)
//             height = map[x][y+1];
//             v.pos.x = (mapRadius+height) * cos(degree);
//             v.pos.y = mapLength * (y+1)/(float)mapSizeY;
//             v.pos.z = (mapRadius+height) * sin(degree);
//             vertices[(x*(mapSizeY-1) + y)*2*3 + 4] = v;

//             // (x+1, y+1)
//             height = map[(x+1)%mapSizeX][y+1];
//             v.pos.x = (mapRadius+height) * cos(degree + unitDegree);
//             v.pos.y = mapLength * (y+1)/(float)mapSizeY;
//             v.pos.z = (mapRadius+height) * sin(degree + unitDegree);
//             vertices[(x*(mapSizeY-1) + y)*2*3 + 2] = v;
//             vertices[(x*(mapSizeY-1) + y)*2*3 + 5] = v;
//         }
//     }

//     // opengl buffer 설정
//     int sizeOfVertices = numVertices * sizeof(GroundVertex);
//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);

//     glBindVertexArray(VAO);

//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeOfVertices, vertices, GL_DYNAMIC_DRAW);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float)*0));
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float)*3));
//     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float)*6));
//     glEnableVertexAttribArray(0);
//     glEnableVertexAttribArray(1);
//     glEnableVertexAttribArray(2);

//     glBindVertexArray(0);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);

// }

// void Ground::updateGL()
// {
//     //glbuffersubdata
// }

// void Ground::randomize(float scale, int seed){
//     randomGen = std::minstd_rand0(seed);
//     static std::uniform_real_distribution<float> randomFloat(-1, 1);
//     // float sizeReduceTimes = (float)(m_sizeX > m_sizeZ ? m_sizeX : m_sizeZ) / blockSize;
//     for(int x=0; x<mapSizeX; x++){
//         for(int y=0; y<mapSizeY; y++){
//             map[x][y] = scale*randomFloat(randomGen);
//         }
//     }
// }

// void Ground::deleteMap()
// {
//     for(int x=0; x < mapSizeX; x++){
//         delete map[x];
//     };
//     delete map;
// }

// void Ground::printMap(){
//     for(int x=0;x<mapSizeX;x++){
//         for(int y=0;y<mapSizeY;y++){
//             std::cout << "***" << x << " , " << y << "***" << std::endl;
//             std::cout << vertices[x*mapSizeY + y + 0].pos.x << "\t"<< vertices[x*mapSizeY + y + 0].pos.y << "\t"<< vertices[x*mapSizeY + y + 0].pos.z << std::endl;
//             std::cout << vertices[x*mapSizeY + y + 1].pos.x << "\t"<< vertices[x*mapSizeY + y + 1].pos.y << "\t"<< vertices[x*mapSizeY + y + 1].pos.z << std::endl;
//             std::cout << vertices[x*mapSizeY + y + 2].pos.x << "\t"<< vertices[x*mapSizeY + y + 2].pos.y << "\t"<< vertices[x*mapSizeY + y + 2].pos.z << std::endl;
//             std::cout << vertices[x*mapSizeY + y + 3].pos.x << "\t"<< vertices[x*mapSizeY + y + 3].pos.y << "\t"<< vertices[x*mapSizeY + y + 3].pos.z << std::endl;
//             std::cout << vertices[x*mapSizeY + y + 4].pos.x << "\t"<< vertices[x*mapSizeY + y + 4].pos.y << "\t"<< vertices[x*mapSizeY + y + 4].pos.z << std::endl;
//             std::cout << vertices[x*mapSizeY + y + 5].pos.x << "\t"<< vertices[x*mapSizeY + y + 5].pos.y << "\t"<< vertices[x*mapSizeY + y + 5].pos.z << std::endl;
//         }
//         std::cout << std::endl;
//     }
//     std::cout << std::endl;
// }

#endif
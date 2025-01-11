#include "ground.h"

#include <stb_image.h>
#include "terrain_formation.h"

const float TEXTURE_STRIDE = 2.f;

Ground::Ground(int x, int y, int r, int l, std::string texturePath) : mapSizeX(x), mapSizeY(y), mapRadius(r), mapLength(l){
    numVertices = mapSizeX*mapSizeY*2*3;
    clearMap();

    TerrainFaultFormation faultForm(mapSizeX, mapSizeY);
    faultForm.map = map;
    faultForm.generate(100, 0.2);

    // randomize(1,1);
    // smooth();

    // randomize(100.0,1);
    initGL();
    getTexture(texturePath);
}

void Ground::clearMap(){
    map = new float*[mapSizeX]; 
    for(int x=0; x < mapSizeX; x++){
        map[x] = new float[mapSizeY];
        for(int y=0;y<mapSizeY;y++){
            map[x][y] = 0.f;
        }
    };
}

void Ground::draw(Shader* shader){
    shader->use();
    shader->setInt("texture_diffuse1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

/**
 * vertices 초기화
 * map[x][y] -> 삼각형 x*y*2개, vertex x*y*2*3개
 * 한 vertex당 들어가는 float 개수 : 3(position) + 3(normal) + 2(texcoord) 
*/
void Ground::initGL(){
    vertices = new GroundVertex[numVertices];
    this->mapToVertices();

    // texture and normals

    // opengl buffer 설정
    int sizeOfVertices = numVertices * sizeof(GroundVertex);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeOfVertices, vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float)*0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float)*3));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float)*6));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Ground::getTexture(std::string path){
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
    unsigned char *image = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!image) {
        printf("texture %s loading error ... \n", path.c_str());
    }
    else printf("texture %s loaded\n", path.c_str());
    
    GLenum format;
    if (nrChannels == 1) format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;
    
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Ground::updateGL()
{
    //glbuffersubdata
}

void Ground::randomize(float scale, int seed){
    randomGen = std::minstd_rand0(seed);
    static std::uniform_real_distribution<float> randomFloat(-1, 1);
    // float sizeReduceTimes = (float)(m_sizeX > m_sizeZ ? m_sizeX : m_sizeZ) / blockSize;
    for(int x=0; x<mapSizeX; x++){
        for(int y=0; y<mapSizeY; y++){
            map[x][y] = scale*randomFloat(randomGen);
        }
    }
}

void Ground::deleteMap()
{
    for(int x=0; x < mapSizeX; x++){
        delete map[x];
    };
    delete map;
}

void Ground::printMap(){
    for(int x=0;x<mapSizeX;x++){
        for(int y=0;y<mapSizeY;y++){
            std::cout << "***" << x << " , " << y << "***" << std::endl;
            std::cout << vertices[x*mapSizeY + y + 0].pos.x << "\t"<< vertices[x*mapSizeY + y + 0].pos.y << "\t"<< vertices[x*mapSizeY + y + 0].pos.z << std::endl;
            std::cout << vertices[x*mapSizeY + y + 1].pos.x << "\t"<< vertices[x*mapSizeY + y + 1].pos.y << "\t"<< vertices[x*mapSizeY + y + 1].pos.z << std::endl;
            std::cout << vertices[x*mapSizeY + y + 2].pos.x << "\t"<< vertices[x*mapSizeY + y + 2].pos.y << "\t"<< vertices[x*mapSizeY + y + 2].pos.z << std::endl;
            std::cout << vertices[x*mapSizeY + y + 3].pos.x << "\t"<< vertices[x*mapSizeY + y + 3].pos.y << "\t"<< vertices[x*mapSizeY + y + 3].pos.z << std::endl;
            std::cout << vertices[x*mapSizeY + y + 4].pos.x << "\t"<< vertices[x*mapSizeY + y + 4].pos.y << "\t"<< vertices[x*mapSizeY + y + 4].pos.z << std::endl;
            std::cout << vertices[x*mapSizeY + y + 5].pos.x << "\t"<< vertices[x*mapSizeY + y + 5].pos.y << "\t"<< vertices[x*mapSizeY + y + 5].pos.z << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Ground::smooth(){
    float** newMap = new float*[mapSizeX];
    for(int x=0; x < mapSizeX; x++){
        newMap[x] = new float[mapSizeY];
    };


    for(int x = 0; x < mapSizeX; x++){
        for(int y = 0; y < mapSizeY; y++){
            newMap[x][y] = (
                map[(mapSizeX + x-1)%mapSizeX][(mapSizeY + y-1)%mapSizeY] +
                map[(mapSizeX + x-1)%mapSizeX][(mapSizeY + y)%mapSizeY] +
                map[(mapSizeX + x-1)%mapSizeX][(mapSizeY + y+1)%mapSizeY] +
                map[(mapSizeX + x)%mapSizeX][(mapSizeY + y-1)%mapSizeY] +
                map[(mapSizeX + x)%mapSizeX][(mapSizeY + y)%mapSizeY] +
                map[(mapSizeX + x)%mapSizeX][(mapSizeY + y+1)%mapSizeY] +
                map[(mapSizeX + x+1)%mapSizeX][(mapSizeY + y-1)%mapSizeY] +
                map[(mapSizeX + x+1)%mapSizeX][(mapSizeY + y)%mapSizeY] +
                map[(mapSizeX + x+1)%mapSizeX][(mapSizeY + y+1)%mapSizeY]
            ) / 9.f;
        }
    }

    for(int x=0; x < mapSizeX; x++){
        delete map[x];
        map[x] = newMap[x];
    };
    delete map;
    map = newMap;
}

void Ground::mapToVertices(){
    GroundVertex v;
    float unitDegree = 1/(float)(mapSizeX) * 2 * M_PI;
    for(int x=0; x < mapSizeX; x++){
        float degree = x * unitDegree;
        for(int y=0; y<(mapSizeY-1); y++){
            float height = map[x][y];
            // (x,y) (x+1, y), (x+1,y+1) 삼각형 : vertex(offset) + 0, 1, 2
            // (x,y) (x, y+1), (x+1,y+1) 삼각형 : vertex(offset) + 3, 4, 5
            // (x,y)
            v.pos.x = (mapRadius+height) * cos(degree);
            v.pos.y = mapLength * y/(float)mapSizeY;
            v.pos.z = (mapRadius+height) * sin(degree);
            vertices[(x*(mapSizeY-1) + y)*2*3 + 0] = v;
            vertices[(x*(mapSizeY-1) + y)*2*3 + 3] = v;
            
            // (x+1, y)
            height = map[(x+1)%mapSizeX][y];
            v.pos.x = (mapRadius+height) * cos(degree + unitDegree);
            v.pos.y = mapLength * y/(float)mapSizeY;
            v.pos.z = (mapRadius+height) * sin(degree + unitDegree);
            vertices[(x*(mapSizeY-1) + y)*2*3 + 1] = v;

            // (x, y+1)
            height = map[x][y+1];
            v.pos.x = (mapRadius+height) * cos(degree);
            v.pos.y = mapLength * (y+1)/(float)mapSizeY;
            v.pos.z = (mapRadius+height) * sin(degree);
            vertices[(x*(mapSizeY-1) + y)*2*3 + 4] = v;

            // (x+1, y+1)
            height = map[(x+1)%mapSizeX][y+1];
            v.pos.x = (mapRadius+height) * cos(degree + unitDegree);
            v.pos.y = mapLength * (y+1)/(float)mapSizeY;
            v.pos.z = (mapRadius+height) * sin(degree + unitDegree);
            vertices[(x*(mapSizeY-1) + y)*2*3 + 2] = v;
            vertices[(x*(mapSizeY-1) + y)*2*3 + 5] = v;
        }
    }

    glm::vec3 normal;
    glm::vec2 texCoord;
    for(int x=0; x < mapSizeX; x++){
        float degree = x * unitDegree;
        for(int y=0; y<(mapSizeY-1); y++){
            // (x,y) (x+1, y), (x+1,y+1) 삼각형 : vertex(offset) + 0, 1, 2
            // (x,y) (x, y+1), (x+1,y+1) 삼각형 : vertex(offset) + 3, 4, 5
            glm::vec3 v0, v1, v2, v3, v4, v5;
            v0 = v3 = vertices[(x*(mapSizeY-1) + y)*2*3 + 0].pos;
            v1 = vertices[(x*(mapSizeY-1) + y)*2*3 + 1].pos;
            v4 = vertices[(x*(mapSizeY-1) + y)*2*3 + 1].pos;
            v2 = v5 = vertices[(x*(mapSizeY-1) + y)*2*3 + 2].pos;

            normal = glm::normalize(glm::cross(v1-v0, v2-v0));
            vertices[(x*(mapSizeY-1) + y)*2*3 + 0].norm = normal;
            vertices[(x*(mapSizeY-1) + y)*2*3 + 1].norm = normal;
            vertices[(x*(mapSizeY-1) + y)*2*3 + 2].norm = normal;

            normal = glm::normalize(glm::cross(v4-v3, v5-v3));
            vertices[(x*(mapSizeY-1) + y)*2*3 + 3].norm = normal;
            vertices[(x*(mapSizeY-1) + y)*2*3 + 4].norm = normal;
            vertices[(x*(mapSizeY-1) + y)*2*3 + 5].norm = normal;

            // texcoord
            // (x,y)
            vertices[(x*(mapSizeY-1) + y)*2*3 + 0].tex = glm::vec2(x / TEXTURE_STRIDE, y / TEXTURE_STRIDE);
            vertices[(x*(mapSizeY-1) + y)*2*3 + 3].tex = glm::vec2(x / TEXTURE_STRIDE, y / TEXTURE_STRIDE);
            // (x+1,y)
            vertices[(x*(mapSizeY-1) + y)*2*3 + 1].tex = glm::vec2((x+1) / TEXTURE_STRIDE, y / TEXTURE_STRIDE);
            // (x,y+1)
            vertices[(x*(mapSizeY-1) + y)*2*3 + 4].tex = glm::vec2(x / TEXTURE_STRIDE, (y+1) / TEXTURE_STRIDE);
            // (x+1,y+1)
            vertices[(x*(mapSizeY-1) + y)*2*3 + 2].tex = glm::vec2((x+1) / TEXTURE_STRIDE, (y+1) / TEXTURE_STRIDE);
            vertices[(x*(mapSizeY-1) + y)*2*3 + 5].tex = glm::vec2((x+1) / TEXTURE_STRIDE, (y+1) / TEXTURE_STRIDE);
        }
    }

}
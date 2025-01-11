#include "game.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <src/lighting/lighting.h>
#include <src/entities/entity.h>
// #include <src/commons/common_entities.h>

#include <random>
#include "collision.h"

#define NUM_INSTANCE 200

extern Camera* camera_for_callback;

float deltaTime;
float lastFrame;

glm::vec3 sunlight_color(0.9922,0.9843,0.8275);
glm::vec3 moonlight_color(0.3125,0.40625,1.2);

Game::Game()
{
}

Game::~Game(){
}

void Game::init(string settingPath){
	displayManager.init(settingPath);

    renderer.init(string(settingPath));
	std::cerr << "Initializing Game" << glfwGetTime() << std::endl;

	gameIO = GameIO(displayManager.window, renderer.getCamera());
	gameIO.init();

    INI::File ft;
	if (!ft.Load(settingPath))
	{
		// Loading from stream
		std::ifstream op("opts.ini",std::ios::in);
		if (op.is_open())
			op >> ft;
	}

    // entity init
    string modelDir = ft.GetValue("MODEL_DIR", "./data").AsString();
    string modelFileBird = ft.GetValue("MODEL_BIRD").AsString();
    bird = new EntityDynamic(modelDir + modelFileBird);
    // bird = new EntityStatic(modelDir + modelFileBird);
    bird->setOffset(glm::vec3(0,0,1), glm::vec3(-0.1,-0.2,0), glm::vec3(0.2f));
	bird->setAnimationSpeed(5.f);
	gameIO.bindEntity(bird);
	bird->setAcceleration(glm::vec3(0,-7.f, 0));
	bird->jumpVel = 5.f;
	bird->collisionX = bird->collisionY = bird->collisionZ = 0.3f;

	// LIGHTS
	string modelFile = ft.GetValue("MODEL_SUN").AsString();
	glm::vec3 sunlight_direction(-1,-1,-1);
	DirLight* sunlight = new DirLight(sunlight_color, 0.2, 0.3, sunlight_direction);
	lightContainer.add(sunlight);
	EntityStatic *sun = new EntityStatic(modelDir + modelFile);
	sun->setOffset(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(3.f));
	sun->dirLight = (sunlight);
	lightSources.push_back(sun);

	modelFile = ft.GetValue("MODEL_MOON").AsString();
	glm::vec3 moonlight_direction(-1,-1,-1);
	DirLight* moonlight = new DirLight(moonlight_color, 0.2, 0.3, moonlight_direction);
	lightContainer.add(moonlight);

	EntityStatic *moon = new EntityStatic(modelDir + modelFile);
	moon->setOffset(glm::vec3(1,0,0), glm::vec3(0,0,0), glm::vec3(50.f));
	moon->dirLight = (moonlight);
	lightSources.push_back(moon);

	glm::vec3 ambient_direction(0,-1,0);
	DirLight* ambientLight = new DirLight(glm::vec3(0.2, 0.2, 0.2), 1, 1, ambient_direction);
	lightContainer.add(ambientLight);

	// TERRAIN
	std::string textureFileTerrain = ft.GetValue("TEXTURE_GROUND").AsString();
	this->terrain = new TerrainDiaSq(128+1, textureFileTerrain);
	terrain->clearHeightMap();
	terrain->generate(1,40);
	terrain->smooth(2);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// TREES
	modelFile = ft.GetValue("MODEL_TREE").AsString();
	std::minstd_rand0 randomGen;
	randomGen.seed(glfwGetTime() * 1000);
	EntityStaticInstanced *tree = new EntityStaticInstanced(modelDir + modelFile);
	
	float sc = ft.GetValue("MODEL_TREE_SCALE").AsDouble();
	tree->setOffset(glm::vec3(1,0,0), glm::vec3(0,0,0), glm::vec3(sc));

	std::uniform_real_distribution<float> rand(2, this->terrain->getSizeX()-2);
	for(int i=0;i<NUM_INSTANCE;i++){
		float x = rand(randomGen);
		float z = rand(randomGen);

		glm::vec3 pos = glm::vec3(x, terrain->getHeight(x, z),z);
		// pos = glm::vec3(i);
		glm::mat4 modelInst = glm::translate(glm::mat4(1.f), pos);
		tree->modelMatInstanced.push_back(modelInst);
	}
	tree->setPhysics(glm::vec3(0), glm::vec3(0), glm::vec3(0));
	tree->collisionX = tree->collisionZ = 1.3;
	tree->collisionY = 6.f;

	trees.push_back(tree);

	// COINS
	modelFile = ft.GetValue("MODEL_COIN").AsString();
	EntityStaticInstanced *coin = new EntityStaticInstanced(modelDir + modelFile);
	
	sc = ft.GetValue("MODEL_COIN_SCALE").AsDouble();
	coin->setOffset(glm::vec3(1,0,0), glm::vec3(0,-0.3,0), glm::vec3(sc));

	std::uniform_real_distribution<float> randY(3, 10);
	for(int i=0;i<NUM_INSTANCE;i++){
		float x = rand(randomGen);
		float dy = randY(randomGen);
		float z = rand(randomGen);

		glm::vec3 pos = glm::vec3(x, terrain->getHeight(x, z) + dy,z);
		// pos = glm::vec3(i);
		glm::mat4 modelInst = glm::translate(glm::mat4(1.f), pos);
		coin->modelMatInstanced.push_back(modelInst);
	}
	coin->setPhysics(glm::vec3(0), glm::vec3(0), glm::vec3(0));
	coins.push_back(coin);

	std::cerr << "Initialized Game" << glfwGetTime() << std::endl;

	float halfCoord = terrain->getSizeX() / 2.f;
	bird->setPosition(glm::vec3(halfCoord, terrain->getHeight(halfCoord, halfCoord) + 10 ,halfCoord));
	// // frames
	// deltaTime = 0.f;
	// lastFrame = glfwGetTime();
}

void Game::start(){
	std::cerr << "Starting Game" << glfwGetTime() << std::endl;
	if(bird != NULL) { bird->startAnimation(); }
	// for(auto l: lightSources){ l->startAnimation(); }
	// for(auto t : trees){ t->startAnimation(); }
    // for(auto i : items){ i->startAnimation(); }
    // if(ground != NULL) { ground->startAnimation(); }
	
	// IMGUI
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui_ImplGlfw_InitForOpenGL(displayManager.window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

	// frames
	deltaTime = 0.f;
	lastFrame = glfwGetTime();

	// render loop
	while (!glfwWindowShouldClose(displayManager.window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// std::cerr << "DeltaTime : " << deltaTime << " CureentFrame: " << currentFrame << "\n";

		// input
		gameIO.poll(deltaTime);

		// update positions
		coins[0]->setOrientation(glm::vec3(glm::sin(currentFrame), 0, glm::cos(currentFrame)), 0);

		// update animation
		bird->updateAnimation(deltaTime);

		// sun
		glm::vec3 curcampos = camera_for_callback->Position;
		float sinTime = glm::sin(currentFrame / 10.f);
		float cosTime = glm::cos(currentFrame / 10.f);
		lightSources[0]->setPosition(300.f * glm::vec3(sinTime, cosTime, 0) + curcampos);
		lightSources[1]->setPosition(-300.f * glm::vec3(sinTime, cosTime, 0) + curcampos);

		lightSources[0]->dirLight->direction = -glm::vec3(sinTime, cosTime, 0);
		lightSources[1]->dirLight->direction = +glm::vec3(sinTime, cosTime, 0);

		bird->updatePhysics(deltaTime);

		// collsion checking
		if( checkCollisionTerrain(bird, terrain)){
			bird->materialColor = glm::vec3(1,1,1);
			glfwSetWindowShouldClose(displayManager.window, GL_TRUE);
		}
		else if(checkCollisionEntityAABBInstanced(bird, trees[0])){
			bird->materialColor = glm::vec3(1,0,0);
			glfwSetWindowShouldClose(displayManager.window, GL_TRUE);
		}
		else if(checkCollsionEntityRadiusInstanced(bird, coins[0])){
			bird->collisionRadius = 0.5;
			coins[0]->collisionRadius = 0.5;
			// bird->materialColor = glm::vec3(0,1,0);
		}
		else{
			bird->materialColor = glm::vec3(0,0,0);
		}

		// events (disappearing at collision, score update, ...)

		// draw : all modifications have to be done before
		lightSources[0]->dirLight->color = sunlight_color * -glm::min<float>(lightSources[0]->dirLight->direction.y, 0);
		lightSources[1]->dirLight->color = moonlight_color * -glm::min<float>(lightSources[1]->dirLight->direction.y, 0);
		renderer.draw(lightContainer, bird, coins, trees, lightSources, terrain);

		// IMGUI : debugging, after the rendering
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.
            ImGui::NewLine();

			ImGui::SliderFloat("birdX", &(bird->collisionX), 0, 3);
			ImGui::SliderFloat("birdY", &(bird->collisionY), 0, 3);
			ImGui::SliderFloat("birdZ", &(bird->collisionZ), 0, 3);

			ImGui::SliderFloat("treeColX", &(trees[0]->collisionX), 0, 3);
			ImGui::SliderFloat("treeColY", &(trees[0]->collisionY), 0, 10);
			ImGui::SliderFloat("treeColZ", &(trees[0]->collisionZ), 0, 3);

			// ImGui::SliderFloat("sunColX", &(lightSources[0]->collisionX), 0, 3);
			// ImGui::SliderFloat("sunColY", &(lightSources[0]->collisionY), 0, 3);
			// ImGui::SliderFloat("sunColZ", &(lightSources[0]->collisionZ), 0, 3);
			
			// ImGui::SliderFloat3("materialColor", (float*)(&bird->materialColor), 0, 1);
			// ImGui::SliderFloat("materialshininess", &(bird->materialShininess), 0, 100);
			static bool gravity_on = true;
			ImGui::Checkbox("gravity", &gravity_on);
			if(bird->getVelocity().y < 0){
				if(gravity_on){
				}
				else{
					glm::vec3 _vel = bird->getVelocity();
					_vel.y = 0.f;
					bird->setVelocity(_vel);
				}
			}
			// if(gravity_on) {
				// bird->setAcceleration(glm::vec3(0.f,-7.f, 0.f));
			// }
			// else {
			// 	bird->setAcceleration(glm::vec3(0.f,0.f, 0.f)); 
			// 	if(bird->getVelocity().y < 0){
			// 		bird->setVelocity(glm::vec3(0,0,0)); 
			// 	}
			// }

			static float r = 1;
			static float d = 1;
			ImGui::NewLine();
			ImGui::SliderFloat("roughness", &(r), 0, 10);
			ImGui::SliderFloat("displacement", &(d), 0, 50);
            if(ImGui::Button("gen")){
                terrain->generate(r,d);
				std::minstd_rand0 randomGen;
				std::uniform_real_distribution<float> rand(2, this->terrain->getSizeX()-2);
				for(int i=0;i<NUM_INSTANCE;i++){
					float x = rand(randomGen);
					float z = rand(randomGen);

					glm::vec3 pos = glm::vec3(x, terrain->getHeight(x, z),z);
					// pos = glm::vec3(i);
					glm::mat4 modelInst = glm::translate(glm::mat4(1.f), pos);
					trees[0]->modelMatInstanced[i] = modelInst;
				}
            }
			
			// static float rad = 0;
			// ImGui::SliderFloat("rad", &rad, 0, 6.28);
			// lightSources[0]->setPosition(-2.f * glm::vec3(glm::sin(rad), glm::cos(rad), 0));
			// lightSources[0]->dirLight->direction = glm::vec3(glm::sin(rad), glm::cos(rad), 0);

			// lightSources[1]->dirLight->direction = -glm::vec3(glm::sin(rad), glm::cos(rad), 0);
			// lightSources[1]->setPosition(2.f * glm::vec3(glm::sin(rad), glm::cos(rad), 0));

			static float yoffset = 0.f;
			ImGui::SliderFloat("yoffset", &yoffset, -3, 3);
			coins[0]->setOffset(glm::vec3(1,0,0), glm::vec3(0,yoffset,0), glm::vec3(1.0));


			// ImGui::SliderFloat("birdVel", &bird->jumpVel, 0, 100);
			// static float gravityAcc = 0.1f;
			// ImGui::SliderFloat("birdacc", &gravityAcc, 0, 10);
			// bird->setAcceleration(glm::vec3(0,-gravityAcc, 0));
			
			ImGui::End();
        }

		ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(displayManager.window);
		// glfwPollEvents();
	}

	// ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}

void Game::terminate(){
	// delete birds, ,,,,
}

// STL
#include <iostream>
// include opengl extension and application library
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// include Engine
#include <Camera.hpp>
#include <Mesh.hpp>
#include <Material.hpp>
#include <RenderObject.hpp>

// include application headers
#include <Fractal.hpp>
#include <DefaultMaterial.hpp>
#include <PerVertexColorMaterial.hpp>
#include <Snowflake.hpp>
#include <Animation.hpp>

// Generate simple rectangle in the background of animation
void GenerateGradientRectangle(Engine::Mesh* mesh)
{
	mesh->SetDrawMode(GL_TRIANGLE_STRIP);
	mesh->AddAttribute(4);
	glm::vec3 back[]= {
		glm::vec3(-9.0f, -1.5f, -9.0f),
		glm::vec3(9.0f, -1.5f, -9.0f),
		glm::vec3(-9.0f, 8.7f, -9.0f),
		glm::vec3(9.0f, 8.7f, -9.0f)
	};
	for (int i = 0; i < 4; i++) {
		mesh->AddVertexData(back[i]);
		mesh->AddVertexData(1.0f);
	}
	mesh->SetNumElements(4);
	mesh->CreateMesh();
    return;
}

	/* 
		Create rectangle using triangulation specified in documentation
		Rectangle is symetric, so (x, y) coordinate of top right corner describes other 4
		Therefore we have corner for big and small rectangle
	*/
void CreateRectangleWithHole(Engine::Mesh* mesh, float outerX, float innerX, float outerY, float innerY)
{
	
	mesh->SetDrawMode(GL_TRIANGLE_STRIP);
	mesh->AddAttribute(4);
	// it is easy to check that sign are actually right
	float signX[] = { -1, -1, 1, 1 }, signY[] = { -1, 1, 1, -1 };
	glm::vec2 frame[10];
	for (int i = 0; i < 5; i++) {
		frame[2 * i] = glm::vec2(outerX * signX[i % 4], outerY * signY[i % 4]);
		frame[2 * i + 1] = glm::vec2(innerX * signX[i % 4], innerY * signY[i % 4]);
	}
	for (int i = 0; i < 10; i++) {
		mesh->AddVertexData(frame[i]);
		mesh->AddVertexData(glm::vec2(1.0f, 1.0f));
	}
	
	mesh->SetNumElements(10);
	mesh->CreateMesh();
    return;
}


// Generates simple rectangle on window
void GenerateWindowCrest(Engine::Mesh* mesh) {
	mesh->SetDrawMode(GL_TRIANGLE_STRIP);
	mesh->AddAttribute(4);
	glm::vec2 firstR[] = {
		glm::vec2(-0.1f, 3.6f),
		glm::vec2(-0.1f, -3.6f),
		glm::vec2(0.1f, 3.6f),
		glm::vec2(0.1f, -3.6f)
	};
	for (int i = 0; i < 4; i++) {
		mesh->AddVertexData(firstR[i]);
		mesh->AddVertexData(glm::vec2(1.0f, 1.0f));
	}
	mesh->SetNumElements(4);
	mesh->CreateMesh();
}

// Generte ground so that trees appear like they stand on it
void CreateGround(Engine::Mesh* mesh) {
	mesh->SetDrawMode(GL_TRIANGLE_STRIP);
	mesh->AddAttribute(4);
	glm::vec3 corners[] = {
		glm::vec3(-5.5f, -4.5f, -1.0f),
		glm::vec3(5.5f, -4.5f, -1.0f),
		glm::vec3(-11.0f, -1.5f, -9.0f),
		glm::vec3(11.0f, -1.5f, -9.0f)
	};
	for (int i = 0; i < 4; i++) {
		mesh->AddVertexData(corners[i]);
		mesh->AddVertexData(1.0f);
	}
	mesh->SetNumElements(4);
	mesh->CreateMesh();
	return;
}

/*
	Create triangle and make 2 more copies shfting them a little bit down
	we need meshA for top of tree,
	and mesh B is simple rectangle to represent stump of tree
*/
void GenerateSimpleTree(Engine::Mesh* meshA, Engine::Mesh* meshB)
{
	meshA->AddAttribute(4);
	float depth = -0.5f;
	glm::vec2 triangle[] = {
		glm::vec2(0.0f, 1.0f),
		glm::vec2(-1.0f, 0.0f),
		glm::vec2(1.0f, 0.0f)
	};
	for (float i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			meshA->AddVertexData(triangle[j] - i * glm::vec2(0.0f, 0.6f));
			meshA->AddVertexData(glm::vec2(depth, 1.0f));
		}
	}
	meshA->SetNumElements(9);
	meshA->CreateMesh();

	meshB->SetDrawMode(GL_TRIANGLE_STRIP);
	meshB->AddAttribute(4);
	glm::vec2 stump[] = {
		glm::vec2(-0.3f, -1.7f),
		glm::vec2(0.3f, -1.7f),
		glm::vec2(-0.3f, -1.2f),
		glm::vec2(0.3f, -1.2f)
	};
	for (int i = 0; i < 4; i++) {
		meshB->AddVertexData(stump[i]);
		meshB->AddVertexData(glm::vec2(depth, 1.0f));
	}
	meshB->SetNumElements(4);
	meshB->CreateMesh();
    return;
}

int main(int argc, char** argv)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    GLFWwindow* window;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1024, 768, "Homework 1 - Snowflake Animation (20170876)", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        std::cout << "GLEW Error: " << glewGetErrorString(glewError) << std::endl;
        exit(1);
    }
    Animation* animation = new Animation();
	Fractal fractal = Fractal();
	Engine::Mesh* mesh = new Engine::Mesh();
    Engine::Camera* mainCamera = new Engine::Camera();
	fractal.GenerateSnowflake(mesh);
	

	/*Generate list of Snowflakes with random colors
		Random colors achivied by using std::rand
		push snowflakes into animation vector and our own vector*/
	std::vector<Snowflake*> myVec;
	for (int i = 0; i < 35; i++) {
		DefaultMaterial* material = new DefaultMaterial();
		material->CreateMaterial();
		material->UpdateColor(glm::vec3((std::rand() % 1000)/1000.0f, (std::rand() % 1000) / 1000.0f, (std::rand() % 1000) / 1000.0f));
		Snowflake* a = new Snowflake(mesh, material);
		animation->AddSnowflake(a);
		myVec.push_back(a);
	} 
	
	// From here I start generation scenary objects, all scene objects are pushed into vector ob to store them
	
	// this is gradient rectangle so I use pervertexcolormaterial
	std::vector<Engine::RenderObject> scene;
	Engine::Mesh* SkyMesh = new Engine::Mesh();
	PerVertexColorMaterial* perC = new PerVertexColorMaterial();
	perC->CreateMaterial();
	GenerateGradientRectangle(SkyMesh);
	Engine::RenderObject background = Engine::RenderObject(SkyMesh, perC);
	scene.push_back(background);


	// Start of window
	Engine::Mesh* windowFrame = new Engine::Mesh();
	CreateRectangleWithHole(windowFrame, 3.2f, 3.0f, 3.6f, 3.4f);
	DefaultMaterial* woodenFrame = new DefaultMaterial();
	woodenFrame->CreateMaterial();
	woodenFrame->UpdateColor(glm::vec3(86, 47, 14) / 255.0f);
	Engine::RenderObject frame = Engine::RenderObject(windowFrame, woodenFrame);
	scene.push_back(frame);

	// rectangle along window
	Engine::Mesh* crest = new Engine::Mesh();
	GenerateWindowCrest(crest);
	Engine::RenderObject rCrest = Engine::RenderObject(crest, woodenFrame);
	scene.push_back(rCrest);

	// Room interior
	Engine::Mesh* roomInterior = new Engine::Mesh();
	CreateRectangleWithHole(roomInterior, 6.0f, 3.2f, 4.0f, 3.6f);
	DefaultMaterial* woodenWall = new DefaultMaterial();
	woodenWall->CreateMaterial();
	woodenWall->UpdateColor(glm::vec3(37, 14, 3) / 255.0f);
	Engine::RenderObject roomWall = Engine::RenderObject(roomInterior, woodenWall);
	scene.push_back(roomWall);
	
	// Glass has to be little bit transparent so when defining color 4th argument is 0.25
	Engine::Mesh* glass = new Engine::Mesh();
	CreateRectangleWithHole(glass, 3.0f, 0.0f, 3.4f, 0.0f);
	DefaultMaterial* glassC = new DefaultMaterial();
	glassC->CreateMaterial();
	glassC->UpdateColorOpacity(glm::vec4(1.0f, 1.0f, 1.0f, 0.25f));
	Engine::RenderObject windowGlass = Engine::RenderObject(glass, glassC);

	// Whitish ground
	Engine::Mesh* ground = new Engine::Mesh();
	DefaultMaterial* white = new DefaultMaterial();
	white->CreateMaterial();
	white->UpdateColor(glm::vec3(238, 238, 238)/255.0f);
	CreateGround(ground);
	Engine::RenderObject whiteGround = Engine::RenderObject(ground, white);
	scene.push_back(whiteGround);


	// coordinates of future trees
	glm::vec3 treePos[] = {
		glm::vec3(-2.0f, -2.5f, -1.0f),
		glm::vec3(2.5f, -1.5f, -2.5f),
		glm::vec3(0.5f, 0.0f, -5.0f)
	};
	// generate them by means of for loop
	for (int i = 0; i < 3; i++) {
		Engine::Mesh* tree = new Engine::Mesh();
		Engine::Mesh* stump = new Engine::Mesh();
		GenerateSimpleTree(tree, stump);
		DefaultMaterial* green = new DefaultMaterial();
		DefaultMaterial* wood = new DefaultMaterial();
		wood->CreateMaterial();
		wood->UpdateColor(glm::vec3(161, 64, 5) / 255.0f);
		green->CreateMaterial();
		green->UpdateColor(glm::vec3(35, 67, 17) / 255.0f);
		Engine::RenderObject up = Engine::RenderObject(tree, green);
		Engine::RenderObject down = Engine::RenderObject(stump, wood);
		up.SetPosition(treePos[i]);
		down.SetPosition(treePos[i]);
		scene.push_back(up);
		scene.push_back(down);
	}

	mainCamera->SetPosition(glm::vec3(0.0f, 0.0f, 7.5f));
	
	// Keep track of last loop
	float lastLoop = 0.0f;
    
	
	/* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Enable depth test
        glEnable(GL_DEPTH_TEST);

        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);

       glEnable(GL_CULL_FACE);
       glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// to ensure proper deltaTime we need time since last loop
		float totalTime = (float)glfwGetTime();
		float elapsedTime = totalTime - lastLoop;
		lastLoop = totalTime;
		animation->Animate(elapsedTime);
		
		//Render all snowflakes and scene object
		for (Snowflake* s : myVec) {
			s->Render(mainCamera);
		}
		for (Engine::RenderObject ob : scene)
			ob.Render(mainCamera);


		//  enable Blend mode so that windowGlass blends with outside appearing as semi-transparent  
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		windowGlass.Render(mainCamera);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    delete mainCamera;

    glfwTerminate();
    return 0;

}
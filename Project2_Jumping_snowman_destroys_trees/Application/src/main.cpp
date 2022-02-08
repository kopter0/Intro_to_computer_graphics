// STL
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <deque>
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
#include <DefaultMaterial.hpp>
#include <PickingMaterial.hpp>
#include <LineMaterial.hpp>
#include <Geometry.hpp>
#include <Animation.hpp>
#include <picking.hpp>
#include <PickableObject.hpp>
#include <Snowman.hpp>
#include <PerVertexColorMaterial.hpp>

#define _USE_MATH_DEFINES

GLFWwindow* g_window;
float g_window_width = 1024.f;
float g_window_height = 768.f;
int g_framebuffer_width = 1024;
int g_framebuffer_height = 768;

// Initialize useful values
int target = -1, direction = -1;
bool action = false, inAction = false, shaking = false, tree_intr = false, rotation_mode = false, rModeJump = false, trailM = false;
float shakeTime = 0.0f;
glm::vec3 tree_fall = glm::vec3(0.0f);
glm::vec3 cursor_pos = glm::vec3(0.0f);

enum Motion {
	forward, backward, right, left
};


bool shake(float deltaTime,Engine::Camera *cam) {
	// dumped oscilation equation 
	// x = e**(-at) * cos(wt - b)
	// w = sqrt(c**2 - a**2)
	shakeTime += deltaTime;
	float a = 3.0f, c = 28.0f, b = 1.6f;
	float eP = glm::pow(glm::e<float>(), -a * shakeTime);
	float w = glm::sqrt(glm::pow(c,2) - glm::pow(a, 2));
	float y = eP * cos(w * shakeTime - b);
	cam->SetPosition(glm::vec3(0.0f, y * 0.3f, 7.5f));
	if (shakeTime > 0.5f) {
		shakeTime = 0;
		return false;
	}
	return true;
}

// TODO: Fill up GLFW mouse button callback function
static void MouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
    //example code for get x position and y position of mouse click
    if (a_button == GLFW_MOUSE_BUTTON_LEFT && a_action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(a_window, &xpos, &ypos);
        xpos = xpos / ((double)g_window_width) * ((double)g_framebuffer_width);
        ypos = ypos / ((double)g_window_height) * ((double)g_framebuffer_height);
        target = pick((int)xpos, (int)ypos, g_framebuffer_width, g_framebuffer_height);
        std::cout << "Picked object index: " << target << std::endl;
	}
}



// TODO: Fill up GLFW cursor position callback function
static void CursorPosCallback(GLFWwindow* a_window, double a_xpos, double a_ypos)
{
	double xpos, ypos;
	glfwGetCursorPos(a_window, &xpos, &ypos);
	// update cursor pos, since window is recongnized as reactangle with origin at top left corner, some adjustments needed
	cursor_pos = glm::vec3(11 * (xpos / g_window_width - 0.5f), -9 * (ypos / g_window_height - 0.5), 0.0f);
}

static void KeyboardCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
    if (a_action == GLFW_PRESS && !action)
    {
        switch (a_key)
        {
        case GLFW_KEY_H:
            std::cout << "CS380 Homework 2" << std::endl;
            std::cout << "keymaps:" << std::endl;
			std::cout << "jumping:\nW: forward\nA: to left\nS: backward\nD: to right" << std::endl;
			std::cout << "trail MODE:\nturn on/turn off: T\n";
            std::cout << "h\t\t Help command" << std::endl;
            break;
		case GLFW_KEY_W:
			direction = forward;
			inAction = action = (rotation_mode) ? false : true;
			break;
		case GLFW_KEY_S:
			direction = backward;
			inAction = action = (rotation_mode) ? false : true;
			break;
		case GLFW_KEY_A:
			direction = left;
			inAction = action = true;
			break;
		case GLFW_KEY_D:
			direction = right;
			inAction =  action = true;
			break;
		case GLFW_KEY_R:
			rotation_mode = (rotation_mode) ? false : true;
			break;
		case GLFW_KEY_SPACE:
			direction = forward;
			inAction = rModeJump = (rotation_mode) ? true : false;
			break;
		case GLFW_KEY_T:
			trailM = (trailM) ? false : true;
			std::cout << trailM << std::endl;
			break;
        default:
            break;
        }
    }

	if (a_action == GLFW_RELEASE) {
		switch (a_key) {
		case GLFW_KEY_W:
			direction = forward;
			action = false;
			break;
		case GLFW_KEY_S:
			direction = backward;
			action = false;
			break;
		case GLFW_KEY_A:
			direction = left;
			action = false;
			break;
		case GLFW_KEY_D:
			direction = right;
			action = false;
			break;
		case GLFW_KEY_SPACE:
			rModeJump = false;
		}
	}
}


int main(int argc, char** argv)
{
    // Initialize GLFW library
    if (!glfwInit())
    {
        return -1;
    }

    // Create window and OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    /* Create a windowed mode window and its OpenGL context */
    g_window = glfwCreateWindow(g_window_width, g_window_height, "HW2 - Do you want to build a snowman?", NULL, NULL);
    if (!g_window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(g_window);

    // Initialize GLEW library
    GLenum glew_error = glewInit();
    if (glew_error != GLEW_OK)
    {
        std::cout << "GLEW Error: " << glewGetErrorString(glew_error) << std::endl;
        exit(1);
    }


    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

    glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetMouseButtonCallback(g_window, MouseButtonCallback);
    glfwSetCursorPosCallback(g_window, CursorPosCallback);
    glfwSetKeyCallback(g_window, KeyboardCallback);

    // Initialize framebuffer object and picking textures
    pickingInitialize(g_framebuffer_width, g_framebuffer_width);

    Engine::Camera* main_camera = new Engine::Camera();
    main_camera->SetPosition(glm::vec3(0.0f, 0.0f, 7.5f));

    // __________________________INIT________________________________________
	Geometry geometry = Geometry();

    Engine::Mesh* cube_mesh = new Engine::Mesh();
    geometry.GenerateCube(cube_mesh);

	Animation* animation = new Animation();

	Engine::Mesh* star_mesh = new Engine::Mesh();
	geometry.GenerateStar(star_mesh);

    DefaultMaterial* material = new DefaultMaterial();
    material->CreateMaterial();

    PickingMaterial* picking_material = new PickingMaterial();
    picking_material->CreateMaterial();
	//__________________________________________________________________________
	
	
	Snowman mySnowman = Snowman(-1.0f);


	// __________________create ground_____________________________
    PickableObject snowyGround = PickableObject(cube_mesh, material);
    snowyGround.SetPickingMat(picking_material);
	snowyGround.SetScale(glm::vec3(11.0f, 0.1f, 7.0f));
	snowyGround.SetPosition(glm::vec3(0.0f, -2.0f, 0.0f));
	// ______________________________________________________________
	
	//_________________________create sky________________________
	PerVertexColorMaterial *sky_mat = new PerVertexColorMaterial();
	sky_mat->CreateMaterial();
	PickableObject *sky = new PickableObject(cube_mesh, sky_mat);
	sky->SetScale(glm::vec3(12.0f, 5.3f, 0.1f));
	sky->SetPosition(glm::vec3(0.0f, 3.2f, -7.0f));
	//_____________________________________________________________

	// _____________________TREES___________________________________________________________
	std::vector<PickableObject*> top, bot;
	Engine::Mesh *cylindrical = new Engine::Mesh(), *pyramidlike = new Engine::Mesh();
	geometry.generateCylinder(cylindrical);
	geometry.GeneratePyramid(pyramidlike);
	PickableObject *stump = new PickableObject(cylindrical, material);
	stump->SetPosition(glm::vec3(-3.0f, -2.0f, -1.0f));
	stump->SetScale(glm::vec3(0.2f, 1.0f, 0.2f));
	for (int i = 0; i < 3; i++) {
		PickableObject *gt = new PickableObject(pyramidlike, material);
		gt->AddParent(stump);
		gt->SetPosition(glm::vec3(0.0f, 1.0f + 0.75f * i, 0.0f)); // every pyramid is higher than previous
		gt->SetOrientation(glm::rotate(glm::mat4(1.0f), 20 * 0.017f, Y_AXIS));
		top.push_back(gt);
	}
	bot.push_back(stump);

	//__________________________________________________________________________________________

	// snoflakes
	std::vector<Engine::RenderObject*> snf;
	std::deque<glm::vec4> trailCol;
	std::deque<Engine::RenderObject*> trail;
	for (int i = 0; i < 40; i++)
	{
		Engine::RenderObject* star = new Engine::RenderObject(star_mesh, material);
		star->SetPosition(glm::vec3(-5.0f + 10.0f * ((rand() % 255) / 255.0f), 5.0f * ((rand() % 255) / 255.0f), (rand() % 10) - 5.0f));
		star->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
		snf.push_back(star);
		animation->AddObject(star);
	}

    float prev_time = 0;
	int counter = 0;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(g_window) && glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
		//disable blend so that all objects appear normal
		glDisable(GL_BLEND);

        float total_time = (float)glfwGetTime();
        float elapsed_time = total_time - prev_time;
        prev_time = total_time;

        // First Pass: Object Selection (Slide No. 20)
        // this is for picking the object using mouse interaction
        // binding framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, picking_fbo);
        // Background: RGB = 0x000000 => objectID: 0
        glClearColor((GLclampf) 0.0f, (GLclampf) 0.0f, (GLclampf) 0.0f, (GLclampf) 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render your objects that you want to select using mouse interaction here
        snowyGround.RenderPicking(main_camera);
		mySnowman.PickingRender(main_camera);
        // Second Pass: Object Rendering (Slide No. 11)
        // Drawing object again
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor((GLclampf)(128. / 255.), (GLclampf)(200. / 255.), (GLclampf)(255. / 255.), (GLclampf)0.);
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Todo: Render object with main camera in the loop
		
		// Prioritize poking to jumping 
		if (target == 2) {
			target = mySnowman.poke(elapsed_time);
		}
		else if (action || inAction) {
			//if (rotation_mode && action) {
			//	s1.rotate(direction);
			//}
			if (!(inAction = mySnowman.jump(elapsed_time, direction))) {
				shaking = true;
			}
		}

		if (shaking) {
			shaking = shake(elapsed_time, main_camera);
		}

		mySnowman.Render(main_camera);
		sky->Render(main_camera);

		material->UpdateColor(glm::vec4(1.0f));
		for (Engine::RenderObject *ob : snf) {
			ob->Render(main_camera);
		}


		material->UpdateColor(glm::vec4(71, 95, 84, 255) / 255.0f);
		for (PickableObject* ob : top) {
			ob->Render(main_camera);
		}


		material->UpdateColor(glm::vec4(192, 137, 103, 255) / 255.0f);
		stump->Render(main_camera);

		material->UpdateColor(glm::vec4(238, 238, 238, 255) / 255.0f);
		snowyGround.Render(main_camera);
		
		animation->Animate(main_camera, elapsed_time);

		// for tree to fall in right direction I need to take perpendicular vector to vector of relative location tree and snowman
		glm::vec3  smpos = mySnowman.getPosition(), tr_pos = stump->GetPosition();
		glm::vec3 vec = tr_pos - smpos;
		if (glm::length(vec) < 1.7f && !tree_intr) {
			tree_intr = true;
			tree_fall = glm::vec3(vec[2], 0.0f, -vec[0]);
		}

		if (tree_intr)
			tree_intr = animation->TreeInteraction(stump, elapsed_time, tree_fall);
		
	
		// only every 20th loop Trail snowflake appears
		counter = (counter + 1) % 20;
		if (trailM && !counter) {
			Engine::RenderObject* star = new Engine::RenderObject(star_mesh, material);
			star->SetPosition(glm::vec3(cursor_pos));
			star->SetScale(glm::vec3(0.2f));
			trail.push_back(star);
			trailCol.push_back(glm::vec4(std::rand() % 1000, std::rand() % 1000, std::rand() % 1000, 1000) / 1000.0f);
			animation->AddTrail(star);
		}
		else if (!trailM){
			// if trail disabled delete all snowflakes
			int t = trail.size();
			while (t-- > 0) {
				Engine::RenderObject* ob = trail.front();
				trail.pop_front();
				trailCol.pop_back();
				animation->RemoveFirst();
				delete ob;
			}
		}
		// Enable blend for fading effect
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (trailM && trail.size()) {

			animation->trailAnim(elapsed_time);
			for (int i = trail.size() - 1; i >= 0; i--) {
				// gradually change alpha comp of color
				material->UpdateColor(trailCol[i] - glm::vec4(0.0f, 0.0f, 0.0f,elapsed_time));
				trailCol[i] = trailCol[i] - glm::vec4(0.0f, 0.0f, 0.0f, elapsed_time);
				trail[i]->Render(main_camera);
			}


			// when alpha less than 0 delete snowflake
			if (trailCol[0][3] < 0.0f) {
				trailCol.pop_front();
				animation->RemoveFirst();
				Engine::RenderObject* temp = trail.front();
				trail.pop_front();
				delete temp;
			}
		}


        /* Swap front and back buffers */
        glfwSwapBuffers(g_window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Delete resources
    delete main_camera;

    glfwTerminate();
    return 0;

}
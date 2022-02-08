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
#include <ModelLoader.hpp>

#include <DiffuseMaterial.hpp>
#include <PickingMaterial.hpp>
#include <Geometry.hpp>
#include <ModelLoader.hpp>

#include <picking.hpp>
#include <PickableObject.hpp>

#include <PhongMaterial.hpp>
#include <ToonMaterial.hpp>
#include <MyMaterial.hpp>
#include <Animation.hpp>

#define X_AXIS  (glm::vec3(1.f, 0.f, 0.f))
#define Y_AXIS  (glm::vec3(0.f, 1.f, 0.f))
#define Z_AXIS  (glm::vec3(0.f, 0.f, 1.f))


// Contains object's material inforamtion 
struct mat_properties {
	glm::vec3 ambP, difP, specP;
	float shine;
};

enum MaterialType {
	DiffuseMaterialType = 0,
	PhongMaterialType,
	ToonMaterialType,
	NightVisionMaterialType
};


GLFWwindow* g_window;
float g_window_width = 1024.f;
float g_window_height = 768.f;
int g_framebuffer_width = 1024;
int g_framebuffer_height = 768;
float toRad = glm::pi<float>() / 180.f;
bool changeMat = false;
MaterialType inUse = PhongMaterialType;

Engine::Camera* main_camera;

Engine::Mesh* ground_mesh;
Engine::Mesh* bunny_mesh;
Engine::Mesh* gun_mesh;
Engine::Mesh* circle_mesh;
Engine::Mesh* sphere_mesh;
std::vector<Engine::Mesh*> ChristmasTree_mesh;

DiffuseMaterial* material;
PhongMaterial* phong_material;
ToonMaterial* toon_material;
MyMaterial* nightVision_material;


Engine::RenderObject* ground_object;
Engine::RenderObject* bunny_object;
Engine::RenderObject* gun_object;
Engine::RenderObject* mask;
std::vector<Engine::RenderObject*> ChristmasTree_object;




mat_properties bunnyMatprop, groundMatprop, treeMatprop;

std::vector<Light> lights;

Animation* bunny_animation;
Animation* camera_animation;
Animation* sun_animation;

Geometry* geometry;

//Unified method to change material properties
static void updateMatProperties(Engine::Material *mat, mat_properties prop) {
	mat->UpdateAmbientReflectance(prop.ambP);
	mat->UpdateDiffuseReflectance(prop.difP);	
	mat->UpdateSpecularReflectance(prop.specP);
	mat->UpdateShininess(prop.shine);
}

//change objects' material
static void setMatTo(Engine::Material* mat) {
	bunny_object->SetMaterial(mat);
	ground_object->SetMaterial(mat);
	for (Engine::RenderObject* ob : ChristmasTree_object)
		ob->SetMaterial(mat);
}

static void MouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
	if (a_button == GLFW_MOUSE_BUTTON_LEFT && a_action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(a_window, &xpos, &ypos);
		xpos = xpos / ((double)g_window_width) * ((double)g_framebuffer_width);
		ypos = ypos / ((double)g_window_height) * ((double)g_framebuffer_height);
		int target = pick((int)xpos, (int)ypos, g_framebuffer_width, g_framebuffer_height);
	}
}

static void CursorPosCallback(GLFWwindow* a_window, double a_xpos, double a_ypos)
{

}

static void KeyboardCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
	if (a_action == GLFW_PRESS)
	{
		glm::mat4 ori = main_camera->GetTransform()->GetOrientation();
		glm::vec3 pos = main_camera->GetTransform()->GetPosition();

		switch (a_key)
		{
		case GLFW_KEY_H:
			std::cout << "CS380 Homework 3" << std::endl;
			std::cout << "keymaps:" << std::endl;
			std::cout << "Materials\n1: Phong Material\n2: Toon Material\n3: NightVision Mode" << std::endl << std::endl;
			std::cout << "Camera Control\n arrow up, down, left, right" << std::endl;
			std::cout << "h\t\t Help command" << std::endl;
			break;
		case GLFW_KEY_1:
			inUse = PhongMaterialType;
			changeMat = true;
			break;
		case GLFW_KEY_2:
			inUse = ToonMaterialType;
			changeMat = true;
			break;
		case GLFW_KEY_3:
			inUse = NightVisionMaterialType;
			changeMat = true;
			break;
		case GLFW_KEY_RIGHT:
			glm::mat4 rotBy5 = glm::rotate(glm::mat4(1), glm::radians(5.f), Y_AXIS);
			main_camera->GetTransform()->SetPosition(rotBy5 * glm::vec4(pos, 1));
			main_camera->GetTransform()->SetOrientation(rotBy5 * ori);
			break;
		case GLFW_KEY_LEFT:
			glm::mat4 rotByN5 = glm::rotate(glm::mat4(1), glm::radians(-5.f), Y_AXIS);
			main_camera->GetTransform()->SetPosition(rotByN5 * glm::vec4(pos, 1));
			main_camera->GetTransform()->SetOrientation(rotByN5 * ori);
			break;
		case GLFW_KEY_DOWN:
			glm::vec3 disp = ori * glm::vec4(Z_AXIS, 1);
			glm::mat4 tr = glm::translate(glm::mat4(1), disp);
			main_camera->GetTransform()->SetPosition( tr * glm::vec4(pos, 1));
			break;
		case GLFW_KEY_UP:
			glm::vec3 dispN = ori * glm::vec4(-Z_AXIS, 1);
			glm::mat4 trN = glm::translate(glm::mat4(1), dispN);
			main_camera->GetTransform()->SetPosition(trN * glm::vec4(pos, 1));
			break;
		default:
			break;
		}
	}
}

static void FramebufferResizeCallback(GLFWwindow* a_window, int width, int height)
{
	glViewport(0, 0, width, height);
	reallocatePickingTexture(width, height);

	int window_width, window_height;
	glfwGetWindowSize(g_window, &window_width, &window_height);
	g_window_width = (float)window_width;
	g_window_height = (float)window_height;
	g_framebuffer_width = width;
	g_framebuffer_height = height;
	
	Engine::ProjectionData proj = main_camera->GetProjection();
	proj.aspectRatio = (float)width / (float)height;
	main_camera->SetProjection(proj.aspectRatio, proj.fov, proj.zNear, proj.zFar);
}

static void InitScene()
{
	main_camera = new Engine::Camera();
	main_camera->GetTransform()->SetPosition(glm::vec3(0.0f, 1.0f, 4.0f));

	geometry = new Geometry();

	ground_mesh = new Engine::Mesh();
	bunny_mesh = new Engine::Mesh();
	circle_mesh = new Engine::Mesh();
	sphere_mesh = new Engine::Mesh();
	for (int i = 0; i < 7; i++) {
		Engine::Mesh * temp = new Engine::Mesh();
		ChristmasTree_mesh.push_back(temp);
	}

	Engine::ModelLoader *loader = new Engine::ModelLoader("Resources\\Models\\scene.obj");
	loader->GenerateMesh(ground_mesh, 0);
	loader->GenerateMesh(bunny_mesh, 1);

	loader = new Engine::ModelLoader("Resources\\Models\\ChristmasTree.obj");
	for (int i = 0; i < 7; i++) {
		loader->GenerateMesh(ChristmasTree_mesh[i], i);
	}

	/*------------------------------------------------------------------------------------*/

	geometry->GenerateCircle(circle_mesh);
	geometry->GenerateSphere(sphere_mesh, 5.0f);


	material = new DiffuseMaterial();
	phong_material = new PhongMaterial();
	toon_material = new ToonMaterial();
	nightVision_material = new MyMaterial();


	material->CreateMaterial();
	phong_material->CreateMaterial();
	toon_material->CreateMaterial();
	nightVision_material->CreateMaterial();


	material->UpdateDiffuseReflectance(glm::vec3(0, 1, 0));

	ground_object = new Engine::RenderObject(ground_mesh, phong_material);
	bunny_object = new Engine::RenderObject(bunny_mesh, phong_material);
	mask = new Engine::RenderObject(circle_mesh, phong_material);

	for (int i = 0; i < 7; i++) {
		Engine::RenderObject* temp = new Engine::RenderObject(ChristmasTree_mesh[i], phong_material);
		temp->GetTransform()->SetScale(glm::vec3(0.1));
		ChristmasTree_object.push_back(temp);

	}


/*----------------------------Set Material Properties-----------------------------------------------------*/

	treeMatprop = {
		glm::vec3(0.1f),
		glm::vec3(0.1f, 0.8f, 0.1),
		glm::vec3(0.2f),
		4.f
	};

	bunnyMatprop = {
		glm::vec3(0.1f),
		glm::vec3(0.8f),
		glm::vec3(0.6f),
		512.f
	};
	groundMatprop = {
		glm::vec3(0.0f, 0.1f, 0.0f),
		glm::vec3(0.1f, 0.9f, 0.1f),
		glm::vec3(0.05f),
		5.f
	};

	/*---------------------------Add Lights-----------------------------------*/

	Light sun = Light();
	sun.type = DirectionalLight;
	sun.illu_dif = glm::vec3(0.9f, 0.8f, 0.7f);
	sun.illu_amb = glm::vec3(0.1f);
	sun.illu_spec = glm::vec3(0.2f);
	sun.angle = glm::radians(10.f);
	sun.enabled = 1;

	sun.transform.SetPosition(glm::vec3(3.f, 500.f, 1.f));


	lights.push_back(sun);

	Light moon = Light();
	moon.type = PointLightLinear;
	moon.transform.SetPosition(glm::vec3(-2.0f, 15.f, 0.f));
	moon.enabled = 1;
	moon.illu_amb = glm::vec3(0.01f);
	moon.illu_dif = glm::vec3(31, 41, 53) / 255.f * 2.f;
	moon.illu_spec = glm::vec3(0.15f);

	lights.push_back(moon);
	/*--------------------------------------------------------------------------*/

	toon_material->UpdateDiffuseReflectance(glm::vec3(1.0f, 0.5f, 0.0f));

	nightVision_material->UpdateDiffuseReflectance(0.5f);

	phong_material->UpdateLight(lights);


	/*Light example = Light();
	example.transform.SetPosition(glm::vec3(1.0f, 2.0f, 10.0f));
	example.illu_amb = glm::vec3(0.1f);
	example.illu_dif = glm::vec3(1.0f);
	example.illu_spec = glm::vec3(0.3f);
	example.lookAt = glm::vec3(0.0f);
	example.angle = glm::radians(10.f);
	example.exponent = 5.f;
	example.size = 3.f;
	example.type = Spotlight;
	lights.push_back(example);
	*/
	/*Initialize mask for stencil buffer*/

	circle_mesh = new Engine::Mesh();
	geometry->GenerateCircle(circle_mesh);
	mask = new Engine::RenderObject(circle_mesh, material);
	mask->GetTransform()->SetScale(glm::vec3(5.f, 5.f, 1.0f));
	mask->GetTransform()->AddParent(main_camera->GetTransform());

}

static void InitAnimation()
{

	/*
	Use polar coordinates to define circular path
	(cos(a), sin(a)) in needed plane
	*/

	bunny_animation = new Animation(bunny_object->GetTransform(), 5, AnimationType::All);


	for (float phi = -180.f; phi <= 180; phi = phi + 5) {
		Engine::Transform tBunnny;
		float phiD = glm::radians(phi);
		tBunnny.SetPosition(glm::vec3(glm::cos(phiD), glm::abs(glm::sin(phiD * 4)) / 8, glm::sin(phiD)) * 2.f);

		//rotate bunny to make him facing in same direction as motion
		tBunnny.SetOrientation(glm::rotate(tBunnny.GetOrientation(), glm::radians(-phi + 90), Y_AXIS));
		tBunnny.SetScale(glm::vec3(0.5f));
		bunny_animation->AddKeyframe((phi + 180) / 72.f, tBunnny);
	}


	sun_animation = new Animation(&lights[0].transform, 5, AnimationType::Position);
	
	for (float phi = 180.f; phi >= -180; phi -= 5) {
		Engine::Transform tSun;
		float phiD = glm::radians(phi);
		tSun.SetPosition(glm::vec3(glm::cos(phiD), glm::sin(phiD), 0) * 20.f);
		sun_animation->AddKeyframe((180 - phi) / 72.f, tSun);
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
	g_window = glfwCreateWindow(g_window_width, g_window_height, "Homework 3 - Let there be light and Blinn-Phong reflection model", NULL, NULL);
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
	glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

	glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetMouseButtonCallback(g_window, MouseButtonCallback);
	glfwSetCursorPosCallback(g_window, CursorPosCallback);
	glfwSetKeyCallback(g_window, KeyboardCallback);
	glfwSetFramebufferSizeCallback(g_window, FramebufferResizeCallback);


	// Initialize framebuffer object and picking textures
	pickingInitialize(g_framebuffer_width, g_framebuffer_width);

	InitScene();
	InitAnimation();

	float prev_time = (float)glfwGetTime();
	float day = 0;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(g_window) && glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		float total_time = (float)glfwGetTime();
		float elapsed_time = total_time - prev_time;
		prev_time = total_time;
		
		/*Every 2.5 sec alternate between day and night*/
		day += elapsed_time;
		if (day >= 5.f) {
			day = 0;
			lights[0].enabled = 1;
		}
		else if (day >= 2.5f)
			lights[0].enabled = 0;

		/*
		depending on time change color of background
		The brightest is when sun in zenith, and complete dark in the middle of night
		*/
		float dayFactor = -glm::sin(2 * day * glm::pi<float>() / 5) + 1;
		dayFactor /= 2;
		glm::vec3 back = glm::mix(glm::vec3(128, 200, 255), glm::vec3(0), dayFactor);

		glDisable(GL_STENCIL_TEST);
		// First Pass: Object Selection
		// this is for picking the object using mouse interaction
		// binding framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, picking_fbo);
		// Background: RGB = 0x000000 => objectID: 0
		glClearColor((GLclampf) 0.0f, (GLclampf) 0.0f, (GLclampf) 0.0f, (GLclampf) 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		/* RenderPicking here */


		// Second Pass: Object Rendering
		// Drawing object again
		glBindFramebuffer(GL_FRAMEBUFFER, 0);




		if (changeMat) {
			switch (inUse) {
			case PhongMaterialType:
				setMatTo(phong_material);
				phong_material->UpdateLight(lights);
				break;
			case ToonMaterialType:
				setMatTo(toon_material);
				toon_material->UpdateLight(lights);
	  			break;
			case NightVisionMaterialType:
				setMatTo(nightVision_material);
				nightVision_material->UpdateLights(lights);
				break;
			}
		}
		
		Engine::Material *curmat;


		/*
		if nightvision used, set background to grey
		Use predefined mask for restricting area of vision
		*/

		if (inUse == NightVisionMaterialType) {
			glClearColor((GLclampf) 0.1f, (GLclampf) 0.1f, (GLclampf) 0.1f, (GLclampf) 0.0f);
			curmat = nightVision_material;
			glEnable(GL_STENCIL_TEST);
			glStencilOp(GL_ZERO, GL_KEEP, GL_REPLACE);
			//write to stencil buffer
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			mask->GetTransform()->SetPosition(glm::vec3(-4.f, 0.f, -10.f));
			mask->Render(main_camera);
			mask->GetTransform()->SetPosition(glm::vec3(4.f, 0.f, -10.f));
			mask->Render(main_camera);
			// disable writing and render only what is covered by mask
			glStencilFunc(GL_EQUAL, 1, 0xFF);
			glStencilMask(0x00);
		}
		else {
			phong_material->UpdateLight(lights);
			toon_material->UpdateLight(lights);
			curmat = phong_material;
			if (inUse == ToonMaterialType)
				curmat = toon_material;
			glClearColor((GLclampf)(back[0] / 255.f), (GLclampf)(back[1] / 255.f), (GLclampf)(back[2] / 255.f), 0);
			phong_material->UpdateFog(back / 255.f);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		/* Render here */
		bunny_animation->Animate(elapsed_time);		// Please don't forget to call Animation.Animate in each frame!
		//camera_animation->Animate(elapsed_time);
		sun_animation->Animate(elapsed_time);
		

		updateMatProperties(curmat, treeMatprop);

		for (Engine::RenderObject* ob: ChristmasTree_object)
			ob->Render(main_camera);


		updateMatProperties(curmat, groundMatprop);
		ground_object->Render(main_camera);

		updateMatProperties(curmat, bunnyMatprop);
		bunny_object->Render(main_camera);


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
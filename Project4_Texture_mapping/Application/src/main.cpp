// 2019 Spring CS380 Homework 4
// You have to implement
// 1. Normal mapping
//	NormalMaterial.cpp, NormalMaterial.hpp
//	Vertex has new attribute vec4 tangent (see ModelLoader.cpp or Geometry.cpp)
// 2. Shadow Mapping
//	ShadowMap.hpp, ShadowMaterial.cpp, ShadowMaterial.hpp
//
// You can create/modify any functions or files in APP
// Use the OpenGL documentation
// You could use glm::ortho(), glm::lookAt() for shadow mapping

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
#include <Texture.hpp>

#include <DiffuseMaterial.hpp>
#include <PickingMaterial.hpp>
#include <NormalMaterial.hpp>
#include <ShadowMaterial.hpp>
#include <Geometry.hpp>
#include <ModelLoader.hpp>

#include <picking.hpp>
#include <PickableObject.hpp>

#include <CubeMaterial.hpp>
#include <Animation.hpp>

#include <ShadowMap.hpp>
#
#define X_AXIS (glm::vec3(1, 0, 0))
#define Y_AXIS (glm::vec3(0, 1, 0))
#define Z_AXIS (glm::vec3(0, 0, 1))

GLFWwindow* g_window;
float g_window_width = 1024.f;
float g_window_height = 768.f;
int g_framebuffer_width = 1024;
int g_framebuffer_height = 768;
int free_unit = 0;

Engine::Camera* main_camera;

Engine::Mesh* bunny_mesh;
Engine::Mesh* cube_mesh;
Engine::Mesh* ground_mesh;
Engine::Mesh* mirror_mesh1;

DiffuseMaterial* material;
CubeMaterial* cubemap_material;
NormalMaterial* normal_material;
ShadowMaterial* shadow_material;

Engine::RenderObject* bunny_object; 
Engine::RenderObject* cube_object;
Engine::RenderObject* cubemap_object;
Engine::RenderObject* ground_object;
Engine::RenderObject* mirror_object1;

std::vector<Light> lights;

Animation* bunny_animation;
Animation* camera_animation;
Animation* light_animation;
Animation* cube_animation;

Engine::Transform* oriT;

MatProp cubeProps;

int bunny_unit, wood_unit;

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
		switch (a_key)
		{
		case GLFW_KEY_H:
			std::cout << "CS380 LAB 4" << std::endl;
			std::cout << "keymaps:" << std::endl;
			std::cout << "h\t\t Help command" << std::endl;
			std::cout << "r\t\t rotate the camera -20 degrees with respect to world's y axis" << std::endl;
			std::cout << "l\t\t rotate the camera +20 degrees with respect to world's y axis" << std::endl;
			break;
		case GLFW_KEY_L:
			oriT->SetOrientation(glm::rotate(oriT->GetOrientation(), glm::radians(-20.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
			break;
		case GLFW_KEY_R:
			oriT->SetOrientation(glm::rotate(oriT->GetOrientation(), glm::radians(+20.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
			break;
		case GLFW_KEY_W:
			oriT->SetOrientation(glm::rotate(oriT->GetOrientation(), glm::radians(+20.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
			break;
		case GLFW_KEY_P:
			if (lights[0].type == DirectionalLight) {
				lights[0].type = PointLight;
				shadow_material->IsPoint(1);
			}
			else {
				lights[0].type = DirectionalLight;
				shadow_material->IsPoint(0);
			}
			break;
		default:
			break;
		}
	}
}

static void FramebufferResizeCallback(GLFWwindow* a_window, int width, int height)
{
	glViewport(0, 0, width, height);
	reallocatePickingTexture(width, height, 0);
	reallocateShadowTexture(width, height, 0);

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

static glm::mat4 ComputeLightProj(Light* light) {
	/* if light is directional than light sees scene with orthonomal vision
		Whereas point light sees it in perspective view.
	*/
	glm::mat4 Projection, View, Final, worldTrans = light->transform.GetWorldTransform();
	if (light->type == DirectionalLight) {
		Projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, -1.f, 5.5f);
		glm::vec4 dir = worldTrans * glm::vec4(0.f, 0.f, 1.f, 0.f);
		View = glm::lookAt(glm::vec3(-dir), glm::vec3(0), Y_AXIS);
	}
	else {
		Projection = glm::perspective(1.f, 4 / 3.f, 0.1f, 5.f);
		View = glm::lookAt(light->transform.GetPosition(), glm::vec3(0.f), Y_AXIS);
	}
	Final = Projection * View;
	light->LightProj = Final;
	return Final;
}

static void InitScene()
{
	main_camera = new Engine::Camera();
	main_camera->GetTransform()->SetPosition(glm::vec3(0.0f, 1.0f, 4.0f));
	oriT = new Engine::Transform;
	main_camera->GetTransform()->AddParent(oriT);

	Engine::ModelLoader *loader = new Engine::ModelLoader("Resources\\Models\\bunny.obj");

	bunny_mesh = new Engine::Mesh();
	cube_mesh = new Engine::Mesh();
	ground_mesh = new Engine::Mesh();
	mirror_mesh1 = new Engine::Mesh();

	Geometry geometry = Geometry();
	geometry.GenerateCube(cube_mesh);
	geometry.GenerateGround(ground_mesh);

	loader->GenerateMesh(bunny_mesh, 0);
	geometry.GenerateCube(mirror_mesh1);

	Engine::TextureLoader* textureloader1 = new Engine::TextureLoader(2, "Resources\\Models\\bunny.png");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	std::string path_prefix = "Resources\\Textures\\skybox\\";
	Engine::TextureLoader* textureloader2 = new Engine::TextureLoader(3, path_prefix + "right.tga", path_prefix + "left.tga",
		path_prefix + "top.tga", path_prefix + "bottom.tga", path_prefix + "front.tga", path_prefix + "back.tga");
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	Engine::TextureLoader* textureloader3 = new Engine::TextureLoader(4, "Resources\\Textures\\NormalMap.png");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	Engine::TextureLoader* textureloader4 = new Engine::TextureLoader(5, "Resources\\Textures\\Wood.jpg");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	wood_unit = textureloader4->GetUnit();

	cubemap_material = new CubeMaterial();
	cubemap_material->CreateMaterial(textureloader2->GetUnit());

	cubemap_object = new Engine::RenderObject(cube_mesh, cubemap_material);
	cubemap_material->AddNormalMap(textureloader3->GetUnit());
	
	normal_material = new NormalMaterial();
	normal_material->CreateMaterial(textureloader3->GetUnit());
	

	material = new DiffuseMaterial();
	bunny_unit = textureloader1->GetUnit();
	material->CreateMaterial(bunny_unit);

	shadow_material = new ShadowMaterial();
	shadow_material->CreateMaterial();

	bunny_object = new Engine::RenderObject(bunny_mesh, material);
	cube_object = new Engine::RenderObject(cube_mesh, normal_material);
	ground_object = new Engine::RenderObject(ground_mesh, material);
	mirror_object1 = new Engine::RenderObject(mirror_mesh1, material);

	Engine::Transform* bunny_T = bunny_object->GetTransform();
	bunny_T->SetPosition(glm::vec3(-1.5f, 0.5f, 0.0f));
	bunny_object->SetTransform(*bunny_T);

	Engine::Transform* cube_T = cube_object->GetTransform();
	cube_T->SetPosition(glm::vec3(1.5f, 0.8f, 0.0f));
	cube_T->SetScale(glm::vec3(0.5f));
	cube_object->SetTransform(*cube_T);

	Engine::Transform* mirror_T = mirror_object1->GetTransform();
	mirror_T->SetPosition(glm::vec3(0, 1, 0.5));
	mirror_T->SetScale(glm::vec3(0.5f, 0.5f, 0.001f));
	//mirror_T->SetOrientation(glm::rotate(glm::mat4(1), glm::radians(-90.f), X_AXIS));
	mirror_object1->SetTransform(*mirror_T);

	Light sun = Light();
	sun.type = DirectionalLight; //directional light
	sun.transform.SetPosition(glm::vec3(0.f, 0.0f, 0.0f));
	sun.transform.LookAt(glm::vec3(-1.f, -1.f , -1.f), glm::vec3(0.0f, 1.0f, 0.0f));
	sun.LightProj = ComputeLightProj(&sun);
	sun.illu_diff = glm::vec3(1.0f, 1.0f, 1.0f);
	sun.illu_amb = glm::vec3(0.1f);
	sun.illu_spec = glm::vec3(0.2f);
	sun.angle = glm::radians(10.f);
	sun.exp = 512.f;
	sun.enabled = 1;
	sun.haveShadowMap = 1;
	lights.push_back(sun);


	cubeProps = {
		glm::vec3(0.1f),
		glm::vec3(0.5f, 0.3f, 0.1f),
		glm::vec3(0.2f),
		512,
		0.5f
	};

	normal_material->UpdateReflectanceProperties(cubeProps);


	ground_object->GetTransform()->SetPosition(glm::vec3(0, 0, 0));
	ground_object->GetTransform()->SetScale(glm::vec3(4, 1, 4));

}


static void InitAnimation() {
	/* Rotate light source in circular path*/
	Animation* sunA = new Animation(&lights[0].transform, 5, AnimationType::All);
	for (float phi = -180.f;  phi <= 180.f; phi += 5) {
		Engine::Transform sunT;
		float rad = glm::radians(phi);
		float cos = glm::cos(rad), sin = glm::sin(rad);
		sunT.SetPosition(glm::vec3(3.f * cos, 2, 4.f * sin));
		sunT.LookAt(-3.f * glm::vec3(cos, 0.5f, sin), Y_AXIS);
		sunA->AddKeyframe((180 + phi) / 72.f, sunT);
	}
	lights[0].animation = sunA;
}

static void RenderScene(Engine::Camera* cam) {

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
	g_window = glfwCreateWindow(g_window_width, g_window_height, "HW4", NULL, NULL);
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

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP);

	glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetMouseButtonCallback(g_window, MouseButtonCallback);
	glfwSetCursorPosCallback(g_window, CursorPosCallback);
	glfwSetKeyCallback(g_window, KeyboardCallback);
	glfwSetFramebufferSizeCallback(g_window, FramebufferResizeCallback);

	// Initialize framebuffer object and picking textures
	pickingInitialize(g_framebuffer_width, g_framebuffer_height, 0);
	shadowInitialize(g_framebuffer_width, g_framebuffer_height, 1);
	InitScene();
	InitAnimation();


	float prev_time = (float)glfwGetTime();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(g_window) && glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		float total_time = (float)glfwGetTime();
		float elapsed_time = total_time - prev_time;
		prev_time = total_time;

		// First Pass: Object Selection
		// this is for picking the object using mouse interaction
		// binding framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, picking_fbo);
		// Background: RGB = 0x000000 => objectID: 0
		glClearColor((GLclampf) 0.0f, (GLclampf) 0.0f, (GLclampf) 0.0f, (GLclampf) 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/* RenderPicking here */


		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// TODO
		// Second Pass: Shadow Mapping (See the first pass and picking.hpp to understand how you implement render to texture)

		//send projection information to shader
		lights[0].animation->Animate(elapsed_time);
		glm::mat4 lightSpaceMatrix = ComputeLightProj(&lights[0]);
		shadow_material->LightProj(lightSpaceMatrix);
		glm::vec3 a = lights[0].transform.GetPosition();
		
		glCullFace(GL_FRONT);
		glViewport(0, 0, g_window_width, g_window_height);
		// Render to shadow_fbo
		glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor((GLclampf) 0.0f, (GLclampf) 0.0f, (GLclampf) 0.0f, (GLclampf) 0.0f);
		glClear(GL_DEPTH_BUFFER_BIT);

		bunny_object->SetMaterial(shadow_material);
		ground_object->SetMaterial(shadow_material);
		mirror_object1->SetMaterial(shadow_material);
		cube_object->SetMaterial(shadow_material);
		cube_object->Render(main_camera);
		mirror_object1->Render(main_camera);
		bunny_object->Render(main_camera);
		ground_object->Render(main_camera);
		glCullFace(GL_BACK);
		// Third Pass: Object Rendering
		// Drawing object again

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor((GLclampf) 0.0f, (GLclampf) 0.0f, (GLclampf) 0.0f, (GLclampf) 0.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		

		/* Change back */
		cube_object->SetMaterial(cubemap_material);
		ground_object->SetMaterial(material);
		bunny_object->SetMaterial(material);
		mirror_object1->SetMaterial(material);
		material->UpdateLight(lights);
		cubemap_material->UpdateLight(lights);

		normal_material->UpdateLight(lights);
		
		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
		cubemap_material->IsSkybox(1);
		cubemap_object->Render(main_camera);
		glEnable(GL_CULL_FACE);
		glDepthMask(GL_TRUE);
		
		material->LightProjection(lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadow_tex);
		material->BindShadowMap(1);

		material->UpdateDiffuseReflectance(glm::vec3(0.9f, 0.9f, 0.9f));
		material->UpdateOurTexture(bunny_unit);
		bunny_object->Render(main_camera);
		material->DisableTexture();
		ground_object->Render(main_camera);
		material->UpdateOurTexture(wood_unit);
		mirror_object1->Render(main_camera);

		cubemap_material->IsSkybox(0);
		cube_object->Render(main_camera);

		glBindTexture(GL_TEXTURE_2D, 0);

		/* Swap front and back buffers */
		glfwSwapBuffers(g_window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	// Delete resources
	delete main_camera;
	deletePickingResources();
	deleteShadowResources();
	glfwTerminate();
	return 0;

}
#pragma once

#include <vector>
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Mesh.hpp>

class Geometry
{
private:

protected:
	Geometry *parent;
	std::vector<Geometry *> children;
public:
	Geometry();
	~Geometry();
	void GenerateCube(Engine::Mesh* mesh);
	void Quad(Engine::Mesh* mesh, glm::vec4* t_vertices,int a_a, int a_b, int a_c, int a_d);
	glm::vec3 ComputeNormal(glm::vec4 a_a, glm::vec4 a_b, glm::vec4 a_c);
	void GenerateTriangleMesh(Engine::Mesh* mesh);
	void GenerateLine(Engine::Mesh* mesh);
	void GenerateStar(Engine::Mesh* mesh);
	void generateSphere(Engine::Mesh *mesh, float angle);
	void generateCylinder(Engine::Mesh* mesh);
	void GenerateCubeWithTranslation(Engine::Mesh* mesh, glm::vec3 vec);
	void GenerateCone(Engine::Mesh* mesh);
	void GenerateCircleWithHole(Engine::Mesh *mesh, float r);
	void AddChild(Geometry *geometry);
	void GeneratePyramid(Engine::Mesh *mesh);
};


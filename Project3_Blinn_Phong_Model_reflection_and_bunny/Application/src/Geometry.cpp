#include <Geometry.hpp>
#include <stdlib.h>

Geometry::Geometry()
{
}


Geometry::~Geometry()
{
}

glm::vec4 vertices[8] = {
    glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
    glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
    glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
    glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
    glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),
    glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
    glm::vec4(1.0f, -1.0f, -1.0f, 1.0f)
};

glm::vec3 Geometry::ComputeNormal(glm::vec4 a_a, glm::vec4 a_b, glm::vec4 a_c)
{
    glm::vec3 normal = glm::normalize(glm::cross(a_b.xyz() - a_a.xyz(), a_c.xyz() - a_a.xyz()));
    return normal;
}

// TODO: Fill up Geometry::Quad (Slide No. 9)
void Geometry::Quad(Engine::Mesh* mesh, int a_a, int a_b, int a_c, int a_d)
{
    /* a -- d
     * |    |
     * b -- c
     */
    // Compute normal for quad
    glm::vec4 n = glm::vec4(ComputeNormal(vertices[a_a], vertices[a_b], vertices[a_c]), 0.0f);
    // Triangle 1, Vertex a
    mesh->AddVertexData(vertices[a_a]);
    mesh->AddVertexData(n);
    // Triangle 1, Vertex b
    mesh->AddVertexData(vertices[a_b]);
    mesh->AddVertexData(n);
    // Triangle 1, Vertex c
    mesh->AddVertexData(vertices[a_c]);
    mesh->AddVertexData(n);
    // Triangle 2, Vertex a
    mesh->AddVertexData(vertices[a_a]);
    mesh->AddVertexData(n);
    // Triangle 2, Vertex c
    mesh->AddVertexData(vertices[a_c]);
    mesh->AddVertexData(n);
    // Triangle 2, Vertex d
    mesh->AddVertexData(vertices[a_d]);
    mesh->AddVertexData(n);
}

// TODO: Fill up Geometry::GenerateCube (Slide No. 8)
void Geometry::GenerateCube(Engine::Mesh* mesh)
{
    /*   5 -- 6
     *  /|   /|
     * 1(4)-2 7
     * |    |/
     * 0 -- 3
     */
    mesh->AddAttribute(4); // for position
    mesh->AddAttribute(4); // for normal

    // Add four quad for generating a cube
    Quad(mesh, 1, 0, 3, 2);
    Quad(mesh, 2, 3, 7, 6);
    Quad(mesh, 3, 0, 4, 7);
    Quad(mesh, 6, 5, 1, 2);
    Quad(mesh, 4, 5, 6, 7);
    Quad(mesh, 5, 4, 0, 1);

    mesh->SetNumElements(36);
    mesh->CreateMesh();
    return;
}

void Geometry::GenerateTriangleMesh(Engine::Mesh* mesh)
{
    mesh->AddAttribute(4);

    glm::vec3 position_data[] = {
        glm::vec3(-1.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, 0.0f),
        glm::vec3(0.0f,1.0f, 0.0f)
    };

    for (int i = 0; i < 3; i++)
    {
        mesh->AddVertexData(position_data[i]);
        mesh->AddVertexData(1.0f);

    }
    mesh->SetNumElements(3);
    mesh->CreateMesh();
    return;
}

void Geometry::GenerateLine(Engine::Mesh* mesh)
{
    mesh->SetDrawMode(GL_LINES);
    mesh->AddAttribute(4);
    mesh->AddVertexData(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    mesh->AddVertexData(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    mesh->SetNumElements(2);
    mesh->CreateMesh();
    return;
}


void Geometry::GenerateCircle(Engine::Mesh* mesh) {
	mesh->AddAttribute(4);
	mesh->AddAttribute(4);
	int ele = 0;
	float delta = 5.0f;
	for (float phi = -180; phi < 180; phi += delta) {
		float phiR = glm::radians(phi);
		float dPhiR = glm::radians(phi + delta);
		glm::vec2 data[] = {
			glm::vec2(0, 0),
			glm::vec2(glm::cos(phiR), glm::sin(phiR)),
			glm::vec2(glm::cos(dPhiR), glm::sin(dPhiR))
		};
		for (int i = 0; i < 3; i++) {
			mesh->AddVertexData(glm::vec4(data[i], 0, 1));
			mesh->AddVertexData(glm::vec4(0, 0, 1, 0));
		}
		ele += 3;
	}
	mesh->SetNumElements(ele);
	mesh->CreateMesh();
}

void Geometry::GenerateSphere(Engine::Mesh *mesh, float angle) {
	/*
		Partition sphere in small pieces and fill with triangles
		region is between phi and phid angle
		create 2 vertices at same level and go up result
	   3|-------|4
		| -		|
		|	-	|
		|	 -	|
		|	  -	|
	   1|-------|2
	*/
	mesh->SetDrawMode(GL_TRIANGLE_STRIP);
	mesh->AddAttribute(4);
	mesh->AddAttribute(4);
	float toRad = glm::pi<float>() / 180.0f;
	int k = 0;
	for (float phi = -89.9f; phi <= 89.9f; phi += angle) { // vary altitude
		float phir = phi * toRad;
		float phirD = (phi + angle) * toRad;

		for (float theta = -180.0f; theta <= 180.0f; theta += angle) { // vary longtitude
			float thetar = theta * toRad;
			glm::vec3 v1 = glm::vec3(sin(thetar)  * cos(phir), cos(thetar) * cos(phir), sin(phir));
			glm::vec3 v2 = glm::vec3(sin(thetar)  * cos(phirD), cos(thetar) * cos(phirD), sin(phirD));
			mesh->AddVertexData(glm::vec4(v1, 1.0f));
			mesh->AddVertexData(glm::vec4(v1, 0.0f));
			mesh->AddVertexData(glm::vec4(v2, 1.0f));
			mesh->AddVertexData(glm::vec4(v2, 0.0f));
			k += 2;
		}
	}
	mesh->SetNumElements(k);
	mesh->CreateMesh();
}
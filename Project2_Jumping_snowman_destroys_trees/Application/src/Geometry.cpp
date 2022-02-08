#include <Geometry.hpp>
#include <stdlib.h>
#include <tuple>
#include <queue>
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
void Geometry::Quad(Engine::Mesh* mesh, glm::vec4* t_vertices, int a_a, int a_b, int a_c, int a_d)
{
	/* a -- d
	 * |    |
	 * b -- c
	 */
	glm::vec4 normal = glm::vec4(ComputeNormal(t_vertices[a_a], t_vertices[a_b], t_vertices[a_c]), 0.0f);

	mesh->AddVertexData(t_vertices[a_a]);
	mesh->AddVertexData(normal);
	mesh->AddVertexData(t_vertices[a_b]);
	mesh->AddVertexData(normal);
	mesh->AddVertexData(t_vertices[a_c]);
	mesh->AddVertexData(normal);
	mesh->AddVertexData(t_vertices[a_a]);
	mesh->AddVertexData(normal);
	mesh->AddVertexData(t_vertices[a_c]);
	mesh->AddVertexData(normal);
	mesh->AddVertexData(t_vertices[a_d]);
	mesh->AddVertexData(normal);

	return;
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
    Quad(mesh, vertices, 1, 0, 3, 2);
    Quad(mesh, vertices, 2, 3, 7, 6);
    Quad(mesh, vertices, 3, 0, 4, 7);
    Quad(mesh, vertices, 6, 5, 1, 2);
    Quad(mesh, vertices, 4, 5, 6, 7);
    Quad(mesh, vertices, 5, 4, 0, 1);

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

void Geometry::GenerateCubeWithTranslation(Engine::Mesh* mesh, glm::vec3 vec) {

	// multiply old verices by translation matrix
	glm::vec4 newPos[8];
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), vec);
	for (int i = 0; i < 8; i++)
		newPos[i] = translate * vertices[i];


	// same as in cube
	mesh->AddAttribute(4);
	mesh->AddAttribute(4);

	Quad(mesh, newPos, 1, 0, 3, 2);
	Quad(mesh, newPos, 2, 3, 7, 6);
	Quad(mesh, newPos, 3, 0, 4, 7);
	Quad(mesh, newPos, 6, 5, 1, 2);
	Quad(mesh, newPos, 4, 5, 6, 7);
	Quad(mesh, newPos, 5, 4, 0, 1);

	mesh->SetNumElements(36);
	mesh->CreateMesh();
	return;
}

void Geometry::GenerateCone(Engine::Mesh* mesh) {
	/*
		partition into triangle strips and fill with traingle
	*/
	float r = 1.0f, h = 1.0f;
	mesh->SetDrawMode(GL_TRIANGLE_STRIP);
	mesh->AddAttribute(4);
	mesh->AddAttribute(4);
	float delta = -5.0f;
	float toRad = glm::pi<float>() / 180;
	int elem = 0;
	for (float phi = 180.0f; phi >= -180.0f; phi += delta) { // vary longtitude
		float phiD = phi + delta;
		for (float R = r; R > 0; R -= 0.1f) { // vary height
			glm::vec4 one = glm::vec4(R * cos(phi * toRad), h / r * (r - R), R * sin(phi * toRad), 1.0f);
			glm::vec4 two = glm::vec4(R * cos((phi + delta) * toRad), h / r * (r - R), R * sin((phi + delta) * toRad), 1.0f);
			glm::vec4 next = glm::vec4((R - 0.1f) * cos(phi * toRad), h / r * (r - R + 0.1f), (R - 0.1f) * sin(phi * toRad), 1.0f);
			glm::vec3 normal = ComputeNormal(one, two, next);
			mesh->AddVertexData(one);
			mesh->AddVertexData(glm::vec4(normal, 0.0f));
			mesh->AddVertexData(two);
			mesh->AddVertexData(glm::vec4(normal, 0.0f));
			elem += 2;
		}
		glm::vec4 top = glm::vec4(0, h, 0, 1.0f);
		glm::vec4 prev1 = glm::vec4(0.1 * cos(phi * toRad), h / r * (r - 0.1), 0.1 * sin(phi * toRad), 1.0f);
		glm::vec4 prev2 = glm::vec4(0.1 * cos((phi + delta) * toRad), h / r * (r - 0.1), 0.1 * sin((phi + delta) * toRad), 1.0f);
		glm::vec3 n = ComputeNormal(prev1, prev2, top);
		mesh->AddVertexData(top);
		mesh->AddVertexData(glm::vec4(n, 0.0f));
		elem++;
	}
	mesh->SetNumElements(elem);
	mesh->CreateMesh();
}


void Geometry::GenerateStar(Engine::Mesh* mesh)
{
		/*  This funtcion creates triangle and from base triangle grow fractal using while loop
		Information on sides of triangle is pushed into loop via Queue
		Instruction for constracting is as follow:
		Take side AB and choose C,D which trisect AB, so that AC = CD = DB
		Now Rotate CD by 60 degree to get point E, so that DCE is an equletrial triangle
		push AC, CE, ED, DB in Queue so that they will continue growing
	*/
	mesh->AddAttribute(4);
	mesh->AddAttribute(4);
	// Create queue which describes sides of triangle and Matrix R60 which rotates vector by 60 degree CCW
	// 3rd integer in tuple is responsible for terminating condition of future while loop
	// it represents step (generation) of fractal growth
	std::queue<std::tuple<glm::vec3, glm::vec3, int>> myQ;
	glm::mat3x3 R60 = {
		{0.5f, 0.866f, 0.0f},
		{-0.866f, 0.5f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	};

	// Initialize Base triangle
	glm::vec3 a1 = glm::vec3(1.0f, -1.0f, 0.0f);
	glm::vec3 a2 = glm::vec3(-1.0f, -1.0f, 0.0f);
	// To get equaletrial trinagle we need to rotate by 60
	glm::vec3 a3 = R60 * (a1 - a2) + a2;
	glm::vec3 a[] = { a1, a2, a3 };

	for (int i = 0; i < 3; i++) {
		myQ.push(std::make_tuple(a[i], a[(i + 1) % 3], 0));  // 
		mesh->AddVertexData(a[2 - i]);
		mesh->AddVertexData(1.0f);
		mesh->AddVertexData(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
	}

	//Keep track of all points
	int num_elem = 3;

	while (!myQ.empty()) {
		// Get tuple and if it's generation is more than 6 ignore
		std::tuple<glm::vec3, glm::vec3, int> seg = myQ.front();
		myQ.pop();
		int generation = std::get<2>(seg);
		if (generation >= 6) { continue; }

		// Else we start constructing new small triangle
		// Get 3 points C,D,E and Push them into mesh
		glm::vec3 temp1 = (2.0f * std::get<0>(seg) + std::get<1>(seg)) / 3.0f;
		glm::vec3 temp2 = (std::get<0>(seg) + 2.0f * std::get<1>(seg)) / 3.0f;
		glm::vec3 temp3 = R60 * (temp2 - temp1) + temp1;
		glm::vec3 temp[] = { temp1, temp2, temp3 };
		for (int i = 0; i < 3; i++) {
			mesh->AddVertexData(temp[i]);
			mesh->AddVertexData(1.0f);
			mesh->AddVertexData(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

		}

		// Push new sides into Q, with and increment generation
		// update num of points
		myQ.push(std::make_tuple(std::get<0>(seg), temp1, generation + 1));
		myQ.push(std::make_tuple(temp1, temp3, generation + 1));
		myQ.push(std::make_tuple(temp3, temp2, generation + 1));
		myQ.push(std::make_tuple(temp2, std::get<1>(seg), generation + 1));
		num_elem += 3;
	}
	mesh->SetNumElements(num_elem);
	mesh->CreateMesh();
	return;
}

void Geometry::generateSphere(Engine::Mesh *mesh, float angle) {
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

void Geometry::generateCylinder(Engine::Mesh *mesh) {
	float delta = 5.0f, height = 1.0f;
	int elem = 0;
	float toRad = glm::pi<float>() / 180.0f;
	mesh->SetDrawMode(GL_TRIANGLE_STRIP);
	mesh->AddAttribute(4);
	mesh->AddAttribute(4);
	for (float phi = -180.0f; phi <= 180.0f; phi += delta) {
		//create rectangle 
		float phir = phi * toRad, phirD = (phi + delta) * toRad;

		glm::vec4 face[] = {
			glm::vec4(glm::cos(phir), 0.0f, glm::sin(phir), 1.0f),
			glm::vec4(glm::cos(phirD), 0.0f, glm::sin(phirD), 1.0f),
			glm::vec4(glm::cos(phir), 1.0f, glm::sin(phir), 1.0f),
			glm::vec4(glm::cos(phirD), 1.0f, glm::sin(phirD), 1.0f)

		};
		glm::vec3 n = ComputeNormal(face[2], face[1], face[0]);

		for (int i = 0; i < 4; i++) {
			mesh->AddVertexData(face[i]);
			mesh->AddVertexData(glm::vec4(n, 0.0f));
		}
		elem += 4;
	}
	mesh->SetNumElements(elem);
	mesh->CreateMesh();
}

void Geometry::GenerateCircleWithHole(Engine::Mesh *mesh, float r) {
	float delta = 5.0f, toRad = glm::pi<float>() / 180.0f;
	int elem = 0;
	mesh->SetDrawMode(GL_TRIANGLE_STRIP);
	mesh->AddAttribute(4);
	mesh->AddAttribute(4);
	for (float phi = -180.0f; phi <= 180.0f; phi += delta) {
		// following polar coordinates constract circle
		float phir = phi * toRad, phirD = (phi + delta) * toRad;
		for (float R = 1; R >= r ; R -= 0.05f) {
			glm::vec4 a = glm::vec4(R * glm::cos(phir), 0, R * glm::sin(phir), 1.0f);
			glm::vec4 b = glm::vec4(R * glm::cos(phirD), 0, R * glm::sin(phirD), 1.0f);
			mesh->AddVertexData(a);
			mesh->AddVertexData(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			mesh->AddVertexData(b);
			mesh->AddVertexData(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			elem += 2;
		}
	}
	mesh->SetNumElements(elem);
	mesh->CreateMesh();
}

void Geometry::GeneratePyramid(Engine::Mesh* mesh) {
	//simple 6 faces 
	mesh->AddAttribute(4);
	mesh->AddAttribute(4);
	glm::vec4 pyramid[] = {
		glm::vec4(-1.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(-1.0f, 0.0f, -1.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, -1.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
	};

	Quad(mesh, pyramid, 0, 1, 2, 3);
	for (int i = 0; i < 4; i++) {
		glm::vec4 fir = pyramid[i], scnd = pyramid[(i + 1) % 4];
		glm::vec3 n = ComputeNormal(fir, pyramid[4], scnd);
		mesh->AddVertexData(fir);
		mesh->AddVertexData(glm::vec4(n, 0.0f));
		mesh->AddVertexData(pyramid[4]);
		mesh->AddVertexData(glm::vec4(n, 0.0f));
		mesh->AddVertexData(scnd);
		mesh->AddVertexData(glm::vec4(n, 0.0f));
	}
	
	mesh->SetNumElements(18);
	mesh->CreateMesh();
}
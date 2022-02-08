#include <Fractal.hpp>

/*
 * Implement Fractal class to generate Koch snowflakes mesh (generate snowflake mesh using triangle list).
 * Koch snowflake: https://en.wikipedia.org/wiki/Koch_snowflake
 */

Fractal::Fractal()
{

}

Fractal::~Fractal()
{

}

/*
 * TODO: Implement koch snowflake mesh generator
 */
void Fractal::GenerateSnowflake(Engine::Mesh* mesh)
{
	/*  This funtcion creates triangle and from base triangle grow fractal using while loop
		Information on sides of triangle is pushed into loop via Queue
		Instruction for constracting is as follow: 
		Take side AB and choose C,D which trisect AB, so that AC = CD = DB
		Now Rotate CD by 60 degree to get point E, so that DCE is an equletrial triangle
		push AC, CE, ED, DB in Queue so that they will continue growing
	*/
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
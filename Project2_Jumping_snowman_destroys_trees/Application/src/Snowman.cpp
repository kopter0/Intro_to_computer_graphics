#include <Snowman.hpp>

Snowman::Snowman(float y_comp) : m_sphere_mesh(new Engine::Mesh()), m_defmat(new DefaultMaterial()), m_pick(new PickingMaterial())
{
	Geometry geo = Geometry();
	geo.generateSphere(m_sphere_mesh, 5.0f);
	m_defmat->CreateMaterial();
	m_defmat->UpdateColor(glm::vec4(1.0f));
	m_pick->CreateMaterial();

	Y_COMP[0] = y_comp;
	Y_COMP[1] = 1.2f;
	Y_COMP[2] = 1.0f;

	/*Main body of snowman, create 3 spheres and put them on top of each other by means of AddParent*/
	m_torso = new PickableObject(m_sphere_mesh, m_defmat);
	m_torso->SetPickingMat(m_pick);
	m_torso->SetIndex(m_cur_idx++);
	mainBody.push_back(m_torso);

	m_body = new PickableObject(m_sphere_mesh, m_defmat);
	m_body->SetScale(glm::vec3(0.75f));
	m_body->SetPickingMat(m_pick);
	m_body->SetIndex(m_cur_idx++);
	m_body->AddParent(m_torso);
	mainBody.push_back(m_body);


	m_head = new PickableObject(m_sphere_mesh, m_defmat);
	m_head->SetScale(glm::vec3(0.5f));
	m_head->SetPickingMat(m_pick);
	m_head->SetIndex(m_cur_idx++);
	m_head->AddParent(m_body);
	mainBody.push_back(m_head);
	allign();

	//update_state();
	//____________________________________________________________________________________//

	Engine::Mesh* cube_mesh = new Engine::Mesh();
	geo.GenerateCube(cube_mesh);

	/*	Create pair of hands: arms, forearms and 2 fingers for each arm
		finger is composed of objects so that it can bend at the middle
		arm is attcahed to body
		forearm is attached to second end of arm
		figner1 attached to forearm 
		finger2 attached to 2nd end of finger1
	*/
	for (int i = 0; i < 2; i++) {
		float sign = (i == 0) ? 1.0f : -1.0f;
		Engine::Mesh* cube_side_mesh = new Engine::Mesh();
		geo.GenerateCubeWithTranslation(cube_side_mesh, sign * X_AXIS); //cube with origin at its side, it is easier to rotate using such mesh
		
		PickableObject* arm = new PickableObject(cube_side_mesh, m_defmat);
		arm->SetScale(glm::vec3(0.35f, 0.05f, 0.025f));
		arm->AddParent(m_body);
		arm->SetPosition(sign * 0.75f * X_AXIS);
		arm->SetOrientation(glm::rotate(glm::mat4(1.0f), sign * 10.0f * toRad, Z_AXIS));
		hands.push_back(arm);

		PickableObject* forearm = new PickableObject(cube_side_mesh, m_defmat);
		forearm->SetScale(glm::vec3(0.4f, 0.05f, 0.025f));
		forearm->AddParent(arm);
		forearm->SetPosition(sign * glm::vec3(0.67f, 0.0f, 0.0f));
		forearm->SetOrientation(glm::rotate(glm::mat4(1.0f), sign * 30.0f * toRad, Z_AXIS));
		hands.push_back(forearm);

		for (int j = 0; j < 2; j++) {
			float signj = j ? 1.0f : -1.0f;

			PickableObject* fing = new PickableObject(cube_side_mesh, m_defmat);
			fing->AddParent(forearm);
			fing->SetScale(glm::vec3(0.125f, 0.025f, 0.025f));
			fing->SetPosition(glm::vec3(sign * 0.5f, signj * 0.04f, 0.0f));
			fing->SetOrientation(glm::rotate(glm::mat4(1.0f), signj * sign * 20.0f * toRad, Z_AXIS));
			hands.push_back(fing);

			PickableObject* fing_c = new PickableObject(cube_side_mesh, m_defmat);
			fing_c->AddParent(fing);
			fing_c->SetScale(glm::vec3(0.125f, 0.025f, 0.025f));
			fing_c->SetPosition(glm::vec3(sign * 0.25f, 0.0f, 0.0f));
			hands.push_back(fing_c);
		}
	}

	for (PickableObject* ob : hands) {
		glm::vec3 pos = ob->GetPosition();
		glm::mat4 or = ob->GetOrientation();
		init_hands.push_back(std::make_pair(pos, or ));
	}

	//_________________________________________________________________________________________________________________//
	
	
	// Create face: pair of spheres for eyes, rescaled cube for eyebrow, cone for nose and hat is cylinder + 2 circles
	for (int i = 0; i < 2; i++) { //eyes
		int sign = (i == 0) ? 1 : -1;
		PickableObject*	eye = new PickableObject(m_sphere_mesh, m_defmat);
		eye->SetScale(glm::vec3(0.1f));
		eye->AddParent(m_head);
		eye->SetPosition(glm::vec3(sign * 0.2f, 0.15f, 0.4f));
		face.push_back(eye);
	}

	for (int i = 0; i < 2; i++) {	// eyebrow
		int sign = (i % 2) ? 1 : -1;
		Engine::Mesh* eb_mesh = new Engine::Mesh();
		geo.GenerateCubeWithTranslation(eb_mesh, (float)sign * X_AXIS);
		PickableObject* eyebrow = new PickableObject(eb_mesh, m_defmat);
		eyebrow->AddParent(face[i % 2]);
		eyebrow->SetScale(glm::vec3(0.09f, 0.035f, 0.02f));
		eyebrow->SetPosition(glm::vec3(-sign * 0.06f, 0.15f, 0.06f));
		face.push_back(eyebrow);
	}

	Engine::Mesh* cone_mesh = new Engine::Mesh();
	Geometry geomm = Geometry();
	geomm.GenerateCone(cone_mesh);
	PickableObject* nose = new PickableObject(cone_mesh, m_defmat);
	nose->SetOrientation(glm::rotate(glm::mat4(1.0f), 90.0f * toRad, X_AXIS));
	nose->AddParent(m_head);
	nose->SetScale(glm::vec3(0.15f, 0.7f, 0.15f));
	nose->SetPosition(glm::vec3(0.0f, -0.05f, 0.4f));
	
	face.push_back(nose);

	Engine::Mesh* cyl_mesh = new Engine::Mesh();
	geo.generateCylinder(cyl_mesh);
	PickableObject* cyl = new PickableObject(cyl_mesh, m_defmat);
	cyl->AddParent(m_head);
	cyl->SetPosition(glm::vec3(0.0f, 0.4f, 0.0f));
	cyl->SetScale(glm::vec3(0.3f, 0.7f, 0.3f));
	hat.push_back(cyl);

	Engine::Mesh* circle_m = new Engine::Mesh();
	geo.GenerateCircleWithHole(circle_m, 0);
	PickableObject* circle = new PickableObject(circle_m, m_defmat);
	circle->AddParent(cyl);
	circle->SetScale(glm::vec3(0.3f));
	circle->SetPosition(0.65f * Y_AXIS);
	hat.push_back(circle);

	Engine::Mesh* circle_m_1 = new Engine::Mesh();
	geo.GenerateCircleWithHole(circle_m_1, 0.25f);
	PickableObject* l_circle = new PickableObject(circle_m_1, m_defmat);
	l_circle->AddParent(cyl);
	hat.push_back(l_circle);


}

Snowman::~Snowman()
{

}

/* not used
void Snowman::update_state() {
	BODY_STATE.emplace(BODY_STATE.begin(), std::make_pair(m_torso->GetPosition(), m_torso->GetOrientation()));
	BODY_STATE.emplace(BODY_STATE.begin() + 1, std::make_pair(m_body->GetPosition(), m_body->GetOrientation()));
	BODY_STATE.emplace(BODY_STATE.begin() + 2, std::make_pair(m_head->GetPosition(), m_head->GetOrientation()));
}
*/

/*
	allign height of each main body part
*/
void Snowman::allign() {
	for (int i = 0; i < 3; i++) {
		PickableObject *cur = mainBody[i];
		glm::vec3 pos = cur->GetPosition();
		glm::vec3 new_pos = glm::vec3(pos[0], Y_COMP[i], pos[2]);
		cur->SetPosition(new_pos);
	}
}

void Snowman::PickingRender(Engine::Camera *cam) {
	m_body->RenderPicking(cam);
}


/*
	Simple rendering and updating color for each part
*/
void Snowman::Render(Engine::Camera *cam) {

	m_defmat->UpdateColor(glm::vec4(1.0f));
	for (PickableObject* ob : mainBody) {
		ob->Render(cam);
	}

	m_defmat->UpdateColor(glm::vec4(133, 60, 8, 255) / 255.0f);
	for (PickableObject* ob : hands) {
		ob->Render(cam);
	}

	m_defmat->UpdateColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	int count = 0;
	for (PickableObject* ob : face) {
		if (count == 4)
			break;
		ob->Render(cam);
		count++;
	}
	m_defmat->UpdateColor(glm::vec4(179, 105, 15, 255) / 255.0f);
	face[4]->Render(cam);

	m_defmat->UpdateColor(glm::vec4(0.0f, 0.0f, 0.75f, 1.0f));
	for (PickableObject* ob : hat) {
		ob->Render(cam);
	}
}


/*
	animation responsible for moving snowman 
*/
bool Snowman::jump(float deltaTime, int ddir) {
	// initialize some useful values
	float alpha = m_action_counter * toRad, threshold = 0.4f, mult = 4.0f;
	glm::vec3 cur_torso = m_torso->GetPosition();
	glm::vec3 torso_scale = m_torso->GetScale();
	glm::vec3 cur_body = m_body->GetPosition();
	glm::vec3 cur_head = m_head->GetPosition();
	float slow = 150.0f;

	// As explained before, I have 3 steps for this function
	switch (jumping_phase) {
	case 0:
		// Contract torso and move down every part of main body
		m_action_counter += deltaTime;
		m_torso->SetScale(torso_scale - deltaTime * Y_AXIS);
		m_torso->SetPosition(cur_torso - deltaTime * Y_AXIS);
		m_body->SetPosition(cur_body - deltaTime * Y_AXIS);

		symmetricHandsRotation(mult * deltaTime, mult * deltaTime);
		clench(1, mult * deltaTime);

		// After threshold second go to next phase
		if (m_action_counter >= threshold) {
			m_action_counter = 0;
			jumping_phase++;
		}
		break;
	case 1:
		// move body upward
		m_action_counter += deltaTime;
		m_torso->SetPosition(cur_torso + 2 * deltaTime * (Y_AXIS + dir[ddir]));
		cur_torso = m_torso->GetPosition();
		
		// revert previous change in body of phase 1
		if (torso_scale[1] < 1)
			m_torso->SetScale(torso_scale + deltaTime * Y_AXIS);
		if (torso_scale[1] > 1)
			m_torso->SetScale(glm::vec3(1.0f));
		m_torso->SetPosition(cur_torso + deltaTime * Y_AXIS);
		m_body->SetPosition(cur_body + deltaTime * Y_AXIS);
		
		// put arms in ititial postion
		symmetricHandsRotation(-deltaTime * mult, -mult * deltaTime);
		clench(-1, mult * deltaTime);

		// make additional upward increment, it is just look nice no particular functionality
		cur_body = m_body->GetPosition();
		cur_head = m_head->GetPosition();
		m_body->SetPosition(cur_body + deltaTime * Y_AXIS);
		m_head->SetPosition(cur_head + deltaTime * Y_AXIS);
		// after treshold seconds go to next
		if (m_action_counter > threshold) {
			m_action_counter = 0;
			jumping_phase++;
		}
		break;
	case 2:

		// now we need to lower body so that it stands on ground
		m_action_counter += deltaTime;
		m_torso->SetPosition(cur_torso - 2.0f * Y_AXIS * deltaTime);
		m_body->SetPosition(cur_body - Y_AXIS * deltaTime);
		m_head->SetPosition(cur_head - Y_AXIS * deltaTime);
		
		// since all deltatimes not the same, there will be small difference between initial and final state
		// so I just put initial heights and orientation 
		if (m_action_counter > threshold) {
			m_action_counter = 0;
			allign();
			correctHands();
			//update_state();
			jumping_phase = 0;
			return false;
		}
		break;
	default:
		std::cout << "Something went wrong with jumping" << std::endl;
		break;
	}
	return true;
}

// apply rotation on both hands for arm and forearm
void Snowman::symmetricHandsRotation(float f_rot, float a_rot) {
	for (int i = 0; i < 2; i++) {
		int sign = (i == 0) ? -1 : 1;
		PickableObject* arm = hands[6 * i];
		PickableObject* forearm = hands[6 * i + 1];

		glm::mat4 arm_or = arm->GetOrientation();
		glm::mat4 forearm_or = forearm->GetOrientation();
		glm::mat4 r_alpha = glm::rotate(glm::mat4(1.0f), sign * f_rot, Z_AXIS);
		glm::mat4 r2_alpha = glm::rotate(glm::mat4(1.0f), -sign * a_rot, Z_AXIS);
		arm->SetOrientation(r_alpha * arm_or);
		forearm->SetOrientation(r2_alpha * forearm_or);
	}
}

/* 
	I have 4 fingers to bend
	2 of them must bend in same direction
	so I take 1 from each hand and orient by given angel
*/
void Snowman::clench(float dir, float angle) {
	for (int i = 0; i < 2; i++) {
		float sign = i ? 1.0f : -1.0f;
		PickableObject* finc1 = hands[6 * ((i + 1) % 2) + 3];
		PickableObject* finc2 = hands[6 * i + 5];

		glm::mat4 finc1_or = finc1->GetOrientation(), finc2_or = finc2->GetOrientation();
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), sign * dir * angle, Z_AXIS);

		finc1->SetOrientation(rot * finc1_or);
		finc2->SetOrientation(rot * finc2_or);
	}
}

// Put back initial orientations
void Snowman::correctHands() {
	for (int i = 0; i < 12; i++) {
		PickableObject* cur = hands[i];
		std::pair<glm::vec3, glm::mat4> state = init_hands[i];
		cur->SetPosition(state.first);
		cur->SetOrientation(state.second);
	}
}


int Snowman::poke(float deltaTime) {
	float threshold = 0.2f;
	float alpha = 1.5f * deltaTime;
	glm::vec3 body_loc = m_body->GetPosition();
	glm::vec3 head_loc = m_head->GetPosition();
	glm::mat4 head_ori = m_head->GetOrientation();
	switch (poking_phase)
	{
	case 0: {
		// fast jerk in z - direction, after threshold seconds goes to next
		glm::mat4 tilt = glm::rotate(glm::mat4(1.0f), alpha, X_AXIS);
		m_body->SetPosition(body_loc - (3 * threshold - m_action_counter) * Z_AXIS / 100.0f);
		m_head->SetOrientation(tilt * head_ori);

		m_action_counter += deltaTime;
		if (m_action_counter > threshold) {
			m_action_counter = 0;
			poking_phase++;
		}
		break;
	}
	case 1:

		//restore initial pos
		glm::mat4 tilt = glm::rotate(glm::mat4(1.0f), -alpha, X_AXIS);
		m_body->SetPosition(body_loc + (3 * threshold - m_action_counter) * Z_AXIS / 100.0f);
		m_head->SetOrientation(tilt * head_ori);

		m_action_counter += deltaTime;
		if (m_action_counter > threshold) {
			m_action_counter = 0;
			poking_phase++;
		}
		break;
	case 2:
		//until angry finishes
		if (angry(deltaTime))
			poking_phase++;
		break;
	case 3:
		poking_phase = 0;
		return -1;
	default:
		break;
	}
	return 2;
}


// animation showing that snowman angry
bool Snowman::angry(float deltaTime) {
	// initialize values
	PickableObject* right_arm = hands[0], *left_forearm = hands[7], *right_forearm = hands[1];
	float fore_angl = angry_angle[FOREARM], arm_angl = angry_angle[ARM];
	int t_mult = (int)glm::floor(m_action_counter / 10);
	float sign = (t_mult % 2) ? -1.0f : 1.0f;
	
	switch (angry_phase)
	{
	case 0:
		// rotate hand to desired place
		angry_rotation(deltaTime, 1);
		break;

	case 1:
		
		// shake fist up and down, sin function used for this
		glm::mat4 r_ori = right_forearm->GetOrientation();
		glm::mat4 rtt = glm::rotate(glm::mat4(1.0f), sign * glm::sin(4 * deltaTime), Z_AXIS);
		right_forearm->SetOrientation(rtt * r_ori);
		m_action_counter += deltaTime / toRad;
		if (m_action_counter > 60)
			angry_phase++;
		break;
	case 2:
		angry_rotation(deltaTime, -1);
		break;
	default:
		angry_phase = 0;
		m_action_counter = 0;
		return true;
		break;
	}
	return false;
}

void Snowman::angry_rotation(float deltaTime, int in_sign) {
	PickableObject* right_arm = hands[0], *left_forearm = hands[7], *right_forearm = hands[1];
	float fore_angl = angry_angle[FOREARM], arm_angl = angry_angle[ARM];
	// rotate till angle written
	if (fore_angl < 140 * toRad) {
		glm::mat4 rtt = glm::rotate(glm::mat4(1.0f), in_sign * 8 * deltaTime, Z_AXIS);
		left_forearm->SetOrientation(rtt * left_forearm->GetOrientation());
		angry_angle.emplace(angry_angle.begin() + 1, fore_angl + 8 * deltaTime);
	}

	if (arm_angl < 60 * toRad) {
		glm::mat4 rtt = glm::rotate(glm::mat4(1.0f), in_sign * 4 * deltaTime, -Y_AXIS);
		right_arm->SetOrientation(rtt * right_arm->GetOrientation());
		angry_angle.emplace(angry_angle.begin(), arm_angl + 4 * deltaTime);
	}

	clench(5.0f * deltaTime, in_sign);
	// Frown
	for (int i = 0; i < 2; i++) {
		int sign = (i % 2) ? -1 : 1;
		PickableObject *eyebrow = face[i + 2];
		glm::mat4 ori = eyebrow->GetOrientation();
		glm::mat4 rtt_e = glm::rotate(glm::mat4(1.0f), in_sign * sign *  arm_angl * toRad / 2, Z_AXIS);
		eyebrow->SetOrientation(rtt_e * ori);
	}
	//End
	if (!(fore_angl < 110 * toRad) && !(arm_angl < 60 * toRad)) {
		angry_phase++;
		angry_angle.emplace(angry_angle.begin() + 1, 0.0f);
		angry_angle.emplace(angry_angle.begin(), 0.0f);
	}
}

glm::vec3 Snowman::getPosition() {
	return m_torso->GetPosition();
}

void Snowman::rotate_body()
{
	body->SetOrientation(glm::rotate(body->GetOrientation(), glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Snowman::rotate_head()
{
	head->SetOrientation(glm::rotate(head->GetOrientation(), glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
}
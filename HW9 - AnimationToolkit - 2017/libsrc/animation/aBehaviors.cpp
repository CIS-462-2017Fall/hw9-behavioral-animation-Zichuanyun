#include "aBehaviors.h"

#include <math.h>
#include "GL/glew.h"
#include "GL/glut.h"

// Base Behavior
///////////////////////////////////////////////////////////////////////////////
Behavior::Behavior()
{
}

Behavior::Behavior( char* name) 
{
	m_name = name;
	m_pTarget = NULL;
}

Behavior::Behavior( Behavior& orig) 
{
	m_name = orig.m_name;
	m_pTarget = NULL;
}

string& Behavior::GetName() 
{
    return m_name;
}

// Behaviors derived from Behavior
//----------------------------------------------------------------------------//
// Seek behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity in world coordinates
// Seek returns a maximum velocity towards the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position


Seek::Seek( AJoint* target) 
{
	m_name = "seek";
	m_pTarget = target;

}

Seek::Seek( Seek& orig) 
{
	m_name = "seek";
	m_pTarget = orig.m_pTarget;
}


Seek::~Seek()
{
}

vec3 Seek::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();

	// TODO: add your code here to compute 
	Vdesired = (targetPos - actorPos).Normalize();
	Vdesired *= 1000.0;

	return Vdesired;
}


// Flee behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity in world coordinates
// Flee calculates a a maximum velocity away from the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position

Flee::Flee( AJoint* target) 
{
	m_name = "flee";
	m_pTarget = target;
}

Flee::Flee( Flee& orig) 
{
	m_name = "flee";
	m_pTarget = orig.m_pTarget;
}

Flee::~Flee()
{
}

vec3 Flee::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();

	// TODO: add your code here to compute Vdesired
	
	Vdesired = (-targetPos + actorPos).Normalize();
	Vdesired *= 1000;
	

	return Vdesired;

}

// Arrival behavior
///////////////////////////////////////////////////////////////////////////////
// Given the actor, return a desired velocity in world coordinates
// Arrival returns a desired velocity vector whose speed is proportional to
// the actors distance from the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position
//  Arrival strength is in BehavioralController::KArrival


Arrival::Arrival( AJoint* target) 
{
	m_name = "arrival";
	m_pTarget = target;
}

Arrival::Arrival( Arrival& orig) 
{
	m_name = "arrival";
	m_pTarget = orig.m_pTarget;
}

Arrival::~Arrival()
{
}

vec3 Arrival::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();

	// TODO: add your code here to compute Vdesired
	Vdesired = targetPos - actorPos;

	return Vdesired;
}


// Departure behavior
///////////////////////////////////////////////////////////////////////////////
// Given the actor, return a desired velocity in world coordinates
// Arrival returns a desired velocity vector whose speed is proportional to
// 1/(actor distance) from the target
// m_pTarget contains target world position
// actor.getPosition() returns Agent's world position
//  Departure strength is in BehavioralController::KDeparture

Departure::Departure(AJoint* target) 
{
	m_name = "departure";
	m_pTarget = target;
}

Departure::Departure( Departure& orig) 
{
	m_name = "departure";
	m_pTarget = orig.m_pTarget;
}

Departure::~Departure()
{
}

vec3 Departure::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();

	// TODO: add your code here to compute Vdesired
	
	double K_dapature = 5 * 10e4;
	vec3 dist = targetPos - actorPos;

	Vdesired = -K_dapature * dist / (dist.Length() * dist.Length());
	
	return Vdesired;
}


// Avoid behavior
///////////////////////////////////////////////////////////////////////////////
//  For the given the actor, return a desired velocity in world coordinates
//  If an actor is near an obstacle, avoid adds a normal response velocity to the 
//  the desired velocity vector computed using arrival
//  Agent bounding sphere radius is in BehavioralController::radius
//  Avoidance parameters are  BehavioralController::TAvoid and BehavioralController::KAvoid

Avoid::Avoid(AJoint* target, vector<Obstacle>* obstacles) 
{
	m_name = "avoid";
	m_pTarget = target;
	mObstacles = obstacles;
}

Avoid::Avoid( Avoid& orig) 
{
	m_name = "avoid";
	m_pTarget = orig.m_pTarget;
	mObstacles = orig.mObstacles;
}

Avoid::~Avoid()
{
}

vec3 Avoid::calcDesiredVel( BehaviorController* actor)
{

	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	m_actorPos = actor->getPosition();
	m_actorVel = actor->getVelocity();

	//TODO: add your code here
	vec3 Varrival(0, 0, 0);
	// Step 1. compute initial value for Vdesired = Varrival so agent moves toward target

	Vdesired = m_pTarget->getLocalTranslation() - m_actorPos;

	vec3 Vavoid(0, 0, 0);

	int rate = 10000;
	vec3 v0 = actor->getVelocity();
	// Step 2. compute Lb
	//TODO: add your code here
	vec3 LB = (v0).Normalize() * rate + actor->getPosition();
	// Step 3. find closest obstacle 
	//TODO: add your code here
	vec3 prob, d, dis;
	bool flag_have = false;
	int K_avoid = 1000;
	double rB = actor->gAgentRadius, r0;
	double dist;
	prob = mObstacles->at(0).m_Center.getLocalTranslation() - actor->getPosition();
	r0 = mObstacles->at(0).m_Radius;
	m_obstaclePos = mObstacles->at(0).m_Center.getLocalTranslation();

	for (auto curObs : *mObstacles) {
		d = curObs.m_Center.getLocalTranslation() - actor->getPosition();
		if (d.Length() < prob.Length()) {
			prob = d;
			r0 = curObs.m_Radius;
			m_obstaclePos = curObs.m_Center.getLocalTranslation();
		}
	}

	vec3 dx = v0.Normalize() * Dot(v0, prob);
	vec3 dy = prob - dx;

	if (dx.Length() <= LB.Length() + r0 + rB) {
		flag_have = dy.Length() <= r0 + rB;
	}

	if (flag_have) {
		vec3 VavoidDir = -dy.Normalize();
		double VavoidMag;
		VavoidMag = K_avoid * ((rB + r0) - dy.Length()) / (rB + r0);
		Vavoid = VavoidDir * VavoidMag;
		return Vdesired + Vavoid;
	}

	else {
		return Vdesired;
	}
	
	return Vdesired;
}

void Avoid::display( BehaviorController* actor)
{
	//  Draw Debug info
	vec3 angle = actor->getOrientation();
	vec3 vel = actor->getVelocity();
	vec3 dir = vec3(cos(angle[1]), 0, sin(angle[1]));
	vec3 probe = dir * (vel.Length()/BehaviorController::gMaxSpeed)*BehaviorController::TAvoid;
	
	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(m_actorPos[0], m_actorPos[1], m_actorPos[2]);
	glVertex3f(m_obstaclePos[0], m_obstaclePos[1], m_obstaclePos[2]);
	glColor3f(0, 1, 1);
	glVertex3f(m_actorPos[0], m_actorPos[1], m_actorPos[2]);
	glVertex3f(m_actorPos[0] + probe[0], m_actorPos[1] + probe[1], m_actorPos[2] + probe[2]);
	glEnd();
}


// Wander Behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity in world coordinates
// Wander returns a desired velocity vector whose direction changes at randomly from frame to frame
// Wander strength is in BehavioralController::KWander

Wander::Wander() 
{
	m_name = "wander";
	m_Wander = vec3(1.0, 0.0, 0.0);
}

Wander::Wander( Wander& orig) 
{
	m_name = "wander";
	m_Wander = orig.m_Wander;
}

Wander::~Wander()
{
}

vec3 Wander::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();

	// compute Vdesired = Vwander

	// Step. 1 find a random direction
	//TODO: add your code here

	int x = rand();
	int y = 0;

	srand(x);
	int z = rand();
	vec3 v = vec3(x % 10 - 5, 0, z % 10 - 5);
	v = v.Normalize();

	double K_norm = 1000;
	v = v * K_norm;

	double K_wander = 10;
	m_Wander = K_wander * ((m_Wander + v).Normalize());
	Vdesired = actor->getVelocity() + m_Wander;

	return Vdesired;
}


// Alignment behavior
///////////////////////////////////////////////////////////////////////////////
// For the given the actor, return a desired velocity vector in world coordinates
// Alignment returns the average velocity of all active agents in the neighborhood
// agents[i] gives the pointer to the ith agent in the environment
// Alignment parameters are in BehavioralController::RNeighborhood and BehavioralController::KAlign


Alignment::Alignment(AJoint* target, vector<AActor>* agents) 
{
	m_name = "alignment";
	m_pAgentList = agents;
	m_pTarget = target;
}



Alignment::Alignment( Alignment& orig) 
{
	m_name = orig.m_name;
	m_pAgentList = orig.m_pAgentList;
	m_pTarget = orig.m_pTarget;

}

Alignment::~Alignment()
{
}

vec3 Alignment::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_pAgentList;
	

	// compute Vdesired 
	
	// Step 1. compute value of Vdesired for fist agent (i.e. m_AgentList[0]) using an arrival behavior so it moves towards the target
	 
	BehaviorController* leader = agentList[0].getBehaviorController(); // first agent is the leader
	//TODO: add your code here

	// Step 2. if not first agent compute Valign as usual
	//TODO: add your code here
	
	if (actor == leader) {
		Vdesired = targetPos - actorPos;
		return Vdesired;
	}

	//made m_AgentList to public at this part
	vector<AActor>* otherAgents = actor->m_AgentList;
	AActor* curActor;
	for (int i = 0; i < otherAgents->size(); i++) {
		curActor = &(otherAgents->at(i));
		Vdesired += curActor->getBehaviorController()->getVelocity();
	}
	return Vdesired / otherAgents->size();
	//return Vdesired;
}

// Separation behavior
///////////////////////////////////////////////////////////////////////////////
// For the given te actor, return a desired velocity vector in world coordinates
// Separation tries to maintain a constant distance between all agents
// within the neighborhood
// agents[i] gives the pointer to the ith agent in the environment
// Separation settings are in BehavioralController::RNeighborhood and BehavioralController::KSeperate

 

Separation::Separation( AJoint* target,  vector<AActor>* agents) 
{
	m_name = "separation";
	m_AgentList = agents;
	m_pTarget = target;
}

Separation::~Separation()
{
}

Separation::Separation( Separation& orig) 
{
	m_name = "separation";
	m_AgentList = orig.m_AgentList;
	m_pTarget = orig.m_pTarget;
}

vec3 Separation::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;
	
	// compute Vdesired = Vseparate
	// TODO: add your code here to compute 
	
	AActor* curActor;
	for (int i = 0; i < agentList.size(); i++) {
		vec3 v;
		curActor = &agentList.at(i);
		v = -(curActor->getBehaviorController()->getPosition() - actorPos);
		Vdesired += v;
	}
	if (Vdesired.Length() < 5.0)
		Vdesired = 0.0;	
	return Vdesired;
}


// Cohesion behavior
///////////////////////////////////////////////////////////////////////////////
// For the given actor, return a desired velocity vector in world coordinates
// Cohesion moves actors towards the center of the group of agents in the neighborhood
//  agents[i] gives the pointer to the ith agent in the environment
//  Cohesion parameters are in BehavioralController::RNeighborhood and BehavioralController::KCohesion


Cohesion::Cohesion( vector<AActor>* agents) 
{
	m_name = "cohesion";
	m_AgentList = agents;
}

Cohesion::Cohesion( Cohesion& orig) 
{
	m_name = "cohesion";
	m_AgentList = orig.m_AgentList;
}

Cohesion::~Cohesion()
{
}

vec3 Cohesion::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;
	
	// compute Vdesired = Vcohesion
	// TODO: add your code here
	vec3 v = vec3(0, 0, 0);

	for (int i = 0; i < agentList.size(); i++) {
		v += agentList.at(i).getBehaviorController()->getPosition();
	}

	v /= agentList.size();
	Vdesired = v - actorPos;
	if (Vdesired.Length() < 5.0)
		Vdesired = 0.0;
	return Vdesired;
}

// Flocking behavior
///////////////////////////////////////////////////////////////////////////////
// For the given actor, return a desired velocity vector  in world coordinates
// Flocking combines separation, cohesion, and alignment behaviors
//  Utilize the Separation, Cohesion and Alignment behaviors to determine the desired velocity vector


Flocking::Flocking( AJoint* target,  vector<AActor>* agents) 
{
	m_name = "flocking";
	m_AgentList = agents;
	m_pTarget = target;
}

Flocking::Flocking( Flocking& orig) 
{
	m_name = "flocking";
	m_AgentList = orig.m_AgentList;
	m_pTarget = orig.m_pTarget;
}

Flocking::~Flocking()
{
}

vec3 Flocking::calcDesiredVel( BehaviorController* actor)
{
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;

	// compute Vdesired = Vflocking
	// TODO: add your code here 
	vec3 v1, v2 = vec3(0, 0, 0);
	{
		AActor* curActor;
		for (int i = 0; i < m_AgentList->size(); i++) {
			curActor = &m_AgentList->at(i);
			v1 = -(curActor->getBehaviorController()->getPosition() - actorPos);
			v2 += v1;
		}
		if (v2.Length() < 5.0)
			v2 = 0.0;
		v2 = v2 / m_AgentList->size();
	}
	vec3 coh_v, v_coh = vec3(0, 0, 0);
	{
		vec3 cen = vec3(0, 0, 0);
		for (int i = 0; i < agentList.size(); i++) {
			cen += agentList.at(i).getBehaviorController()->getPosition();
		}
		cen /= agentList.size();
		v_coh = cen - actorPos;
		if (v_coh.Length() < 5.0)
			v_coh = 0.0;
	}

	vec3 ali_v, v_ali = vec3(0, 0, 0);
	{
		vec3 targetPos = m_pTarget->getLocalTranslation();
		BehaviorController* leader = agentList[0].getBehaviorController();
		if (actor == leader) {
			Vdesired = (targetPos - actorPos);
			return Vdesired;
		}
		else {
			AActor* curActor;
			for (int i = 0; i < m_AgentList->size(); i++) {
				curActor = &m_AgentList->at(i);
				v_ali += curActor->getBehaviorController()->getVelocity();
			}
			v_ali = v_ali / m_AgentList->size();
		}
	}
	double c_sep = 1, c_coh = 1, c_ali = 1;
	Vdesired = (c_ali * v_ali + c_coh * v_coh + c_sep * v2) / (c_ali + c_coh + c_sep);
	return Vdesired;
}

//	Leader behavior
///////////////////////////////////////////////////////////////////////////////
// For the given actor, return a desired velocity vector in world coordinates
// If the agent is the leader, move towards the target; otherwise, 
// follow the leader at a set distance behind the leader without getting to close together
//  Utilize Separation and Arrival behaviors to determine the desired velocity vector
//  You need to find the leader, who is always agents[0]

Leader::Leader( AJoint* target, vector<AActor>* agents) 
{
	m_name = "leader";
	m_AgentList = agents;
	m_pTarget = target;
}

Leader::Leader( Leader& orig) 
{
	m_name = "leader";
	m_AgentList = orig.m_AgentList;
	m_pTarget = orig.m_pTarget;
}

Leader::~Leader()
{
}

vec3 Leader::calcDesiredVel( BehaviorController* actor)
{
	
	vec3 Vdesired = vec3(0.0, 0.0, 0.0);
	vec3 actorPos = actor->getPosition();
	vector<AActor>& agentList = *m_AgentList;

	// TODO: compute Vdesired  = Vleader
	// followers should stay directly behind leader at a distance of -200 along the local z-axis

	float c_sep = 4.0;  float c_arr = 2.0;

	BehaviorController* leader = agentList[0].getBehaviorController(); // first agent is the leader
	mat3 Rmat = leader->getGuide().getLocalRotation();  // is rotattion matrix of lead agent

	vec3 targetPos = m_pTarget->getLocalTranslation();
	vec3 v1, v2 = vec3(0, 0, 0);
	vec3 v_arrl;
	{
		AActor* curActor;
		for (int i = 0; i < m_AgentList->size(); i++) {
			curActor = &m_AgentList->at(i);
			v1 = -(curActor->getBehaviorController()->getPosition() - actorPos);
			v2 += v1;
		}
		if (v2.Length() < 5.0)
			v2 = 0.0;
		v2 = v2 / m_AgentList->size();
	}
	if (actor == leader) {
		Vdesired = targetPos - actorPos;
		return Vdesired;
	}
	else {
		v_arrl = leader->getPosition() - actorPos;
	}
	Vdesired = (c_sep * v2 + c_arr * v_arrl) / (c_sep + c_arr);
	return Vdesired;
}

///////////////////////////////////////////////////////////////////////////////


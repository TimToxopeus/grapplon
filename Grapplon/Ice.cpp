#include "Ice.h"
#include "ODEManager.h"

CIce::CIce(PlanetaryData &data) 
	:	CPlanet(data)
{
	m_eType = ICE;
	m_iTempRadius = data.tempradius;

	dJointID sunJoint = CODEManager::Instance()->createHingeJoint("Ice joint");
	Vector pos = GetPosition();
	dJointAttach(sunJoint, m_oPhysicsData.body, 0);
	dJointSetHingeAnchor(sunJoint , pos[0], pos[1], 0.0f);
}

CIce::~CIce(){}

void CIce::Render()
{
	CBaseObject::Render();
}
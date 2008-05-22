#include "Sun.h"
#include "ODEManager.h"

CSun::CSun(PlanetaryData &data) : CPlanet(data)
{
	m_eType = SUN;
	dJointID sunJoint = CODEManager::Instance()->createHingeJoint("Sun joint");
	dJointAttach(sunJoint, m_oPhysicsData.body, 0);
}

CSun::~CSun(){}

void CSun::Render()
{
	CBaseObject::Render();
}
#pragma once

#include "BaseMovableObject.h"
#include <ode/ode.h>

class CPlayerObject;
class CChainLink;

class CHook : public CBaseMovableObject
{
private:
	CPlayerObject *m_pOwner;
	bool m_bDisconnected;
	std::vector<CChainLink*> chainLinks;
	dJointGroupID chainJoints;
	dJointID hookJoint;

public:
	CHook( CPlayerObject *pOwner );
	~CHook();

	bool IsDisconnected() { return m_bDisconnected; }
	void Disconnect();
	void Reconnect();
	void AddRope();
	void AddChainForce(float x_force, float y_force);
	virtual void ApplyForceFront();
	virtual void Update( float fTime );
};

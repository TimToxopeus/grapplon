#pragma once

#include "BaseMovableObject.h"
#include <ode/ode.h>

class CPlayerObject;
class CChainLink;

enum HookState { CONNECTED, HOMING, DISCONNECTED };


class CHook : public CBaseMovableObject
{
private:
	CPlayerObject *m_pOwner;
	HookState m_eHookState;
	std::vector<CChainLink*> chainLinks;
	dJointGroupID chainJoints;
	dJointID hookJoint;
	float startUp;

public:
	
	
	CHook( CPlayerObject *pOwner );
	~CHook();

	bool IsDisconnected() { return m_eHookState == DISCONNECTED; }
	void Eject();
	void Reconnect();
	void AddRope();
	void AddChainForce(float x_force, float y_force);
	virtual void ApplyForceFront();
	virtual void Update( float fTime );
};

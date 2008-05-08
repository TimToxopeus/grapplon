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
	std::vector<CChainLink*> chainLinks;
	dJointGroupID chainJoints;
	dJointID m_pHookJoint;
	float startUp;

public:
	
	
	CHook( CPlayerObject *pOwner );
	~CHook();

	HookState m_eHookState;

	bool IsDisconnected() { return m_eHookState == DISCONNECTED; }
	void Eject();
	void Reconnect();
	void AddRope();
	void AddChainForce(float x_force, float y_force);
	virtual void ApplyForceFront();
	virtual void Update( float fTime );
};

#pragma once

#include "BaseMovableObject.h"
#include <ode/ode.h>

class CPlayerObject;
class CChainLink;

enum HookState { CONNECTED, HOMING, GRASPING, RETURNING };


class CHook : public CBaseMovableObject
{
private:
	CPlayerObject *m_pOwner;
	std::vector<CChainLink*> chainLinks;
	std::vector<dJointID>    chainJoints;
	dJointID m_pLastChainJoint;
	dJointID m_oMiddleChainJoint;
	dJointID m_oHookGrabJoint;
	dJointID m_oAngleJoint;
	bool isRadialCorrected;
	PhysicsData *m_pGrabbedObject;

	float startUp;

public:
	
	
	CHook( CPlayerObject *pOwner );
	~CHook();

	HookState m_eHookState;

	bool IsDisconnected() { return m_eHookState != CONNECTED; }
	void Eject();
	void Grasp(PhysicsData*);
	void Throw();
	void Reconnect();
	void AddRope();
	void AddChainForce(float x_force, float y_force);
	virtual void ApplyForceFront();
	virtual void Update( float fTime );
};

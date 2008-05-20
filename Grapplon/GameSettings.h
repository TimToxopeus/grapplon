#pragma once

#include <string>

class CGameSettings
{
private:
	static CGameSettings *m_pInstance;
	CGameSettings();
	virtual ~CGameSettings();

	FILE *pFile;
	std::string ReadLine();

public:
	static CGameSettings *Instance() { if ( !m_pInstance ) m_pInstance = new CGameSettings(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }
	
	float THROW_FORCE;
	float MAX_TURN_SPEED;
	float TURN_ACCEL;
	float EJECT_FORCE;
	float RETRACT_FORCE;
	float PITCH_ACCEL_OUT;
	float PITCH_ACCEL_IN;
	float MIN_ACCEL_FOR_PROCESS;
	float LINK_LENGTH;
	int LINK_AMOUNT;
	int LINK_GRASP_CON;
	float CENT_DIST_HOOK;
	float CFM;
	float ERP;
	float HOOK_AIR_DRAG;
	float HOOK_MASS;
	float AUTO_AIM_ANGLE;
	bool Init();
};

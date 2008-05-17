#pragma once

#include <string>

class CGameSettings
{
private:
	static CGameSettings *m_pInstance;
	CGameSettings();
	virtual ~CGameSettings();

	float m_fThrowForce;
	float m_fMaxTurnSpeed;
	float m_fTurnAcceleration;
	float m_fEjectForce;
	float m_fRetractForce;

	FILE *pFile;
	std::string ReadLine();

public:
	static CGameSettings *Instance() { if ( !m_pInstance ) m_pInstance = new CGameSettings(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	bool Init();

	float GetThrowForce() { return m_fThrowForce; }
	float GetMaxTurnSpeed() { return m_fMaxTurnSpeed; }
	float GetTurnAcceleration() { return m_fTurnAcceleration; }
	float GetEjectForce() { return m_fEjectForce; }
	float GetRetractForce() { return m_fRetractForce; }
};

#pragma once

#include "ActiveObject.h"
#include "ParticleEmitter.h"

#include <vector>
#include <string>

class CParticleSystemManager : public IActiveObject
{
private:
	static CParticleSystemManager *m_pInstance;
	CParticleSystemManager();
	virtual ~CParticleSystemManager();

	std::vector<CParticleEmitter *> m_vEmitters;

	std::string ReadLine( FILE *pFile );
	CParticle *ReadParticle( std::string szEmitterScript, std::string szParticleName, CParticleEmitter *pEmitter );
	CParticleBehaviour ReadParticleBehaviour( std::string szEmitterScript, std::string szParticleBehaviourName );

public:
	static CParticleSystemManager *Instance() { if ( !m_pInstance ) m_pInstance = new CParticleSystemManager(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	void Update( float fTime );
	void Render();

	CParticleEmitter *LoadEmitter( std::string szEmitterScript );
};

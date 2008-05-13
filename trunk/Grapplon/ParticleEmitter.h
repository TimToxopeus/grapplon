#pragma once

#include "ParticleBehaviour.h"
#include "Particle.h"

#include "Vector.h"

#include <vector>

enum EmitterType
{
	NONE,
	LINE,
	ARC
};

struct ParticleFactoryEntry
{
	CParticle *m_pParticle;
	unsigned int m_iChance;
};

class CParticleEmitter
{
private:
	// Non-changing information
	EmitterType m_eType;
	float m_fTypeParameter;
	unsigned int m_iMaxParticles;
	unsigned int m_iLifespan;
	unsigned int m_iSpawnrate;
	float m_fRadius;
	std::vector<ParticleFactoryEntry> m_vParticleFactory;

	// Changing information
	unsigned int m_iCurParticles;
	unsigned int m_iAge;
	float m_fParticleSpawnTime;
	
	Vector m_vPosition, m_vDirection;

	CParticle *m_pFirst;

public:
	CParticleEmitter( EmitterType eType, float fTypeParameter, unsigned int iMaxParticles, unsigned int iLifespan, unsigned int iSpawnrate, float fRadius );
	~CParticleEmitter();

	void AddToFactory( CParticle *pParticle, unsigned int iChance );

	void SpawnParticle();
	void Update( float fTime );
	void Render();

	void SetPosition( Vector vPosition ) { m_vPosition = vPosition; }
	void SetDirection( Vector vDirection ) { m_vDirection = vDirection; }
	Vector GetPosition() { return m_vPosition; }
	Vector GetDirection() { return m_vDirection; }

	bool IsAlive() { return (m_iAge < m_iLifespan); }
};

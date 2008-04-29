#include "PlayerObject.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Hook.h"

#include "ODEManager.h"
#include "Vector.h"

#pragma warning(disable:4244)

CPlayerObject::CPlayerObject( int iPlayer )
{
	m_iPlayer = iPlayer;
	m_pImage = (CTexture *)CResourceManager::Instance()->GetResource("media/images/Player.png", RT_TEXTURE);
	m_pRadius = (CTexture *)CResourceManager::Instance()->GetResource("media/images/white_radius.png", RT_TEXTURE);
	SetDepth( -1.0f );
	timeSinceNoInput = 5.0f;

	m_oHookJoint = 0;

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(m_oPhysicsData, 32.0f);
	SetMass( 10.0f );
	m_oPhysicsData.m_bAffectedByGravity = false;

	m_pHook = new CHook( this );

	y = p = r = 10.0f;
}

CPlayerObject::~CPlayerObject()
{
	if ( m_oHookJoint )
	{
		m_oHookJoint = 0;
	}
}

bool CPlayerObject::HandleWiimoteEvent( wiimote_t* pWiimoteEvent )
{
	if ( pWiimoteEvent->event == WIIUSE_EVENT )
	{
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_PLUS))
			wiiuse_motion_sensing(pWiimoteEvent, 1);
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_MINUS))
			wiiuse_motion_sensing(pWiimoteEvent, 0);
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A))
		{
			if ( !m_pHook->IsDisconnected() )
			{
				m_pHook->AddForce( GetForwardVector() * 250000.0f );
				m_pHook->Disconnect();
			}
		}
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B))
		{
			if ( m_oHookJoint )
			{
				m_oHookJoint = 0;
				m_pHook->Reconnect();
			}
		}

		if (WIIUSE_USING_ACC(pWiimoteEvent))
		{
			y = pWiimoteEvent->orient.yaw;
			p = pWiimoteEvent->orient.pitch;
			r = pWiimoteEvent->orient.roll;
		}

		if (pWiimoteEvent->exp.type == EXP_NUNCHUK)
		{
			if ( pWiimoteEvent->exp.nunchuk.js.mag > 0.05f )
			{
				float difference = -(m_fAngle - pWiimoteEvent->exp.nunchuk.js.ang);
				float direction = difference;
				if ( difference > 180.0f )
					direction = difference - 360.0f;
				else if ( difference < -180.0f )
					direction = difference + 360.0f;

				if ( difference < 0.0f ) difference = -difference;
				if ( difference < 3.0f )
					m_fAngle = pWiimoteEvent->exp.nunchuk.js.ang;
				else
					m_fAngle += (direction / 6.0f);
				m_fVelocityForward = 25000.0f * (sin(1.57f * pWiimoteEvent->exp.nunchuk.js.mag));

				timeSinceNoInput = 0.0f;

				Vector v = GetPosition();
				float angle = (pWiimoteEvent->exp.nunchuk.js.ang - 90.0f)*(3.14f/180.0f);
				v += Vector( cos(angle), sin(angle), 0.0f );
				v -= GetPosition();
				v.Normalize();
				SetVelocity( v * m_fVelocityForward * 0.025f );

				return true;
			}
			else
				m_fVelocityForward = 0.0f;
		}
	}
	return false;
}

void CPlayerObject::Render()
{
	SDL_Rect target, size;
	size = m_pRadius->GetSize();
	target.x = GetX() - (size.w / 2);
	target.y = GetY() - (size.h / 2);
	target.w = size.w;
	target.h = size.h;

	SDL_Color colour;
	colour.r = colour.g = colour.b = 0;
	if ( m_iPlayer == 0 ) // red
	{
		colour.r = 255;
	}
	else if ( m_iPlayer == 1 ) // blue
	{
		colour.g = 255;
	}
	else if ( m_iPlayer == 2 ) // purple
	{
		colour.r = 255;
		colour.b = 255;
	}
	else // yellow
	{
		colour.r = 255;
		colour.g = 255;
	}

	RenderQuad( target, m_pRadius, m_fAngle, colour );

	target.w = 25;
	target.h = (p < 0 ? -p : p);
	target.x = 100 + (100 * m_iPlayer);
	target.y = 200 + (p < 0 ? p : 0);
	RenderQuad( target, NULL, 0, colour );
	target.h = (r < 0 ? -r : r);
	target.x = 125 + (100 * m_iPlayer);
	target.y = 200 + (r < 0 ? r : 0);
	RenderQuad( target, NULL, 0, colour );

	CBaseMovableObject::Render();
}

void CPlayerObject::Update( float fTime )
{
	Uint8 *keystate = SDL_GetKeyState(NULL);
	if ( keystate[SDLK_a] )
	{
		if ( !m_pHook->IsDisconnected() )
		{
			m_pHook->AddForce( GetForwardVector() * 250000.0f );
			m_pHook->Disconnect();
		}
	}
	if ( keystate[SDLK_b] )
	{
		if ( m_oHookJoint )
		{
			m_oHookJoint = 0;
			m_pHook->Reconnect();
		}
	}

	timeSinceNoInput += fTime;
	if ( timeSinceNoInput > 5.0f )
	{
		m_fAngle += 15.0f * fTime;
		m_fVelocityForward = 50.0f;
	}

	if ( !m_pHook->IsDisconnected() )
	{
		Vector f = GetForwardVector();
		m_pHook->SetPosition( GetPosition() + (f * 5.0f) );
		m_pHook->SetRotation( m_fAngle );
	}
	else
	{
		Vector p = m_pHook->GetPosition();
		Vector diff = p - GetPosition();
		if ( ((diff.Length() >= 64.0f ) && m_oHookJoint == 1) || diff.Length() >= 64.0f )
		{
			diff.Normalize();
			Vector newPos = GetPosition() + (diff * 63.5f);
			m_pHook->SetPosition(newPos);
			Vector n;
			n.CopyInto(m_pHook->GetBody()->lvel);
			m_pHook->SetRotation( GetPosition().CalculateAngle(newPos) );
			m_oHookJoint = 1;
		}
	}

	//m_fAngle = GetPosition().CalculateAngle( GetPosition() + Vector(m_oPhysicsData.body->lvel) );
	CBaseMovableObject::Update( fTime );
}

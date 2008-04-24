#include "PlayerObject.h"
#include "ResourceManager.h"
#include "Texture.h"

#include "Vector.h"

#pragma warning(disable:4244)

CPlayerObject::CPlayerObject( int iPlayer )
{
	m_iPlayer = iPlayer;
	m_pImage = (CTexture *)CResourceManager::Instance()->GetResource("media/images/Player.png", RT_TEXTURE);
	m_pRadius = (CTexture *)CResourceManager::Instance()->GetResource("media/images/white_radius.png", RT_TEXTURE);
	SetDepth( -1.0f );
	timeSinceNoInput = 5.0f;

	y = p = r = 10.0f;
}

CPlayerObject::~CPlayerObject()
{
}

bool CPlayerObject::HandleWiimoteEvent( wiimote_t* pWiimoteEvent )
{
	if ( pWiimoteEvent->event == WIIUSE_EVENT )
	{
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_PLUS))
			wiiuse_motion_sensing(pWiimoteEvent, 1);
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_MINUS))
			wiiuse_motion_sensing(pWiimoteEvent, 0);

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
				m_fVelocityForward = 250.0f * (sin(1.57f * pWiimoteEvent->exp.nunchuk.js.mag));

				timeSinceNoInput = 0.0f;

				Vector v = GetPosition();
				float angle = (pWiimoteEvent->exp.nunchuk.js.ang - 90.0f)*(3.14f/180.0f);
				v = GetPosition() - Vector( cos(angle), sin(angle), 0.0f );
				v.Normalize();
				AddForce( v * 25.0f );

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

	CBaseMovableObject::Render();
}

void CPlayerObject::Update( float fTime )
{
	timeSinceNoInput += fTime;
	if ( timeSinceNoInput > 5.0f )
	{
		m_fAngle += 15.0f * fTime;
		m_fVelocityForward = 50.0f;
	}
	CBaseMovableObject::Update( fTime );
}

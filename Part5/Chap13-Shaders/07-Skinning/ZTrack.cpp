#include "ZTrack.h"

ZTrack::ZTrack( Track& keys )
{
	int		i;

	if( keys.pos.size() )
	{
		m_keys.pos.resize( keys.pos.size() );
		for( i = 0 ; i < m_keys.pos.size() ; i++ )
		{
			m_keys.pos[i].frame = keys.pos[i].frame;
			memcpy( &m_keys.pos[i].value, &keys.pos[i].value, sizeof D3DXVECTOR3 );
		}
	}

	if( keys.rot.size() )
	{
		D3DXQUATERNION	q = D3DXQUATERNION( 0, 0, 0, 1 );
		m_keys.rot.resize( keys.rot.size() );
		m_keys.rot[0].frame = keys.rot[0].frame;
		m_keys.rot[0].value = q;
		for( i = 1 ; i < m_keys.rot.size() ; i++ )
		{
			m_keys.rot[i].frame = keys.rot[i].frame;
			
			if( keys.rot[i].frame <= keys.start )	// 애니메이션이 시작되는 프레임보다 작은 키값들은 전부 단위 쿼터니온으로!
				q = D3DXQUATERNION( 0, 0, 0, 1 );
			else
				q = D3DXQUATERNION( keys.rot[i].value.x, keys.rot[i].value.y, keys.rot[i].value.z, keys.rot[i].value.w );

			D3DXQuaternionMultiply( &m_keys.rot[i].value, &m_keys.rot[i-1].value, &q );
		}
	}

	if( keys.scl.size() )
	{
		m_keys.scl.resize( keys.scl.size() );
		for( i = 0 ; i < m_keys.scl.size() ; i++ )
		{
			m_keys.scl[i].frame = keys.scl[i].frame;
			memcpy( &m_keys.scl[i].value, &keys.scl[i].value, sizeof D3DXVECTOR3 );
		}
	}

}

ZTrack::~ZTrack()
{
	stl_wipe_vector( m_keys.pos );
	stl_wipe_vector( m_keys.rot );
	stl_wipe_vector( m_keys.scl );
}

void ZTrack::_Get2Keys( float frame, vector<ZKeyPos>& keys, ZKeyPos*& k1, ZKeyPos*& k2 )
{
	int		mkey;
	int		lkey = 0;
	int		rkey = keys.size()-1;

	if( keys.size() > 1 )
	{
		if( frame > keys[rkey].frame )
		{
			k1 = &keys[rkey];
			k2 = &keys[rkey];
			return;
		}
		if( frame < keys[lkey].frame)
		{
			k1 = &keys[lkey];
			k2 = &keys[lkey];
			return;
		}

		while( rkey >= lkey )
		{
			mkey = (rkey + lkey)/2;
			if( keys[mkey].frame <= frame && keys[mkey+1].frame >= frame )
			{
				k1 = &keys[mkey];
				k2 = &keys[mkey+1];
				return;
			}
			if( keys[mkey].frame > frame )
				rkey = mkey;
			else
				lkey = mkey;
		}
	}

	k1 = k2 = NULL;
}

void ZTrack::_Get2Keys( float frame, vector<ZKeyRot>& keys, ZKeyRot*& k1, ZKeyRot*& k2 )
{
	int		mkey;
	int		lkey = 0;
	int		rkey = keys.size()-1;

	if( keys.size() > 1 )
	{
		if( frame > keys[rkey].frame )
		{
			k1 = &keys[rkey];
			k2 = &keys[rkey];
			return;
		}
		if( frame < keys[lkey].frame)
		{
			k1 = &keys[lkey];
			k2 = &keys[lkey];
			return;
		}

		while( rkey >= lkey )
		{
			mkey = (rkey + lkey)/2;
			if( keys[mkey].frame <= frame && keys[mkey+1].frame >= frame )
			{
				k1 = &keys[mkey];
				k2 = &keys[mkey+1];
				return;
			}
			if( keys[mkey].frame > frame )
				rkey = mkey;
			else
				lkey = mkey;
		}
	}

	k1 = k2 = NULL;
}

void ZTrack::_Get2Keys( float frame, vector<ZKeyScl>& keys, ZKeyScl*& k1, ZKeyScl*& k2 )
{
	int		mkey;
	int		lkey = 0;
	int		rkey = keys.size()-1;

	if( keys.size() > 1 )
	{
		if( frame > keys[rkey].frame )
		{
			k1 = &keys[rkey];
			k2 = &keys[rkey];
			return;
		}
		if( frame < keys[lkey].frame)
		{
			k1 = &keys[lkey];
			k2 = &keys[lkey];
			return;
		}

		while( rkey >= lkey )
		{
			mkey = (rkey + lkey)/2;
			if( keys[mkey].frame <= frame && keys[mkey+1].frame >= frame )
			{
				k1 = &keys[mkey];
				k2 = &keys[mkey+1];
				return;
			}
			if( keys[mkey].frame > frame )
				rkey = mkey;
			else
				lkey = mkey;
		}
	}

	k1 = k2 = NULL;
}

BOOL ZTrack::_GetPosKey( float frame, D3DXVECTOR3* pv )
{
	float	a;
	ZKeyPos *p1, *p2;

	_Get2Keys( frame, m_keys.pos, p1, p2 );

	if( !p1 || !p2 ) return FALSE;

	if( p1 == p2 )
	{
		pv->x = p1->value.x;
		pv->y = p1->value.y;
		pv->z = p1->value.z;
	}
	else
	{
		a = _GetAlpha( p1->frame, p2->frame, frame );
		D3DXVec3Lerp( pv, &p1->value, &p2->value, a );
	}

	return TRUE;
}

BOOL ZTrack::_GetRotKey( float frame, D3DXQUATERNION* pq )
{
	float	a;
	ZKeyRot *p1,*p2;

	_Get2Keys( frame, m_keys.rot, p1, p2 );

	if( !p1 || !p2 ) return FALSE;

	if( p1 == p2 )
	{
		pq->x = p2->value.x;
		pq->y = p2->value.y;
		pq->z = p2->value.z;
		pq->w = p2->value.w;
	}
	else
	{
		a = _GetAlpha( p1->frame, p2->frame, frame );
		D3DXQuaternionSlerp( pq, &p1->value, &p2->value, a );
	}

	return TRUE;
}

BOOL ZTrack::_GetSclKey( float frame, D3DXVECTOR3* pv )
{
	float	a;
	ZKeyScl *p1, *p2;

	_Get2Keys( frame, m_keys.scl, p1, p2 );

	if( !p1 || !p2 ) return FALSE;

	if( p1 == p2 )
	{
		pv->x = p1->value.x;
		pv->y = p1->value.y;
		pv->z = p1->value.z;
	}
	else
	{
		a = _GetAlpha( p1->frame, p2->frame, frame );
		D3DXVec3Lerp( pv, &p1->value, &p2->value, a );
	}

	return TRUE;
}

D3DXMATRIXA16*	ZTrack::Animate( float frame )
{
	D3DXVECTOR3		v;
	D3DXQUATERNION	q;

	D3DXMatrixIdentity( &m_matAni );

	if( _GetRotKey( frame, &q ) )
		D3DXMatrixRotationQuaternion( &m_matAni, &q );

	if( _GetPosKey( frame, &v ) )
	{
		m_matAni._41 = v.x;
		m_matAni._42 = v.y;
		m_matAni._43 = v.z;
	}
	if( _GetSclKey( frame, &v ) )
	{
		m_matAni._11 *= v.x;
		m_matAni._22 *= v.y;
		m_matAni._33 *= v.z;
	}

	return &m_matAni;
}

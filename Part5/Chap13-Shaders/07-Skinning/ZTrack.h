#ifndef _ZTRACK_H_
#define _ZTRACK_H_

#include "define.h"
#include "ZDefine.h"
#include <d3d9.h>
#include <d3dx9.h>

/// �ִϸ��̼� Ű���� �����ϱ� ���� Ŭ����
class ZTrack
{
public:
	enum INTERPOL { INTP_LINEAR, INTP_SPLINE, INTP_HERMIT };	// ����� LINEAR�� ����

protected:
	ZKeys			m_keys;		/// Ű����
	D3DXMATRIXA16	m_matAni;	/// ���������� ���� �ִϸ��̼� ���

	float _GetAlpha( float f1, float f2, float frame )
	{
		return ( (frame-f1) / (f2-f1) );
	}

	void _Get2Keys( float frame, vector<ZKeyPos>& keys, ZKeyPos*& k1, ZKeyPos*& k2 );
	void _Get2Keys( float frame, vector<ZKeyRot>& keys, ZKeyRot*& k1, ZKeyRot*& k2 );
	void _Get2Keys( float frame, vector<ZKeyScl>& keys, ZKeyScl*& k1, ZKeyScl*& k2 );
	BOOL _GetPosKey( float frame, D3DXVECTOR3* pv );
	BOOL _GetRotKey( float frame, D3DXQUATERNION* pq );
	BOOL _GetSclKey( float frame, D3DXVECTOR3* pv );
public:
	ZTrack( Track& keys );
	~ZTrack();
	
	D3DXMATRIXA16*	Animate( float frame );
};

#endif // _ZTRACK_H_
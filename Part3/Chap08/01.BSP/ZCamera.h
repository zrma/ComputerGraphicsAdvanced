#include <d3d9.h>
#include <d3dx9.h>

class ZCamera
{
	D3DXVECTOR3		m_vEye;
	D3DXVECTOR3		m_vLookat;
	D3DXVECTOR3		m_vUp;

	D3DXVECTOR3		m_vView;		// view unit vector
	D3DXVECTOR3		m_vCross;		// cross( view, up )

	D3DXMATRIXA16	m_matView;
	D3DXMATRIXA16	m_matBill;		// billboard matrix
	D3DXMATRIXA16	m_matWorld;
	D3DXMATRIXA16	m_matProj;
public:
	ZCamera();
	D3DXMATRIXA16*	GetViewMatrix() { return &m_matView; }
	D3DXMATRIXA16*	GetBillMatrix() { return &m_matBill; }

public:
	D3DXMATRIXA16*	SetView( D3DXVECTOR3* pvEye,D3DXVECTOR3* pvLookat,D3DXVECTOR3* pvUp);

	void			SetEye( D3DXVECTOR3* pv ) { m_vEye = *pv; }
	D3DXVECTOR3*	GetEye() { return &m_vEye; }
	void			SetLookat( D3DXVECTOR3* pv ) { m_vLookat = *pv; }
	D3DXVECTOR3*	GetLookat() { return &m_vLookat; }
	void			SetUp( D3DXVECTOR3* pv ) { m_vUp = *pv; }
	D3DXVECTOR3*	GetUp() { return &m_vUp; }
	void			Flush() { SetView( &m_vEye, &m_vLookat, &m_vUp ); }

	D3DXMATRIXA16*	RotateLocalX( float angle );	// local x축을 중심으로 회전
	D3DXMATRIXA16*	RotateLocalY( float angle );	// local y축을 중심으로 회전
//	D3DXMATRIXA16*	RotateLocalZ( float angle );	// local z축을 중심으로 회전

	D3DXMATRIXA16*	MoveTo( D3DXVECTOR3* pv );
	D3DXMATRIXA16*	MoveLocalX( float dist );	// local x축을 중심으로 이동
	D3DXMATRIXA16*	MoveLocalY( float dist );	// local y축을 중심으로 이동
	D3DXMATRIXA16*	MoveLocalZ( float dist );	// local z축을 중심으로 이동
};
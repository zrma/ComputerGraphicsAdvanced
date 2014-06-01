#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <d3d9.h>
#include <d3dx9.h>

#ifndef MAINBODY
#define DECLARE extern
#else
#define DECLARE
#endif

#ifndef DEL
	#define DEL(p)		{ if(p) { delete (p); (p) = NULL; } }
#endif

#ifndef DELS
	#define DELS(p)		{ if(p) { delete[] (p); (p) = NULL; } }
#endif

#ifndef REL
	#define REL(p)		{ if(p) { (p)->Release(); (p) = NULL; } }
#endif

#define Z_EPSILON		0.0001f

#define IS_IN_RANGE(value,r0,r1) (( ((r0) <= (value)) && ((value) <= (r1)) ) ? 1 : 0)
struct TERRAINVERTEX
{
	enum _FVF { FVF=(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
	D3DXVECTOR2	t;
};

#define _USE_INDEX16

struct TRIINDEX
{
#ifdef _USE_INDEX16
	WORD _0, _1, _2;
#else
	DWORD	_0, _1, _2;
#endif
};

template<class T> 
class ZTSingleton
{
	static T* m_pInstance;
public:
	ZTSingleton()
	{
		assert( !m_pInstance );
		int offset = (int)(T*)1-(int)(ZTSingleton<T>*)(T*)1;
		m_pInstance = (T*)((int)this + offset );
	}
	~ZTSingleton() { assert( m_pInstance ); m_pInstance = 0; }
	static T& GetInstance() { assert( m_pInstance ); return (*m_pInstance ); }
	static T* GetInstancePtr() { return m_pInstance; }
};

template <class T> T* ZTSingleton<T>::m_pInstance = 0;


/*
template<typename TContainer>
inline void stl_wipe(TContainer& container)
{
	for( TContainer::iterator i = container.begin() ; i != container.end() ; ++i )
		delete *i;
	container.clear();
}

template<typename T>
void stl_wipe_vector(vector<T>& rVector)
{
	vector<T> emptyData;
	rVector.swap( emptyData );
}
*/

#endif // _DEFINE_H_ 
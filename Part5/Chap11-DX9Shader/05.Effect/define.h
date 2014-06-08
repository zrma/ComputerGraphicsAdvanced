#ifndef _ZDEFINES_H_
#define _ZDEFINES_H_

#pragma warning(disable:4786)	// to reduce STL warning

#ifdef MAINBODY
#define DECLARE
#else
#define DECLARE extern
#endif

#include <assert.h>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
using namespace std;

#define S_REL(p) { if(p) p->Release(); p = NULL; }
#define S_DEL(p) { if(p) delete p; p = NULL; }
#define S_DELS(p) { if(p) delete[] p; }
#define MAX_STR_LEN		128
#define BLENDINDEX( i4, i3, i2, i1 )		((i4)<<24|(i3)<<16|(i2)<<8|(i1))

#define BIGFLOAT		9999.0f

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

struct Index3w
{
	unsigned short i[3];
};

struct Index3i
{
	int		i[3];
};

struct IndexWithID
{
	int		ID;
	Index3i	index;
};

struct Vector2f
{
	float	x;
	float	y;
};

struct Vector3f
{
	float	x;
	float	y;
	float	z;
};

struct Vector4f
{
	float	x;
	float	y;
	float	z;
	float	w;
};

typedef Vector4f Quat;

struct Color3f
{
	float	r;
	float	g;
	float	b;
};

struct Color4f
{
	float	r;
	float	g;
	float	b;
	float	a;
};

struct VertWeight
{
	vector<int>		ID;
	vector<float>	fWeight;
	~VertWeight() { stl_wipe_vector( ID ); stl_wipe_vector( fWeight ); }
};

struct VertMerged
{
	Vector3f	p;			// pos
	float		b[3];		// blend weight
	unsigned int i;			// index
	Vector3f	n;			// normal
	Vector2f	t;			// texture coord
};

struct Mat4x4
{
	Vector4f	row[4];
};

struct KeyRot
{
	float		frame;
	Quat		value;
};

struct KeyPos
{
	float		frame;
	Vector3f	value;
};

struct KeyScl
{
	float		frame;
	Vector3f	value;
};

struct Track
{
	float				start;
	float				end;
	vector<KeyPos>		pos;
	vector<KeyRot>		rot;
	vector<KeyScl>		scl;
	~Track() { stl_wipe_vector( pos ); stl_wipe_vector( rot); stl_wipe_vector( scl ); }
};

typedef struct tagMaterial
{
	Color3f	ambient;
	Color3f	diffuse;
	Color3f	specular;
	Color3f	emissive;
	float	power;
	float	opacity;
	string	strMapDiffuse;
	string	strMapSpecular;
	string	strMapBump;
	string	strMapDisplace;
} Material;

struct Info
{
	string			strFilename;
	int				nObjectCount;
	int				nMeshCount;
	float			fAnimationStart;
	float			fAnimationEnd;
	int				nKeyType;
	int				nSkinType;
	vector<string>	strObjectNameTable;
	vector<int>		BoneTable;
	~Info() { stl_wipe_vector( strObjectNameTable ); stl_wipe_vector( BoneTable ); }
};

#endif // _ZDEFINES_H_
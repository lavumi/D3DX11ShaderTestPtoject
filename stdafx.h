#pragma once

#include <Windows.h>
#include <assert.h>
#include <time.h>
#pragma comment(lib, "winmm.lib")

#include <string>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <fstream>
using namespace std;

//#include <xnamath.h>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <D3Dcompiler.h>




#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dcompiler.lib")


#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")
using namespace fbxsdk;

#include "AntTweakBar.h"
#pragma comment(lib, "AntTweakBar.lib")




#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE(p) { if (p) { delete (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p)=NULL; } }



static float RandFloat(float min, float max) {
	float rndFloat = (float)(rand() % (int)((max - min) * 1000)) / ((max - min) * 1000) + min;
	return rndFloat;
}




#include "./System/D3D.h"
#include "./System/Keyboard.h"
#include "./System/Mouse.h"
#include "./System/Frames.h"
#include "./System/TweakBar.h"

#include "./Render/UserInterface.h"

#include "./Render/VertexLayout.h"
#include "./Shader/WorldBuffer.h"


#include "./Camera/Camera.h"


#include "./Utility/String.h"
#include "./Utility/BinaryFile.h"



#include "./System/TextManager.h"



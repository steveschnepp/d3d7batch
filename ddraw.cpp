#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdarg.h>

#define INITGUID
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#define WRAP
#define TRACE
#define TRACE_(x, ...)
#define INFO lprintf
#define TRACE_(x, ...)
#define FIXME lprintf
#define FIXME_(x, ...)
#define ERR lprintf
#define ERR_(x, ...)
#define WARN lprintf
#define WARN_(x, ...)

#define d3d_perf
#define DEBUG_STDOUT 0
#define DEBUG 0

static void lprintf(const char* format, ...) {
#if DEBUG
    va_list args;
    char buf[2048];

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

#if DEBUG_STDOUT
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#endif

    OutputDebugString(buf);
#endif
}

#define GET_TEXCOUNT_FROM_FVF(d3dvtVertexType) \
    (((d3dvtVertexType) & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT)

#define GET_TEXCOORD_SIZE_FROM_FVF(d3dvtVertexType, tex_num) \
    (((((d3dvtVertexType) >> (16 + (2 * (tex_num)))) + 1) & 0x03) + 1)

DWORD
get_flexible_vertex_size(DWORD d3dvtVertexType)
{
    DWORD size = 0;
    DWORD i;

    if (d3dvtVertexType & D3DFVF_NORMAL) size += 3 * sizeof(D3DVALUE);
    if (d3dvtVertexType & D3DFVF_DIFFUSE) size += sizeof(DWORD);
    if (d3dvtVertexType & D3DFVF_SPECULAR) size += sizeof(DWORD);
    if (d3dvtVertexType & D3DFVF_RESERVED1) size += sizeof(DWORD);
    switch (d3dvtVertexType & D3DFVF_POSITION_MASK)
    {
        case D3DFVF_XYZ:    size += 3 * sizeof(D3DVALUE); break;
        case D3DFVF_XYZRHW: size += 4 * sizeof(D3DVALUE); break;
        case D3DFVF_XYZB1:  size += 4 * sizeof(D3DVALUE); break;
        case D3DFVF_XYZB2:  size += 5 * sizeof(D3DVALUE); break;
        case D3DFVF_XYZB3:  size += 6 * sizeof(D3DVALUE); break;
        case D3DFVF_XYZB4:  size += 7 * sizeof(D3DVALUE); break;
        case D3DFVF_XYZB5:  size += 8 * sizeof(D3DVALUE); break;
        default: ERR("Unexpected position mask\n");
    }
    for (i = 0; i < GET_TEXCOUNT_FROM_FVF(d3dvtVertexType); i++)
    {
        size += GET_TEXCOORD_SIZE_FROM_FVF(d3dvtVertexType, i) * sizeof(D3DVALUE);
    }

    return size;
}

typedef HRESULT ( WINAPI* LPDIRECTDRAWCREATEEX )( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter );

HINSTANCE orig_hInstDDraw;
LPDIRECTDRAW7 orig_lpDD7;

class Direct3D7Patched;
class Direct3DDevicePatched;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    const char *DllMain_REASON[] = { "DLL_PROCESS_DETACH", "DLL_PROCESS_ATTACH", "DLL_THREAD_ATTACH", "DLL_THREAD_DETACH" };

    WRAP("%s:%d \t DllMain(fdwReason: %s)\n", __FILE__, __LINE__, DllMain_REASON[fdwReason]);
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            return TRUE;
    }

    return TRUE;
}

HRESULT WINAPI 	DirectDrawCreate(LPGUID lpGUID, LPDIRECTDRAW *lplpDD, LPUNKNOWN pUnkOuter) {
    return DDERR_UNSUPPORTED;
}

class DirectDraw7Patched : public IDirectDraw7
{
    IDirectDraw7* mWrapped;

    IDirect3D7 *new_direct3D7 = nullptr;

public:
    DirectDraw7Patched(IDirectDraw7* wrapped) : mWrapped(wrapped) {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    }

    /* IUnknown */
    HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid, void** ppvObject);

    ULONG STDMETHODCALLTYPE AddRef() { return 0; }
    ULONG STDMETHODCALLTYPE Release() { return 0; }

    HRESULT STDMETHODCALLTYPE Compact();
    HRESULT STDMETHODCALLTYPE CreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER *lplpDDClipper, IUnknown *pUnkOuter);
    HRESULT STDMETHODCALLTYPE CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette, IUnknown *pUnkOuter);
    HRESULT STDMETHODCALLTYPE CreateSurface(LPDDSURFACEDESC2 lpDDSurfaceDesc, LPDIRECTDRAWSURFACE7 *lplpDDSurface, IUnknown *pUnkOuter);
    HRESULT STDMETHODCALLTYPE DuplicateSurface(LPDIRECTDRAWSURFACE7 lpDDSurface, LPDIRECTDRAWSURFACE7 *lplpDupDDSurface);
    HRESULT STDMETHODCALLTYPE EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext, LPDDENUMMODESCALLBACK2 lpEnumModesCallback);
    HRESULT STDMETHODCALLTYPE EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSD, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback);
    HRESULT STDMETHODCALLTYPE FlipToGDISurface();
    HRESULT STDMETHODCALLTYPE GetCaps(LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps);
    HRESULT STDMETHODCALLTYPE GetDisplayMode(LPDDSURFACEDESC2 lpDDSurfaceDesc);
    HRESULT STDMETHODCALLTYPE GetFourCCCodes(LPDWORD lpNumCodes, LPDWORD lpCodes);
    HRESULT STDMETHODCALLTYPE GetGDISurface(LPDIRECTDRAWSURFACE7 *lplpGDIDDSurface);
    HRESULT STDMETHODCALLTYPE GetMonitorFrequency(LPDWORD lpdwFrequency);
    HRESULT STDMETHODCALLTYPE GetScanLine(LPDWORD lpdwScanLine);
    HRESULT STDMETHODCALLTYPE GetVerticalBlankStatus(WINBOOL *lpbIsInVB);
    HRESULT STDMETHODCALLTYPE Initialize(GUID *lpGUID);
    HRESULT STDMETHODCALLTYPE RestoreDisplayMode();
    HRESULT STDMETHODCALLTYPE SetCooperativeLevel(HWND hWnd, DWORD dwFlags);
    HRESULT STDMETHODCALLTYPE SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwRefreshRate, DWORD dwFlags);
    HRESULT STDMETHODCALLTYPE WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent);
    HRESULT STDMETHODCALLTYPE GetAvailableVidMem(LPDDSCAPS2 lpDDCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree);
    HRESULT STDMETHODCALLTYPE GetSurfaceFromDC(HDC hdc, LPDIRECTDRAWSURFACE7 *pSurf);
    HRESULT STDMETHODCALLTYPE RestoreAllSurfaces();
    HRESULT STDMETHODCALLTYPE TestCooperativeLevel();
    HRESULT STDMETHODCALLTYPE GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2 pDDDI, DWORD dwFlags);
    HRESULT STDMETHODCALLTYPE StartModeTest(LPSIZE pModes, DWORD dwNumModes, DWORD dwFlags);
    HRESULT STDMETHODCALLTYPE EvaluateMode(DWORD dwFlags, DWORD  *pTimeout);
};

    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::Compact()
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->Compact();
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::CreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER *lplpDDClipper, IUnknown *pUnkOuter)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->CreateClipper(dwFlags, lplpDDClipper, pUnkOuter);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette, IUnknown *pUnkOuter)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->CreatePalette(dwFlags, lpColorTable, lplpDDPalette, pUnkOuter);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::CreateSurface(LPDDSURFACEDESC2 lpDDSurfaceDesc, LPDIRECTDRAWSURFACE7 *lplpDDSurface, IUnknown *pUnkOuter)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->CreateSurface(lpDDSurfaceDesc, lplpDDSurface, pUnkOuter);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::DuplicateSurface(LPDIRECTDRAWSURFACE7 lpDDSurface, LPDIRECTDRAWSURFACE7 *lplpDupDDSurface)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->DuplicateSurface(lpDDSurface, lplpDupDDSurface);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext, LPDDENUMMODESCALLBACK2 lpEnumModesCallback)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->EnumDisplayModes(dwFlags, lpDDSurfaceDesc, lpContext, lpEnumModesCallback);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSD, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->EnumSurfaces(dwFlags, lpDDSD, lpContext, lpEnumSurfacesCallback);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::FlipToGDISurface()
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->FlipToGDISurface();
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetCaps(LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetCaps(lpDDDriverCaps, lpDDHELCaps);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetDisplayMode(LPDDSURFACEDESC2 lpDDSurfaceDesc)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetDisplayMode(lpDDSurfaceDesc);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetFourCCCodes(LPDWORD lpNumCodes, LPDWORD lpCodes)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetFourCCCodes(lpNumCodes, lpCodes);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetGDISurface(LPDIRECTDRAWSURFACE7 *lplpGDIDDSurface)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetGDISurface(lplpGDIDDSurface);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetMonitorFrequency(LPDWORD lpdwFrequency)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetMonitorFrequency(lpdwFrequency);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetScanLine(LPDWORD lpdwScanLine)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetScanLine(lpdwScanLine);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetVerticalBlankStatus(WINBOOL *lpbIsInVB)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetVerticalBlankStatus(lpbIsInVB);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::Initialize(GUID *lpGUID)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->Initialize(lpGUID);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::RestoreDisplayMode()
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->RestoreDisplayMode();
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->SetCooperativeLevel(hWnd, dwFlags);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwRefreshRate, DWORD dwFlags)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->SetDisplayMode(dwWidth, dwHeight, dwBPP, dwRefreshRate, dwFlags);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->WaitForVerticalBlank(dwFlags, hEvent);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetAvailableVidMem(LPDDSCAPS2 lpDDCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetAvailableVidMem(lpDDCaps, lpdwTotal, lpdwFree);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetSurfaceFromDC(HDC hdc, LPDIRECTDRAWSURFACE7 *pSurf)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetSurfaceFromDC(hdc, pSurf);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::RestoreAllSurfaces()
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->RestoreAllSurfaces();
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::TestCooperativeLevel()
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->TestCooperativeLevel();
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2 pDDDI, DWORD dwFlags)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetDeviceIdentifier(pDDDI, dwFlags);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::StartModeTest(LPSIZE pModes, DWORD dwNumModes, DWORD dwFlags)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->StartModeTest(pModes, dwNumModes, dwFlags);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::EvaluateMode(DWORD dwFlags, DWORD *pTimeout)
    {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->EvaluateMode(dwFlags, pTimeout);
    }

#define VERTEX_BATCH_SIZE_INITIAL 16
#define VERTEX_BATCH_SIZE_MAX     4096

struct vertex_batch {
    D3DPRIMITIVETYPE primitive_type;
    DWORD fvf;
    UINT stride;
    char *vertices;

    /* vertex_count arg has a DWORD type in D3D calls,
     * but we won't go beyond an unsigned int capacity, as documentation says
     * it is limited to D3DMAXNUMVERTICES (65,535) */
    unsigned int vertex_count;
    unsigned int vertex_pos;
    unsigned int vertex_batch_size;
};

class Direct3DDevicePatched : public IDirect3DDevice7
{
    IDirect3DDevice7* mWrapped;


static HRESULT ddraw_buffer_add(Direct3DDevicePatched *device, D3DPRIMITIVETYPE primitive_type, DWORD fvf, void *vertices, DWORD vertex_count, DWORD flags, UINT stride);
static void ddraw_buffer_add_indices_list(Direct3DDevicePatched *device, void* vertices, DWORD vertex_count);

public:

static HRESULT ddraw_buffer_flush(Direct3DDevicePatched *device);
    /* Vertex buffer for squashing DrawPrimitive() calls before sending it to Direct3D7 */
    struct vertex_batch vertex_batch;

    Direct3DDevicePatched(IDirect3DDevice7* wrapped) : mWrapped(wrapped) {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	vertex_batch.vertex_count = 0;
	vertex_batch.vertex_batch_size = VERTEX_BATCH_SIZE_INITIAL;
	vertex_batch.vertices = 0;
    }

    /* IUnknown */
    HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid , void** ppvObject) {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
        return mWrapped->QueryInterface(riid, ppvObject);
    }
    ULONG STDMETHODCALLTYPE AddRef() { return 0; }
    ULONG STDMETHODCALLTYPE Release() { return 0; }

    /* IDirect3DDevice7 */
   HRESULT STDMETHODCALLTYPE GetCaps(D3DDEVICEDESC7 *desc);
   HRESULT STDMETHODCALLTYPE EnumTextureFormats(LPD3DENUMPIXELFORMATSCALLBACK cb, void *ctx);
   HRESULT STDMETHODCALLTYPE BeginScene();
   HRESULT STDMETHODCALLTYPE EndScene();
   HRESULT STDMETHODCALLTYPE GetDirect3D(IDirect3D7 **d3d);
   HRESULT STDMETHODCALLTYPE SetRenderTarget(IDirectDrawSurface7 *surface, DWORD flags);
   HRESULT STDMETHODCALLTYPE GetRenderTarget(IDirectDrawSurface7 **surface);
   HRESULT STDMETHODCALLTYPE Clear(DWORD count, D3DRECT *rects, DWORD flags, D3DCOLOR color, D3DVALUE z, DWORD stencil);
   HRESULT STDMETHODCALLTYPE SetTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix);
   HRESULT STDMETHODCALLTYPE GetTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix);
   HRESULT STDMETHODCALLTYPE SetViewport(D3DVIEWPORT7 *data);
   HRESULT STDMETHODCALLTYPE MultiplyTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix);
   HRESULT STDMETHODCALLTYPE GetViewport(D3DVIEWPORT7 *data);
   HRESULT STDMETHODCALLTYPE SetMaterial(D3DMATERIAL7 *data);
   HRESULT STDMETHODCALLTYPE GetMaterial(D3DMATERIAL7 *data);
   HRESULT STDMETHODCALLTYPE SetLight(DWORD idx, D3DLIGHT7 *data);
   HRESULT STDMETHODCALLTYPE GetLight(DWORD idx, D3DLIGHT7 *data);
   HRESULT STDMETHODCALLTYPE SetRenderState(D3DRENDERSTATETYPE dwRenderStateType, DWORD dwRenderState);
   HRESULT STDMETHODCALLTYPE GetRenderState(D3DRENDERSTATETYPE dwRenderStateType, LPDWORD lpdwRenderState);
   HRESULT STDMETHODCALLTYPE BeginStateBlock();
   HRESULT STDMETHODCALLTYPE EndStateBlock(LPDWORD lpdwBlockHandle);
   HRESULT STDMETHODCALLTYPE PreLoad(IDirectDrawSurface7 *surface);
   HRESULT STDMETHODCALLTYPE DrawPrimitive(D3DPRIMITIVETYPE primitive_type, DWORD fvf, void *vertices, DWORD vertex_count, DWORD flags);
   HRESULT STDMETHODCALLTYPE DrawIndexedPrimitive(D3DPRIMITIVETYPE primitive_type, DWORD fvf, void *vertices, DWORD vertex_count, WORD *indices, DWORD index_count, DWORD flags);
   HRESULT STDMETHODCALLTYPE SetClipStatus(D3DCLIPSTATUS *clip_status);
   HRESULT STDMETHODCALLTYPE GetClipStatus(D3DCLIPSTATUS *clip_status);
   HRESULT STDMETHODCALLTYPE DrawPrimitiveStrided(D3DPRIMITIVETYPE primitive_type, DWORD fvf, D3DDRAWPRIMITIVESTRIDEDDATA *strided_data, DWORD vertex_count, DWORD flags);
   HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE primitive_type, DWORD fvf, D3DDRAWPRIMITIVESTRIDEDDATA *strided_data, DWORD vertex_count, WORD *indices, DWORD index_count, DWORD flags);
   HRESULT STDMETHODCALLTYPE DrawPrimitiveVB(D3DPRIMITIVETYPE primitive_type, struct IDirect3DVertexBuffer7 *vb, DWORD start_vertex, DWORD vertex_count, DWORD flags);
   HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE primitive_type, struct IDirect3DVertexBuffer7 *vb, DWORD start_vertex, DWORD vertex_count, WORD *indices, DWORD index_count, DWORD flags);
   HRESULT STDMETHODCALLTYPE ComputeSphereVisibility(D3DVECTOR *centers, D3DVALUE *radii, DWORD sphere_count, DWORD flags, DWORD *ret);
   HRESULT STDMETHODCALLTYPE GetTexture(DWORD stage, IDirectDrawSurface7 **surface);
   HRESULT STDMETHODCALLTYPE SetTexture(DWORD stage, IDirectDrawSurface7 *surface);
   HRESULT STDMETHODCALLTYPE GetTextureStageState(DWORD dwStage,D3DTEXTURESTAGESTATETYPE d3dTexStageStateType,LPDWORD lpdwState);
   HRESULT STDMETHODCALLTYPE SetTextureStageState(DWORD dwStage,D3DTEXTURESTAGESTATETYPE d3dTexStageStateType,DWORD dwState);
   HRESULT STDMETHODCALLTYPE ValidateDevice(LPDWORD lpdwPasses);
   HRESULT STDMETHODCALLTYPE ApplyStateBlock(DWORD dwBlockHandle);
   HRESULT STDMETHODCALLTYPE CaptureStateBlock(DWORD dwBlockHandle);
   HRESULT STDMETHODCALLTYPE DeleteStateBlock(DWORD dwBlockHandle);
   HRESULT STDMETHODCALLTYPE CreateStateBlock(D3DSTATEBLOCKTYPE d3dsbType,LPDWORD lpdwBlockHandle);
   HRESULT STDMETHODCALLTYPE Load(IDirectDrawSurface7 *dst_surface, POINT *dst_point, IDirectDrawSurface7 *src_surface, RECT *src_rect, DWORD flags);
   HRESULT STDMETHODCALLTYPE LightEnable(DWORD dwLightIndex,WINBOOL bEnable);
   HRESULT STDMETHODCALLTYPE GetLightEnable(DWORD dwLightIndex,WINBOOL *pbEnable);
   HRESULT STDMETHODCALLTYPE SetClipPlane(DWORD dwIndex,D3DVALUE *pPlaneEquation);
   HRESULT STDMETHODCALLTYPE GetClipPlane(DWORD dwIndex,D3DVALUE *pPlaneEquation);
   HRESULT STDMETHODCALLTYPE GetInfo(DWORD info_id, void *info, DWORD info_size);

   // Adding an extra, unbuffered DrawPrimitive()
   HRESULT STDMETHODCALLTYPE DrawPrimitiveUnbuffered(D3DPRIMITIVETYPE primitive_type, DWORD fvf, void *vertices, DWORD vertex_count, DWORD flags);
};

static HRESULT ddraw_buffer_flush_if_needed(Direct3DDevicePatched *device) {
    /* Nothing to do if it is empty */
    if (! device)
	    return D3D_OK;
    if (! device->vertex_batch.vertex_count)
	    return D3D_OK;

    return Direct3DDevicePatched::ddraw_buffer_flush(device);
}

class Direct3D7Patched : public IDirect3D7
{
    IDirect3D7* mWrapped;
    IDirect3DDevice7 *new_d3dDevice7 = nullptr;

public:
    Direct3D7Patched(IDirect3D7* wrapped) : mWrapped(wrapped) {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    }

    /* IUnknown */
    HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid, void** ppvObject) {
        WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
        return mWrapped->QueryInterface(riid, ppvObject);
    }

    ULONG STDMETHODCALLTYPE AddRef() { return 0; }
    ULONG STDMETHODCALLTYPE Release() { return 0; }

    /* IDirect3D7 */
    HRESULT STDMETHODCALLTYPE EnumDevices(LPD3DENUMDEVICESCALLBACK7, void*);
    HRESULT STDMETHODCALLTYPE CreateDevice(const IID&, IDirectDrawSurface7*, IDirect3DDevice7**);
    HRESULT STDMETHODCALLTYPE CreateVertexBuffer(D3DVERTEXBUFFERDESC*, IDirect3DVertexBuffer7**, DWORD);
    HRESULT STDMETHODCALLTYPE EnumZBufferFormats(const IID&, LPD3DENUMPIXELFORMATSCALLBACK, void*);
    HRESULT STDMETHODCALLTYPE EvictManagedTextures();
};

void ddraw_buffer_histogram(Direct3DDevicePatched *device);

/* Implementation of the wrapper */
HRESULT Direct3DDevicePatched::BeginScene() { WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->BeginScene(); }
HRESULT Direct3DDevicePatched::EndScene() { WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	ddraw_buffer_flush_if_needed(this);
    ddraw_buffer_histogram(this);
    return mWrapped->EndScene(); }

HRESULT Direct3DDevicePatched::GetCaps(D3DDEVICEDESC7 *desc) { WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetCaps(desc); }

HRESULT Direct3DDevicePatched::EnumTextureFormats(LPD3DENUMPIXELFORMATSCALLBACK cb, void *ctx)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->EnumTextureFormats(cb, ctx);
}

HRESULT Direct3DDevicePatched::GetDirect3D(IDirect3D7 **d3d)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetDirect3D(d3d);
}
HRESULT Direct3DDevicePatched::SetRenderTarget(IDirectDrawSurface7 *surface, DWORD flags)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	ddraw_buffer_flush_if_needed(this);
    return mWrapped->SetRenderTarget(surface, flags);
}
HRESULT Direct3DDevicePatched::GetRenderTarget(IDirectDrawSurface7 **surface)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetRenderTarget(surface);
}
HRESULT Direct3DDevicePatched::Clear(DWORD count, D3DRECT *rects, DWORD flags, D3DCOLOR color, D3DVALUE z, DWORD stencil)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->Clear(count, rects, flags, color, z, stencil);
}

HRESULT Direct3DDevicePatched::SetTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	ddraw_buffer_flush_if_needed(this);
    return mWrapped->SetTransform(state, matrix);
}
HRESULT Direct3DDevicePatched::GetTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetTransform(state, matrix);
}
HRESULT Direct3DDevicePatched::SetViewport(D3DVIEWPORT7 *data)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	ddraw_buffer_flush_if_needed(this);
    return mWrapped->SetViewport(data);
}
HRESULT Direct3DDevicePatched::MultiplyTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	ddraw_buffer_flush_if_needed(this);
    return mWrapped->MultiplyTransform(state, matrix);
}
HRESULT Direct3DDevicePatched::GetViewport(D3DVIEWPORT7 *data)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetViewport(data);
}
HRESULT Direct3DDevicePatched::SetMaterial(D3DMATERIAL7 *data)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	ddraw_buffer_flush_if_needed(this);
    return mWrapped->SetMaterial(data);
}
HRESULT Direct3DDevicePatched::GetMaterial(D3DMATERIAL7 *data)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetMaterial(data);
}
HRESULT Direct3DDevicePatched::SetLight(DWORD idx, D3DLIGHT7 *data)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	ddraw_buffer_flush_if_needed(this);
    return mWrapped->SetLight(idx, data);
}
HRESULT Direct3DDevicePatched::GetLight(DWORD idx, D3DLIGHT7 *data)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetLight(idx, data);
}
HRESULT Direct3DDevicePatched::SetRenderState(D3DRENDERSTATETYPE dwRenderStateType, DWORD dwRenderState)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	ddraw_buffer_flush_if_needed(this);
    return mWrapped->SetRenderState(dwRenderStateType, dwRenderState);
}
HRESULT Direct3DDevicePatched::GetRenderState(D3DRENDERSTATETYPE dwRenderStateType, LPDWORD lpdwRenderState)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetRenderState(dwRenderStateType, lpdwRenderState);
}
HRESULT Direct3DDevicePatched::BeginStateBlock()
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->BeginStateBlock();
}
HRESULT Direct3DDevicePatched::EndStateBlock(LPDWORD lpdwBlockHandle)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->EndStateBlock(lpdwBlockHandle);
}
HRESULT Direct3DDevicePatched::PreLoad(IDirectDrawSurface7 *surface)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->PreLoad(surface);
}
HRESULT Direct3DDevicePatched::DrawPrimitive(D3DPRIMITIVETYPE primitive_type, DWORD fvf, void *vertices, DWORD vertex_count, DWORD flags)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);

    UINT stride = get_flexible_vertex_size(fvf);
    HRESULT hr = ddraw_buffer_add(this, primitive_type, fvf, vertices, vertex_count, flags, stride);

    /* buffered, return immediatly */
    if (hr == D3D_OK) return D3D_OK;

    /* buffering failed, delegate */
    return mWrapped->DrawPrimitive(primitive_type, fvf, vertices, vertex_count, flags);
}
HRESULT Direct3DDevicePatched::DrawIndexedPrimitive(D3DPRIMITIVETYPE primitive_type, DWORD fvf, void *vertices, DWORD vertex_count, WORD *indices, DWORD index_count, DWORD flags)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->DrawIndexedPrimitive(primitive_type, fvf, vertices, vertex_count, indices, index_count, flags);
}
HRESULT Direct3DDevicePatched::SetClipStatus(D3DCLIPSTATUS *clip_status)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->SetClipStatus(clip_status);
}
HRESULT Direct3DDevicePatched::GetClipStatus(D3DCLIPSTATUS *clip_status)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->SetClipStatus(clip_status);
}
HRESULT Direct3DDevicePatched::DrawPrimitiveStrided(D3DPRIMITIVETYPE primitive_type, DWORD fvf, D3DDRAWPRIMITIVESTRIDEDDATA *strided_data, DWORD vertex_count, DWORD flags)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->DrawPrimitiveStrided(primitive_type, fvf, strided_data, vertex_count, flags);
}
HRESULT Direct3DDevicePatched::DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE primitive_type, DWORD fvf, D3DDRAWPRIMITIVESTRIDEDDATA *strided_data, DWORD vertex_count, WORD *indices, DWORD index_count, DWORD flags)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->DrawIndexedPrimitiveStrided(primitive_type, fvf, strided_data, vertex_count, indices, index_count, flags);
}
HRESULT Direct3DDevicePatched::DrawPrimitiveVB(D3DPRIMITIVETYPE primitive_type, struct IDirect3DVertexBuffer7 *vb, DWORD start_vertex, DWORD vertex_count, DWORD flags)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->DrawPrimitiveVB(primitive_type, vb, start_vertex, vertex_count, flags);
}
HRESULT Direct3DDevicePatched::DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE primitive_type, struct IDirect3DVertexBuffer7 *vb, DWORD start_vertex, DWORD vertex_count, WORD *indices, DWORD index_count, DWORD flags)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->DrawIndexedPrimitiveVB(primitive_type, vb, start_vertex, vertex_count, indices, index_count, flags);
}
HRESULT Direct3DDevicePatched::ComputeSphereVisibility(D3DVECTOR *centers, D3DVALUE *radii, DWORD sphere_count, DWORD flags, DWORD *ret)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->ComputeSphereVisibility(centers, radii, sphere_count, flags, ret);
}
HRESULT Direct3DDevicePatched::GetTexture(DWORD stage, IDirectDrawSurface7 **surface)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetTexture(stage, surface);
}
HRESULT Direct3DDevicePatched::SetTexture(DWORD stage, IDirectDrawSurface7 *surface)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	ddraw_buffer_flush_if_needed(this);
    return mWrapped->SetTexture(stage, surface);
}
HRESULT Direct3DDevicePatched::GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE d3dTexStageStateType,LPDWORD lpdwState)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetTextureStageState(dwStage, d3dTexStageStateType, lpdwState);
}
HRESULT Direct3DDevicePatched::SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE d3dTexStageStateType,DWORD dwState)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	ddraw_buffer_flush_if_needed(this);
    return mWrapped->SetTextureStageState(dwStage, d3dTexStageStateType, dwState);
}
HRESULT Direct3DDevicePatched::ValidateDevice(LPDWORD lpdwPasses)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->ValidateDevice(lpdwPasses);
}
HRESULT Direct3DDevicePatched::ApplyStateBlock(DWORD dwBlockHandle)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->ApplyStateBlock(dwBlockHandle);
}
HRESULT Direct3DDevicePatched::CaptureStateBlock(DWORD dwBlockHandle)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->CaptureStateBlock(dwBlockHandle);
}
HRESULT Direct3DDevicePatched::DeleteStateBlock(DWORD dwBlockHandle)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->DeleteStateBlock(dwBlockHandle);
}
HRESULT Direct3DDevicePatched::CreateStateBlock(D3DSTATEBLOCKTYPE d3dsbType,LPDWORD lpdwBlockHandle)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->CreateStateBlock(d3dsbType, lpdwBlockHandle);
}
HRESULT Direct3DDevicePatched::Load(IDirectDrawSurface7 *dst_surface, POINT *dst_point, IDirectDrawSurface7 *src_surface, RECT *src_rect, DWORD flags)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->Load(dst_surface, dst_point, src_surface, src_rect, flags);
}
HRESULT Direct3DDevicePatched::LightEnable(DWORD dwLightIndex,WINBOOL bEnable)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
	ddraw_buffer_flush_if_needed(this);
    return mWrapped->LightEnable(dwLightIndex, bEnable);
}
HRESULT Direct3DDevicePatched::GetLightEnable(DWORD dwLightIndex,WINBOOL *pbEnable)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetLightEnable(dwLightIndex, pbEnable);
}
HRESULT Direct3DDevicePatched::SetClipPlane(DWORD dwIndex,D3DVALUE *pPlaneEquation)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->SetClipPlane(dwIndex, pPlaneEquation);
}
HRESULT Direct3DDevicePatched::GetClipPlane(DWORD dwIndex,D3DVALUE *pPlaneEquation)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetClipPlane(dwIndex, pPlaneEquation);
}
HRESULT Direct3DDevicePatched::GetInfo(DWORD info_id, void *info, DWORD info_size)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->GetInfo(info_id, info, info_size);
}

/***/

HRESULT Direct3D7Patched::EnumDevices(LPD3DENUMDEVICESCALLBACK7 cb, void *ctx)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->EnumDevices(cb, ctx);
}
HRESULT Direct3D7Patched::CreateVertexBuffer(D3DVERTEXBUFFERDESC *desc, struct IDirect3DVertexBuffer7 **buffer, DWORD flags)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->CreateVertexBuffer(desc,  buffer, flags);
}
HRESULT Direct3D7Patched::EnumZBufferFormats(REFCLSID device_iid, LPD3DENUMPIXELFORMATSCALLBACK cb, void *ctx)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->EnumZBufferFormats(device_iid, cb, ctx);
}
HRESULT Direct3D7Patched::EvictManagedTextures()
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->EvictManagedTextures();
}

/* wrapping setup on demand */
static IDirectDraw7 *new_directDraw7 = nullptr;

LPDIRECTDRAWCREATEEX orig_DirectDrawCreateEx;
HRESULT WINAPI DirectDrawCreateEx (GUID FAR *lpGUID, LPVOID* lplpDD, REFIID iid, IUnknown FAR *pUnkOuter ) {
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);

    if (! orig_DirectDrawCreateEx) {
        orig_hInstDDraw = LoadLibraryW( L"C:\\Windows\\System32\\ddraw.dll" );
        if(! orig_hInstDDraw) {
            WRAP("%s:%d \t LoadLibraryW(ddraw.dll) Failed\n", __FILE__, __LINE__);
            return FALSE;
        }

        orig_DirectDrawCreateEx = (LPDIRECTDRAWCREATEEX) GetProcAddress( orig_hInstDDraw, "DirectDrawCreateEx" );
        if(! orig_DirectDrawCreateEx) {
            WRAP("%s:%d \t GetProcAddress(ddraw.DirectDrawCreateEx) Failed\n", __FILE__, __LINE__);
            return FALSE;
        }
    }

    HRESULT r = orig_DirectDrawCreateEx(lpGUID, (LPVOID*) (&orig_lpDD7), iid, pUnkOuter);
    WRAP("%s:%d \t orig_lpDD7:%p\n", __FILE__, __LINE__, orig_lpDD7);

    new_directDraw7 = new DirectDraw7Patched(orig_lpDD7);

    *lplpDD = new_directDraw7;
    return r;
}

HRESULT STDMETHODCALLTYPE DirectDraw7Patched::QueryInterface(const IID& riid , void** ppvObject)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);

    {
        LPOLESTR lplpsz;
        StringFromIID(riid, &lplpsz);
        WRAP("QueryInterface(%ls)\n", lplpsz);
    }

    IDirect3D7 *orig_direct3D7;
    HRESULT r = mWrapped->QueryInterface(riid, (void**) &orig_direct3D7);

    // Assume it is IDirect3D7
    new_direct3D7 = new Direct3D7Patched(orig_direct3D7);

    *ppvObject = new_direct3D7;
    return r;
}

HRESULT Direct3D7Patched::CreateDevice(REFCLSID rclsid, IDirectDrawSurface7 *surface, struct IDirect3DDevice7 **device)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);

    {
        LPOLESTR lplpsz;
        StringFromIID(rclsid, &lplpsz);
        WRAP("CreateDevice(%ls)\n", lplpsz);
    }

    HRESULT r = S_OK;
    if (new_d3dDevice7) goto cached;

    struct IDirect3DDevice7 *orig_device;
    r = mWrapped->CreateDevice(rclsid, surface, &orig_device);

    new_d3dDevice7 = new Direct3DDevicePatched(orig_device);

cached:
    *device = new_d3dDevice7;
    return r;
}

/** Buffering **/

/* static counters. No need to protect them too much, it's just for basics stats
 * therefore their performance is much more important than accurary */
static unsigned int buffer_adds = 0;
static unsigned int buffer_flushs = 0;

/* Flushing the buffer if it isn't empty.
 *
 * It will delegate to a single call to wined3d with the correct parameters,
 * and a (hopefully) huge list of vertices/indices. */
HRESULT Direct3DDevicePatched::ddraw_buffer_flush(Direct3DDevicePatched *device)
{
    INFO("ddraw_buffer_flush: device %p, vertex_count %lu\n", device, device->vertex_batch.vertex_count);
    HRESULT hr;

    buffer_flushs ++;

    // Delegate the call with the full buffer
    {
	D3DPRIMITIVETYPE primitive_type = device->vertex_batch.primitive_type;
	if (primitive_type == D3DPT_TRIANGLEFAN) primitive_type = D3DPT_TRIANGLELIST;
	DWORD fvf = device->vertex_batch.fvf;
	void *vertices = device->vertex_batch.vertices;
	DWORD vertex_count = device->vertex_batch.vertex_count;
	DWORD flags = 0;

	hr = device->mWrapped->DrawPrimitive(primitive_type, fvf, vertices, vertex_count, flags);
    }

done:

//    free(device->vertex_batch.vertices);
//    device->vertex_batch.vertices = NULL;

    /* Reset the buffer */
    device->vertex_batch.vertex_count = 0;

    return hr;
}

void Direct3DDevicePatched::ddraw_buffer_add_indices_list(Direct3DDevicePatched *device, void* vertices, DWORD vertex_count) {
    UINT stride = device->vertex_batch.stride;

    /* All the points are already in the same order, copy them over */
    memcpy(device->vertex_batch.vertices + device->vertex_batch.vertex_count * stride, vertices, vertex_count * stride);
    device->vertex_batch.vertex_count += vertex_count;
}

static void ddraw_buffer_add_indices_trianglefan(Direct3DDevicePatched *device, void* vertices, DWORD vertex_count) {
    UINT stride = device->vertex_batch.stride;
    unsigned int idx;

    /* The first triangle is the same, therefore the vertexesh are simply copied over */
    memcpy(device->vertex_batch.vertices + device->vertex_batch.vertex_count * stride, vertices, 3 * stride);
    device->vertex_batch.vertex_count += 3;

    /* Next triangles are recreated with : 2 next vertices then the 1rst one.
     * So, it will *increase* the number of total vertices from 4 to 6, 5 to 9, 6 to 12, ... */
    for (idx = 2; idx < vertex_count-1; idx ++) {
        char* next_vertice = (char*) vertices + idx * stride;

        /* Copy the 2 next ones */
        memcpy(device->vertex_batch.vertices + device->vertex_batch.vertex_count * stride, next_vertice, 2 * stride);
        device->vertex_batch.vertex_count += 2;

        /* Copy the first one again */
        memcpy(device->vertex_batch.vertices + device->vertex_batch.vertex_count * stride, vertices, stride);
        device->vertex_batch.vertex_count += 1;
    }
}

/*
 * Note : Adding to the buffer transforms the D3DPT_TRIANGLEFAN primitive into a D3DPT_TRIANGLELIST.
 * Otherwise, we cannot concatenate them as TRIANGLE FAN has the first vertex in common to the whole list.
 */
HRESULT Direct3DDevicePatched::ddraw_buffer_add(Direct3DDevicePatched *device, D3DPRIMITIVETYPE primitive_type, DWORD fvf, void *vertices, DWORD vertex_count, DWORD flags, UINT stride) {
    HRESULT hr;

    TRACE("device %p, primitive_type %#x, fvf %#lx, vertices %p, vertex_count %lu, flags %#lx, stride %u.\n",
            device, primitive_type, fvf, vertices, vertex_count, flags, stride);
    buffer_adds ++;

    int orig_vertex_batch_size = device->vertex_batch.vertex_batch_size;

    if (device->vertex_batch.vertex_count) {
        /* if already-buffered vertexes do not match the one that we want to add, flush. */
        if (primitive_type != device->vertex_batch.primitive_type) {
            INFO("Buffering failed due to mismatched primitive_type %d != buffer.primitive_type %d \n", primitive_type, device->vertex_batch.primitive_type);
            ddraw_buffer_flush(device);
        } else if (fvf != device->vertex_batch.fvf) {
            INFO("Buffering failed due to mismatched fvf %ld != buffer.fvf %ld \n", fvf, device->vertex_batch.fvf);
            ddraw_buffer_flush(device);
        } else if (device->vertex_batch.vertex_count + vertex_count * 2 > device->vertex_batch.vertex_batch_size) {
            /* We double the number of vertices to add since
             * - it is a very fast mul
             * - the number will never more than double
             * - the precision it offers is good enough */
            FIXME("Full buffer vertex_batch.vertex_count %u, vertex_count %lu, vertex_batch_size %u \n",
			    device->vertex_batch.vertex_count, vertex_count, device->vertex_batch.vertex_batch_size);
            ddraw_buffer_flush(device);

	    /* Adding 30% until quite big */
	    if (device->vertex_batch.vertex_batch_size < VERTEX_BATCH_SIZE_MAX)
		    device->vertex_batch.vertex_batch_size *= 1.3;

        }
    }

    switch(primitive_type) {
        case D3DPT_TRIANGLEFAN:
        case D3DPT_LINELIST:
        case D3DPT_POINTLIST:
            /* Supported primitives */
            break;
        default:
            FIXME("primitive_type %#x is not supported, not buffering\n", primitive_type);
            goto fail;
    }

    /* Need to test again vertex_count as a flush resets it to 0 */
    if (! device->vertex_batch.vertex_count) {
        /* New buffer, setting everything up */
        device->vertex_batch.primitive_type = primitive_type;
        device->vertex_batch.fvf = fvf;
        device->vertex_batch.stride = stride;
	if (device->vertex_batch.vertices == 0 || device->vertex_batch.vertex_batch_size != orig_vertex_batch_size)
		device->vertex_batch.vertices = (char*) realloc(device->vertex_batch.vertices, device->vertex_batch.vertex_batch_size * stride);
    }

    /* Create the index */
    switch(primitive_type) {
        case D3DPT_TRIANGLEFAN:
            if (vertex_count < 3) {
                WARN("vertex_count %lu lower than 3. not buffering.\n", vertex_count);
                goto fail;
            }
            ddraw_buffer_add_indices_trianglefan(device, vertices, vertex_count);
            break;
        case D3DPT_TRIANGLELIST:
        case D3DPT_LINELIST:
        case D3DPT_POINTLIST:
            ddraw_buffer_add_indices_list(device, vertices, vertex_count);
            break;
        default:
            ERR("primitive_type %#x not supported\n", primitive_type);
    }

    /* Buffered ! */
    return D3D_OK;

fail:
    if (device->vertex_batch.vertices) {
//        device->vertex_batch.vertices = NULL;
    }
    return E_NOTIMPL;
}

struct CUSTOMVERTEX
{
    FLOAT x, y, z, rhw;
    DWORD color;
};

#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
static DWORD D3DCOLOR_ARGB(char a, char r, char g, char b) {
    DWORD d = 0;
    d |= a <<  0;
    d |= r <<  8;
    d |= g << 16;
    d |= b << 24;
    return d;
}

// #define D3DCOLOR_ARGB(x, ...)

static float histogram[256] = {0};

void ddraw_buffer_histogram(Direct3DDevicePatched *device) {
    static int histo_idx = -1;
    histo_idx = (histo_idx + 1) % 256;
    histogram[histo_idx] = device->vertex_batch.vertex_count;

    CUSTOMVERTEX vertices[512];

    for (int i = 0; i < 256; i ++) {
        int vtx_idx = i*2;
        vertices[vtx_idx].x = 0.0 + i;
        vertices[vtx_idx].y = 0.0;
        vertices[vtx_idx].z = -1.f;
        vertices[vtx_idx].rhw = 1.f;
        vertices[vtx_idx].color = D3DCOLOR_ARGB(255, 0, 255, 0);

        vtx_idx ++;
        vertices[vtx_idx].x = 0.f + i;
        vertices[vtx_idx].y = 0.f + histogram[i];
        vertices[vtx_idx].z = -1.f;
        vertices[vtx_idx].rhw = 1.f;
        vertices[vtx_idx].color = D3DCOLOR_ARGB(0, 0, 255, 0);
    }

    INFO("ddraw_buffer_histogram");

    device->DrawPrimitiveUnbuffered(D3DPT_LINELIST, CUSTOMFVF, vertices, 512, NULL);
}

HRESULT Direct3DDevicePatched::DrawPrimitiveUnbuffered(D3DPRIMITIVETYPE primitive_type, DWORD fvf, void *vertices, DWORD vertex_count, DWORD flags)
{
    WRAP("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    return mWrapped->DrawPrimitive(primitive_type, fvf, vertices, vertex_count, flags);
}

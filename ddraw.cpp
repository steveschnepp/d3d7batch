#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdarg.h>

#define INITGUID
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

//#define STDMETHODCALLTYPE WINAPI
//typedef unsigned long ULONG;

// extern "C" {

static void log_printf(const char* format, ...) {
    va_list args;
    char buf[2048];

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    OutputDebugString(buf);
}

typedef HRESULT ( WINAPI* LPDIRECTDRAWCREATEEX )( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter );

HINSTANCE orig_hInstDDraw;
LPDIRECTDRAW7 orig_lpDD7;

class Direct3D7Patched;
class Direct3DDevicePatched;


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    const char *DllMain_REASON[] = { "DLL_PROCESS_DETACH", "DLL_PROCESS_ATTACH", "DLL_THREAD_ATTACH", "DLL_THREAD_DETACH" };

    log_printf("%s:%d \t DllMain(fdwReason: %s)\n", __FILE__, __LINE__, DllMain_REASON[fdwReason]);
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            return TRUE;
    }

    return TRUE;
}

HRESULT WINAPI 	DirectDrawCreate(LPGUID lpGUID, LPDIRECTDRAW *lplpDD, LPUNKNOWN pUnkOuter) {
    return DDERR_UNSUPPORTED;
}

// } // extern C

#define Direct3DDevicePatched_Impl 1
#define Direct3D7Patched_Impl 1
#define DirectDraw7Patched_Impl 1

class DirectDraw7Patched : public IDirectDraw7
{
    IDirectDraw7* mWrapped;

    IDirect3D7 *new_direct3D7 = nullptr;
    IDirect3DDevice7 *new_d3dDevice7 = nullptr;

public:
    DirectDraw7Patched(IDirectDraw7* wrapped) : mWrapped(wrapped) {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    }

    /* IUnknown */
    HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid, void** ppvObject);

    ULONG STDMETHODCALLTYPE AddRef() { return 0; }
    ULONG STDMETHODCALLTYPE Release() { return 0; }

#if DirectDraw7Patched_Impl
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
#endif
};

#if DirectDraw7Patched_Impl
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::Compact()
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->Compact();
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::CreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER *lplpDDClipper, IUnknown *pUnkOuter)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->CreateClipper(dwFlags, lplpDDClipper, pUnkOuter);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE *lplpDDPalette, IUnknown *pUnkOuter)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->CreatePalette(dwFlags, lpColorTable, lplpDDPalette, pUnkOuter);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::CreateSurface(LPDDSURFACEDESC2 lpDDSurfaceDesc, LPDIRECTDRAWSURFACE7 *lplpDDSurface, IUnknown *pUnkOuter)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->CreateSurface(lpDDSurfaceDesc, lplpDDSurface, pUnkOuter);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::DuplicateSurface(LPDIRECTDRAWSURFACE7 lpDDSurface, LPDIRECTDRAWSURFACE7 *lplpDupDDSurface)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->DuplicateSurface(lpDDSurface, lplpDupDDSurface);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext, LPDDENUMMODESCALLBACK2 lpEnumModesCallback)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->EnumDisplayModes(dwFlags, lpDDSurfaceDesc, lpContext, lpEnumModesCallback);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSD, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->EnumSurfaces(dwFlags, lpDDSD, lpContext, lpEnumSurfacesCallback);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::FlipToGDISurface()
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->FlipToGDISurface();
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetCaps(LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetCaps(lpDDDriverCaps, lpDDHELCaps);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetDisplayMode(LPDDSURFACEDESC2 lpDDSurfaceDesc)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetDisplayMode(lpDDSurfaceDesc);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetFourCCCodes(LPDWORD lpNumCodes, LPDWORD lpCodes)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetFourCCCodes(lpNumCodes, lpCodes);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetGDISurface(LPDIRECTDRAWSURFACE7 *lplpGDIDDSurface)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetGDISurface(lplpGDIDDSurface);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetMonitorFrequency(LPDWORD lpdwFrequency)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetMonitorFrequency(lpdwFrequency);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetScanLine(LPDWORD lpdwScanLine)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetScanLine(lpdwScanLine);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetVerticalBlankStatus(WINBOOL *lpbIsInVB)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetVerticalBlankStatus(lpbIsInVB);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::Initialize(GUID *lpGUID)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->Initialize(lpGUID);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::RestoreDisplayMode()
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->RestoreDisplayMode();
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetCooperativeLevel(hWnd, dwFlags);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwRefreshRate, DWORD dwFlags)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetDisplayMode(dwWidth, dwHeight, dwBPP, dwRefreshRate, dwFlags);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->WaitForVerticalBlank(dwFlags, hEvent);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetAvailableVidMem(LPDDSCAPS2 lpDDCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetAvailableVidMem(lpDDCaps, lpdwTotal, lpdwFree);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetSurfaceFromDC(HDC hdc, LPDIRECTDRAWSURFACE7 *pSurf)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetSurfaceFromDC(hdc, pSurf);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::RestoreAllSurfaces()
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->RestoreAllSurfaces();
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::TestCooperativeLevel()
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->TestCooperativeLevel();
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2 pDDDI, DWORD dwFlags)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetDeviceIdentifier(pDDDI, dwFlags);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::StartModeTest(LPSIZE pModes, DWORD dwNumModes, DWORD dwFlags)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->StartModeTest(pModes, dwNumModes, dwFlags);
    }
    HRESULT STDMETHODCALLTYPE DirectDraw7Patched::EvaluateMode(DWORD dwFlags, DWORD *pTimeout)
    {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->EvaluateMode(dwFlags, pTimeout);
    }
#endif

class Direct3DDevicePatched : public IDirect3DDevice7
{
    IDirect3DDevice7* mWrapped;

public:
    Direct3DDevicePatched(IDirect3DDevice7* wrapped) : mWrapped(wrapped) {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    }

    /* IUnknown */
    HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid , void** ppvObject) {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
        return mWrapped->QueryInterface(riid, ppvObject);
    }
    ULONG STDMETHODCALLTYPE AddRef() { return 0; }
    ULONG STDMETHODCALLTYPE Release() { return 0; }

#if Direct3DDevicePatched_Impl
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
#endif
};

class Direct3D7Patched : public IDirect3D7
{
    IDirect3D7* mWrapped;

public:
    Direct3D7Patched(IDirect3D7* wrapped) : mWrapped(wrapped) {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
    }

    /* IUnknown */
    HRESULT STDMETHODCALLTYPE QueryInterface(const IID& riid, void** ppvObject) {
        log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);
        return mWrapped->QueryInterface(riid, ppvObject);
    }

    ULONG STDMETHODCALLTYPE AddRef() { return 0; }
    ULONG STDMETHODCALLTYPE Release() { return 0; }

#if Direct3D7Patched_Impl
    /* IDirect3D7 */
    HRESULT STDMETHODCALLTYPE EnumDevices(LPD3DENUMDEVICESCALLBACK7, void*);
    HRESULT STDMETHODCALLTYPE CreateDevice(const IID&, IDirectDrawSurface7*, IDirect3DDevice7**);
    HRESULT STDMETHODCALLTYPE CreateVertexBuffer(D3DVERTEXBUFFERDESC*, IDirect3DVertexBuffer7**, DWORD);
    HRESULT STDMETHODCALLTYPE EnumZBufferFormats(const IID&, LPD3DENUMPIXELFORMATSCALLBACK, void*);
    HRESULT STDMETHODCALLTYPE EvictManagedTextures();
#endif
};


#if Direct3DDevicePatched_Impl

/* Implementation of the wrapper */
HRESULT Direct3DDevicePatched::BeginScene() { log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->BeginScene(); }
HRESULT Direct3DDevicePatched::EndScene() { log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->EndScene(); }

HRESULT Direct3DDevicePatched::GetCaps(D3DDEVICEDESC7 *desc) { log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetCaps(desc); }

HRESULT Direct3DDevicePatched::EnumTextureFormats(LPD3DENUMPIXELFORMATSCALLBACK cb, void *ctx)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->EnumTextureFormats(cb, ctx);
}

HRESULT Direct3DDevicePatched::GetDirect3D(IDirect3D7 **d3d)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetDirect3D(d3d);
}
HRESULT Direct3DDevicePatched::SetRenderTarget(IDirectDrawSurface7 *surface, DWORD flags)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetRenderTarget(surface, flags);
}
HRESULT Direct3DDevicePatched::GetRenderTarget(IDirectDrawSurface7 **surface)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetRenderTarget(surface);
}
HRESULT Direct3DDevicePatched::Clear(DWORD count, D3DRECT *rects, DWORD flags, D3DCOLOR color, D3DVALUE z, DWORD stencil)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->Clear(count, rects, flags, color, z, stencil);
}

HRESULT Direct3DDevicePatched::SetTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetTransform(state, matrix);
}
HRESULT Direct3DDevicePatched::GetTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetTransform(state, matrix);
}
HRESULT Direct3DDevicePatched::SetViewport(D3DVIEWPORT7 *data)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetViewport(data);
}
HRESULT Direct3DDevicePatched::MultiplyTransform(D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->MultiplyTransform(state, matrix);
}
HRESULT Direct3DDevicePatched::GetViewport(D3DVIEWPORT7 *data)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetViewport(data);
}
HRESULT Direct3DDevicePatched::SetMaterial(D3DMATERIAL7 *data)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetMaterial(data);
}
HRESULT Direct3DDevicePatched::GetMaterial(D3DMATERIAL7 *data)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetMaterial(data);
}
HRESULT Direct3DDevicePatched::SetLight(DWORD idx, D3DLIGHT7 *data)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetLight(idx, data);
}
HRESULT Direct3DDevicePatched::GetLight(DWORD idx, D3DLIGHT7 *data)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetLight(idx, data);
}
HRESULT Direct3DDevicePatched::SetRenderState(D3DRENDERSTATETYPE dwRenderStateType, DWORD dwRenderState)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetRenderState(dwRenderStateType, dwRenderState);
}
HRESULT Direct3DDevicePatched::GetRenderState(D3DRENDERSTATETYPE dwRenderStateType, LPDWORD lpdwRenderState)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetRenderState(dwRenderStateType, lpdwRenderState);
}
HRESULT Direct3DDevicePatched::BeginStateBlock()
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->BeginStateBlock();
}
HRESULT Direct3DDevicePatched::EndStateBlock(LPDWORD lpdwBlockHandle)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->EndStateBlock(lpdwBlockHandle);
}
HRESULT Direct3DDevicePatched::PreLoad(IDirectDrawSurface7 *surface)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->PreLoad(surface);
}
HRESULT Direct3DDevicePatched::DrawPrimitive(D3DPRIMITIVETYPE primitive_type, DWORD fvf, void *vertices, DWORD vertex_count, DWORD flags)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->DrawPrimitive(primitive_type, fvf, vertices, vertex_count, flags);
}
HRESULT Direct3DDevicePatched::DrawIndexedPrimitive(D3DPRIMITIVETYPE primitive_type, DWORD fvf, void *vertices, DWORD vertex_count, WORD *indices, DWORD index_count, DWORD flags)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->DrawIndexedPrimitive(primitive_type, fvf, vertices, vertex_count, indices, index_count, flags);
}
HRESULT Direct3DDevicePatched::SetClipStatus(D3DCLIPSTATUS *clip_status)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetClipStatus(clip_status);
}
HRESULT Direct3DDevicePatched::GetClipStatus(D3DCLIPSTATUS *clip_status)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetClipStatus(clip_status);
}
HRESULT Direct3DDevicePatched::DrawPrimitiveStrided(D3DPRIMITIVETYPE primitive_type, DWORD fvf, D3DDRAWPRIMITIVESTRIDEDDATA *strided_data, DWORD vertex_count, DWORD flags)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->DrawPrimitiveStrided(primitive_type, fvf, strided_data, vertex_count, flags);
}
HRESULT Direct3DDevicePatched::DrawIndexedPrimitiveStrided(D3DPRIMITIVETYPE primitive_type, DWORD fvf, D3DDRAWPRIMITIVESTRIDEDDATA *strided_data, DWORD vertex_count, WORD *indices, DWORD index_count, DWORD flags)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->DrawIndexedPrimitiveStrided(primitive_type, fvf, strided_data, vertex_count, indices, index_count, flags);
}
HRESULT Direct3DDevicePatched::DrawPrimitiveVB(D3DPRIMITIVETYPE primitive_type, struct IDirect3DVertexBuffer7 *vb, DWORD start_vertex, DWORD vertex_count, DWORD flags)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->DrawPrimitiveVB(primitive_type, vb, start_vertex, vertex_count, flags);
}
HRESULT Direct3DDevicePatched::DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE primitive_type, struct IDirect3DVertexBuffer7 *vb, DWORD start_vertex, DWORD vertex_count, WORD *indices, DWORD index_count, DWORD flags)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->DrawIndexedPrimitiveVB(primitive_type, vb, start_vertex, vertex_count, indices, index_count, flags);
}
HRESULT Direct3DDevicePatched::ComputeSphereVisibility(D3DVECTOR *centers, D3DVALUE *radii, DWORD sphere_count, DWORD flags, DWORD *ret)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->ComputeSphereVisibility(centers, radii, sphere_count, flags, ret);
}
HRESULT Direct3DDevicePatched::GetTexture(DWORD stage, IDirectDrawSurface7 **surface)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetTexture(stage, surface);
}
HRESULT Direct3DDevicePatched::SetTexture(DWORD stage, IDirectDrawSurface7 *surface)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetTexture(stage, surface);
}
HRESULT Direct3DDevicePatched::GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE d3dTexStageStateType,LPDWORD lpdwState)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetTextureStageState(dwStage, d3dTexStageStateType, lpdwState);
}
HRESULT Direct3DDevicePatched::SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE d3dTexStageStateType,DWORD dwState)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetTextureStageState(dwStage, d3dTexStageStateType, dwState);
}
HRESULT Direct3DDevicePatched::ValidateDevice(LPDWORD lpdwPasses)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->ValidateDevice(lpdwPasses);
}
HRESULT Direct3DDevicePatched::ApplyStateBlock(DWORD dwBlockHandle)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->ApplyStateBlock(dwBlockHandle);
}
HRESULT Direct3DDevicePatched::CaptureStateBlock(DWORD dwBlockHandle)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->CaptureStateBlock(dwBlockHandle);
}
HRESULT Direct3DDevicePatched::DeleteStateBlock(DWORD dwBlockHandle)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->DeleteStateBlock(dwBlockHandle);
}
HRESULT Direct3DDevicePatched::CreateStateBlock(D3DSTATEBLOCKTYPE d3dsbType,LPDWORD lpdwBlockHandle)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->CreateStateBlock(d3dsbType, lpdwBlockHandle);
}
HRESULT Direct3DDevicePatched::Load(IDirectDrawSurface7 *dst_surface, POINT *dst_point, IDirectDrawSurface7 *src_surface, RECT *src_rect, DWORD flags)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->Load(dst_surface, dst_point, src_surface, src_rect, flags);
}
HRESULT Direct3DDevicePatched::LightEnable(DWORD dwLightIndex,WINBOOL bEnable)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->LightEnable(dwLightIndex, bEnable);
}
HRESULT Direct3DDevicePatched::GetLightEnable(DWORD dwLightIndex,WINBOOL *pbEnable)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetLightEnable(dwLightIndex, pbEnable);
}
HRESULT Direct3DDevicePatched::SetClipPlane(DWORD dwIndex,D3DVALUE *pPlaneEquation)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->SetClipPlane(dwIndex, pPlaneEquation);
}
HRESULT Direct3DDevicePatched::GetClipPlane(DWORD dwIndex,D3DVALUE *pPlaneEquation)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetClipPlane(dwIndex, pPlaneEquation);
}
HRESULT Direct3DDevicePatched::GetInfo(DWORD info_id, void *info, DWORD info_size)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->GetInfo(info_id, info, info_size);
}

#endif


/***/

#if Direct3D7Patched_Impl
HRESULT Direct3D7Patched::EnumDevices(LPD3DENUMDEVICESCALLBACK7 cb, void *ctx)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->EnumDevices(cb, ctx);
}
HRESULT Direct3D7Patched::CreateVertexBuffer(D3DVERTEXBUFFERDESC *desc, struct IDirect3DVertexBuffer7 **buffer, DWORD flags)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->CreateVertexBuffer(desc,  buffer, flags);
}
HRESULT Direct3D7Patched::EnumZBufferFormats(REFCLSID device_iid, LPD3DENUMPIXELFORMATSCALLBACK cb, void *ctx)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->EnumZBufferFormats(device_iid, cb, ctx);
}
HRESULT Direct3D7Patched::EvictManagedTextures()
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__); return mWrapped->EvictManagedTextures();
}
#endif

/* wrapping setup on demand */
static IDirectDraw7 *new_directDraw7 = nullptr;

LPDIRECTDRAWCREATEEX orig_DirectDrawCreateEx;
HRESULT WINAPI DirectDrawCreateEx (GUID FAR *lpGUID, LPVOID* lplpDD, REFIID iid, IUnknown FAR *pUnkOuter ) {
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);

    if (! orig_DirectDrawCreateEx) {
        orig_hInstDDraw = LoadLibraryW( L"C:\\Windows\\System32\\ddraw.dll" );
        if(! orig_hInstDDraw) {
            log_printf("%s:%d \t LoadLibraryW(ddraw.dll) Failed\n", __FILE__, __LINE__);
            return FALSE;
        }

        orig_DirectDrawCreateEx = (LPDIRECTDRAWCREATEEX) GetProcAddress( orig_hInstDDraw, "DirectDrawCreateEx" );
        if(! orig_DirectDrawCreateEx) {
            log_printf("%s:%d \t GetProcAddress(ddraw.DirectDrawCreateEx) Failed\n", __FILE__, __LINE__);
            return FALSE;
        }
    }

    HRESULT r = orig_DirectDrawCreateEx(lpGUID, (LPVOID*) (&orig_lpDD7), iid, pUnkOuter);
    log_printf("%s:%d \t orig_lpDD7:%p\n", __FILE__, __LINE__, orig_lpDD7);

    new_directDraw7 = new DirectDraw7Patched(orig_lpDD7);

    *lplpDD = new_directDraw7;
    return r;
}

HRESULT STDMETHODCALLTYPE DirectDraw7Patched::QueryInterface(const IID& riid , void** ppvObject)
{
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);

    {
        LPOLESTR lplpsz;
        StringFromIID(riid, &lplpsz);
        log_printf("QueryInterface(%ls)\n", lplpsz);
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
    log_printf("%s:%d \t%s\n", __FILE__, __LINE__, __FUNCTION__);

    struct IDirect3DDevice7 *orig_device;
    HRESULT r = mWrapped->CreateDevice(rclsid, surface, &orig_device);

    IDirect3DDevice7 *new_d3dDevice7 = new Direct3DDevicePatched(orig_device);

    *device = new_d3dDevice7;

    return r;
}

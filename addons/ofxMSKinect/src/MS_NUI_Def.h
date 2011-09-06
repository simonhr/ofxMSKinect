
#pragma once

// This will stop compiler errors in CodeBlocks Gcc
//Define to nothing if not defined
#ifndef _In_
#define _In_
#define _In_opt_
#define _Out_
#define _Deref_out_
#endif


//Note: If you wish to use the multidevice interface provided by the MSKinect sdk and are
//using the CodeBlocks Gcc compiler define this, USING_CODE_BLOCKS_GCC_COMPILER, in order that
//the compiler will correctly configure stack and registers to call into the the MSKinect dlls
//pure virtual functions.

#if defined(USING_CODE_BLOCKS_GCC_COMPILER)
// idea taken from here, thanks 
// http://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-bots-programs/wow-memory-editing/281008-gcc-thiscall-calling-convention-linux-win32-mingw.html
#define CODEBLOCK_FASTCALL __attribute__((fastcall)) 
#define START_ARG_DECL int,
#define NO_ARG_DECL int
#define START_ARG 0,
#define NO_ARG 0

#else

#define CODEBLOCK_FASTCALL
#define START_ARG_DECL
#define NO_ARG_DECL
#define START_ARG
#define NO_ARG

#endif

// must include MRS_NuiApi.h just here
#include "MSR_NuiApi.h"

//Create another version of the INuiInstance interface (called ofxINuiInstance) and configure for fastcall and an extra param "IF" using codeblocks
//This is done in order to get the "this" ptr passed to the MSKinect dll in correct register and to have stack set correctly
interface ofxINuiInstance
{
    virtual CODEBLOCK_FASTCALL int         InstanceIndex(NO_ARG_DECL) = 0; 
    virtual CODEBLOCK_FASTCALL HRESULT     NuiInitialize(START_ARG_DECL _In_ DWORD dwFlags ) = 0;
    virtual CODEBLOCK_FASTCALL VOID        NuiShutdown(NO_ARG_DECL ) = 0;
    virtual CODEBLOCK_FASTCALL HRESULT     NuiImageStreamOpen(START_ARG_DECL
        _In_ NUI_IMAGE_TYPE eImageType,
        _In_ NUI_IMAGE_RESOLUTION eResolution,
        _In_ DWORD dwImageFrameFlags_NotUsed,
        _In_ DWORD dwFrameLimit,
        _In_opt_ HANDLE hNextFrameEvent,
        _Out_ HANDLE *phStreamHandle
        ) = 0;

    virtual CODEBLOCK_FASTCALL HRESULT     NuiImageStreamGetNextFrame(START_ARG_DECL
        _In_  HANDLE hStream,
        _In_  DWORD dwMillisecondsToWait,
        _Deref_out_ CONST NUI_IMAGE_FRAME **ppcImageFrame
        ) = 0;

    virtual CODEBLOCK_FASTCALL HRESULT     NuiImageStreamReleaseFrame(START_ARG_DECL
        _In_ HANDLE hStream,
        CONST NUI_IMAGE_FRAME *pImageFrame
        ) = 0;

    virtual CODEBLOCK_FASTCALL HRESULT NuiImageGetColorPixelCoordinatesFromDepthPixel(START_ARG_DECL
        _In_ NUI_IMAGE_RESOLUTION eColorResolution,
        _In_opt_ CONST NUI_IMAGE_VIEW_AREA *pcViewArea,
        _In_ LONG   lDepthX,
        _In_ LONG   lDepthY,
        _In_ USHORT usDepthValue,
        _Out_ LONG *plColorX,
        _Out_ LONG *plColorY
        ) = 0;

    virtual CODEBLOCK_FASTCALL HRESULT NuiCameraElevationSetAngle(START_ARG_DECL
        _In_ LONG lAngleDegrees
        ) = 0;

    virtual CODEBLOCK_FASTCALL HRESULT NuiCameraElevationGetAngle(START_ARG_DECL
        _In_ LONG * plAngleDegrees
        ) = 0;

    virtual CODEBLOCK_FASTCALL HRESULT NuiSkeletonTrackingEnable(START_ARG_DECL
        _In_opt_ HANDLE hNextFrameEvent,
        _In_     DWORD  dwFlags
        ) = 0;

    virtual CODEBLOCK_FASTCALL HRESULT NuiSkeletonTrackingDisable(NO_ARG_DECL
        ) = 0;

    virtual CODEBLOCK_FASTCALL HRESULT NuiSkeletonGetNextFrame(START_ARG_DECL
        _In_  DWORD dwMillisecondsToWait,
        _Out_ NUI_SKELETON_FRAME *pSkeletonFrame
        ) = 0;

    virtual CODEBLOCK_FASTCALL HRESULT NuiTransformSmooth(START_ARG_DECL
        NUI_SKELETON_FRAME *pSkeletonFrame,
        const NUI_TRANSFORM_SMOOTH_PARAMETERS *pSmoothingParams
        ) = 0;

    virtual CODEBLOCK_FASTCALL  bool  MSR_NuiGetPropsBlob( START_ARG_DECL MsrNui::NUI_PROPSINDEX Index, void * pBlob, DWORD * pdwInOutSize ) = 0;
    virtual CODEBLOCK_FASTCALL MsrNui::NUI_PROPSTYPE MSR_NuiGetPropsType( START_ARG_DECL MsrNui::NUI_PROPSINDEX Index ) = 0;
};




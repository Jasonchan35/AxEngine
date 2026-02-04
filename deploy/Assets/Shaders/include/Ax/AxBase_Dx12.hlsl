#ifndef __AxBase_Dx12_hlsl__
#define __AxBase_Dx12_hlsl__

#ifdef AX_RENDER_DX12

typedef struct D3D12_DISPATCH_ARGUMENTS
{
    u32 ThreadGroupCountX;
    u32 ThreadGroupCountY;
    u32 ThreadGroupCountZ;
} D3D12_DISPATCH_ARGUMENTS;

#endif // AX_RENDER_DX12


#endif // __AxBase_Dx12_hlsl__
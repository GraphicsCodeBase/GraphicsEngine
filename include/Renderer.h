#pragma once
#include "GameObject.h"
#include "Camera.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <nvrhi/nvrhi.h>
#include <nvrhi/Utils.h>
#include <vector>
#include <dxgi.h>
#include <nvrhi/d3d11.h>
class Renderer
{
public:
    Renderer(HWND hwnd, uint32_t width, uint32_t height);
    ~Renderer();

    void Initialize();           // Sets up device, swapchain, NVRHI
    //void Resize(uint32_t width, uint32_t height);
    void RenderFrame(int windowWidth, int windowHeight);         // Handles per-frame rendering
    void Shutdown();            // Releases resources

private:
    //object container 
	std::vector<std::shared_ptr<GameObject>> m_Objects;

    void CreateDeviceAndSwapchain();
    void CreateNvrhiDevice();
	void CreateSwapChainTexture();
    void CreatePipeline();
	void CreateResources();
    //void CreateFramebufferResources();
    //void RecordDrawCommands(nvrhi::ICommandList* commandList);

    // DX11 Core
    Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_BackBuffer;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BackBufferRTV;

    // NVRHI
    nvrhi::DeviceHandle m_NvrhiDevice;
    nvrhi::TextureHandle m_SwapChainTexture;
    nvrhi::FramebufferHandle m_Framebuffer;

    // Dimensions
    uint32_t m_Width;
    uint32_t m_Height;
    HWND m_Window;

    //Graphics pipeline fields ->  Pipeline, shaders, vertex buffer...
    nvrhi::ShaderHandle m_VertexShader;
    nvrhi::ShaderHandle m_PixelShader;
    nvrhi::InputLayoutHandle m_InputLayout;
    nvrhi::BindingLayoutHandle m_BindingLayout;
    nvrhi::GraphicsPipelineHandle m_GraphicsPipeline;

    // NVRHI handles
    nvrhi::BufferHandle m_ConstantBuffer;
    nvrhi::BufferHandle m_VertexBuffer;
    nvrhi::BufferHandle m_IndexBuffer;
    nvrhi::TextureHandle m_GeometryTexture;
    nvrhi::BindingSetHandle m_BindingSet;
    nvrhi::CommandListHandle m_CommandList;
};

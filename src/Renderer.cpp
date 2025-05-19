#include "Renderer.h"
#include <stdexcept>
#include <ShaderCompile.h>
using Microsoft::WRL::ComPtr;

std::string vertexShaderPath = "../Resources/Shaders/vertex.hlsl";
std::string pixelShaderPath = "../Resources/Shaders/pixel.hlsl";

Renderer::Renderer(HWND hwnd, uint32_t width, uint32_t height)
             : m_Window(hwnd), m_Width(width), m_Height(height)
{
}

Renderer::~Renderer()
{
    Shutdown();//calll the shutdown function when its done
}

void Renderer::Initialize()
{
	CreateDeviceAndSwapchain();
	CreateNvrhiDevice(); 
    CreateSwapChainTexture();
	CreateResources();
	CreatePipeline();
}
void Renderer::RenderFrame(int windowWidth, int windowHeight)
{
    // Begin frame
    m_CommandList->open();

    nvrhi::utils::ClearColorAttachment(m_CommandList, m_Framebuffer, 0, nvrhi::Color(1.0f, 0.773f, 0.827f, 1.0f));

    //// Update constant buffer
    //float viewProjectionMatrix[16] = {
    //    1, 0, 0, 0,
    //    0, 1, 0, 0,
    //    0, 0, 1, 0,
    //    0, 0, 0, 1
    //};
    //m_CommandList->writeBuffer(m_ConstantBuffer, viewProjectionMatrix, sizeof(viewProjectionMatrix));

	auto object = m_Objects[0]; // Get the first object
	auto vertexBuffer = object->getVetexBuffer(); // Get the vertex buffer from the object
	m_VertexBuffer = vertexBuffer->getHandle(); // Get the handle from the vertex buffer
	m_IndexBuffer = object->getIndexBuffer()->getHandle(); // Get the index buffer handle

    // Set graphics state
    auto graphicsState = nvrhi::GraphicsState()
        .setPipeline(m_GraphicsPipeline)
        .setFramebuffer(m_Framebuffer)
        .setViewport(nvrhi::ViewportState()
        .addViewport(nvrhi::Viewport(float(windowWidth), float(windowHeight)))
        .addScissorRect(nvrhi::Rect(0, windowWidth, 0, windowHeight)))
        .addBindingSet(m_BindingSet)
        .addVertexBuffer(nvrhi::VertexBufferBinding{ m_VertexBuffer, 0, 0 })
        .setIndexBuffer(nvrhi::IndexBufferBinding{m_IndexBuffer,nvrhi::Format::R32_UINT});

    m_CommandList->setGraphicsState(graphicsState);


    // Draw triangle(square)
    nvrhi::DrawArguments drawArgs;
    drawArgs.vertexCount = 36;
    drawArgs.instanceCount = 1;
    drawArgs.startIndexLocation = 0;
    drawArgs.startVertexLocation = 0;
    m_CommandList->drawIndexed(drawArgs);

    // End frame
    m_CommandList->close();
    m_NvrhiDevice->executeCommandList(m_CommandList);

    // Present
    m_SwapChain->Present(1, 0); // Present with vsync
}
void Renderer::Shutdown()
{
    // Optional: explicitly release resources
    m_CommandList = nullptr;
    m_BindingSet = nullptr;
    m_ConstantBuffer = nullptr;
    m_VertexBuffer = nullptr;
    m_GraphicsPipeline = nullptr;
    m_BindingLayout = nullptr;
    m_InputLayout = nullptr;
    m_PixelShader = nullptr;
    m_VertexShader = nullptr;
    m_Framebuffer = nullptr;
    m_SwapChainTexture = nullptr;
    m_NvrhiDevice = nullptr;

    m_BackBufferRTV.Reset();
    m_BackBuffer.Reset();
    m_SwapChain.Reset();
    m_Context.Reset();
    m_Device.Reset();
}
//====================
// brief:
// Create the D3D11 device and swapchain
//====================
void Renderer::CreateDeviceAndSwapchain()
{
    // Flags for D3D11 device
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    // Create the D3D11 device and immediate context
    ComPtr<IDXGIFactory> dxgiFactory;
    ComPtr<IDXGIAdapter> adapter;

    // Create DXGI Factory first (used for creating the swapchain)
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)dxgiFactory.GetAddressOf());
    if (FAILED(hr))
        throw std::runtime_error("Failed to create DXGI Factory");

    // (Optional) Choose adapter here. We'll just use the default one.
    // hr = dxgiFactory->EnumAdapters(0, &adapter);

    hr = D3D11CreateDevice(
        adapter.Get(),                        // Adapter (null = default)
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        &m_Device,
        nullptr,                              // Optional: feature level
        &m_Context
    );

    if (FAILED(hr))
        throw std::runtime_error("Failed to create D3D11 Device");

    // ========================
    // Create the Swap Chain
    // ========================
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 2;                               // Double buffering
    swapChainDesc.BufferDesc.Width = m_Width;                          // Use window size
    swapChainDesc.BufferDesc.Height = m_Height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = m_Window;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    hr = dxgiFactory->CreateSwapChain(
        m_Device.Get(),
        &swapChainDesc,
        &m_SwapChain
    );

    if (FAILED(hr))
        throw std::runtime_error("Failed to create Swap Chain");
}

void Renderer::CreateSwapChainTexture()
{
    // Get the back buffer from the swap chain
    ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to get swap chain back buffer");
    }
    nvrhi::TextureDesc textureDesc;
	textureDesc.isRenderTarget = true;
    textureDesc.debugName = "SwapChain BackBuffer";
    textureDesc.width = m_Width;
    textureDesc.height = m_Height;
    textureDesc.format = nvrhi::Format::RGBA8_UNORM; // Match your DXGI_FORMAT
    textureDesc.dimension = nvrhi::TextureDimension::Texture2D;

    // Wrap the native D3D11 texture in an NVRHI texture handle
    m_SwapChainTexture = m_NvrhiDevice->createHandleForNativeTexture(
        nvrhi::ObjectTypes::D3D11_Resource,
        backBuffer.Get(),
        textureDesc
    );
}

void Renderer::CreatePipeline()
{
	ShaderCompile shaderCompiler(m_NvrhiDevice);
    // Compile vertex and pixel shaders
    nvrhi::ShaderHandle vertexShader = shaderCompiler.LoadVertexShader(vertexShaderPath);
    nvrhi::ShaderHandle pixelShader = shaderCompiler.LoadPixelShader(pixelShaderPath);

	m_VertexShader = vertexShader;
	m_PixelShader = pixelShader;

    // Define vertex input layout
    nvrhi::VertexAttributeDesc attributes[] = {
        nvrhi::VertexAttributeDesc()
            .setName("POSITION")
            .setFormat(nvrhi::Format::RGB32_FLOAT)
            .setOffset(offsetof(Vertex, position))
            .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
            .setName("TEXCOORD")
            .setFormat(nvrhi::Format::RG32_FLOAT)
            .setOffset(offsetof(Vertex, texCoord))
            .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
            .setName("COLOR")
            .setFormat(nvrhi::Format::RGBA32_FLOAT)
            .setOffset(offsetof(Vertex, color))
            .setElementStride(sizeof(Vertex)),
    };

    m_InputLayout = m_NvrhiDevice->createInputLayout(
        attributes, uint32_t(std::size(attributes)), m_VertexShader);

    // Create binding layout (e.g., texture at t0, constants at b0)
    nvrhi::BindingLayoutDesc layoutDesc;
    layoutDesc.setVisibility(nvrhi::ShaderType::All);
    //layoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
    layoutDesc.addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0));

    m_BindingLayout = m_NvrhiDevice->createBindingLayout(layoutDesc);

    // === Binding Set ===
    auto bindingSetDesc = nvrhi::BindingSetDesc()
        .addItem(nvrhi::BindingSetItem::ConstantBuffer(0, m_ConstantBuffer));
    // .addItem(nvrhi::BindingSetItem::Texture_SRV(0, m_GeometryTexture)); // optional

    m_BindingSet = m_NvrhiDevice->createBindingSet(bindingSetDesc, m_BindingLayout);

    // Create framebuffer from your swap chain texture
    nvrhi::FramebufferDesc framebufferDesc;
    framebufferDesc.addColorAttachment(m_SwapChainTexture);

    m_Framebuffer = m_NvrhiDevice->createFramebuffer(framebufferDesc);

    nvrhi::RenderState renderState;
	renderState.rasterState.setFrontCounterClockwise(true);
    renderState.rasterState.cullMode = nvrhi::RasterCullMode::Back; // or Back, Front
    renderState.rasterState.setFillMode(nvrhi::RasterFillMode::Wireframe);
    
    // Create the graphics pipeline
    nvrhi::GraphicsPipelineDesc pipelineDesc;
    pipelineDesc.setInputLayout(m_InputLayout);
    pipelineDesc.setVertexShader(m_VertexShader);
    pipelineDesc.setPixelShader(m_PixelShader);
    pipelineDesc.addBindingLayout(m_BindingLayout);
    pipelineDesc.setRenderState(renderState);
	pipelineDesc.primType = nvrhi::PrimitiveType::TriangleList;
    m_GraphicsPipeline = m_NvrhiDevice->createGraphicsPipeline(pipelineDesc, m_Framebuffer);
}

void Renderer::CreateResources()
{
    
    // === 2. SceneConstants Struct for Uniforms ===
    struct alignas(16) SceneConstants
    {
        DirectX::XMFLOAT4X4 modelMatrix;
        DirectX::XMFLOAT4X4 View;
        DirectX::XMFLOAT4X4 projection;
    };

    // === Create Test GameObject and Upload Mesh === 
    std::shared_ptr<GameObject> m_TestObject = std::make_shared<GameObject>();
	m_TestObject->SetPosition(0.5f, 0.0f, 3.5f);
    m_TestObject->SetScale(2.0f, 2.0f, 2.0f);
	m_TestObject->setRotation(90.0f);
    //building the model matrix.
    // === 3. Build Model Matrix from Object Transform ===
    DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(m_TestObject->GetPosition());
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(
        m_TestObject->GetRotationAngle()
    );
    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(m_TestObject->GetScale());
    DirectX::XMMATRIX modelMatrix = scaleMatrix * rotationMatrix * translationMatrix;
    // == = 4. Setup Projection Matrix == =
    //create camera object.
    Camera camera;
    DirectX::XMMATRIX view = camera.GetViewMatrix();
    DirectX::XMMATRIX proj = camera.GetProjectionMatrix();

    //DirectX::XMMATRIX mvp =  modelMatrix * view * proj; 

    // === 5. Write into SceneConstants, using Transpose for HLSL compatibility ===
    SceneConstants constants = {};
    DirectX::XMStoreFloat4x4(&constants.modelMatrix, DirectX::XMMatrixTranspose(modelMatrix));
    DirectX::XMStoreFloat4x4(&constants.View, DirectX::XMMatrixTranspose(view));
    DirectX::XMStoreFloat4x4(&constants.projection, DirectX::XMMatrixTranspose(proj));
    // === Constant Buffer (for ViewProjection matrix, for example) ===
    //This stores your matrix (like a projection or model-view-projection matrix):
    auto constantBufferDesc = nvrhi::BufferDesc()
        .setByteSize(sizeof(SceneConstants))
        .setIsConstantBuffer(true)
        .setIsVolatile(true)
        .setMaxVersions(16);
    
    m_ConstantBuffer = m_NvrhiDevice->createBuffer(constantBufferDesc);

    // After creating m_ConstantBuffer
   /* float identityMatrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };*/
	//upload the constant buffer data to the GPU.
    nvrhi::CommandListHandle initCommandList = m_NvrhiDevice->createCommandList();
    initCommandList->open();
    initCommandList->writeBuffer(m_ConstantBuffer, &constants, sizeof(constants));
    initCommandList->close();
    m_NvrhiDevice->executeCommandList(initCommandList);

    // === Create Command List for Mesh Upload ===
    m_CommandList = m_NvrhiDevice->createCommandList();
    m_CommandList->open();
    // === Define Vertex and Index Data ===
   // Optimal vertex definition (4 unique vertices)
    std::vector<Vertex> vertices = {
        // Front face (Z = -0.5)
        {{-0.5f, -0.5f, -0.5f}, {0, 0}, {1, 0, 0, 1}}, // 0 - bottom-left
        {{ 0.5f, -0.5f, -0.5f}, {1, 0}, {0, 1, 0, 1}}, // 1 - bottom-right
        {{ 0.5f,  0.5f, -0.5f}, {1, 1}, {0, 0, 1, 1}}, // 2 - top-right
        {{-0.5f,  0.5f, -0.5f}, {0, 1}, {1, 1, 0, 1}}, // 3 - top-left

        // Back face (Z = 0.5)
        {{-0.5f, -0.5f,  0.5f}, {1, 0}, {1, 0, 1, 1}}, // 4 - bottom-left
        {{ 0.5f, -0.5f,  0.5f}, {0, 0}, {0, 1, 1, 1}}, // 5 - bottom-right
        {{ 0.5f,  0.5f,  0.5f}, {0, 1}, {1, 0.5, 0, 1}}, // 6 - top-right
        {{-0.5f,  0.5f,  0.5f}, {1, 1}, {0.5, 0, 1, 1}}  // 7 - top-left
    };

    // Index buffer reusing vertices (2 triangles -> 6 indices)
    std::vector<uint32_t> indices = {
        // Front face (CCW)
        0, 1, 2,  0, 2, 3,
        // Back face (CCW when viewed from front)
        5, 4, 7,  5, 7, 6,
        // Left face (CCW)
        4, 0, 3,  4, 3, 7,
        // Right face (CCW)
        1, 5, 6,  1, 6, 2,
        // Top face (CCW)
        3, 2, 6,  3, 6, 7,
        // Bottom face (CCW)
        4, 5, 1,  4, 1, 0
    };
    m_TestObject->setMesh(vertices, indices, m_NvrhiDevice);
    //pass it into the vector of game objects.
	m_Objects.push_back(m_TestObject);  
    // === Finish Command List and Submit ===
    m_CommandList->close();
    m_NvrhiDevice->executeCommandList(m_CommandList);
}

void Renderer::CreateNvrhiDevice()
{
    if (!m_Device || !m_Context)
    {
        throw std::runtime_error("D3D11 Device and Context must be created before creating NVRHI device");
    }

	//deviceDesc.messageCallback = &MessageCallback; <- use this if you want to set a message callback
    nvrhi::d3d11::DeviceDesc deviceDesc;
    deviceDesc.context = m_Context.Get();
    nvrhi::DeviceHandle nvrhiDevice = nvrhi::d3d11::createDevice(deviceDesc);

	m_NvrhiDevice = nvrhiDevice; // assingn device to member var.
    
    if (!nvrhiDevice) {
        printf("Failed to create NVRHI device\n");
        //return false;
    }

    printf("Successfully created NVRHI device\n");
    //return true;
}


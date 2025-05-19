#include "ShaderCompile.h"
#include <fstream>
#include <sstream>
#include <iostream>

ShaderCompile::ShaderCompile(nvrhi::IDevice* device)
    : m_Device(device)
{
}

Microsoft::WRL::ComPtr<ID3DBlob> ShaderCompile::CompileShader(const std::string& path, const std::string& entryPoint, const std::string& target)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return nullptr;
    }

    std::stringstream shaderSource;
    shaderSource << file.rdbuf();
    std::string sourceStr = shaderSource.str();

    Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompile(
        sourceStr.c_str(), sourceStr.size(),
        path.c_str(),      // source name
        nullptr, nullptr,
        entryPoint.c_str(), target.c_str(),
        D3DCOMPILE_ENABLE_STRICTNESS, 0,
        &shaderBlob, &errorBlob
    );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            std::cerr << "Shader compilation failed for " << path << ":\n";
            std::cerr << static_cast<const char*>(errorBlob->GetBufferPointer()) << std::endl;
        }
        return nullptr;
    }

    return shaderBlob;
}

nvrhi::ShaderHandle ShaderCompile::LoadVertexShader(const std::string& path, const std::string& entryPoint)
{
    auto blob = CompileShader(path, entryPoint, "vs_5_0");
    if (!blob) return nullptr;

    nvrhi::ShaderDesc desc;
    desc.setShaderType(nvrhi::ShaderType::Vertex);
    desc.setEntryName(entryPoint);
    desc.setDebugName(path);

    return m_Device->createShader(
        desc,
        blob->GetBufferPointer(),
        blob->GetBufferSize()
    );
}

nvrhi::ShaderHandle ShaderCompile::LoadPixelShader(const std::string& path, const std::string& entryPoint)
{
    auto blob = CompileShader(path, entryPoint, "ps_5_0");
    if (!blob) return nullptr;

    nvrhi::ShaderDesc desc;
    desc.setShaderType(nvrhi::ShaderType::Pixel);
    desc.setEntryName(entryPoint);
    desc.setDebugName(path);

    return m_Device->createShader(
           desc,
        blob->GetBufferPointer(),
        blob->GetBufferSize()
    );
}

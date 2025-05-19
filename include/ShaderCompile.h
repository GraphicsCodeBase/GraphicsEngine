#pragma once

#include <string>
#include <vector>
#include <nvrhi/nvrhi.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

class ShaderCompile
{
public:
    ShaderCompile(nvrhi::IDevice* device);
    //void SetDevice(nvrhi::DeviceHandle device){ m_Device = device; }
    nvrhi::ShaderHandle LoadVertexShader(const std::string& path, const std::string& entryPoint = "main");
    nvrhi::ShaderHandle LoadPixelShader(const std::string& path, const std::string& entryPoint = "main");

private:
    nvrhi::IDevice* m_Device;

    Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::string& path, const std::string& entryPoint, const std::string& target);
};

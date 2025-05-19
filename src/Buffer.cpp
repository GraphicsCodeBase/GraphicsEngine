#include "Buffer.h"

Buffer::Buffer(nvrhi::IDevice* device, Type type, uint32_t sizeInBytes, const std::string& debugName, uint32_t maxVersions)
    : m_Type(type)
{
    switch (type)
    {
    case Type::Constant_Static:
        m_Desc = nvrhi::utils::CreateStaticConstantBufferDesc(sizeInBytes, debugName.c_str());
        break;
    case Type::Constant_Volatile:
        m_Desc = nvrhi::utils::CreateVolatileConstantBufferDesc(sizeInBytes, debugName.c_str(), maxVersions);
        break;
    case Type::Vertex:
        m_Desc = nvrhi::BufferDesc()
            .setByteSize(sizeInBytes)
            .setIsVertexBuffer(true)
            .setInitialState(nvrhi::ResourceStates::VertexBuffer)
            .setDebugName(debugName);
        break;
    case Type::Index:
        m_Desc = nvrhi::BufferDesc()
            .setByteSize(sizeInBytes)
            .setIsIndexBuffer(true)
            .setInitialState(nvrhi::ResourceStates::IndexBuffer)
            .setDebugName(debugName);
        break;
    }

    m_Buffer = device->createBuffer(m_Desc);
}

std::shared_ptr<Buffer> Buffer::CreateWithData(nvrhi::IDevice* device, 
                                                Type type, const void* data, 
                                                uint32_t sizeInBytes, 
                                                const std::string& debugName)
{
    nvrhi::BufferDesc desc = {};
    desc.byteSize = sizeInBytes;
    desc.debugName = debugName;
	desc.cpuAccess = nvrhi::CpuAccessMode::Write;

    switch (type)
    {
    case Type::Vertex:
        desc.isVertexBuffer = true;
        break;
    case Type::Index:
        desc.isIndexBuffer = true;
        break;
    case Type::Constant_Static:
        desc.isConstantBuffer = true;
        break;
    case Type::Constant_Volatile:
        desc.isConstantBuffer = true;
        desc.isVolatile = true;
        desc.maxVersions = 16; // or whatever
        break;
    }

    nvrhi::BufferHandle buffer = device->createBuffer(desc);

    if (data && buffer)
    {
        void* mappedPtr = device->mapBuffer(buffer, nvrhi::CpuAccessMode::Write);
        if (mappedPtr)
        {
            std::memcpy(mappedPtr, data, sizeInBytes);
            device->unmapBuffer(buffer);
        }
        else
        {
            throw std::runtime_error("Failed to map buffer for writing");
        }
    }

    auto result = std::make_shared<Buffer>(device, type, sizeInBytes, debugName);
    result->m_Buffer = buffer;
    result->m_Desc = desc;
    return result;

}

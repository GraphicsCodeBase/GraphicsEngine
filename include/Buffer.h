#pragma once

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>
#include <string>
#include <memory>

class Buffer
{
public:
    enum class Type
    {
        Vertex,
        Index,
        Constant_Static,
        Constant_Volatile
    };

    Buffer(nvrhi::IDevice* device, Type type, uint32_t sizeInBytes, const std::string& debugName, uint32_t maxVersions = 0);

    nvrhi::IBuffer* get() const { return m_Buffer; }
    nvrhi::BufferHandle getHandle() const { return m_Buffer; }

    const nvrhi::BufferDesc& getDesc() const { return m_Desc; }
    Type getType() const { return m_Type; }

    //create buffer function 
    static std::shared_ptr<Buffer> CreateWithData(
        nvrhi::IDevice* device,
        Type type,
        const void* data,
        uint32_t sizeInBytes,
        const std::string& debugName);

private:
    nvrhi::BufferDesc m_Desc;
    nvrhi::BufferHandle m_Buffer;
    Type m_Type;
};

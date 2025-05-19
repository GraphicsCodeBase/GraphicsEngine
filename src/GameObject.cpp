#include "GameObject.h"

void GameObject::setMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, nvrhi::IDevice* device)
{
    // Upload vertex buffer
    vertexBuffer = Buffer::CreateWithData(
        device,
        Buffer::Type::Vertex,
        vertices.data(),
        static_cast<uint32_t>(sizeof(Vertex) * vertices.size()),
        "GameObject_VertexBuffer"
    );

    // Upload index buffer
    indexBuffer = Buffer::CreateWithData(
        device,
        Buffer::Type::Index,
        indices.data(),
        static_cast<uint32_t>(sizeof(uint32_t) * indices.size()),
        "GameObject_IndexBuffer"
    );
}

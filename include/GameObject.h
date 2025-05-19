#include "Buffer.h"
#include <DirectXMath.h>

using namespace DirectX;
struct Vertex
{
	float position[3];
	float texCoord[2];
	float color[4];
};

class GameObject
{
public:
	//default constructor.
	GameObject()
		: position(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
		rotation(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)),
		scale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f)) ,
		vertexBuffer(nullptr), indexBuffer(nullptr)
	{
	}
	// Getter functions
	XMVECTOR GetPosition() const { return position; }
	//XMVECTOR GetRotation() const { return rotation; }
	XMVECTOR GetScale() const { return scale; }
	float GetRotationAngle() const { return roationAngle; }


	// Setter functions
	void SetPosition(float x, float y, float z) {
		position = XMVectorSet(x, y, z, 0.0f);
	}

	/*void SetRotation(float x, float y, float z, float w) {
		rotation = XMVectorSet(x, y, z, w);
	}*/
	void setRotation(float angle)
	{
		roationAngle = angle;
	}

	void SetScale(float x, float y, float z) {
		scale = XMVectorSet(x, y, z, 0.0f);
	}
	//set mesh function 
	void setMesh(const std::vector<Vertex>& vertices,
		const std::vector<uint32_t>& indices,
		nvrhi::IDevice* device);
	//return the vertex buffer
	std::shared_ptr<Buffer> getVetexBuffer() { return vertexBuffer; }
	std::shared_ptr<Buffer> getIndexBuffer() { return indexBuffer; }

private:
	XMVECTOR position;   // 3D position (x, y, z)
	XMVECTOR rotation;   // Rotation (use Quaternion or Euler angles)
	XMVECTOR scale;      // Scale (x, y, z)
	float roationAngle; // Rotation angle (if using Euler angles)

	// Buffers for this GameObject (vertex and index buffers)
	std::shared_ptr<Buffer> vertexBuffer;  // The vertex buffer holding the geometry data
	std::shared_ptr<Buffer> indexBuffer;   // The index buffer holding indices for drawing the geometry
};


#include "Camera.h"
#include <DirectXMath.h>
#include <algorithm>
using namespace DirectX;

Camera::Camera()
{
    m_Position = XMFLOAT3(0.f, 0.f, -5.f);
    m_Target = XMFLOAT3(0.f, 0.f, 0.f);
    m_Up = XMFLOAT3(0.f, 1.f, 0.f);

    m_FOV = XM_PIDIV4;
    m_AspectRatio = 16.f / 9.f;
    m_NearPlane = 0.5f;
    m_FarPlane = 100.f;
}

void Camera::SetPosition(const XMFLOAT3& position)
{
    m_Position = position;
}

void Camera::SetTarget(const XMFLOAT3& target)
{
    m_Target = target;
}

void Camera::SetAspectRatio(float aspect)
{
    m_AspectRatio = aspect;
}

void Camera::SetFOV(float fovRadians)
{
    m_FOV = fovRadians;
}

void Camera::SetClipPlanes(float nearPlane, float farPlane)
{
    m_NearPlane = nearPlane;
    m_FarPlane = farPlane;
}

void Camera::updateCamera(float deltaTime, const Input& input)
{
    float velocity = m_MoveSpeed * deltaTime;

    XMVECTOR pos = XMLoadFloat3(&m_Position);
    XMVECTOR front = XMLoadFloat3(&m_Front);
    XMVECTOR right = XMLoadFloat3(&m_Right);
    XMVECTOR up = XMLoadFloat3(&m_Up);

    if (input.IsKeyDown('W'))  // forward
        pos = XMVectorAdd(pos, XMVectorScale(front, velocity));
    if (input.IsKeyDown('S'))  // backward
        pos = XMVectorSubtract(pos, XMVectorScale(front, velocity));
    if (input.IsKeyDown('A'))  // left
        pos = XMVectorSubtract(pos, XMVectorScale(right, velocity));
    if (input.IsKeyDown('D'))  // right
        pos = XMVectorAdd(pos, XMVectorScale(right, velocity));
    //if (input.IsKeyDown('Space'))  // up
    //    pos = XMVectorAdd(pos, XMVectorScale(up, velocity));
    //if (input.IsKeyDown('LeftShift'))  // down
    //    pos = XMVectorSubtract(pos, XMVectorScale(up, velocity));

    XMStoreFloat3(&m_Position, pos);
	DirectX::XMFLOAT2 mouse = input.GetMousePosition();
    // Handle mouse movement
    float dx = mouse.x;
    float dy = mouse.y;
    m_Yaw += dx * m_MouseSensitivity;
    m_Pitch -= dy * m_MouseSensitivity;

    // Clamp pitch to avoid gimbal lock
    m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);

    updateCameraVector(); // Recalculates m_Front, m_Right, m_Up based on Yaw/Pitch
}

void Camera::updateCameraVector()
{
    DirectX::XMVECTOR front = DirectX::XMVectorSet(
        cosf(DirectX::XMConvertToRadians(m_Yaw)) * cosf(DirectX::XMConvertToRadians(m_Pitch)),
        sinf(DirectX::XMConvertToRadians(m_Pitch)),
        sinf(DirectX::XMConvertToRadians(m_Yaw)) * cosf(DirectX::XMConvertToRadians(m_Pitch)),
        0.0f
    );

    front = DirectX::XMVector3Normalize(front);
    DirectX::XMStoreFloat3(&m_Front, front);

    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, front));
    DirectX::XMVECTOR cameraUp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(front, right));

    DirectX::XMStoreFloat3(&m_Right, right);
    DirectX::XMStoreFloat3(&m_Up, cameraUp);
}

XMMATRIX Camera::GetViewMatrix() const
{
    return XMMatrixLookAtLH(
        XMLoadFloat3(&m_Position),
        XMLoadFloat3(&m_Target),
        XMLoadFloat3(&m_Up)
    );
}

XMMATRIX Camera::GetProjectionMatrix() const
{
    return XMMatrixPerspectiveFovLH(
        m_FOV,
        m_AspectRatio,
        m_NearPlane,
        m_FarPlane
    );
}

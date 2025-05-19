#pragma once

#include <DirectXMath.h>
#include <Input.h>
class Camera
{
public:
    Camera();

    void SetPosition(const DirectX::XMFLOAT3& position);
    void SetTarget(const DirectX::XMFLOAT3& target);
    void SetAspectRatio(float aspect);
    void SetFOV(float fovRadians);
    void SetClipPlanes(float nearPlane, float farPlane);
    void updateCamera(float deltaTime, const Input& input);
    void updateCameraVector();

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;

private:
    DirectX::XMFLOAT3 m_Position;
    DirectX::XMFLOAT3 m_Target;
    DirectX::XMFLOAT3 m_Up;

    float m_FOV;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;

    //flying camera member variable.
    //DirectX::XMFLOAT3 m_Up = { 0.0f, 1.0f, 0.0f };
    DirectX::XMFLOAT3 m_Front = { 0.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT3 m_Right = { 1.0f, 0.0f, 0.0f };

    float m_Yaw = 90.0f;   // Looking along +Z initially
    float m_Pitch = 0.0f;
    float m_MoveSpeed = 5.0f;
    float m_MouseSensitivity = 0.1f;

};

/*	FIT2096 - Example Code
*	Camera.cpp
*	Created by Elliott Wilson - 2015 - Monash University
*	Implementation of Camera.h
*/

#include "Camera.h"
#include "MathsHelper.h"
#include <cmath>
#include <iostream>

Camera::Camera()
{
	//Set up a "default" camera!
	m_cameraAngle = (float)ToRadians(270.0f);
	m_cameraDistance = 16.0f;
	m_spinRate = 0.005f;
	m_cameraHeight = 2.5f;

	m_position = m_cameraDistance*Vector3(cos(m_cameraAngle), m_cameraHeight, sin(m_cameraAngle)); // Camera position: this gives an angled top-down view
	m_lookAtTarget = Vector3::Zero; // Point towards the origin
	m_up = Vector3::Up;

	m_aspectRatio = 1280.0f / 720.0f;
	m_fieldOfView = (float)ToRadians(45.0f);
	m_nearClip = 0.1f;
	m_farClip = 100.0f;

	m_viewDirty = true;
	m_projectionDirty = true;
}

Camera::Camera(Vector3 pos, Vector3 lookAt, Vector3 up, float aspect, float fov, float nearClip, float farClip)
{
	m_position = pos;
	m_lookAtTarget = lookAt;
	m_up = up;

	m_aspectRatio = aspect;
	m_fieldOfView = fov;
	m_nearClip = nearClip;
	m_farClip = farClip;

	m_viewDirty = true;
	m_projectionDirty = true;
}

Camera::~Camera()
{

}

void Camera::SetPosition(Vector3 pos)
{
	m_position = pos;
	m_viewDirty = true;		//Every time a value is changed then the respective matrix is set "dirty"
}

void Camera::SetLookAt(Vector3 lookAt)
{
	m_lookAtTarget = lookAt;
	m_viewDirty = true;
}

void Camera::SetUp(Vector3 up)
{
	m_up = up;
	m_viewDirty = true;
}

void Camera::SetAspectRatio(float aspect)
{
	m_aspectRatio = aspect;
	m_projectionDirty = true;
}

void Camera::SetFieldOfView(float fov)
{
	m_fieldOfView = fov;
	m_projectionDirty = true;
}

void Camera::SetNearClip(float nearClip)
{
	m_nearClip = nearClip;
	m_projectionDirty = true;
}

void Camera::SetFarClip(float farClip)
{
	m_farClip = farClip;
	m_projectionDirty = true;
}

void Camera::Update(float timestep)
{

	if (m_cameraAngle >= (float)ToRadians(280.0f) || m_cameraAngle <= (float)ToRadians(260.0f)) {
		m_spinRate = -m_spinRate; // Change spin direction to stop the camera from rotating too far
	}
	m_cameraAngle += m_spinRate * timestep; // Update camera angle
	SetPosition(m_cameraDistance*Vector3(cos(m_cameraAngle), m_cameraHeight, sin(m_cameraAngle))); // Move camera

	if (m_viewDirty)	//We will only recalculate a matrix if it is "dirty"
	{
		m_view = DirectX::XMMatrixLookAtLH(m_position, m_lookAtTarget, m_up);
		m_viewDirty = false;	//Once we recalculate the matrix then it is no longer dirty!
	}

	if (m_projectionDirty)
	{
		m_projection = DirectX::XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_nearClip, m_farClip);
		m_projectionDirty = false;
	}
}
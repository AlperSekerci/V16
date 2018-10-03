#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <DefaultComponents/Physics/RigidBodyComponent.h>
#include "MyCamera.h"
#include "Common/ILogitechG920.h"

#define CAR_BRAKE_MIN 1.0f
#define CAR_DRIFT_THRESHOLD 0.5f
#define CAR_DRIFT_CLAMP 10.0f
#define CAR_DRIFT_FORCE 3.0f

class CCarDynamicsComp final : public IEntityComponent
{
private:
	Cry::DefaultComponents::CRigidBodyComponent *m_pRigidBody;

	ILogitechG920 *m_pController;

	MyCamera *m_pCamera;
	void CreateCamera();
	void MakeCamFollow();

	float mWheelbase = 2;
	float mHalfWheelbase = 1;
	float mMass = 100;

	float mGas = 7;	
	float mBrake = 70;
	float GetDeviceGas(Vec3 vel);
	float GetDeviceSteerAngle();

	float driftVel = 0;

public:
	CCarDynamicsComp() = default;
	virtual ~CCarDynamicsComp() {}

	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	static void Register(Schematyc::IEnvRegistrar& registrar);
	static void ReflectType(Schematyc::CTypeDesc<CCarDynamicsComp>& desc)
	{
		desc.SetGUID("{5A5C864B-EBE0-4F3C-8C6B-6FE4AC0C5E10}"_cry_guid);
		desc.SetEditorCategory("Vehicle Components");
		desc.SetLabel("Car Dynamics Component");
				
		desc.AddMember(&CCarDynamicsComp::mWheelbase, 'wbs', "Wheelbase", "Wheelbase", 0, 2);
		desc.AddMember(&CCarDynamicsComp::mGas, 'gas', "Gas", "Gas", 0, 7);
		desc.AddMember(&CCarDynamicsComp::mBrake, 'brk', "Brake", "Brake", 0, 70);
	}
};
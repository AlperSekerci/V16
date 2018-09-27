#include "CCarDynamicsComp.h"

void CCarDynamicsComp::Register(Schematyc::IEnvRegistrar & registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CCarDynamicsComp));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&CCarDynamicsComp::Register)

void CCarDynamicsComp::CreateCamera()
{
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	spawnParams.sName = "MyCameraEntity";
	IEntity *camEnt = gEnv->pEntitySystem->SpawnEntity(spawnParams);

	m_pCamera = camEnt->GetOrCreateComponentClass<MyCamera>();
}

void CCarDynamicsComp::Initialize()
{
	m_pRigidBody = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CRigidBodyComponent>();
	CreateCamera();

	ILogitechG920Plugin* pPlugin = gEnv->pSystem->GetIPluginManager()->QueryPlugin<ILogitechG920Plugin>();
	if (pPlugin) m_pController = pPlugin->GetLogitechG920();
	m_pController->Init();

	mHalfWheelbase = mWheelbase * 0.5f;
}

uint64 CCarDynamicsComp::GetEventMask() const
{
	return ENTITY_EVENT_BIT(ENTITY_EVENT_PREPHYSICSUPDATE) | ENTITY_EVENT_BIT(ENTITY_EVENT_UPDATE) | ENTITY_EVENT_BIT(ENTITY_EVENT_RESET);
}

void CCarDynamicsComp::MakeCamFollow()
{
	Matrix34 localModif = Matrix34(Vec3(1, 1, 1), Quat(Ang3(-0.6, 0, 0)), Vec3(0, -4, 5));
	Matrix34 camTM = m_pEntity->GetWorldTM() * localModif;
	// Matrix34 camTM = Matrix34(Vec3(1,1,1), IDENTITY, m_pEntity->GetWorldPos()) * localModif;
	m_pCamera->GetEntity()->SetWorldTM(camTM);
}

void CCarDynamicsComp::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_UPDATE:
	{
		float dt = event.fParam[0];
		MakeCamFollow();
	}
	break;
	case ENTITY_EVENT_PREPHYSICSUPDATE:
	{
		float dt = event.fParam[0];

		if (m_pController) m_pController->Update();

		m_pController->PlayContantForce(0, 0);
		m_pController->PlayDamperForce(0, 0);

		Matrix33 worldRot = Matrix33(m_pEntity->GetWorldRotation());
		Matrix33 invertedWorldRot = worldRot.GetInverted();

		Vec3 worldVel = m_pRigidBody->GetVelocity();
		Vec3 worldAngVel = m_pRigidBody->GetAngularVelocity();

		Vec3 localVel = invertedWorldRot * worldVel;
		Vec3 localAngVel = invertedWorldRot * worldAngVel;

		Vec3 gasForce = m_pEntity->GetForwardDir() * GetDeviceGas(worldVel) * dt * 10;
		// Vec3 gasForce = m_pEntity->GetForwardDir() * 1;
		// gasForce += Vec3(0, 0, -100);
		m_pRigidBody->ApplyImpulse(gasForce);
		// gEnv->pAuxGeomRenderer->DrawLine(m_pEntity->GetWorldPos(), Col_Black, m_pEntity->GetWorldPos() + gasForce, Col_Black, 10);

		Vec2 frontVel;
		frontVel.x = localVel.x - mHalfWheelbase * localAngVel.z;
		frontVel.y = localVel.y;

		float steerAngle = GetDeviceSteerAngle();
		Vec2 frontStaticDir = Vec2(cosf(steerAngle), sinf(steerAngle));
		Vec2 frontDir = Vec2(-frontStaticDir.y, frontStaticDir.x);

		float reducedFront = frontStaticDir.Dot(frontVel);
		Vec2 reducedFrontVec = -frontStaticDir * reducedFront;
		Vec2 targetFrontVel = frontVel + reducedFrontVec;
		// targetFrontVel += frontDir * (GetDeviceGas(worldVel) * dt * 10);

		Matrix33 matrix;
		Vec3 target;

		matrix.m00 = 1;
		matrix.m01 = 0;
		matrix.m02 = -mHalfWheelbase;
		target.x = targetFrontVel.x;

		matrix.m10 = 0;
		matrix.m11 = 1;
		matrix.m12 = 0;
		target.y = targetFrontVel.y;

		matrix.m20 = 1;
		matrix.m21 = 0;
		matrix.m22 = mHalfWheelbase;
		target.z = 0;
		// target.z = 0.9f * (localVel.x + mHalfWheelbase * localAngVel.z);

		Vec3 sol = matrix.GetInverted() * target;

		localVel.x = sol.x;
		localVel.y = sol.y;
		localAngVel.z = sol.z;

		// CryLog("sol x y z: %f %f %f", sol.x, sol.y, sol.z);

		Vec3 newVel = worldRot * localVel;
		Vec3 newAngVel = worldRot * localAngVel;

		m_pRigidBody->SetVelocity(newVel);
		m_pRigidBody->SetAngularVelocity(newAngVel);
	}
	break;
	case ENTITY_EVENT_RESET:
	{
		/*m_pRigidBody->SetVelocity(ZERO);
		m_pRigidBody->SetAngularVelocity(ZERO);*/
	}
	break;
	}
}

float CCarDynamicsComp::GetDeviceSteerAngle()
{
	return -m_pController->GetCurrentState().wheel;
}

float CCarDynamicsComp::GetDeviceGas(Vec3 vel)
{
	LogitechG920State state = m_pController->GetCurrentState();

	if (state.brake > 0)
	{
		if (vel.GetLength() < CAR_BRAKE_MIN)
		{
			// m_pRigidBody->SetVelocity(ZERO);		
			return 0;
		}
		
		int forwOrBackw = vel.Dot(m_pEntity->GetForwardDir()) >= 0 ? 1 : -1;
		return state.brake * mBrake * -forwOrBackw;				
	}

	return state.accel * mGas;
}
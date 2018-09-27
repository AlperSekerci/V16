#include "MyCamera.h"

void MyCamera::Initialize()
{
	IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem();
	IView *pView = pViewSystem->CreateView();
	pView->LinkTo(m_pEntity);
	pViewSystem->SetActiveView(pView);

	REGISTER_COMMAND("SetCamPos", MyCamera::CmdSetCamPos, VF_NULL, "set camera position in world space");
	REGISTER_COMMAND("SetEulerAngles", MyCamera::CmdSetEulerAngles, VF_NULL, "set euler angles of camera");

	gEnv->pInput->ShowCursor(true);
}

void MyCamera::CmdSetCamPos(IConsoleCmdArgs* pArgs)
{
	if (pArgs->GetArgCount() > 3)
	{
		const char* strX = pArgs->GetArg(1);
		float x = std::strtof(strX, 0);
		const char* strY = pArgs->GetArg(2);
		float y = std::strtof(strY, 0);
		const char* strZ = pArgs->GetArg(3);
		float z = std::strtof(strZ, 0);

		Vec3 pos = Vec3(x, y, z);

		IEntity *pCamEntity = gEnv->pEntitySystem->FindEntityByName("MyCameraEntity");
		pCamEntity->GetOrCreateComponentClass<MyCamera>()->SetPos(pos);
	}
}

void MyCamera::CmdSetEulerAngles(IConsoleCmdArgs* pArgs)
{
	if (pArgs->GetArgCount() > 3)
	{
		const char* strX = pArgs->GetArg(1);
		float x = std::strtof(strX, 0);
		const char* strY = pArgs->GetArg(2);
		float y = std::strtof(strY, 0);
		const char* strZ = pArgs->GetArg(3);
		float z = std::strtof(strZ, 0);

		Vec3 angles = Vec3(x, y, z);

		IEntity *pCamEntity = gEnv->pEntitySystem->FindEntityByName("MyCameraEntity");
		pCamEntity->GetOrCreateComponentClass<MyCamera>()->SetEulerAngles(angles);
	}
}

uint64 MyCamera::GetEventMask() const
{
	return ENTITY_EVENT_BIT(ENTITY_EVENT_START_GAME) | ENTITY_EVENT_BIT(ENTITY_EVENT_UPDATE);
}

void MyCamera::ProcessEvent(const SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_START_GAME:
	{
	}
	break;
	case ENTITY_EVENT_UPDATE:
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];
		float dt = pCtx->fFrameTime;

		Matrix34 transform = m_pEntity->GetWorldTM();		
		Vec3 newPos;

		if (smooth)
		{
			newPos = Vec3::CreateLerp(m_pEntity->GetWorldPos(), mPos, dt * 6);
		}
		else
		{
			newPos = mPos;
		}

		transform.SetTranslation(newPos);
		// m_pEntity->SetWorldTM(transform);

		Quat newQuat;

		if (smooth)
		{
			Quat currentQuat = m_pEntity->GetWorldRotation();
			Quat targetQuat = Quat(mEulerAngles * 0.0174532925f);
			newQuat = Quat::CreateSlerp(currentQuat, targetQuat, dt * 3);
		}
		else
		{
			newQuat = Quat(mEulerAngles * 0.0174532925f);
		}

		// m_pEntity->SetRotation(newQuat);

		// Mouse Ray Test
		/*Vec3 origin;
		Vec3 dir;
		MousePointToRay(origin, dir);
		dir *= 9999;

		ray_hit hit;
		int numHits = gEnv->pPhysicalWorld->RayWorldIntersection(origin,
			dir, ent_static | ent_terrain, rwi_stop_at_pierceable |
			rwi_colltype_any, &hit, 1);

		if (numHits > 0)
		{
			gEnv->pAuxGeomRenderer->DrawSphere(hit.pt, 0.1f, Col_Black);
		}*/
	}
	break;
	}
}

void MyCamera::MousePointToRay(Vec3& origin, Vec3& dir)
{
	float mouseX;
	float mouseY;
	gEnv->pHardwareMouse->GetHardwareMouseClientPosition(&mouseX, &mouseY);
	mouseY = gEnv->pRenderer->GetHeight() - mouseY;

	Vec3 viewportPos = Vec3(mouseX, mouseY, 0);
	Vec3 mousePos = Vec3(0, 0, 0);

	CCamera camera = gEnv->pSystem->GetViewCamera();	
	camera.Unproject(viewportPos, mousePos);

	origin = mPos;
	dir = (mousePos - origin).normalized();
}

Vec3 MyCamera::GetPos()
{
	return mPos;
}

void MyCamera::SetPos(Vec3 pos)
{
	mPos = pos;
}

Vec3 MyCamera::GetEulerAngles()
{
	return mEulerAngles;
}

void MyCamera::SetEulerAngles(Vec3 eulerAngles)
{
	mEulerAngles = eulerAngles;
}
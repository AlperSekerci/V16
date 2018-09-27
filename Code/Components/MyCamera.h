#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <CrySystem/IConsole.h>
#include <IViewSystem.h>
#include <CryGame\IGameFramework.h>
#include <CryInput/IHardwareMouse.h>
#include <CrySystem\ISystem.h>

class MyCamera final : public IEntityComponent
{
private:
	Vec3 mPos;
	Vec3 mEulerAngles;
	bool smooth = false;

	static void CmdSetCamPos(IConsoleCmdArgs *pCmdArgs);
	static void CmdSetEulerAngles(IConsoleCmdArgs *pCmdArgs);

public:	
	MyCamera() = default;
	virtual ~MyCamera() {}

	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;

	static void ReflectType(Schematyc::CTypeDesc<MyCamera>& desc)
	{
		desc.SetGUID("{E8FBE1A2-C2CB-4DA9-BE84-CA0373E840A1}"_cry_guid);
	}

	void MousePointToRay(Vec3& origin, Vec3& dir);

	Vec3 GetPos();
	void SetPos(Vec3 pos);

	Vec3 GetEulerAngles();
	void SetEulerAngles(Vec3 eulerAngles);
};
// Copyright 2018 Augmea Inc. All rights reserved.
// Created: 13:58 9.01.2018 by Ali Mehmet Altundag

#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryInput/IInput.h>

#define LOGITECH_G920_BUTTON_COUNT 0x80

struct LogitechG920State
{
	LogitechG920State()
		: wheel(0.f)
		, accel(0.f)
		, brake(0.f)
		, clutch(0.f)
	{
		memset(&buttons, 0, sizeof(buttons));
	}

	float wheel;
	float accel;
	float brake;
	float clutch;
	bool buttons[LOGITECH_G920_BUTTON_COUNT];
};

/* key IDs defined for the G920, please mind the EKeyId
so that we dont have any conflict */
enum ELogitechG920KeyId : uint32
{
	eKI_LogitechG920_Accel = 0x4000,
	eKI_LogitechG920_Brake,
	eKI_LogitechG920_Clutch,
	eKI_LogitechG920_Wheel,
	eKI_LogitechG920_Button_Right,
	eKI_LogitechG920_Button_Left,
	eKI_LogitechG920_Button_A,
	eKI_LogitechG920_Button_B,
	eKI_LogitechG920_Button_X,
	eKI_LogitechG920_Button_Y,
};

struct ILogitechG920
{
	/* initializes the device, returns if init process
	succesfull. must be called once after creation */
	virtual bool Init() = 0;

	/* restarts logitech g920 */
	virtual void Reset() = 0;

	/* returns device name */
	virtual const char * GetName() = 0;

	/* updates device state and propagate event if needed.
	must be called for each system tick */
	virtual void Update() = 0;

	/* resets all input state */
	virtual void ClearInputState() = 0;

	/* plays constant force */
	virtual bool PlayContantForce(const int index, const int magnitudePercentage) = 0;

	/**/
	virtual bool PlaySpringForce(const int index, const int offsetPercentage, const int saturationPercentage, const int coefficientPercentage) = 0;

	/**/
	virtual bool PlayDamperForce(const int index, const int coefficientPercentage) = 0;

	virtual const LogitechG920State& GetCurrentState() = 0;
};

struct ILogitechG920Plugin
	: public Cry::IEnginePlugin
{
	CRYINTERFACE_DECLARE_GUID(ILogitechG920Plugin, "93786a03-66be-49de-bc5b-34f62c31677e"_cry_guid);

public:
    /* returns logitech G920 interface */
	virtual ILogitechG920* GetLogitechG920() const = 0;
};
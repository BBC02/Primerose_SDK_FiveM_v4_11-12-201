#pragma once

enum class OverlayType
{
	Steam,
	Discord
};

struct HookData 
{
	const char* Module;
};

class CRenderer
{
public:
	static CRenderer* Get ( )
	{
		static auto Instance = new CRenderer;
		return Instance;
	}

	CRenderer ( ) : InitSuccess ( false ) { }

public:
	HookData m_Hook;

	bool InitSuccess;
	bool Open = true;

	int OpenValue = VK_DELETE;

	bool Initialize ( OverlayType Type );
};


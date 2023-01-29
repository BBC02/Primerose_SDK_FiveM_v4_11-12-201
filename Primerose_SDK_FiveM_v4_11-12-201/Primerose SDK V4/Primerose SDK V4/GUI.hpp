#pragma once

class CGUI
{
	int Tab = 0;
	bool Selected = false;

	void Background ( );

public:
	static CGUI* Get ( )
	{
		static auto Instance = new CGUI;
		return Instance;
	}

	void Theme ( );
	void Render ( );
};


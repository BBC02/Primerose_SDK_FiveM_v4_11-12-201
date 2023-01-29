#pragma once

class Nt
{
public:
	static Nt* Get ( )
	{
		static auto Instance = new Nt;
		return Instance;
	}
public:
	void Thread ( void* Routine );
	void SpoofThread ( void* Routine ); // Don't use on FiveM lol.
};


#include "pch.h"
#include "Cheat.hpp"
#include "Memory.hpp"
#include "Functions.hpp"
#include "ESP.hpp"

enum class FVersion
{
	b2372,
	b2189,
	b2060,
	b1604
};

void Cheat::Execute ( )
{
	static auto GetVersion = []() -> FVersion
	{
		static auto name = CMem::Get ( )->GetProcessName ( );

		if ( name.find ( _ ( "b2372" ) ) != std::string::npos )
			return FVersion::b2372;
		else if ( name.find ( _ ( "b2189" ) ) != std::string::npos )
			return FVersion::b2189;
		else if ( name.find ( _ ( "b2060" ) ) != std::string::npos )
			return FVersion::b2060;
		else
			return FVersion::b1604;
	};

	CALL_ONCE ( [ & ] ( ) 
	{ 
		Base = CMem::Get ( )->Module ( CMem::Get ( )->GetProcessName ( ).c_str() );

		if ( !Base )
		{
			CMem::Get ( )->Box ( _ ( "Developer error: #C0" ) );
			_Exit ( 0 );
		}

		CO->Offsets.WorldPattern = CMem::Get()->Pattern ( CMem::Get ( )->GetProcessName ( ).c_str ( ) , _ ( "48 8B 05 ? ? ? ? 45 ? ? ? ? 48 8B 48 08 48 85 C9 74 07 0F 28" ) );
		CO->Offsets.ReplayInterface = CMem::Get ( )->Pattern ( CMem::Get ( )->GetProcessName ( ).c_str ( ) , _ ( "48 8D 0D ? ? ? ? 48 8B D7 E8 ? ? ? ? 48 8D 0D ? ? ? ? 8A D8 E8 ? ? ? ? 84 DB 75 13 48 8D 0D ? ? ? ?" ) );;
		CO->Offsets.Bones = CMem::Get ( )->Pattern ( CMem::Get ( )->GetProcessName ( ).c_str ( ) , _ ( "48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 60 48 8B 01 41 8B E8 48 8B F2 48 8B F9 33 DB" ) );
		CO->Offsets.WorldToScreen = CMem::Get ( )->Pattern ( CMem::Get ( )->GetProcessName ( ).c_str ( ) , _ ( "48 89 5C 24 ?? 55 56 57 48 83 EC 70 65 4C 8B 0C 25" ) );
	} );


	if ( CO->Offsets.WorldPattern )
	{
		CO->Offsets.World = *reinterpret_cast< uintptr_t* >( CO->Offsets.WorldPattern + *( int* ) ( CO->Offsets.WorldPattern + 0x3 ) + 0x7 );

		if ( CO->Offsets.World )
		{
			CO->Offsets.LocalPlayer = *reinterpret_cast< uintptr_t* >( CO->Offsets.World + 0x8 );

			if ( CO->Offsets.LocalPlayer )
			{
				if ( GetVersion ( ) == FVersion::b1604 || GetVersion() == FVersion::b2189 )
				{
					uintptr_t playerInfo = 0 , weaponManager = 0 , weaponInfo = 0;

					if ( GetVersion ( ) == FVersion::b1604 )
					{
						playerInfo = *( uintptr_t* ) ( CO->Offsets.LocalPlayer + 0x10B8 );
						weaponManager = *( uintptr_t* ) ( CO->Offsets.LocalPlayer + 0x10C8 );
					}
					else if ( GetVersion ( ) == FVersion::b2189 )
					{
						playerInfo = *( uintptr_t* ) ( CO->Offsets.LocalPlayer + 0x10C8 );
						weaponManager = *( uintptr_t* ) ( CO->Offsets.LocalPlayer + 0x10D8 );
					}

					if ( weaponManager )
						weaponInfo = *( uintptr_t* ) ( weaponManager + 0x20 );

					if ( weaponInfo && weaponManager )
					{
						if ( CO->Aimbot.Weapon.Enabled )
						{
							if ( CO->Aimbot.Weapon.NoRecoil )
								*( float* ) ( weaponInfo + 0x2E8 ) = 0.0f;

							if ( CO->Aimbot.Weapon.NoSpread )
								*( float* ) ( weaponInfo + 0x74 ) = 0.0f;

							if ( CO->Aimbot.Weapon.Onetap )
								*( float* ) ( weaponInfo + 0xB0 ) = 10000.0f;
						}
					}
				}

				auto localPlayer = CLocalPlayer ( CO->Offsets.LocalPlayer );
				auto entities = GetAllEntities ( !CO->Visuals.DisplayNPC );

				for ( auto& entity : entities )
				{
					if ( CO->Visuals.Enabled )
					{
						if ( CO->Visuals.Boxes )
							ESP::Boxes ( entity );

						if ( CO->Visuals.Corners )
							ESP::Corners ( entity );

						if ( CO->Visuals.Snaplines )
							ESP::Snaplines ( localPlayer.Pelvis , entity , CO->Visuals.Snapline, ImColor(255,0,0));
					}

				}
			}
		}
	}
	
}
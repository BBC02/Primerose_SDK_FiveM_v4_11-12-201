#pragma once

namespace SDK
{
	static ImVec2 WorldToScreen ( Vec3 pos )
	{
		ImVec2 tempVec2;

		auto& io = ImGui::GetIO ( );
		reinterpret_cast< bool ( __fastcall* )( Vec3* , float* , float* ) >( CO->Offsets.WorldToScreen )( &pos , &tempVec2.x , &tempVec2.y );

		tempVec2.x *= ImGui::GetIO ( ).DisplaySize.x;
		tempVec2.y *= ImGui::GetIO ( ).DisplaySize.y;

		return tempVec2;
	}

	static ImVec2 GetBone ( const long long centity , const int wMask )
	{
		__m128 tempVec4;
		reinterpret_cast< void* ( __fastcall* )( long long , __m128* , int ) >( CO->Offsets.Bones )( centity , &tempVec4 , wMask );
		return WorldToScreen ( Vec3::FromM128 ( tempVec4 ) );
	}

	static Vec3 GetBoneLocation3D ( const long long centity , const int wMask )
	{
		__m128 tempVec4;
		reinterpret_cast< void* ( __fastcall* )( long long , __m128* , int ) >( CO->Offsets.Bones )( centity , &tempVec4 , wMask );
		return Vec3::FromM128 ( tempVec4 );
	}
}

struct CLocalPlayer
{
private:
	uintptr_t Address;
public:
	CLocalPlayer ( uintptr_t& address ) : Address ( address )
	{
		Location3D = *( Vec3* ) ( address + 0x90 );
		Location = SDK::WorldToScreen ( Location3D );
		Head = SDK::GetBone ( address , SKEL_Head );
		Pelvis = SDK::GetBone ( address , SKEL_Pelvis );
		LeftFoot = SDK::GetBone ( address , SKEL_L_Foot );
		RightFoot = SDK::GetBone ( address , SKEL_R_Foot );
	}

	float Health;
	float Armour;

	Vec3 Location3D;
	ImVec2 Location;

	ImVec2 Head;
	ImVec2 Pelvis;
	ImVec2 LeftFoot;
	ImVec2 RightFoot;
};


struct CPed
{
	uintptr_t Address;

	float Health;
	float Armour;

	Vec3 Location3D;
	ImVec2 Location;

	ImVec2 Head;
	ImVec2 Pelvis;
	ImVec2 LeftFoot;
	ImVec2 RightFoot;

	float Width;
	float Height;
	float Foot;

	double Distance;

	void Clear ( )
	{
		Address = 0;
		Health = 0;
		Armour = 0;
		Location3D = { 0,0,0 };
		Location = { 0,0 };
		Head = { 0, 0 };
		Pelvis = { 0,0 };
		LeftFoot = { 0, 0 };
		RightFoot = { 0, 0 };
		Width = 0;
		Height = 0;
		Foot = 0;
		Distance = 0;
	}

	bool OnScreen ( ImVec2 coords )
	{
		static auto size = ImGui::GetMainViewport ( )->Size;

		if ( coords.x < 0.1f || coords.y < 0.1 || coords.x > size.x || coords.y > size.y )
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool IsValid ( )
	{
		return ( Address > 0 && Health > 0 && Width > 0 && Height > 0 );
	}
};

struct Entity
{
	CPed Ped;

	Entity ( CPed _Ped ) : Ped ( _Ped ) { }
	~Entity ( ) { Ped.Clear ( ); }
};

class Cheat
{
	uintptr_t Base;

public:
	static Cheat* Get ( )
	{
		static auto Instance = new Cheat;
		return Instance;
	}

	void Execute ( );
	std::vector<Entity> GetAllEntities ( bool SkipNPC );

	std::string GetServerIP ( );

	std::string RandomString ( );
	std::vector<fx::Resource*> GetAllResources ( );

	bool InServer ( );
	void ExecuteCode ( std::string code );
	void ExecuteMenu ( std::string menu );
};


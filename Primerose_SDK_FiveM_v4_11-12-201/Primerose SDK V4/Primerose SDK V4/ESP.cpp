#include "pch.h"
#include "Cheat.hpp"
#include "ESP.hpp"

namespace ESP
{
	void Boxes ( Entity& entity )
	{
		if ( !entity.Ped.IsValid ( ) )
			return;

		ImVec2 pos = entity.Ped.Location;
		ImVec2 dim = ImVec2 ( entity.Ped.Width , entity.Ped.Height );

		ImColor color = ImColor ( 255 , 0 , 0 );
		int thick = 1;

		ImGui::GetBackgroundDrawList ( )->AddLine ( pos , ImVec2 ( pos.x - dim.x , pos.y ) , color , thick );
		ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x - dim.x , pos.y ) , ImVec2 ( pos.x - dim.x , pos.y + dim.y ) , color , thick );
		ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x - dim.x , pos.y + dim.y ) , ImVec2 ( pos.x , pos.y + dim.y ) , color , thick );
		ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x , pos.y + dim.y ) , ImVec2 ( pos.x , pos.y ) , color , thick );
	}

	void FilledBoxes ( Entity& entity )
	{
		if ( !entity.Ped.IsValid ( ) )
			return;
	}

	void Corners ( Entity& entity )
	{
		if ( !entity.Ped.IsValid ( ) )
			return;

		ImVec2 pos = entity.Ped.Location;
		ImVec2 dim = ImVec2 ( entity.Ped.Width , entity.Ped.Height );

		ImColor col = ImColor ( 255 , 0 , 0 );
		int thick = 1;

		float cornersizex = dim.x / 3;
		float cornersizey = dim.y / 4;

		ImDrawList* drawlist = ImGui::GetBackgroundDrawList ( );

		drawlist->AddLine ( ImVec2 ( pos ) , ImVec2 ( pos.x - cornersizex , pos.y ) , col , thick );

		drawlist->AddLine ( ImVec2 ( pos.x - dim.x , pos.y ) , ImVec2 ( ( pos.x - dim.x ) + cornersizex , pos.y ) , col , thick );

		drawlist->AddLine ( ImVec2 ( pos.x - dim.x , pos.y ) , ImVec2 ( pos.x - dim.x , pos.y - cornersizey ) , col , thick );

		drawlist->AddLine ( ImVec2 ( pos.x - dim.x , ( pos.y - dim.y ) ) , ImVec2 ( pos.x - dim.x , ( pos.y - dim.y ) + cornersizey ) , col , thick );

		drawlist->AddLine ( ImVec2 ( pos.x - dim.x , ( pos.y - dim.y ) ) , ImVec2 ( ( pos.x - dim.x ) + cornersizex , pos.y - dim.y ) , col , thick );

		drawlist->AddLine ( ImVec2 ( pos.x , ( pos.y - dim.y ) ) , ImVec2 ( pos.x - cornersizex , pos.y - dim.y ) , col , thick );

		drawlist->AddLine ( ImVec2 ( pos.x , ( pos.y - dim.y ) ) , ImVec2 ( pos.x , ( pos.y - dim.y ) + cornersizey ) , col , thick );

		drawlist->AddLine ( ImVec2 ( pos.x , pos.y ) , ImVec2 ( pos.x , pos.y - cornersizey ) , col , thick );
	}

	void Snaplines ( ImVec2& src , Entity& entity , int& type , ImColor color )
	{
		if ( !entity.Ped.IsValid ( ) )
			return;

		auto size = ImGui::GetIO ( ).DisplaySize;

		switch ( type )
		{
		case 0:
			ImGui::GetBackgroundDrawList ( )->AddLine ( src , entity.Ped.Head , color , 1 );
			break;
		case 1:
			ImGui::GetBackgroundDrawList ( )->AddLine ( src , entity.Ped.Pelvis , color , 1 );
			break;
		case 2:
			ImGui::GetBackgroundDrawList ( )->AddLine ( src , entity.Ped.LeftFoot , color , 1 );
			break;
		case 3:
			ImGui::GetBackgroundDrawList ( )->AddLine ( src , entity.Ped.RightFoot , color , 1 );
			break;
		case 4:
			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( size.x / 2 , size.y / 2 ) , entity.Ped.Head , color , 1 );
			break;
		}
	}

	void Skeleton ( Entity& entity )
	{
		if ( !entity.Ped.IsValid ( ) )
			return;
	}
}
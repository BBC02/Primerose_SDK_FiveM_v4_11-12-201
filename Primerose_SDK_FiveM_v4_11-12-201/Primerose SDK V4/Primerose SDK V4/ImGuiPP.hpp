#pragma once
#define HELPMARKER(PREFIX, STRING, ...) ImGui::SameLine(); ImGui::TextColored(ImColor(255,0,0), _("(%s)"), PREFIX); if (ImGui::IsItemHovered()) { ImGui::SetTooltip(STRING, __VA_ARGS__); }

struct Vec2
{
	float x , y;
};

struct Vec3
{
	float x , y , z;
	void Normalize ( ) {
		while ( y < -180 ) {
			y = 360;
		}
		while ( y > 180 ) {
			y = -360;
		}

		while ( x > 89 ) {
			x = -89;
		}
		while ( x < -89 ) {
			x = 89;
		}
	}

	Vec3 operator - ( Vec3 i ) {
		return {
		  x - i.x,
		  y - i.y,
		  z - i.z
		};
	}

	Vec3 operator + ( Vec3 i ) {
		return {
		  x + i.x,
		  y + i.y,
		  z + i.z
		};
	}

	Vec3 operator / ( float i ) {
		return {
		  x / i,
		  y / i,
		  z / i
		};
	}

	Vec3 operator * ( float i ) {
		return {
		  x * i,
		  y * i,
		  z * i
		};
	}

	static Vec3 FromM128 ( __m128 in )
	{
		return Vec3
		{
			in.m128_f32[0],
			in.m128_f32[1],
			in.m128_f32[2]
		};
	}
};

struct Vec4
{
	float x , y , z , w;
};


namespace ImGui
{
	static float X ( )
	{
		return ImGui::GetContentRegionAvail ( ).x;
	}

	static float Y ( )
	{
		return ImGui::GetContentRegionAvail ( ).y;
	}

	static void Line ( const char* str_id )
	{
		ImGui::Spacing ( );

		ImGui::PushID ( _ ( "##ImGui:Line_" ) );
		ImGui::PushStyleColor ( ImGuiCol_ChildBg , IM_COL32 ( 0 , 0 , 0 , 0 ) );

		ImGui::BeginChild ( str_id , ImVec2 ( X ( ) , 1 ) , false );
		ImGui::Separator ( );
		ImGui::EndChild ( );

		ImGui::PopStyleColor ( );
		ImGui::PopID ( );

		ImGui::Spacing ( );
	}

	static void LineEx ( const char* str_id , float width )
	{
		ImGui::PushID ( _ ( "##ImGui:LineEx_" ) );
		ImGui::PushStyleColor ( ImGuiCol_ChildBg , IM_COL32 ( 0 , 0 , 0 , 0 ) );

		if ( ImGui::BeginChild ( str_id , ImVec2 ( width , 1 ) , false ) )
		{
			ImGui::Separator ( );
			ImGui::EndChild ( );
		}

		ImGui::PopStyleColor ( );
		ImGui::PopID ( );
	}

	static void LineVertical ( )
	{
		ImGui::SameLine ( );
		ImGui::SeparatorEx ( ImGuiSeparatorFlags_Vertical );
		ImGui::SameLine ( );
	}

	static void CenterItem ( float spaceAvailable , float elementWidth , float padding = 0 )
	{
		ImGui::SameLine ( ( ( spaceAvailable / 2 ) - ( elementWidth / 2 ) ) + padding );
	}

	static void Center ( const char* text , bool separator = false )
	{
		if ( text == nullptr )
			return;

		ImGui::Spacing ( );
		ImGui::SameLine ( ( ImGui::GetContentRegionAvail ( ).x / 2 ) - ( ImGui::CalcTextSize ( text ).x / 2 ) );
		ImGui::Text ( text );
		ImGui::Spacing ( );

		if ( separator )
			Line ( text );
	}

	static void CenterEx ( const char* text = ( const char* ) 0 , int padding = 0 , bool separator = false )
	{
		if ( text == nullptr )
			return;

		ImGui::Spacing ( );
		ImGui::SameLine ( ( ( ImGui::GetContentRegionAvail ( ).x / 2 ) - ( ImGui::CalcTextSize ( text ).x / 2 ) ) + padding );
		ImGui::Text ( text );
		ImGui::Spacing ( );

		if ( separator )
			Line ( text );
	}

	static void CenterButton ( const char* format , int* currentTab , int newTab , bool separator = false )
	{
		if ( !format || !currentTab )
			return;

		if ( *currentTab == newTab )
		{
			ImGui::GetFont ( )->FontSize = 22;
			ImGui::TextColored ( ImColor ( 255 , 0 , 0 ) , _ ( "|" ) );
			ImGui::SameLine ( );
		}

		static ImVec4 inactive = ImVec4 ( 1.0f , 1.0f , 1.0f , 1.0f );
		static ImVec4 active = ImVec4 ( 1.0f , 0.0f , 0.0f , 1.0f );

		Center ( format , separator );

		if ( ImGui::IsItemClicked ( ) )
			*currentTab = newTab;

		ImGui::Spacing ( );
	}

	static void CenterWithHelpmarker ( const char* text , const char* format , bool separator )
	{
		if ( text == nullptr )
			return;

		ImGui::Spacing ( );
		ImGui::SameLine ( ( ImGui::GetContentRegionAvail ( ).x / 2 ) - ( ImGui::CalcTextSize ( text ).x / 2 ) );
		ImGui::Text ( text );
		ImGui::SameLine ( );
		ImGui::TextColored ( ImColor ( 255 , 0 , 0 ) , _ ( "(?)" ) );
		if ( ImGui::IsItemHovered ( ) )
			ImGui::SetTooltip ( format );
		ImGui::Spacing ( );

		if ( separator )
			Line ( text );
	}

	static void CreateItem ( const char* label , const char* marker , bool* key )
	{
		if ( !label );

		if ( marker == 0 || marker == nullptr )
		{
			ImGui::Center ( label , true );
		}
		else
		{
			ImGui::CenterWithHelpmarker ( label , marker , key );
		}

		ImGui::PushID ( label );
		ImGui::Checkbox ( _ ( "Enabled" ) , key );
		ImGui::PopID ( );

		ImGui::PushID ( _ ( "CreateItem" ) );
		ImGui::Line ( label );
		ImGui::PopID ( );
	}

	static void CenterExEx ( const char* text , float width , bool separator )
	{
		if ( text == nullptr )
			return;

		ImGui::Spacing ( );
		ImGui::SameLine ( ( width / 2 ) - ( ImGui::CalcTextSize ( text ).x / 2 ) );
		ImGui::Text ( text );
		ImGui::Spacing ( );

		if ( separator )
			Line ( text );
	}

	static ImVec4 ToVec4 ( float r , float g , float b , float a )
	{
		return ImVec4 ( r / 255.0f , g / 255.0f , b / 255.0f , a / 255.0f );
	}

	static ImVec4 ToVec4 ( float* ref )
	{
		return ImVec4 ( ref[0] / 255.0f , ref[1] / 255.0f , ref[2] / 255.0f , ref[3] / 255.0f );
	}

	static ImVec4 ToVec4 ( ImColor ref )
	{
		return ImVec4 ( ref.Value.x / 255.0f , ref.Value.y / 255.0f , ref.Value.z / 255.0f , ref.Value.w / 255.0f );
	}

	static ImColor ToColor ( float* ref )
	{
		return ImColor ( ref[0] , ref[1] , ref[2] , ref[3] );
	}

	static ImColor ToColor ( float r , float g , float b , float a )
	{
		return ImColor ( r , g , b , a );
	}

	static ImColor ToColor ( ImVec4 ref )
	{
		return ImColor ( ref.x * 255.0f , ref.y * 255.0f , ref.z * 255.0f , ref.w * 255.0f );
	}

	static bool Spinner ( const char* label , float radius , int thickness , const ImU32& color )
	{
		ImGuiWindow* window = GetCurrentWindow ( );
		if ( window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID ( label );

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size ( ( radius ) * 2 , ( radius + style.FramePadding.y ) * 2 );

		const ImRect bb ( pos , ImVec2 ( pos.x + size.x , pos.y + size.y ) );
		ItemSize ( bb , style.FramePadding.y );
		if ( !ItemAdd ( bb , id ) )
			return false;

		// Render
		window->DrawList->PathClear ( );

		int num_segments = 30;
		int start = abs ( ImSin ( g.Time * 1.8f ) * ( num_segments - 5 ) );

		const float a_min = IM_PI * 2.0f * ( ( float ) start ) / ( float ) num_segments;
		const float a_max = IM_PI * 2.0f * ( ( float ) num_segments - 3 ) / ( float ) num_segments;

		const ImVec2 centre = ImVec2 ( pos.x + radius , pos.y + radius + style.FramePadding.y );

		for ( int i = 0; i < num_segments; i++ ) {
			const float a = a_min + ( ( float ) i / ( float ) num_segments ) * ( a_max - a_min );
			window->DrawList->PathLineTo ( ImVec2 ( centre.x + ImCos ( a + g.Time * 8 ) * radius ,
				centre.y + ImSin ( a + g.Time * 8 ) * radius ) );
		}

		window->DrawList->PathStroke ( color , false , thickness );
	}

	static void LoadingCircle ( const char* label , const float indicator_radius , const ImVec4& main_color , const ImVec4& backdrop_color , const int circle_count , const float speed )
	{
		ImGuiWindow* window = GetCurrentWindow ( );

		if ( window->SkipItems )
			return;

		ImGuiContext& g = *GImGui;
		const ImGuiID id = window->GetID ( label );

		const ImVec2 pos = window->DC.CursorPos;
		const float circle_radius = indicator_radius / 10.0f;
		const ImRect bb ( pos , ImVec2 ( pos.x + indicator_radius * 2.0f , pos.y + indicator_radius * 2.0f ) );

		ItemSize ( bb , ImGui::GetStyle ( ).FramePadding.y );

		if ( !ItemAdd ( bb , id ) )
			return;

		const float t = g.Time;
		const auto degree_offset = 2.0f * IM_PI / circle_count;

		for ( int i = 0; i < circle_count; ++i )
		{
			const auto x = indicator_radius * std::sin ( degree_offset * i );
			const auto y = indicator_radius * std::cos ( degree_offset * i );
			const auto growth = max ( 0.0f , std::sin ( t * speed - i * degree_offset ) );
			ImVec4 color;
			color.x = main_color.x * growth + backdrop_color.x * ( 1.0f - growth );
			color.y = main_color.y * growth + backdrop_color.y * ( 1.0f - growth );
			color.z = main_color.z * growth + backdrop_color.z * ( 1.0f - growth );
			color.w = 1.0f;

			window->DrawList->AddCircleFilled ( ImVec2 ( pos.x + indicator_radius + x , pos.y + indicator_radius - y ) , circle_radius + growth * circle_radius , GetColorU32 ( color ) );
		}
	}

	static ImColor Rainbow ( float* _Hue, float _RainbowSpeed = 0.001f )
	{
		if ( !_Hue )
			return ImColor ( 255 , 255 , 255 );

		*_Hue -= _RainbowSpeed;

		if ( *_Hue < -1.0f )
			*_Hue += 1.0f;

		for ( unsigned int i = 0; i < 255; i++ )
		{
			float _hue = *_Hue + ( 1.f / 255.0f ) * i;

			if ( _hue < 0.f )
				_hue += 1.0f;

			return ImColor::HSV ( _hue , 1.f , 1.f );
		}
	}

	static void MakeItem ( const char* logo , const char* text , int* tab , int newTab , bool* sectionChosen )
	{
		if ( !logo || !text || !tab )
			return;

		ImGui::PushID ( text );
		ImGui::BeginChild ( _ ( "##MenuItemFirst_" ) , ImVec2 ( 145 , 140 ) );

		ImGui::SetCursorPosY ( ( ImGui::GetContentRegionAvail ( ).y / 2.2 ) );
		ImGui::Center ( logo , 0 );

		{
			ImGui::Spacing ( );
			ImGui::Spacing ( );

			ImGui::SetCursorPosY ( ImGui::GetContentRegionAvail ( ).y + 67.5 );
			ImGui::PushStyleColor ( ImGuiCol_ChildBg , IM_COL32 ( 201 , 103 , 93 , 255 ) );

			ImGui::BeginChild ( _ ( "##MenuItemSecond_" ) , ImVec2 ( 145 , 33 ) );
			ImGui::Spacing ( );

			ImGui::Center ( text );

			ImGui::EndChild ( );

			if ( ImGui::IsItemClicked ( ) )
			{
				*tab = newTab;
				*sectionChosen = true;
			}

			ImGui::PopStyleColor ( );
		}

		ImGui::EndChild ( );

		if ( ImGui::IsItemClicked ( ) )
		{
			*tab = newTab;
			*sectionChosen = true;
		}

		ImGui::PopID ( );
	}

	static void CenterItemEx ( float availableWidth , float elementWidth , float padding )
	{
		ImGui::SameLine ( ( ( availableWidth / 2 ) - ( elementWidth / 2 ) ) - padding );
	}

	template<typename T>
	static void MakeSection ( const char* str_id , ImVec2 size , T&& func )
	{
		if ( !str_id )
			return;

		ImGui::PushID ( str_id );

		ImGui::PushStyleColor ( ImGuiCol_ChildBg , IM_COL32 ( 0 , 0 , 0 , 0 ) );

		ImGui::BeginChild ( _ ( "##MakeSection__" ) , size , false );

		func ( );

		ImGui::EndChild ( );

		ImGui::PopStyleColor ( );

		ImGui::PopID ( );
	}

	static void ICheckbox ( const char* format , const char* helpmarker , float width , bool* value )
	{
		if ( !format || !value )
			return;

		ImGui::CenterItem ( width , ImGui::CalcTextSize ( format ).x );
		ImGui::Text ( format );

		if ( helpmarker != "" )
		{
			ImGui::SameLine ( );
			ImGui::TextColored ( ImColor ( 255 , 0 , 0 ) , _ ( "(?)" ) );

			if ( ImGui::IsItemHovered ( ) )
				ImGui::SetTooltip ( helpmarker );
		}

		ImGui::Spacing ( );
		ImGui::Separator ( );
		ImGui::Spacing ( );

		ImGui::PushID ( format );
		ImGui::Checkbox ( _ ( "Enabled" ) , value );
		ImGui::PopID ( );

		ImGui::Spacing ( );
		ImGui::Separator ( );
		ImGui::Spacing ( );
	}

	static void ISlider ( const char* format , float width , float min , float max , float* value )
	{
		if ( !format || !value )
			return;

		ImGui::PushStyleVar ( ImGuiStyleVar_FrameBorderSize , 1 );
		ImGui::PushStyleColor ( ImGuiCol_Border , IM_COL32 ( 0 , 0 , 0 , 0 ) );
		ImGui::PushItemWidth ( width );

		ImGui::SliderFloat ( _ ( "##ISliderFloat_" ) , value , min , max , format );

		ImGui::PopItemWidth ( );
		ImGui::PopStyleColor ( );
		ImGui::PopStyleVar ( );
	}

	static void ColorEditor ( const char* format , float* color )
	{
		if ( !format || !color )
			return;

		static auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip;

		ImGui::ColorEdit4 ( format , color , flags );
		ImGui::Spacing ( );
	}

	static void ICombo ( const char* label , std::vector<const char*> items , int* selected )
	{
		if ( !label || !selected )
			return;

		ImGui::PushItemWidth ( ImGui::X ( ) );
		ImGui::PushID ( label );
		ImGui::Combo ( _ ( "##ICombo_" ) , selected , items.data ( ) , items.size ( ) );
		ImGui::PopID ( );
		ImGui::PopItemWidth ( );
	}
	
	static void ICheckmark ( const char* label , bool* value )
	{
		ImGui::Checkbox ( label , value );
		ImGui::Spacing ( );
	}

}


class CDraw
{
public:
	static CDraw* Instance ( )
	{
		static auto _this = new CDraw;
		return _this;
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

	void Box ( ImVec2 pos , ImVec2 dim , ImColor color , int thickness )
	{
		//if (OnScreen(pos))
		{
			ImGui::GetBackgroundDrawList ( )->AddLine ( pos , ImVec2 ( pos.x - dim.x , pos.y ) , color , thickness );
			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x - dim.x , pos.y ) , ImVec2 ( pos.x - dim.x , pos.y + dim.y ) , color , thickness );
			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x - dim.x , pos.y + dim.y ) , ImVec2 ( pos.x , pos.y + dim.y ) , color , thickness );
			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x , pos.y + dim.y ) , ImVec2 ( pos.x , pos.y ) , color , thickness );
		}
	}

	void CornerBox ( ImVec2 pos , ImVec2 dim , ImColor col , int thickness )
	{
		//if (OnScreen(pos))
		{
			float cornersizex = dim.x / 3;
			float cornersizey = dim.y / 4;

			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos ) , ImVec2 ( pos.x - cornersizex , pos.y ) , col , thickness );

			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x - dim.x , pos.y ) , ImVec2 ( ( pos.x - dim.x ) + cornersizex , pos.y ) , col , thickness );

			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x - dim.x , pos.y ) , ImVec2 ( pos.x - dim.x , pos.y - cornersizey ) , col , thickness );

			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x - dim.x , ( pos.y - dim.y ) ) , ImVec2 ( pos.x - dim.x , ( pos.y - dim.y ) + cornersizey ) , col , thickness );

			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x - dim.x , ( pos.y - dim.y ) ) , ImVec2 ( ( pos.x - dim.x ) + cornersizex , pos.y - dim.y ) , col , thickness );

			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x , ( pos.y - dim.y ) ) , ImVec2 ( pos.x - cornersizex , pos.y - dim.y ) , col , thickness );

			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x , ( pos.y - dim.y ) ) , ImVec2 ( pos.x , ( pos.y - dim.y ) + cornersizey ) , col , thickness );

			ImGui::GetBackgroundDrawList ( )->AddLine ( ImVec2 ( pos.x , pos.y ) , ImVec2 ( pos.x , pos.y - cornersizey ) , col , thickness );
		}
	}

	void RectFilled ( float x0 , float y0 , float x1 , float y1 , ImColor color , float rounding , int rounding_corners_flags )
	{
		ImGui::GetBackgroundDrawList ( )->AddRectFilled ( ImVec2 ( x0 , y0 ) , ImVec2 ( x1 , y1 ) , color , rounding , rounding_corners_flags );
	}

	void HealthBar ( float x , float y , float w , float h , int phealth , bool Outlined )
	{
		int healthValue = max ( 0 , min ( phealth , 100 ) );

		ImColor barColor = ImColor
		(
			min ( 510 * ( 100 - healthValue ) / 100 , 255 ) , min ( 510 * healthValue / 100 , 255 ) ,
			25 ,
			255
		);

		if ( Outlined )
			ImGui::GetBackgroundDrawList ( )->AddRect ( ImVec2 ( x - 1 , y - 1 ) , ImVec2 ( x + w + 1 , y + h + 1 ) , ImColor ( 0.f , 0.f , 0.f ) , 0.0f , 0 , 1.0f );

		RectFilled ( x , y , x + w , y + ( int ) ( ( ( float ) h / 100.0f ) * ( float ) phealth ) , barColor , 0.0f , 0 );
	}

	void Line ( ImVec2 src , ImVec2 dst , ImColor , int )
	{
		ImGui::GetBackgroundDrawList ( )->AddLine ( src , dst , ImColor ( 255 , 0 , 0 ) , 1 );
	}
};

#define TO_COLOR(value) ImColor(value[0], value[1], value[2], value[3])
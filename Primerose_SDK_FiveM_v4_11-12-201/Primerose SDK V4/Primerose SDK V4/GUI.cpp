#include "pch.h"
#include "GUI.hpp"
#include "Renderer.hpp"
#include "TextEditor.h"
#include "Cheat.hpp"

extern ImFont* PFont;

static auto Renderer = CRenderer::Get ( );
static TextEditor Editor;

void CGUI::Theme ( )
{
    auto& style = ImGui::GetStyle ( );

    style.FrameBorderSize = 0;
    style.WindowRounding = 0;
    style.WindowPadding = ImVec2 ( 16 , 8 );
    style.TabRounding = 0;
    style.ScrollbarRounding = 0;
    style.FramePadding = ImVec2 ( 8 , 6 );
    style.WindowTitleAlign = ImVec2 ( 0.5f , 0.5f );

    style.Colors[ImGuiCol_TitleBg] = ImColor ( 227 , 0 , 40 , 255 );
    style.Colors[ImGuiCol_TitleBgActive] = ImColor ( 227 , 0 , 40 , 255 );
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor ( 15 , 15 , 15 , 50 );

    style.Colors[ImGuiCol_Border] = ImColor ( 0 , 0 , 0 , 0 );
    style.Colors[ImGuiCol_Text] = ImColor ( 255 , 255 , 255 );
    style.Colors[ImGuiCol_WindowBg] = ImColor ( 12 , 12 , 12 );

    style.Colors[ImGuiCol_Button] = ImColor ( 24 , 25 , 24 , 255 );
    style.Colors[ImGuiCol_ButtonActive] = ImColor ( 44 , 44 , 44 , 255 );
    style.Colors[ImGuiCol_ButtonHovered] = ImColor ( 44 , 44 , 44 , 255 );

    style.Colors[ImGuiCol_CheckMark] = ImColor ( 255 , 255 , 255 );

    style.Colors[ImGuiCol_FrameBg] = ImColor ( 36 , 37 , 36 , 255 );
    style.Colors[ImGuiCol_FrameBgActive] = ImColor ( 48 , 48 , 48 , 255 );
    style.Colors[ImGuiCol_FrameBgHovered] = ImColor ( 48 , 48 , 48 , 255 );

    style.Colors[ImGuiCol_Header] = ImColor ( 54 , 56 , 54 , 255 );
    style.Colors[ImGuiCol_HeaderActive] = ImColor ( 54 , 56 , 54 , 255 );
    style.Colors[ImGuiCol_HeaderHovered] = ImColor ( 54 , 56 , 54 , 255 );

    style.Colors[ImGuiCol_ResizeGrip] = ImColor ( 51 , 49 , 50 , 255 );
    style.Colors[ImGuiCol_ResizeGripActive] = ImColor ( 54 , 53 , 55 );
    style.Colors[ImGuiCol_ResizeGripHovered] = ImColor ( 51 , 49 , 50 , 255 );

    style.Colors[ImGuiCol_SliderGrab] = ImColor ( 62 , 62 , 62 , 255 );
    style.Colors[ImGuiCol_SliderGrabActive] = ImColor ( 62 , 62 , 62 , 255 );

    style.Colors[ImGuiCol_Border] = ImColor ( 54 , 54 , 54 );
    style.Colors[ImGuiCol_Separator] = ImColor ( 54 , 54 , 54 );

    style.Colors[ImGuiCol_SeparatorActive] = ImColor ( 54 , 54 , 54 );
    style.Colors[ImGuiCol_SeparatorHovered] = ImColor ( 54 , 54 , 54 );

    style.Colors[ImGuiCol_ChildBg] = ImColor ( 27 , 27 , 27 , 255 );
}

void CGUI::Background ( )
{
    auto Size = ImGui::GetIO ( ).DisplaySize;
    static auto Invisible = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::SetNextWindowPos ( ImVec2 ( 0 , 0 ) );
    ImGui::SetNextWindowSize ( Size );

    ImGui::PushStyleColor ( ImGuiCol_WindowBg ,IM_COL32 ( 255 , 255 , 255 , 50 ));
    ImGui::Begin ( _ ( "##Background" ) , &Renderer->Open , Invisible );
    {
        if ( !Selected )
        {
            ImGui::CenterItemEx ( Size.x , 145 , 70 * 2 );

            ImGui::SetCursorPosY ( ImGui::Y ( ) / 3.2 );

            ImGui::MakeItem ( ICON_FA_CODE , _ ( "Executor" ) , &Tab , 0, &Selected );
            ImGui::SameLine ( );

            ImGui::MakeItem ( ICON_FA_SERVER , _ ( "Resources" ) , &Tab , 1 , &Selected );
            ImGui::SameLine ( );

            ImGui::MakeItem ( ICON_FA_BOOK , _ ( "Menus" ) , &Tab , 2 , &Selected );

            ImGui::CenterItemEx ( Size.x , 145 , 70 * 2 );

            ImGui::SetCursorPosY ( ImGui::GetCursorPosY ( ) + 155 );

            ImGui::MakeItem ( ICON_FA_CROSSHAIRS , _ ( "Aimbot" ) , &Tab , 3 , &Selected );
            ImGui::SameLine ( );

            ImGui::MakeItem ( ICON_FA_USERS , _ ( "Visuals" ) , &Tab , 4 , &Selected );
            ImGui::SameLine ( );

            ImGui::MakeItem ( ICON_FA_COGS , _ ( "Settings" ) , &Tab , 5 , &Selected );
        }
    }
    ImGui::End ( );
    ImGui::PopStyleColor ( );
}

void CGUI::Render ( )
{
    CALL_ONCE ( [ & ] ( ) 
    { 
        Editor.SetPalette ( TextEditor::GetDarkPalette ( ) );
        Editor.SetShowWhitespaces ( false );
        Editor.SetReadOnly ( false );
    } );

    static ImGuiTextFilter ResourceFilter;
    auto Size = ImGui::GetIO ( ).DisplaySize;

    static int  Resource = 0;
    auto Resources = Cheat::Get ( )->GetAllResources ( );

    Background ( );

    ImGui::SetNextWindowSize ( ImVec2 ( 900 , 470 ) );
    ImGui::SetNextWindowPos ( ImVec2 ( ( ( Size.x / 2 ) - ( 900 / 2 ) ) , ImGui::Y ( ) / 2 ) );

    if ( Selected )
    {
        if ( Tab == 0 )
        {
            ImGui::PushStyleVar ( ImGuiStyleVar_WindowPadding , ImVec2 ( 0 , 0 ) );
            ImGui::Begin ( _ ( "Primerose - Executor" ) , &Selected , ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize );
            {
                {
                    ImGui::PushFont ( PFont );
                    Editor.Render ( _ ( "" ) , ImVec2 ( ImGui::X ( ) , ImGui::Y ( ) - 34 ) );
                    ImGui::PopFont ( );

                }

                ImGui::PushStyleVar ( ImGuiStyleVar_FrameBorderSize , 1 );

                {
                    if ( ImGui::Button ( _ ( "Execute" ) ) )
                    {
                        if ( !CO->Safemode )
                        {
                            Cheat::Get ( )->ExecuteCode ( Editor.GetText ( ) );
                        }
                    }
                }

                ImGui::SameLine ( 0 , 1 );

                {
                    if ( ImGui::Button ( _ ( "Execute from File" ) ) )
                    {
                        if ( !CO->Safemode )
                        {
                            CO->Dialog.Open ( );
                        }
                    }
                }

                ImGui::SameLine ( 0 , 1 );

                {
                    ImGui::PushItemWidth ( ImGui::X ( ) );

                    if ( ImGui::BeginCombo ( _ ( "##ResourcesList" ) , "Select a resource:" ) )
                    {
                        ImGui::EndCombo ( );
                    }

                    ImGui::PopItemWidth ( );
                }

                ImGui::PopStyleVar ( );
            }
            ImGui::End ( );
            ImGui::PopStyleVar ( );
        }

        else if ( Tab == 1 )
        {
            ImGui::PushStyleVar ( ImGuiStyleVar_WindowPadding , ImVec2 ( 18 , 18 ) );
            ImGui::Begin ( _ ( "Primerose - Resources" ) , &Selected , ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize );
            {
                ImGui::PushID ( _ ( "##SECTION_AIMBOT" ) );
                ImGui::PushStyleColor ( ImGuiCol_ChildBg , IM_COL32 ( 0 , 0 , 0 , 0 ) );

                ImGui::BeginChild ( _ ( "##SectionLeft" ) , ImVec2 ( ImGui::X ( ) / 2 , ImGui::Y ( ) ) );

                {
                    ImGui::ListBoxHeader ( _ ( "##ResourcesView" ), ImVec2(ImGui::X(), ImGui::Y()) );
                    if ( !CO->Safemode )
                    {
                        for ( int i = 0; i < Resources.size ( ); i++ )
                        {
                            auto resource = Resources[i];
                            bool selected = ( Resource == i );

                            if ( ResourceFilter.PassFilter ( resource->GetName ( ).c_str ( ) ) )
                            {
                                if ( ImGui::Selectable ( resource->GetName ( ).c_str ( ) , &selected ) )
                                    Resource = i;

                                if ( selected )
                                    ImGui::SetItemDefaultFocus ( );

                                ImGui::SameLine ( );
                                ImGui::Text ( _ ( "[%s]" ) , resource->GetState ( ) == fx::ResourceState::Started ? _ ( "STARTED" ) : _ ( "STOPPED" ) );
                            }
                        }
                    }
                    ImGui::ListBoxFooter ( );
                }

                ImGui::EndChild ( );

                // -------------------------------------

                ImGui::LineVertical ( );

                // -------------------------------------

                ImGui::BeginChild ( _ ( "##SectionRight" ) , ImVec2 ( ImGui::X ( ) , ImGui::Y ( ) ) );

                {
                    ResourceFilter.Draw ( _ ( "Search for a resource" ) , ImGui::X ( ) );

                    ImGui::Spacing ( );

                    if ( ImGui::Button ( _ ( "Start" ) , ImVec2 ( ImGui::X ( ) , 33 ) ) )
                    {
                        if ( !CO->Safemode )
                            Resources[Resource]->Start ( );
                    }

                    ImGui::Spacing ( );

                    if ( ImGui::Button ( _ ( "Stop" ) , ImVec2 ( ImGui::X ( ) , 33 ) ) )
                    {
                        if ( !CO->Safemode )
                            Resources[Resource]->Stop ( );
                    }
                }

                ImGui::EndChild ( );

                ImGui::PopStyleColor ( );
                ImGui::PopID ( );
            }
            ImGui::End ( );
            ImGui::PopStyleVar ( );
        }

        else if ( Tab == 2 )
        {
            ImGui::PushStyleVar ( ImGuiStyleVar_WindowPadding , ImVec2 ( 18 , 18 ) );
            ImGui::Begin ( _ ( "Primerose - Menus" ) , &Selected , ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize );
            {
                ImGui::PushStyleColor ( ImGuiCol_ChildBg , IM_COL32 ( 0 , 0 , 0 , 0 ) );
                {
                    ImGui::BeginChild ( _ ( "##LeftSideNonInternal" ) , ImVec2 ( ImGui::X ( ) / 2 , ImGui::Y ( ) ) );

                    ImGui::Center ( _ ( "Free Menus" ) , true );

                    {
                        ImGui::ListBoxHeader ( _ ( "##FreeMenusList" ) , ImVec2 ( ImGui::X ( ) , ImGui::Y ( ) - 41 ) );
                        ImGui::PushStyleVar ( ImGuiStyleVar_ItemSpacing , ImVec2 ( 60 , 8 ) );

                        if ( CO->Menus.size ( ) > 0 )
                        {
                            for ( int i = 0; i < CO->Menus.size ( ); i++ )
                            {
                                bool selected = ( CO->Menu == i );

                                if ( ImGui::Selectable ( CO->Menus[i].c_str ( ) , &selected ) )
                                    CO->Menu = i;

                                if ( selected )
                                    ImGui::SetItemDefaultFocus ( );
                            }
                        }

                        ImGui::PopStyleVar ( );
                        ImGui::ListBoxFooter ( );

                        ImGui::Spacing ( );

                        if ( ImGui::Button ( _("Execute Menu" ) , ImVec2 ( ImGui::X ( ) , 33 ) ) ) {
                            if ( CO->Menus.size ( ) > 0 )
                            {
                               Cheat::Get ( )->ExecuteMenu ( CO->Menus[CO->Menu] );
                            }
                        }
                    }

                    ImGui::EndChild ( );
                }
                ImGui::PopStyleColor ( );

                ImGui::SameLine ( );

                ImGui::PushStyleColor ( ImGuiCol_ChildBg , IM_COL32 ( 0 , 0 , 0 , 0 ) );
                {
                    ImGui::BeginChild ( _ ( "##RightSideNonInternal" ) , ImVec2 ( ImGui::X ( ) , ImGui::Y ( ) ) );

                    ImGui::Center ( _ ( "Premium Menus" ) , true );

                    {
                        ImGui::ListBoxHeader ( _ ( "##PremiumMenusList" ) , ImVec2 ( ImGui::X ( ) , ImGui::Y ( ) - 41 ) );

                        ImGui::ListBoxFooter ( );

                        ImGui::Spacing ( );

                        if ( ImGui::Button ( _ ( "Execute Menu" ) , ImVec2 ( ImGui::X ( ) , 33 ) ) ) { }
                    }

                    ImGui::EndChild ( );
                }
                ImGui::PopStyleColor ( );
            }
            ImGui::End ( );
            ImGui::PopStyleVar ( );
        }

        else if ( Tab == 3 )
        {
            ImGui::Begin ( _ ( "Primerose - Aimbot" ) , &Selected , ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize );
            {
                ImGui::PushID ( _ ( "##SECTION_AIMBOT" ) );
                ImGui::PushStyleColor ( ImGuiCol_ChildBg , IM_COL32 ( 0 , 0 , 0 , 0 ) );

                ImGui::BeginChild ( _ ( "##SectionLeft" ) , ImVec2 ( ImGui::X ( ) / 2 , ImGui::Y ( ) ) );

                {

                }

                ImGui::EndChild ( );

                // -------------------------------------

                ImGui::LineVertical ( );

                // -------------------------------------

                ImGui::BeginChild ( _ ( "##SectionRight" ) , ImVec2 ( ImGui::X ( ) , ImGui::Y ( ) ) );
                {
                    ImGui::ICheckbox ( _ ( "Weapons Enabled" ) , _ ( "Will allow you to use weapon features such as no recoil, etc." ) , ImGui::X ( ) , &CO->Aimbot.Weapon.Enabled );
                    ImGui::ICheckbox ( _ ( "No Recoil" ) , _ ( "Removes recoil from your current weapon." ) , ImGui::X ( ) , &CO->Aimbot.Weapon.NoRecoil );
                    ImGui::ICheckbox ( _ ( "No Spread" ) , _ ( "Removes any bullet spread from your current weapon" ) , ImGui::X ( ) , &CO->Aimbot.Weapon.NoSpread );
                    ImGui::ICheckbox ( _ ( "Onetap" ) , _ ( "Instantly kills anyone with 1 bullet." ) , ImGui::X ( ) , &CO->Aimbot.Weapon.Onetap );
                }
                ImGui::EndChild ( );

                ImGui::PopStyleColor ( );
                ImGui::PopID ( );
            }
            ImGui::End ( );
        }

        else if ( Tab == 4 )
        {
            ImGui::Begin ( _ ( "Primerose - Visuals" ) , &Selected , ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize );
            {
                ImGui::PushID ( _ ( "##SECTION_VISUALS" ) );
                ImGui::PushStyleColor ( ImGuiCol_ChildBg , IM_COL32 ( 0 , 0 , 0 , 0 ) );

                ImGui::BeginChild ( _ ( "##SectionLeft" ) , ImVec2 ( ImGui::X ( ) / 2 , ImGui::Y ( ) ) );

                {
                    ImGui::ICheckbox ( _ ( "Enabled" ) , "" , ImGui::X ( ) , &CO->Visuals.Enabled );
                    ImGui::ICheckbox ( _ ( "Boxes" ) , "" , ImGui::X ( ) , &CO->Visuals.Boxes );
                    ImGui::ICheckbox ( _ ( "Corners" ) , "" , ImGui::X ( ) , &CO->Visuals.Corners );
                    ImGui::ICheckbox ( _ ( "Snaplines" ) , "" , ImGui::X ( ) , &CO->Visuals.Snaplines );
                    ImGui::ICheckbox ( _ ( "Display NPC" ) , "" , ImGui::X ( ) , &CO->Visuals.DisplayNPC );
                }

                ImGui::EndChild ( );

                // -------------------------------------

                ImGui::LineVertical ( );

                // -------------------------------------

                ImGui::BeginChild ( _ ( "##SectionRight" ) , ImVec2 ( ImGui::X ( ) , ImGui::Y ( ) ) );
                {

                }
                ImGui::EndChild ( );

                ImGui::PopStyleColor ( );
                ImGui::PopID ( );
            }
            ImGui::End ( );
        }

        else if ( Tab == 5 )
        {
            ImGui::Begin ( _ ( "Primerose - Settings" ) , &Selected , ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize );
            {
                ImGui::PushID ( _ ( "##SECTION_SETTINGS" ) );
                ImGui::PushStyleColor ( ImGuiCol_ChildBg , IM_COL32 ( 0 , 0 , 0 , 0 ) );

                ImGui::BeginChild ( _ ( "##SectionLeft" ) , ImVec2 ( ImGui::X ( ) / 2 , ImGui::Y ( ) ) );

                {
                    // ImGui::ICheckbox ( _ ( "Safemode" ) , _ ( "Prevents you from using possibly detected features :)" ) , ImGui::X ( ) , &CO->Safemode );
                }

                ImGui::EndChild ( );

                // -------------------------------------

                ImGui::LineVertical ( );

                // -------------------------------------

                ImGui::BeginChild ( _ ( "##SectionRight" ) , ImVec2 ( ImGui::X ( ) , ImGui::Y ( ) ) );

                {
                    ImGui::Center ( fmt::format ( _ ( "IP: {}" ) , Cheat::Get ( )->GetServerIP ( ) ).c_str ( ) , true );
                }

                ImGui::EndChild ( );

                ImGui::PopStyleColor ( );
                ImGui::PopID ( );
            }
            ImGui::End ( );
        }
    }
}

#include "pch.h"
#include "Cheat.hpp"
#include "Memory.hpp"
#include "Auth.hpp"

std::vector<Entity> Cheat::GetAllEntities ( bool SkipNPC )
{
    std::vector<Entity> results = { };

    if ( CO->Offsets.LocalPlayer )
    {
        {
            auto localPlayerHealth = *( float* ) ( CO->Offsets.LocalPlayer + 0x280 );

            if ( localPlayerHealth > 0 )
            {
                auto replayInterface = *( uintptr_t* ) ( CO->Offsets.ReplayInterface + *( int* ) ( CO->Offsets.ReplayInterface + 0x3 ) + 0x7 );

                if ( replayInterface )
                {
                    auto pedReplayInterface = *( uintptr_t* ) ( replayInterface + 0x18 );

                    if ( pedReplayInterface )
                    {
                        int maxPeds = *( int* ) ( pedReplayInterface + 0x108 );

                        if ( maxPeds )
                        {
                            for ( int i = 0; i < maxPeds; i++ )
                            {
                                auto entityList = *( uintptr_t* ) ( pedReplayInterface + 0x100 );

                                if ( entityList )
                                {
                                    uintptr_t entity = *( uintptr_t* ) ( entityList + ( i * 0x10 ) );

                                    if ( entity )
                                    {
                                        if ( entity == CO->Offsets.LocalPlayer )
                                            continue;

                                        int entityType = *( int* ) ( entity + 0x10A8 );
                                        entityType = entityType << 11 >> 25;

                                        if ( SkipNPC && entityType != 2 /*NETWORK_PLAYER*/ )
                                            continue;

                                        float localHealth = *( float* ) ( CO->Offsets.LocalPlayer + 0x280 );
                                        float localArmour = *( float* ) ( CO->Offsets.LocalPlayer + 0x14E0 );

                                        float entityHealth = *( float* ) ( entity + 0x280 );
                                        float entityArmour = *( float* ) ( entity + 0x14E0 );

                                        if ( entityHealth <= 0 )
                                            continue;

                                        Vec3 entityLocation3D = *( Vec3* ) ( entity + 0x90 );
                                        Vec3 localLocation3D = *( Vec3* ) ( CO->Offsets.LocalPlayer + 0x90 );

                                        ImVec2 entityLocation = SDK::WorldToScreen ( entityLocation3D );
                                        ImVec2 localLocation = SDK::WorldToScreen ( localLocation3D );

                                        ImVec2 head = SDK::GetBone ( entity , SKEL_Head );
                                        ImVec2 pelvis = SDK::GetBone ( entity , SKEL_Pelvis );

                                        ImVec2 leftFoot = SDK::GetBone ( entity , SKEL_L_Foot );
                                        ImVec2 rightFoot = SDK::GetBone ( entity , SKEL_R_Foot );

                                        ImVec2 localHead = SDK::GetBone ( entity , SKEL_Head );
                                        ImVec2 localPelvis = SDK::GetBone ( entity , SKEL_Pelvis );

                                        ImVec2 localLeftFoot = SDK::GetBone ( entity , SKEL_L_Foot );
                                        ImVec2 localRightFoot = SDK::GetBone ( entity , SKEL_R_Foot );

                                        double distance = sqrt ( pow ( localLocation3D.x - entityLocation3D.x , 2 ) + pow ( localLocation3D.y - entityLocation3D.y , 2 ) + pow ( localLocation3D.y - entityLocation3D.y , 2 ) ) * 0.625;

                                        float FootAdv = ( leftFoot.y + rightFoot.y ) / 2;
                                        float height = fabs ( head.y - FootAdv ) * 1.2;
                                        float width = height / 2.5;

                                        results.push_back ( Entity ( CPed{ entity, entityHealth, entityArmour, entityLocation3D, entityLocation, head, pelvis, leftFoot, rightFoot, width, height, FootAdv, distance } ) );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return results;
}

std::string Cheat::GetServerIP ( )
{
    class Net
    {
    public:
        char IP[16];
        char pad_0010[56];
    };

    Net* pNet = ( Net* ) ( ( uintptr_t ) CMem::Get ( )->Module ( "net.dll" ) + 0xD8268 );
    return ( pNet ? ( strlen ( pNet->IP ) > 0 ? pNet->IP : _ ( "null" ) ) : _ ( "Invalid" ) );
}

std::string Cheat::RandomString ( )
{
    int randNum = rand ( ) % ( 24 - 8 + 1 ) + 8;
    std::string str ( _ ( "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" ) );

    std::random_device rd;
    std::mt19937 generator ( rd ( ) );

    std::shuffle ( str.begin ( ) , str.end ( ) , generator );

    return str.substr ( 0 , randNum );
}

std::vector<fx::Resource*> Cheat::GetAllResources ( )
{
    std::vector<fx::Resource*> results = { };
    auto resources = ( std::vector<fx::Resource*>* )( CMem::Get()->Module("citizen-resources-core.dll" )  + 0xC46D0 );

    for ( int i = 0; i < resources->size ( ); i++ )
        results.push_back ( resources->at ( i ) );

    return results;
}

bool Cheat::InServer ( )
{
    return GetAllResources ( ).size ( ) > 1;
}

void Cheat::ExecuteCode ( std::string code )
{
    if ( code.length ( ) <= 0 || !InServer() )
    {
        return;
    }

    std::string path = _ ( "C:\\ProgramData\\" );
    path.append ( RandomString ( ) );

    std::filesystem::create_directory ( path );

    std::ofstream fMetadata ( std::string ( path ).append ( _ ( "\\__resource.lua" ) ) );
    fMetadata << _ ( "fx_version 'cerulean'\n" );
    fMetadata << _ ( "client_script 'client.lua'\n" );
    fMetadata.close ( );

    std::ofstream fScript ( std::string ( path ).append ( _ ( "\\" ) ).append ( "client.lua" ) );
    fScript << code;
    fScript.close ( );

    auto manager = Instance<fx::ResourceManager>::Get ( );

    if ( manager )
    {
        auto resource = manager->CreateResource ( _ ( "bacon toaster" ) , 0 );

        resource->SetComponent ( new ResourceCacheEntryList{} );
        resource->LoadFrom ( path );
        resource->Start ( );
    }

    if ( std::filesystem::exists ( path ) )
        std::filesystem::remove_all ( path );
}

void Cheat::ExecuteMenu ( std::string menu )
{
    if ( menu.length ( ) <= 0)
        return;
    
    auto url = CO->Urls[CO->Menu];

    if ( url.length ( ) <= 0 )
    {
        CMem::Get ( )->Box ( _ ( "Failed to execute menu #1" ) );
        return;
    }

    auto response = cpr::Get ( cpr::Url{ url } );

    if ( response.text.length ( ) <= 0 )
    {
        CMem::Get ( )->Box ( _ ( "Failed to execute menu #2" ) );
        return;
    }

    ExecuteCode ( response.text );

    /*
    auto username = CAuth::Get()->userInfo[_ ( "username" )].get<std::string> ( );
    auto password = CAuth::Get ( )->userInfo[_ ( "password" )].get<std::string> ( );

    auto EncryptionKey = CAuth::Get ( )->RandomString ( 32 );

    AuthResponse response;
    AuthPayload payload{ };

    CAuth::Get ( )->Encrypt ( &payload , { { _ ( "username" ), username }, { _ ( "password" ) , password }, { _ ( "menu" ), Encryption::base64_encode(menu) } } , &EncryptionKey );
    CAuth::Get()->SendRequest ( _ ( "getMenus" ) , &payload , &response );

    EncryptionKey.clear ( );

    if ( response.IsSuccessful ( ) )
    {
        ExecuteCode ( response.Json[_ ( "status" )].get<std::string> ( ) );
    }*/
}
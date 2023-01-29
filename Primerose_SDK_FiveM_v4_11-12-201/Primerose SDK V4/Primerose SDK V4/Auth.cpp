#include "pch.h"
#include "Auth.hpp"
#include "Memory.hpp"

std::string CAuth::RandomString ( int length )
{
	std::string str ( "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" );

	std::random_device rd;

	std::mt19937 generator ( rd ( ) );
	std::shuffle ( str.begin ( ) , str.end ( ) , generator );

	return str.substr ( 0 , rand ( ) % ( length - 8 + 1 ) + 8 );
}

void CAuth::Encrypt ( AuthPayload* payload , std::unordered_map<std::string , std::string> params , std::string* encryptionKey )
{
	if ( !payload || !encryptionKey || params.size ( ) <= 0 )
		return;

	/*for ( const std::pair<std::string , std::string>& param : params )
	{
		auto temp = AesEncryption::encrypt ( param.second , *encryptionKey );
		auto encrypted = Encryption::base64_encode ( Encryption::base64_encode ( Encryption::base64_encode ( std::string ( temp.begin ( ) , temp.end ( ) ) ) ) );

		payload->Add ( cpr::Pair{ param.first, encrypted } );
	}

	payload->Add ( cpr::Pair{ _ ( "value" ), Encryption::base64_encode ( Encryption::base64_encode ( Encryption::base64_encode ( *encryptionKey ) ) ) } );*/
}

void CAuth::SendRequest ( std::string action , AuthPayload* payload , AuthResponse* response )
{
	if ( action.length ( ) <= 0 || !response )
		return;

	/*
	std::string szUrl = std::string ( _ ( "https://primerose.xyz/api/" ) ) + action;

	auto request = cpr::Post
	(
		cpr::Url{ szUrl } ,
		*payload
	);

	// Base64 Decode Encoded Response
	auto string = Encryption::base64_decode ( request.text );

	// Find the encrypted decryption key (for AES)
	auto encryptionKey = ( std::string ( string ).substr ( string.find ( ":" ) + 1 ) );

	// Get the encrypted message without the encryptionkey
	auto messageDecrypted = std::string ( );

	for ( int i = 0; i < ( string.length ( ) - encryptionKey.length ( ) ) - 1; i++ )
		messageDecrypted += string[i];

	// Decrypt the encrypted encryption key
	encryptionKey = Encryption::base64_decode ( Encryption::base64_decode ( Encryption::base64_decode ( encryptionKey ) ) );

	// Decrypt the entire message
	auto buffer = AesEncryption::decryptData ( messageDecrypted , encryptionKey );
	auto decryptedResponse = nlohmann::json::parse ( Encryption::base64_decode ( std::string ( buffer.begin ( ) , buffer.end ( ) ) ) );

	// Get the decryption key for the actual message
	auto decryptionKey = Encryption::base64_decode ( Encryption::base64_decode ( Encryption::base64_decode ( decryptedResponse["key"].get<std::string> ( ) ) ) );

	// Decrypt the actual message
	auto encryptedMessage = AesEncryption::decryptData ( decryptedResponse["message"].get<std::string> ( ) , decryptionKey );

	// Output the final decrypted message
	auto decryptedMessage = nlohmann::json::parse ( Encryption::base64_decode ( std::string ( encryptedMessage.begin ( ) , encryptedMessage.end ( ) ) ) );

	response->Status = decryptedMessage["status"].get<std::string> ( );

	if ( response->IsSuccessful ( ) )
	{
		response->Json = decryptedMessage;
		response->Size = response->Json.size ( );
	}
	else
	{
		response->Json = { };
		response->Size = 0;
	}*/
}

bool CAuth::GetStorage()
{
	auto Response = CMem::Get ( )->ReadRegistry ( _ ( "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0" ) , _ ( "CPUHash" ) , REG_SZ );

	if ( Response.size ( ) > 1 )
	{
		const auto Decrypted = Encryption::SecureDecrypt ( Response );

		if ( Decrypted.size ( ) > 1 )
		{
			userInfo = nlohmann::json::parse ( Decrypted );
			return true;
		}
	}

	return false;
}

bool CAuth::Authenticate()
{
	if ( !GetStorage ( ) )
		return false;

	std::string username = userInfo[_ ( "username" )].get<std::string> ( );
	std::string password = userInfo[_ ( "password" )].get<std::string> ( );

	auto EncryptionKey = RandomString ( 32 );

	AuthResponse response = AuthResponse{};
	AuthPayload payload = AuthPayload{};

	Encrypt ( &payload , { { _ ( "username" ), username }, { _ ( "password" ) , password }, { _ ( "hwid" ), CMem::Get( )->Hwid( ) } } , &EncryptionKey );
	SendRequest ( _ ( "login" ) , &payload , &response );

	EncryptionKey.clear ( );

	if ( response.IsSuccessful ( ) )
	{
		userInfo = response.Json;
		userInfo[_ ( "username" )] = username;
		userInfo[_ ( "password" )] = password;

		return true;
	}

	return false;
}

bool CAuth::UserAuthenticated()
{
	if ( userInfo.size ( ) <= 0 )
	{
		return false;
	}

	std::string username;
	std::string password;

	try
	{
		username = userInfo[_ ( "username" )].get<std::string> ( );
		password = userInfo[_ ( "password" )].get<std::string> ( );
	}
	catch ( nlohmann::json::exception& )
	{
		return false;
	}

	if ( username.length ( ) <= 0 || password.length ( ) <= 0 )
		return false;

	std::vector<std::string> products = { };

	try
	{
		for ( auto& product : userInfo[_ ( "products" )].get<std::vector<std::string>> ( ) )
		{
			products.push_back ( product );
		}

		if ( !( products.size ( ) >= 1 ) )
			return false;
	}
	catch ( nlohmann::json::exception& )
	{
		return false;
	}

	return ( username.length ( ) > 1 && password.length ( ) > 1 && products.size ( ) >= 1 );
}

std::vector<std::string> CAuth::GetProducts() const
{
	return userInfo[_ ( "products" )].get<std::vector<std::string>> ( );
}

bool CAuth::OwnsProduct(std::string product)
{
	static auto values = GetProducts ( );

	for ( const auto& value : values )
		if ( value.find ( product ) != std::string::npos )
			return true;

	return false;
}

json CAuth::GetChatMessages ( )
{
	auto username = userInfo[_ ( "username" )].get<std::string> ( );
	auto password = userInfo[_ ( "password" )].get<std::string> ( );

	auto EncryptionKey = RandomString ( 32 );

	AuthResponse response;
	AuthPayload payload{ };

	Encrypt ( &payload , { { _ ( "username" ), username }, { _ ( "password" ) , password } } , &EncryptionKey );
	SendRequest ( _ ( "sendMessage" ) , &payload , &response );

	EncryptionKey.clear ( );

	if ( response.IsSuccessful ( ) )
		return response.Json;

	else
	{
		CMem::Get ( )->Box ( _ ( "Failed to update chat messages!" ) );
		return nlohmann::json ( );
	}
}

void CAuth::SendChatMessage ( std::string message )
{
	if ( message.length ( ) <= 0 || message.length() > 248 )
		return;

	auto username = userInfo[_ ( "username" )].get<std::string> ( );
	auto password = userInfo[_ ( "password" )].get<std::string> ( );

	auto EncryptionKey = RandomString ( 32 );

	AuthResponse response;
	AuthPayload payload{ };

	Encrypt ( &payload , { { _ ( "username" ), username }, { _ ( "password" ) , password }, { _ ( "message" ), message } } , &EncryptionKey );
	SendRequest ( _ ( "sendMessage" ) , &payload , &response );

	EncryptionKey.clear ( );

	if ( !response.IsSuccessful ( ) )
	{
		CMem::Get ( )->Box ( _ ( "Failed to send chat message!" ) );
		return;
	}
}

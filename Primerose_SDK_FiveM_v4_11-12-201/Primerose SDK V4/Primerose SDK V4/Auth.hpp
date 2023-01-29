#pragma once

struct AuthResponse
{
	size_t Size;
	std::string Status;
	nlohmann::json Json;

	bool IsSuccessful ( )
	{
		return Status == ( "success" );
	}

	bool IsValid ( )
	{
		return Json.size ( ) > 1 && Size > 1;
	}

	void clear ( )
	{
		memset ( &Size , 0 , sizeof ( size_t ) );
		Status.clear ( );
		Json.clear ( );
	}
};

typedef cpr::Payload AuthPayload;

class CAuth
{
public:
	static CAuth* Get()
	{
		static auto Instance = new CAuth;
		return Instance;
	}
public:
	std::string RandomString ( int length );
	void Encrypt ( AuthPayload* payload , std::unordered_map<std::string , std::string> params , std::string* encryptionKey );
	void SendRequest ( std::string action , AuthPayload* payload , AuthResponse* response );
	bool GetStorage();

	json userInfo;

	bool Authenticate();
	bool UserAuthenticated();

	std::vector<std::string> GetProducts() const;
	bool OwnsProduct(std::string product);

	json GetChatMessages ( );
	void SendChatMessage ( std::string message );
};

#define PRIMEROSE_USERNAME CAuth::Get()->userInfo[_("username")].get<std::string>()
#define PRIMEROSE_LICENSE std::to_string(CAuth::Get()->userInfo[_("id")].get<int>()).c_str()
#define PRIMEROSE_PRODUCTS CAuth::Get()->userInfo[_("products")].get<std::string>()
#define PRIMEROSE_MENUS CAuth::Get()->userInfo[_("menus")].get<std::string>()

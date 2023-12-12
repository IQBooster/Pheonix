#include "stdafx.h"
#include "Features.h"
#include <string>
#include <string.h>
#include <WinUser.h>
#include <xutility>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <comutil.h>

bool FileExists(const std::string& fileName)
{
	struct stat buffer;
	return (stat(fileName.c_str(), &buffer) == 0);
}

char* AddStrings(char* string1, char* string2)
{
	std::ostringstream Text;
	Text << string1 << string2;
	return (char*)Text.str().c_str();
}

std::string DocumentsPath()
{
	/*
	wchar_t Folder[1024];
	HRESULT hr = SHGetFolderPathW(nullptr, CSIDL_MYDOCUMENTS, nullptr, 0, Folder);
	if (SUCCEEDED(hr))
	{
		char str[1024];
		wcstombs(str, Folder, 1023);
		return AddStrings(str, (char*)"\\Anim\\");
	}
	return "";
	*/
	return "C:\\Anim\\";
}

void setPlayerModel(Hash modelHash)
{
	if (STREAMING::IS_MODEL_IN_CDIMAGE(modelHash) && STREAMING::IS_MODEL_VALID(modelHash))
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(playerPed, true);

		//Getting Vehicle Infos
		Vehicle playerVehicle = NULL;
		int playerVehicleSeat = -1;
		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, false))
		{
			playerVehicle = PED::GET_VEHICLE_PED_IS_IN(playerPed, false);
			Hash playerVehicleModel = ENTITY::GET_ENTITY_MODEL(playerVehicle);

			if (VEHICLE::GET_VEHICLE_MODEL_NUMBER_OF_SEATS(playerVehicleModel) > 1)
			{
				for (int i = 0; i < (VEHICLE::GET_VEHICLE_MODEL_NUMBER_OF_SEATS(playerVehicleModel) - 1); i++)
				{
					if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(playerVehicle, i) == playerPed)
						playerVehicleSeat = i;
				}
			}
		}

		STREAMING::REQUEST_MODEL(modelHash);
		while (!STREAMING::HAS_MODEL_LOADED(modelHash)) WAIT(1);
		Ped newPed = PED::CREATE_PED(26, modelHash, playerCoords.x, playerCoords.y, playerCoords.z - 0.4f, ENTITY::GET_ENTITY_HEADING(playerPed), true, true);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);

		float gameplayCamHeading = CAM::GET_GAMEPLAY_CAM_RELATIVE_HEADING();
		float gameplayCamPitch = CAM::GET_GAMEPLAY_CAM_RELATIVE_PITCH();

		PLAYER::CHANGE_PLAYER_PED(PLAYER::PLAYER_ID(), newPed, true, true);

		CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(gameplayCamHeading);
		CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(gameplayCamPitch, 1.0f);

		PED::DELETE_PED(&playerPed);
		newPed = PLAYER::PLAYER_PED_ID();

		if (playerVehicle != NULL)
		{
			if (ENTITY::DOES_ENTITY_EXIST(playerVehicle))
				PED::SET_PED_INTO_VEHICLE(newPed, playerVehicle, playerVehicleSeat);
		}

		char* WeaponIDs[] = { "weapon_dagger", "weapon_bat", "weapon_bottle", "weapon_crowbar", "weapon_flashlight", "weapon_golfclub", "weapon_hammer", "weapon_hatchet", "weapon_knuckle", "weapon_knife", "weapon_machete", "weapon_switchblade", "weapon_nightstick", "weapon_wrench", "weapon_battleaxe", "weapon_poolcue", "weapon_stone_hatchet", "weapon_pistol", "weapon_pistol_mk2", "weapon_combatpistol", "weapon_appistol", "weapon_stungun", "weapon_pistol50", "weapon_snspistol", "weapon_snspistol_mk2", "weapon_heavypistol", "weapon_vintagepistol", "weapon_flaregun", "weapon_marksmanpistol", "weapon_revolver", "weapon_revolver_mk2", "weapon_doubleaction", "weapon_raypistol", "weapon_ceramicpistol", "weapon_navyrevolver", "weapon_gadgetpistol", "weapon_microsmg", "weapon_smg", "weapon_smg_mk2", "weapon_assaultsmg", "weapon_combatpdw", "weapon_machinepistol", "weapon_minismg", "weapon_raycarbine", "weapon_pumpshotgun", "weapon_pumpshotgun_mk2", "weapon_sawnoffshotgun", "weapon_assaultshotgun", "weapon_bullpupshotgun", "weapon_musket", "weapon_heavyshotgun", "weapon_dbshotgun", "weapon_autoshotgun", "weapon_combatshotgun", "weapon_assaultrifle", "weapon_assaultrifle_mk2", "weapon_carbinerifle", "weapon_carbinerifle_mk2", "weapon_advancedrifle", "weapon_specialcarbine", "weapon_specialcarbine_mk2", "weapon_bullpuprifle", "weapon_bullpuprifle_mk2", "weapon_compactrifle", "weapon_militaryrifle", "weapon_mg", "weapon_combatmg", "weapon_combatmg_mk2", "weapon_gusenberg", "weapon_sniperrifle", "weapon_heavysniper", "weapon_heavysniper_mk2", "weapon_marksmanrifle", "weapon_marksmanrifle_mk2", "weapon_rpg", "weapon_grenadelauncher", "weapon_minigun", "weapon_firework", "weapon_railgun", "weapon_hominglauncher", "weapon_compactlauncher", "weapon_rayminigun", "weapon_grenade", "weapon_bzgas", "weapon_molotov", "weapon_stickybomb", "weapon_proxmine", "weapon_snowball", "weapon_pipebomb", "weapon_ball", "weapon_smokegrenade", "weapon_flare", "weapon_petrolcan", "weapon_fireextinguisher", "weapon_hazardcan" };
		for (int i = 0; i < (sizeof(WeaponIDs) / sizeof(WeaponIDs[0])); i++)
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), GAMEPLAY::GET_HASH_KEY(WeaponIDs[i]), 9999, 1);
	}
}

int onlinePlayerCount()
{
	int count = 0;

	for (int i = 0; i < 32; i++)
	{
		if (ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i)) && NETWORK::NETWORK_IS_PLAYER_CONNECTED(i) && NETWORK::NETWORK_IS_PLAYER_ACTIVE(i))
		{
			count++;
		}
	}

	return count; 
}

Vector3 addVector(Vector3 vector, Vector3 vector2) 
{
	vector.x += vector2.x;
	vector.y += vector2.y;
	vector.z += vector2.z;
	return vector;
}
Vector3 RotationToDirection(Vector3 rot) 
{
	double num = rot.z * 0.017453292519943295;
	double num2 = rot.x * 0.017453292519943295;
	double val = cos(num2);
	double num3 = abs(val);
	rot.x = (float)(-(float)sin(num) * num3);
	rot.y = (float)(cos(num) * num3);
	rot.z = (float)sin(num2);
	return rot;
}
Vector3 multiplyVector(Vector3 vector, float inc) 
{
	vector.x *= inc;
	vector.y *= inc;
	vector.z *= inc;
	return vector;
}
float getDistance(Vector3 pointA, Vector3 pointB) 
{
	float a_x = pointA.x;
	float a_y = pointA.y;
	float a_z = pointA.z;
	float b_x = pointB.x;
	float b_y = pointB.y;
	float b_z = pointB.z;
	double x_ba = (double)(b_x - a_x);
	double y_ba = (double)(b_y - a_y);
	double z_ba = (double)(b_z - a_z);
	double y_2 = y_ba * y_ba;
	double x_2 = x_ba * x_ba;
	double sum_2 = y_2 + x_2;
	return(float)sqrt(sum_2 + z_ba);
}

char* CharKeyboard(char* windowName, int maxInput, char* defaultText)
{
	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(0, "", "", defaultText, "", "", "", maxInput);
	while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) WAIT(1);
	if (!GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT()) return "";
	return GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
}

double NumberKeyboard(double originalValue, double min, double max)
{
	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(1, "", "", "", "", "", "", 10);
	while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) WAIT(1);
	if (!GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT()) return originalValue;
	double result = atof(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT());
	if (result < min) return min;
	if (result > max) return max;
	return result;
}

double NumberKeyboard(double originalValue, double min, double max, double step)
{
	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(1, "", "", "", "", "", "", 10);
	while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) WAIT(1);
	if (!GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT()) return originalValue;
	double result = atof(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT());
	result = round(result / step);
	result = (result * step);
	if (result < min) return min;
	if (result > max) return max;
	return result;
}

void SetGameTime(int h, int m, int s)
{
	for (int i = 1; i < 15; i++)
	{
		WAIT(1);
		Hooking::set_session_time(4, 0);
		Hooking::ClockTime->hour = h;
		Hooking::ClockTime->minute = m;
		Hooking::ClockTime->second = s;
	}
}

void SetGameWeather(int weatherID)
{
	static int timer;
	if ((GetTickCount() - timer) > 300) {
		WAIT(1);
		Hooking::set_session_weather(1, weatherID, 76, 0);
		timer = GetTickCount();
	}
}

bool IsPedShooting(Ped ped) 
{
	Vector3 coords = ENTITY::GET_ENTITY_COORDS(ped, true);
	return PED::IS_PED_SHOOTING_IN_AREA(ped, coords.x, coords.y, coords.z, coords.x, coords.y, coords.z, true, true);
}

bool IsPedShooting2(Ped ped)
{
	return PED::GET_PED_CONFIG_FLAG(ped, 58, true);
}

void RequestControlOfid(Entity netid)
{
	int Tries = 0;
	while (!NETWORK::NETWORK_HAS_CONTROL_OF_NETWORK_ID(netid) && Tries <= 15)
	{
		Tries++;
		NETWORK::NETWORK_REQUEST_CONTROL_OF_NETWORK_ID(netid);
	}
}

void RequestControlOfEnt(Entity entity)
{
	int Tries = 0;
	while (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(entity) && Tries <= 15)
	{
		Tries++;
		NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(entity);
	}
	if (NETWORK::NETWORK_IS_SESSION_STARTED())
	{
		int netID = NETWORK::NETWORK_GET_NETWORK_ID_FROM_ENTITY(entity);
		RequestControlOfid(netID);
		NETWORK::SET_NETWORK_ID_CAN_MIGRATE(netID, 1);
	}
}

void DoAnimation(Player player, char* anim, char* animid)
{
	int playerPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player);
	for (int i = 1; i < 5; i++)
	{
		RequestControlOfEnt(playerPed);
		STREAMING::REQUEST_ANIM_DICT(anim);
		while (!STREAMING::HAS_ANIM_DICT_LOADED(anim)) WAIT(1);
		AI::TASK_PLAY_ANIM(playerPed, anim, animid, 8.0f, 0.0f, -1, 9, 0, 0, 0, 0);
	}
}

void DoScenario(Player player, char* scenario)
{
	int playerPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player);
	for (int i = 1; i < 5; i++)
	{
		RequestControlOfEnt(playerPed);
		AI::TASK_START_SCENARIO_IN_PLACE(playerPed, scenario, 0, true);
	}
}

Vector3 GetBlipMarker()
{
	Vector3 zero = { 0, 0, 0 };
	Vector3 coords;
	bool blipFound = false;
	int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
	for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
	{
		if (UI::GET_BLIP_INFO_ID_TYPE(i) == 4)
		{
			coords = UI::GET_BLIP_INFO_ID_COORD(i);
			blipFound = true;
			break;
		}
	}
	if (blipFound)
		return coords;
	return zero;
}

void TeleportToMarker()
{
	Vector3 coords = GetBlipMarker();
	if (coords.x == 0 && coords.y == 0)
		iconNotify("Info", "No Waypoint set!");
	else
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
			playerPed = PED::GET_VEHICLE_PED_IS_USING(playerPed);
		float groundZ;
		bool groundFound = false;
		for (int i = 0; i < 15; ++i)
		{
			for (float z = 0.f; z <= 900.f; z += 90.f)
			{
				STREAMING::REQUEST_COLLISION_AT_COORD(coords.x, coords.y, z);
				WAIT(1);
			}
			if (GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, 900.f, &groundZ, 0))
			{
				groundFound = true;
				break;
			}
		}
		if (!groundFound)
		{
			groundZ = 900.0f;
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), 0xFBAB5776, 1, 0);
		}
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, coords.x, coords.y, groundZ + 1.25f, 0, 0, 1);
	}
}

void notifyMap(char* fmt, ...)
{
	char buf[2048] = { 0 };
	va_list va_alist;

	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);

	char buff2[2048] = { 0 };
	sprintf_s(buff2, "%s", buf);

	UI::SET_TEXT_OUTLINE();
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(buff2);
	UI::_DRAW_NOTIFICATION(FALSE, FALSE);
}	void notifyMap(std::string str) { notifyMap(&str[0]); }

void iconNotify(char* subject, char* message)
{
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(message);
	UI::_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_LESTER_DEATHWISH", "CHAR_LESTER_DEATHWISH", true, 8, "Anim", subject, 1, "___Menu", 9, 0);
	UI::_DRAW_NOTIFICATION(FALSE, FALSE);
}

void DrawPlayerMarker(Player player)
{
	if (ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player)) && NETWORK::NETWORK_IS_PLAYER_CONNECTED(player))
	{
		Vector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player), false);
		GRAPHICS::DRAW_MARKER(20, coords.x, coords.y, coords.z + 1.35f, 0, 0, 0, 0, 180, 0, 0.75f, 0.75f, 0.75f, Menu::Settings::scroller.r, Menu::Settings::scroller.g, Menu::Settings::scroller.b, 200, 1, 1, 1, 0, 0, 0, 0);
	}
}

void LoadPlayerInfo(Player player)
{
    if (ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player)) && NETWORK::NETWORK_IS_PLAYER_CONNECTED(player))
    {
        char* playerName = PLAYER::GET_PLAYER_NAME(player);
        Ped playerPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player);
        char* rockstarID;
        bool isfriend;
        int NETWORK_HANDLE[76];
        NETWORK::NETWORK_HANDLE_FROM_PLAYER(player, &NETWORK_HANDLE[0], 13);
        if (NETWORK::NETWORK_IS_HANDLE_VALID(&NETWORK_HANDLE[0], 13))
        {
            rockstarID = NETWORK::NETWORK_MEMBER_ID_FROM_GAMER_HANDLE(&NETWORK_HANDLE[0]);
            isfriend = NETWORK::NETWORK_IS_FRIEND(&NETWORK_HANDLE[0]);
        }
        std::ostringstream RockstarID; RockstarID << "~r~RockstarID:~s~ " << rockstarID;
        std::ostringstream Title; Title << "Player Title:";
        bool ishost; if (NETWORK::NETWORK_GET_HOST_OF_SCRIPT("Freemode", -1, 0) == player) { ishost = true; }
        else { ishost = false; }
        bool inside = INTERIOR::IS_VALID_INTERIOR(INTERIOR::GET_INTERIOR_FROM_ENTITY(playerPed));
        bool title; if (PLAYER::PLAYER_ID() != player && !isfriend && !ishost && !inside) { title = false; }
        else { title = true; }
        if (PLAYER::PLAYER_ID() == player)
        {
            Title << " ~o~[Me]";
        }
        if (isfriend)
        {
            Title << " ~g~[F]";
        }
        if (ishost)
        {
            Title << " ~y~[SH]";
        }
        if (inside)
        {
            Title << " ~c~[I]";
        }
        if (!title)
        {
            Title << " [P]";
        }
		char ipBuff[32];
		auto CPed = Hooking::get_player_address(player);
		if (player != PLAYER::PLAYER_ID())
		{
			if (CPed)
			{
				auto CPlayerInfo = *(uintptr_t*)(CPed + 0x10C8);
				if (CPlayerInfo)
				{
					auto OnlineIP = (BYTE*)(CPlayerInfo + 0x44);
					std::snprintf(ipBuff, sizeof(ipBuff) - 1, "~r~IPv4:~w~ %u.%u.%u.%u", OnlineIP[3], OnlineIP[2], OnlineIP[1], *OnlineIP);
				}
			}
		}
		else
			std::snprintf(ipBuff, sizeof(ipBuff) - 1, "~r~IPv4:~w~ ***.***.***.***");

		int rank = globalHandle(1590682).At(player, 883).At(211).At(6).As<int>();
		std::ostringstream Rank; Rank << "Rank:~b~ ";
		if (!NETWORK::NETWORK_IS_SESSION_STARTED())
			Rank << "Offline";
		else
			Rank << rank;

		int money;
		if (!NETWORK::NETWORK_IS_SESSION_STARTED())
		{
			if (PED::IS_PED_MODEL(PLAYER::PLAYER_PED_ID(), GAMEPLAY::GET_HASH_KEY("player_zero")))
				STATS::STAT_GET_INT(GAMEPLAY::GET_HASH_KEY("SP0_TOTAL_CASH"), &money, 0);
			else if (PED::IS_PED_MODEL(PLAYER::PLAYER_PED_ID(), GAMEPLAY::GET_HASH_KEY("player_one")))
				STATS::STAT_GET_INT(GAMEPLAY::GET_HASH_KEY("SP1_TOTAL_CASH"), &money, 0);
			else if (PED::IS_PED_MODEL(PLAYER::PLAYER_PED_ID(), GAMEPLAY::GET_HASH_KEY("player_two")))
				STATS::STAT_GET_INT(GAMEPLAY::GET_HASH_KEY("SP2_TOTAL_CASH"), &money, 0);
			else money = 0;
		}
		else 
			money = globalHandle(1590682).At(player, 883).At(211).At(3).As<int>();

		std::ostringstream Money; Money << "Money:~g~ " << money << "$";
		int earnings = globalHandle(1590682).At(player, 883).At(211).At(56).As<int>();
		int bankMoney = earnings - money;
		std::ostringstream BankMoney; BankMoney << "Bank Money:~g~ ";
		if (!NETWORK::NETWORK_IS_SESSION_STARTED())
			BankMoney << "Offline";
		else if (bankMoney < 0)
			BankMoney << "Loading...";
		else
			BankMoney << bankMoney << "$";
        float health = ENTITY::GET_ENTITY_HEALTH(playerPed);
        float maxHealth = ENTITY::GET_ENTITY_MAX_HEALTH(playerPed);
        float healthPercent = health * 100 / maxHealth;
        std::ostringstream Health; Health << "Health: " << healthPercent << "%";
        float armor = PED::GET_PED_ARMOUR(playerPed);
        float maxArmor = PLAYER::GET_PLAYER_MAX_ARMOUR(player);
        float armorPercent = armor * 100 / maxArmor;
        std::ostringstream Armor; Armor << "Armor: " << armorPercent << "%";
        bool alive = !PED::IS_PED_DEAD_OR_DYING(playerPed, 1);
        char* aliveStatus;
        if (alive) aliveStatus = "~g~Yes"; else aliveStatus = "~r~No";
        std::ostringstream Alive; Alive << "Alive: " << aliveStatus;
        bool inVehicle = PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0);
        std::ostringstream VehicleModel; VehicleModel << "Vehicle: ";
        std::ostringstream Speed; Speed << "Speed: ";
        std::ostringstream IsInAVehicle; IsInAVehicle << "In Vehicle: ";
        if (inVehicle) {
            IsInAVehicle << "~g~Yes";
            Hash vehHash = ENTITY::GET_ENTITY_MODEL(PED::GET_VEHICLE_PED_IS_IN(playerPed, 0));
            VehicleModel << UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(vehHash));
            float vehSpeed = ENTITY::GET_ENTITY_SPEED(PED::GET_VEHICLE_PED_IS_IN(playerPed, 0));
            float vehSpeedConverted;
            vehSpeedConverted = round(vehSpeed * 3.6f);
            Speed << vehSpeedConverted << " KM/H";
        }
        else {
            IsInAVehicle << "~r~No";
            float speed = round(ENTITY::GET_ENTITY_SPEED(playerPed) * 100) / 100;
            Speed << speed << " M/S";
            VehicleModel << "--------";
        }
        std::ostringstream WantedLevel; WantedLevel << "Wanted Level: " << PLAYER::GET_PLAYER_WANTED_LEVEL(player);
        std::ostringstream Weapon; Weapon << "Weapon: ";
        Hash weaponHash;
        if (WEAPON::GET_CURRENT_PED_WEAPON(playerPed, &weaponHash, 1)) {
            char* weaponName = "Unknown";

			char* WeaponNames[] = { "Antique Cavalry Dagger", "Baseball Bat", "Broken Bottle", "Crowbar", "Flashlight", "Golf Club", "Hammer", "Hatchet", "Brass Knuckles", "Knife", "Machete", "Switchblade", "Nightstick", "Pipe Wrench", "Battle Axe", "Pool Cue", "Stone Hatchet", "Pistol", "Pistol Mk II", "Combat Pistol", "AP Pistol", "Stun Gun", "Pistol .50", "SNS Pistol", "SNS Pistol Mk II", "Heavy Pistol", "Vintage Pistol", "Flare Gun", "Marksman Pistol", "Heavy Revolver", "Heavy Revolver Mk II", "Double Action Revolver", "Up-n-Atomizer", "Ceramic Pistol", "Navy Revolver", "Perico Pistol", "Micro SMG", "SMG", "SMG Mk II", "Assault SMG", "Combat PDW", "Machine Pistol", "Mini SMG", "Unholy Hellbringer", "Pump Shotgun" , "Pump Shotgun Mk II", "Sawed-Off Shotgun", "Assault Shotgun", "Bullpup Shotgun", "Musket", "Heavy Shotgun", "Double Barrel Shotgun", "Sweeper Shotgun", "Combat Shotgun", "Assault Rifle", "Assault Rifle Mk II", "Carbine Rifle", "Carbine Rifle Mk II", "Advanced Rifle", "Special Carbine", "Special Carbine Mk II", "Bullpup Rifle", "Bullpup Rifle Mk II", "Compact Rifle", "Military Rifle", "MG", "Combat MG", "Combat MG Mk II", "Gusenberg Sweeper", "Sniper Rifle", "Heavy Sniper", "Heavy Sniper Mk II", "Marksman Rifle", "Marksman Rifle Mk II", "RPG", "Grenade Launcher", "Minigun", "Firework Launcher", "Railgun", "Homing Launcher", "Compact Grenade Launcher", "Widowmaker", "Grenade", "BZ Gas", "Molotov Cocktail", "Sticky Bomb", "Proximity Mines", "Snowballs", "Pipe Bombs", "Baseball", "Tear Gas", "Flare", "Jerry Can", "Fire Extinguisher", "Hazardous Jerry Can" };
			char* WeaponIDs[] = { "weapon_dagger", "weapon_bat", "weapon_bottle", "weapon_crowbar", "weapon_flashlight", "weapon_golfclub", "weapon_hammer", "weapon_hatchet", "weapon_knuckle", "weapon_knife", "weapon_machete", "weapon_switchblade", "weapon_nightstick", "weapon_wrench", "weapon_battleaxe", "weapon_poolcue", "weapon_stone_hatchet", "weapon_pistol", "weapon_pistol_mk2", "weapon_combatpistol", "weapon_appistol", "weapon_stungun", "weapon_pistol50", "weapon_snspistol", "weapon_snspistol_mk2", "weapon_heavypistol", "weapon_vintagepistol", "weapon_flaregun", "weapon_marksmanpistol", "weapon_revolver", "weapon_revolver_mk2", "weapon_doubleaction", "weapon_raypistol", "weapon_ceramicpistol", "weapon_navyrevolver", "weapon_gadgetpistol", "weapon_microsmg", "weapon_smg", "weapon_smg_mk2", "weapon_assaultsmg", "weapon_combatpdw", "weapon_machinepistol", "weapon_minismg", "weapon_raycarbine", "weapon_pumpshotgun", "weapon_pumpshotgun_mk2", "weapon_sawnoffshotgun", "weapon_assaultshotgun", "weapon_bullpupshotgun", "weapon_musket", "weapon_heavyshotgun", "weapon_dbshotgun", "weapon_autoshotgun", "weapon_combatshotgun", "weapon_assaultrifle", "weapon_assaultrifle_mk2", "weapon_carbinerifle", "weapon_carbinerifle_mk2", "weapon_advancedrifle", "weapon_specialcarbine", "weapon_specialcarbine_mk2", "weapon_bullpuprifle", "weapon_bullpuprifle_mk2", "weapon_compactrifle", "weapon_militaryrifle", "weapon_mg", "weapon_combatmg", "weapon_combatmg_mk2", "weapon_gusenberg", "weapon_sniperrifle", "weapon_heavysniper", "weapon_heavysniper_mk2", "weapon_marksmanrifle", "weapon_marksmanrifle_mk2", "weapon_rpg", "weapon_grenadelauncher", "weapon_minigun", "weapon_firework", "weapon_railgun", "weapon_hominglauncher", "weapon_compactlauncher", "weapon_rayminigun", "weapon_grenade", "weapon_bzgas", "weapon_molotov", "weapon_stickybomb", "weapon_proxmine", "weapon_snowball", "weapon_pipebomb", "weapon_ball", "weapon_smokegrenade", "weapon_flare", "weapon_petrolcan", "weapon_fireextinguisher", "weapon_hazardcan" };

			for (int i = 0; i < (sizeof(WeaponIDs) / sizeof(WeaponIDs[0])); i++)
			{
				if (weaponHash == GAMEPLAY::GET_HASH_KEY(WeaponIDs[i])) 
				{
					weaponName = WeaponNames[i];
					break;
				}
			}
            Weapon << weaponName;
        }
        else Weapon << "Unarmed";
        Vector3 myCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
        Vector3 coords = ENTITY::GET_ENTITY_COORDS(playerPed, 1);
        std::ostringstream Zone; Zone << "Zone: " << UI::_GET_LABEL_TEXT(ZONE::GET_NAME_OF_ZONE(coords.x, coords.y, coords.z));
        Hash streetName, crossingRoad;
        PATHFIND::GET_STREET_NAME_AT_COORD(coords.x, coords.y, coords.z, &streetName, &crossingRoad);
        std::ostringstream Street; Street << "Street: " << UI::GET_STREET_NAME_FROM_HASH_KEY(streetName);
        float distance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(myCoords.x, myCoords.y, myCoords.z, coords.x, coords.y, coords.z, false);
        std::ostringstream Distance; Distance << "Distance: ";
        if (distance > 1000) {
			distance = round((distance / 1000) * 100) / 100;
			Distance << distance << " Kilometers";
        }
        else {
            distance = round(distance * 1000) / 1000;
            Distance << distance << " Meters";
        }
        std::ostringstream X; X << "X:~g~ " << round(coords.x * 100) / 100;
        std::ostringstream Y; Y << "Y:~g~ " << round(coords.y * 100) / 100;
        std::ostringstream Z; Z << "Z:~g~ " << round(coords.z * 100) / 100;
        Menu::AddSmallTitle(playerName);
        Menu::AddSmallInfo((char*)RockstarID.str().c_str(), 0);
		Menu::AddSmallInfo(ipBuff, 1);
        Menu::AddSmallInfo((char*)Title.str().c_str(), 2);
		Menu::AddSmallInfo((char*)BankMoney.str().c_str(), 3);
		Menu::AddSmallInfo((char*)Money.str().c_str(), 4);
		Menu::AddSmallInfo((char*)Rank.str().c_str(), 5);
        Menu::AddSmallInfo((char*)Health.str().c_str(), 6);
        Menu::AddSmallInfo((char*)Armor.str().c_str(), 7);
        Menu::AddSmallInfo((char*)Alive.str().c_str(), 8);
        Menu::AddSmallInfo((char*)IsInAVehicle.str().c_str(), 9);
        Menu::AddSmallInfo((char*)VehicleModel.str().c_str(), 10);
        Menu::AddSmallInfo((char*)Speed.str().c_str(), 11);
        Menu::AddSmallInfo((char*)WantedLevel.str().c_str(), 12);
        Menu::AddSmallInfo((char*)Weapon.str().c_str(), 13);
        Menu::AddSmallInfo((char*)Zone.str().c_str(), 14);
        Menu::AddSmallInfo((char*)Street.str().c_str(), 15);
        Menu::AddSmallInfo((char*)Distance.str().c_str(), 16);
        Menu::AddSmallInfo((char*)X.str().c_str(), 17);
        Menu::AddSmallInfo((char*)Y.str().c_str(), 18);
        Menu::AddSmallInfo((char*)Z.str().c_str(), 19);
		Menu::AddSmallEnd(20);
    }
}

Vehicle SpawnVehicle(Hash vehicleHash, float x, float y, float z)
{
	if (STREAMING::IS_MODEL_IN_CDIMAGE(vehicleHash) && STREAMING::IS_MODEL_A_VEHICLE(vehicleHash))
	{
		STREAMING::REQUEST_MODEL(vehicleHash);
		while (!STREAMING::HAS_MODEL_LOADED(vehicleHash)) WAIT(1);
		Vehicle vehicle = VEHICLE::CREATE_VEHICLE(vehicleHash, x, y, z, 1.0f, 1, 1);
		VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(vehicle);
		ENTITY::SET_ENTITY_CAN_BE_DAMAGED(vehicle, true);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(vehicleHash);
		return vehicle;
	}
	return 0;
}


bool spawnmaxed = false;

Vehicle SpawnVehicleOnPlayer(std::string hash)
{
	Hash hts = GAMEPLAY::GET_HASH_KEY((char*)hash.c_str());
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vector3 coords = ENTITY::GET_ENTITY_COORDS(playerPed, 1);
	Vehicle vehicle = SpawnVehicle(hts, coords.x, coords.y, coords.z);
	if (ENTITY::DOES_ENTITY_EXIST(vehicle) && ENTITY::IS_ENTITY_A_VEHICLE(vehicle))
	{
		OnlineKickBypassToVehicle(vehicle);
		if (spawnmaxed) SetVehicleMaxed(vehicle);
		VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(vehicle, "Anim");
		PED::SET_PED_INTO_VEHICLE(playerPed, vehicle, -1);
		return vehicle;
	}
	return 0;
}

void OnlineKickBypassToVehicle(Vehicle vehicle)
{
	DECORATOR::DECOR_SET_INT(vehicle, "MPBitset", 0);
	auto networkId = NETWORK::VEH_TO_NET(vehicle);
	ENTITY::_SET_ENTITY_SOMETHING(vehicle, true);
	if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(vehicle))
		NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(networkId, true);
}


void SetVehicleMaxed(Vehicle vehicle)
{
	VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
	VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, 18, true);
	VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, 22, true);
	for (int i = 0; i < 55; i++)
	{
		if (i == 48)
			continue;
		else
		{
			if (VEHICLE::IS_THIS_MODEL_A_BIKE(ENTITY::GET_ENTITY_MODEL(vehicle)))
			{
				if (i == 23 || i == 24)
					continue;
			}
			VEHICLE::SET_VEHICLE_MOD(vehicle, i, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, i) - 1, false);
		}
	}
}

void SpawnModVehicleOnPlayer(std::string hash)
{
	if (strcmp((char*)hash.c_str(), "6x6 War RC") == 0)
	{
		int Handle = PLAYER::PLAYER_PED_ID();
		Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		Vector3 null; null.x = 0, null.y = 0; null.z = 0;
		Vector3 a; a.x = 0, a.y = 0, a.z = 0.03f;
		Vector3 b; b.x = 0, b.y = 0, b.z = 0;
		int hash = GAMEPLAY::GET_HASH_KEY("dune2");
		int hash2 = GAMEPLAY::GET_HASH_KEY("t20");
		int hash3 = GAMEPLAY::GET_HASH_KEY("dune2");
		int hash4 = GAMEPLAY::GET_HASH_KEY("dune2");
		int hash5 = GAMEPLAY::GET_HASH_KEY("monster");
		int hash6 = GAMEPLAY::GET_HASH_KEY("monster");

		STREAMING::REQUEST_MODEL(hash);
		STREAMING::REQUEST_MODEL(hash2);
		STREAMING::REQUEST_MODEL(hash3);
		STREAMING::REQUEST_MODEL(hash4);
		STREAMING::REQUEST_MODEL(hash5);
		STREAMING::REQUEST_MODEL(hash6);

		while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash2)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash3)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash4)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash5)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash6)) WAIT(1);


		if (STREAMING::HAS_MODEL_LOADED(hash) && STREAMING::HAS_MODEL_LOADED(hash2) && STREAMING::HAS_MODEL_LOADED(hash3) && STREAMING::HAS_MODEL_LOADED(hash4) && STREAMING::HAS_MODEL_LOADED(hash5) && STREAMING::HAS_MODEL_LOADED(hash6))
		{
			int Object1 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object2 = VEHICLE::CREATE_VEHICLE(hash3, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object3 = VEHICLE::CREATE_VEHICLE(hash4, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object4 = VEHICLE::CREATE_VEHICLE(hash5, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object5 = VEHICLE::CREATE_VEHICLE(hash6, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);

			if (ENTITY::DOES_ENTITY_EXIST(Object1) && ENTITY::DOES_ENTITY_EXIST(Object2) && ENTITY::DOES_ENTITY_EXIST(Object3) && ENTITY::DOES_ENTITY_EXIST(Object4) && ENTITY::DOES_ENTITY_EXIST(Object5))
			{
				int Vehicle = VEHICLE::CREATE_VEHICLE(hash2, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
				if (ENTITY::DOES_ENTITY_EXIST(Vehicle))
				{
					PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);

					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object1, Vehicle, 0, a.x, a.y, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object2, Vehicle, 0, a.x - 0.1f, a.y, a.z, b.x, b.y + 41.1f, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object3, Vehicle, 0, a.x + 0.1f, a.y, a.z, b.x, b.y - 41.4f, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object4, Vehicle, 0, a.x + 1.41f, a.y, a.z - 0.2f, b.x - 179.99f, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object5, Vehicle, 0, a.x - 1.41f, a.y, a.z - 0.2f, b.x - 179.99f, b.y, b.z, 0, 1, 0, 0, 2, 1);

					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Vehicle, 255, 255, 255);
					VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Vehicle, 255, 255, 255);
					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Object1, 255, 255, 255);
					VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Object1, 255, 255, 255);
					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Object4, 255, 255, 255);
					VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Object4, 255, 255, 255);
					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Object5, 255, 255, 255);
					VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Object5, 255, 255, 255);
				}

			}

		}
	}
	if (strcmp((char*)hash.c_str(), "Flightsquad") == 0)
	{
		int Handle = PLAYER::PLAYER_PED_ID();
		Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		Vector3 null; null.x = 0, null.y = 0; null.z = 0;
		Vector3 a; a.x = 0, a.y = 0, a.z = 0.03f;
		Vector3 b; b.x = 0, b.y = 0, b.z = 0;
		int hash = GAMEPLAY::GET_HASH_KEY("hydra");
		int hash1 = GAMEPLAY::GET_HASH_KEY("hydra");
		int hash2 = GAMEPLAY::GET_HASH_KEY("hydra");
		int hash3 = GAMEPLAY::GET_HASH_KEY("hydra");
		int hash4 = GAMEPLAY::GET_HASH_KEY("hydra");
		int hash5 = GAMEPLAY::GET_HASH_KEY("hydra");
		int hash6 = GAMEPLAY::GET_HASH_KEY("hydra");
		int hash7 = GAMEPLAY::GET_HASH_KEY("hydra");
		int hash8 = GAMEPLAY::GET_HASH_KEY("hydra");

		STREAMING::REQUEST_MODEL(hash);
		STREAMING::REQUEST_MODEL(hash2);
		STREAMING::REQUEST_MODEL(hash1);
		STREAMING::REQUEST_MODEL(hash3);
		STREAMING::REQUEST_MODEL(hash4);
		STREAMING::REQUEST_MODEL(hash5);
		STREAMING::REQUEST_MODEL(hash6);
		STREAMING::REQUEST_MODEL(hash7);
		STREAMING::REQUEST_MODEL(hash8);

		while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash2)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash3)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash4)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash5)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash6)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash7)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash8)) WAIT(1);


		if (STREAMING::HAS_MODEL_LOADED(hash) && STREAMING::HAS_MODEL_LOADED(hash2) && STREAMING::HAS_MODEL_LOADED(hash1) && STREAMING::HAS_MODEL_LOADED(hash3) && STREAMING::HAS_MODEL_LOADED(hash4) && STREAMING::HAS_MODEL_LOADED(hash5) && STREAMING::HAS_MODEL_LOADED(hash6) && STREAMING::HAS_MODEL_LOADED(hash7) && STREAMING::HAS_MODEL_LOADED(hash8))
		{
			int Object = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object1 = VEHICLE::CREATE_VEHICLE(hash1, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object2 = VEHICLE::CREATE_VEHICLE(hash3, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object3 = VEHICLE::CREATE_VEHICLE(hash4, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object4 = VEHICLE::CREATE_VEHICLE(hash5, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object5 = VEHICLE::CREATE_VEHICLE(hash6, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object6 = VEHICLE::CREATE_VEHICLE(hash7, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object7 = VEHICLE::CREATE_VEHICLE(hash8, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			if (ENTITY::DOES_ENTITY_EXIST(Object))
			{
				int Vehicle = VEHICLE::CREATE_VEHICLE(hash2, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
				if (ENTITY::DOES_ENTITY_EXIST(Vehicle))
				{

					PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);


					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object, Vehicle, 0, a.x, a.y + 15.541162f, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object1, Vehicle, 0, a.x, a.y + 30.541162f, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object2, Vehicle, 0, a.x - 15.0f, a.y + 15.541162f, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object3, Vehicle, 0, a.x + 15.0f, a.y + 15.541162f, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object4, Vehicle, 0, a.x + 15.0f, a.y, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object5, Vehicle, 0, a.x + 30.0f, a.y, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object6, Vehicle, 0, a.x - 15.0f, a.y, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object7, Vehicle, 0, a.x - 30.0f, a.y, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);

				}
			}
		}
	}
	if (strcmp((char*)hash.c_str(), "Bike Robot") == 0)
	{
		int Handle = PLAYER::PLAYER_PED_ID();
		Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		Vector3 null; null.x = 0, null.y = 0; null.z = 0;
		Vector3 a; a.x = 0, a.y = 0, a.z = 0.03f;
		Vector3 b; b.x = 0, b.y = 0, b.z = 0;
		int hash = GAMEPLAY::GET_HASH_KEY("bati");

		STREAMING::REQUEST_MODEL(hash);

		while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);

		if (STREAMING::HAS_MODEL_LOADED(hash))
		{
			int Object = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object1 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object2 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object3 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object4 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object5 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object6 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object7 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object8 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object9 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object10 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object11 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			int Object12 = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
			if (ENTITY::DOES_ENTITY_EXIST(Object))
			{
				int Vehicle = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
				if (ENTITY::DOES_ENTITY_EXIST(Vehicle))
				{
					PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object, Vehicle, 0, a.x - 1.25f, a.y, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object1, Object, 0, a.x, a.y, a.z + 1.16f, b.x + 62.0f, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object2, Object, 0, a.x + 2.5f, a.y, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object3, Object, 0, a.x + 2.5f, a.y, a.z + 1.16f, b.x + 62.0f, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object4, Object, 0, a.x + 0.5f, a.y + 0.5f, a.z + 2.27f, b.x + 50.0f, b.y - 70.0f, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object5, Object, 0, a.x + 1.95f, a.y + 0.5f, a.z + 2.27f, b.x + 50.0f, b.y + 70.0f, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object6, Object, 0, a.x + 1.24f, a.y + 0.8f, a.z + 5.0f, b.x + 90.0f, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object7, Object, 0, a.x + 1.24f, a.y + 0.8f, a.z + 3.5f, b.x + 90.0f, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object8, Object, 0, a.x + 1.24f, a.y + 0.8f, a.z + 6.0f, b.x - 15.0f, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object9, Object, 0, a.x + 2.0f, a.y + 0.8f, a.z + 4.43f, b.x - 30.0f, b.y - 50.0f, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object10, Object, 0, a.x + 0.6f, a.y + 0.8f, a.z + 4.43f, b.x - 30.0f, b.y + 50.0f, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object11, Object, 0, a.x, a.y + 2.1f, a.z + 3.7f, b.x - 10.0f, b.y + 12.8f, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object12, Object, 0, a.x + 2.8f, a.y + 1.9f, a.z + 3.7f, b.x - 10.0f, b.y - 13.6f, b.z, 0, 1, 0, 0, 2, 1);
				}
			}
		}
	}
	if (strcmp((char*)hash.c_str(), "Flying UFO") == 0)
	{
		int Handle = PLAYER::PLAYER_PED_ID();
		Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		Vector3 null; null.x = 0, null.y = 0; null.z = 0;
		Vector3 a; a.x = -0.27, a.y = -0.24, a.z = 0.13f;
		Vector3 b; b.x = 0, b.y = 0, b.z = 180;
		int hash = GAMEPLAY::GET_HASH_KEY("skylift");
		int hash2 = GAMEPLAY::GET_HASH_KEY("p_spinning_anus_s");

		STREAMING::REQUEST_MODEL(hash);
		STREAMING::REQUEST_MODEL(hash2);

		while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash2)) WAIT(1);

		if (STREAMING::HAS_MODEL_LOADED(hash) && STREAMING::HAS_MODEL_LOADED(hash2))
		{
			int Object = OBJECT::CREATE_OBJECT(hash2, null.x, null.y, null.z, 1, 0, 1);
			if (ENTITY::DOES_ENTITY_EXIST(Object))
			{
				int Vehicle = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);
				if (ENTITY::DOES_ENTITY_EXIST(Vehicle))
				{
					ENTITY::SET_ENTITY_VISIBLE(Vehicle, false, true);
					ENTITY::SET_ENTITY_ALPHA(Vehicle, 0, false);
					PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(Object, Vehicle, 0, a.x, a.y, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Vehicle, 2, 153, 57);
					VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Vehicle, 2, 153, 57);
					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Object, 2, 153, 57);
					VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Object, 2, 153, 57);

				}
			}
		}
	}
	if (strcmp((char*)hash.c_str(), "Modded T20") == 0)
	{
		uint Handle = PLAYER::PLAYER_PED_ID();
		Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		//hash2_EMP
		Vector3 null; null.x = 0.0, null.y = 0.0; null.z = 0.0;
		Vector3 a; a.x = 0.0, a.y = 1.61, a.z = -0.12;//X Coord - Y Coord - Z Coord
		Vector3 b; b.x = -12.0, b.y = 0.0, b.z = 0.0;//Pitch - Roll - Yaw
													 //hash3_MiniGun
		Vector3 null1; null1.x = 0.0, null1.y = 0.0; null1.z = 0.0;
		Vector3 a1; a1.x = -0.60, a1.y = 1.60, a1.z = 0.20;
		Vector3 b1; b1.x = 0.0, b1.y = 0.0, b1.z = 88.0;
		//hash3_MiniGun2
		Vector3 null0; null0.x = 0.0, null0.y = 0.0; null0.z = 0.0;
		Vector3 a0; a0.x = 0.60, a0.y = 1.60, a0.z = 0.20;
		Vector3 b0; b0.x = 0.0, b0.y = 0.0, b0.z = 88.0;
		//hash4_FLWheel1
		Vector3 null2; null2.x = 0.0, null2.y = 0.0; null2.z = 0.0;
		Vector3 a2; a2.x = -0.94, a2.y = 1.32, a2.z = -0.25;
		Vector3 b2; b2.x = 9.0, b2.y = -28.0, b2.z = 87.0;
		//hash4_RLWheel1
		Vector3 null3; null3.x = 0.0, null3.y = 0.0; null3.z = 0.0;
		Vector3 a3; a3.x = -0.95, a3.y = -1.34, a3.z = -0.25f;
		Vector3 b3; b3.x = 9.0, b3.y = 38.0, b3.z = 87.0;
		//hash4_RFWheel1
		Vector3 null4; null4.x = 0.0, null4.y = 0.0; null4.z = 0.0;
		Vector3 a4; a4.x = 0.95, a4.y = 1.33, a4.z = -0.23;
		Vector3 b4; b4.x = 11.0, b4.y = 26.0, b4.z = -89.0;
		//hash4_RRWheel1
		Vector3 null5; null5.x = 0.0, null5.y = 0.0; null5.z = 0.0;
		Vector3 a5; a5.x = 0.96, a5.y = -1.31, a5.z = -0.23;
		Vector3 b5; b5.x = 10.0, b5.y = 20.0, b5.z = -92.0;

		int hash_T20 = GAMEPLAY::GET_HASH_KEY("t20");
		int hash2_EMP = GAMEPLAY::GET_HASH_KEY("hei_prop_heist_emp");
		int hash3_MiniGun = GAMEPLAY::GET_HASH_KEY("prop_minigun_01");
		int hash4_Wheel = GAMEPLAY::GET_HASH_KEY("prop_wheel_01");

		STREAMING::REQUEST_MODEL(hash_T20);
		STREAMING::REQUEST_MODEL(hash2_EMP);
		STREAMING::REQUEST_MODEL(hash3_MiniGun);
		STREAMING::REQUEST_MODEL(hash4_Wheel);

		while (!STREAMING::HAS_MODEL_LOADED(hash_T20)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash2_EMP)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash3_MiniGun)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash4_Wheel)) WAIT(1);

		if (STREAMING::HAS_MODEL_LOADED(hash_T20) && STREAMING::HAS_MODEL_LOADED(hash2_EMP) && STREAMING::HAS_MODEL_LOADED(hash3_MiniGun) && STREAMING::HAS_MODEL_LOADED(hash4_Wheel))
		{
			int EMP = OBJECT::CREATE_OBJECT(hash2_EMP, null.x, null.y, null.z, 1, 0, 1);
			int MiniGun = OBJECT::CREATE_OBJECT(hash3_MiniGun, null1.x, null1.y, null1.z, 1, 0, 1);
			int MiniGun2 = OBJECT::CREATE_OBJECT(hash3_MiniGun, null0.x, null0.y, null0.z, 1, 0, 1);
			int FLWheel1 = OBJECT::CREATE_OBJECT(hash4_Wheel, null2.x, null2.y, null2.z, 1, 0, 1);
			int RLWheel1 = OBJECT::CREATE_OBJECT(hash4_Wheel, null3.x, null3.y, null3.z, 1, 0, 1);
			int RFWheel1 = OBJECT::CREATE_OBJECT(hash4_Wheel, null4.x, null4.y, null4.z, 1, 0, 1);
			int RRWheel1 = OBJECT::CREATE_OBJECT(hash4_Wheel, null5.x, null5.y, null5.z, 1, 0, 1);

			if (ENTITY::DOES_ENTITY_EXIST(EMP) && ENTITY::DOES_ENTITY_EXIST(MiniGun) && ENTITY::DOES_ENTITY_EXIST(MiniGun2) && ENTITY::DOES_ENTITY_EXIST(FLWheel1) && ENTITY::DOES_ENTITY_EXIST(RLWheel1) && ENTITY::DOES_ENTITY_EXIST(RFWheel1) && ENTITY::DOES_ENTITY_EXIST(RRWheel1))
			{
				int Vehicle = VEHICLE::CREATE_VEHICLE(hash_T20, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);

				if (ENTITY::DOES_ENTITY_EXIST(Vehicle))
				{
					PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);
					STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash_T20);
					ENTITY::SET_ENTITY_VISIBLE(Vehicle, true, true);
					ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), true, true);

					ENTITY::ATTACH_ENTITY_TO_ENTITY(EMP, Vehicle, 0, a.x, a.y, a.z, b.x, b.y, b.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(MiniGun, Vehicle, 0, a1.x, a1.y, a1.z, b1.x, b1.y, b1.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(MiniGun2, Vehicle, 0, a0.x, a0.y, a0.z, b0.x, b0.y, b0.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(FLWheel1, Vehicle, 0, a2.x, a2.y, a2.z, b2.x, b2.y, b2.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(RLWheel1, Vehicle, 0, a3.x, a3.y, a3.z, b3.x, b3.y, b3.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(RFWheel1, Vehicle, 0, a4.x, a4.y, a4.z, b4.x, b4.y, b4.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(RRWheel1, Vehicle, 0, a5.x, a5.y, a5.z, b5.x, b5.y, b5.z, 0, 1, 0, 0, 2, 1);

				}

			}

		}

	}
	if (strcmp((char*)hash.c_str(), "Monster Swat") == 0)
	{
		uint Me = PLAYER::PLAYER_PED_ID();
		Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		Vector3 null; null.x = 0, null.y = 0; null.z = 0;
		Vector3 a; a.x = 0, a.y = 0, a.z = 0;
		Vector3 b; b.x = 0, b.y = 0, b.z = 0;
		int hash = GAMEPLAY::GET_HASH_KEY("monster");
		int hash2 = GAMEPLAY::GET_HASH_KEY("riot");
		STREAMING::REQUEST_MODEL(hash);
		STREAMING::REQUEST_MODEL(hash2);

		while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash2)) WAIT(1);

		if (STREAMING::HAS_MODEL_LOADED(hash) && STREAMING::HAS_MODEL_LOADED(hash2))
		{
			int Vehicle = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Me), 1, 0);
			int Object = VEHICLE::CREATE_VEHICLE(hash2, null.x, null.y, null.z, ENTITY::GET_ENTITY_HEADING(Me), 1, 0);
			if (ENTITY::DOES_ENTITY_EXIST(Vehicle) && ENTITY::DOES_ENTITY_EXIST(Object))
			{
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
				PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);
				ENTITY::SET_ENTITY_VISIBLE(Vehicle, true, true);
				VEHICLE::SET_VEHICLE_MOD_KIT(Object, 0);
				VEHICLE::SET_VEHICLE_WINDOW_TINT(Object, 5);
				ENTITY::ATTACH_ENTITY_TO_ENTITY(Object, Vehicle, 0, a.x, a.y, a.z, b.x, b.y, b.z, 0, false, 0, 0, 2, true);

			}
		}
	}
	if (strcmp((char*)hash.c_str(), "Insurgent 2.0") == 0)
	{
		uint Me = PLAYER::PLAYER_PED_ID();
		Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		Vector3 null; null.x = 0, null.y = 0; null.z = 0;
		Vector3 a; a.x = 0, a.y = 0, a.z = 0;
		Vector3 b; b.x = 0, b.y = 0, b.z = 0;
		int hash = GAMEPLAY::GET_HASH_KEY("insurgent");
		int hash2 = GAMEPLAY::GET_HASH_KEY("insurgent2");

		STREAMING::REQUEST_MODEL(hash);
		STREAMING::REQUEST_MODEL(hash2);

		while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash2)) WAIT(1);

		if (STREAMING::HAS_MODEL_LOADED(hash) && STREAMING::HAS_MODEL_LOADED(hash2))
		{
			int Vehicle = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Me), 1, 0);
			int Object = VEHICLE::CREATE_VEHICLE(hash2, null.x, null.y, null.z, ENTITY::GET_ENTITY_HEADING(Me), 1, 0);
			if (ENTITY::DOES_ENTITY_EXIST(Vehicle) && ENTITY::DOES_ENTITY_EXIST(Object))
			{
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
				PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);
				ENTITY::SET_ENTITY_VISIBLE(Vehicle, true, true);
				VEHICLE::SET_VEHICLE_MOD_KIT(Object, 0);
				VEHICLE::SET_VEHICLE_WINDOW_TINT(Object, 5);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Object, 0, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Object, 0, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Vehicle, 0, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Vehicle, 0, 0, 0);
				ENTITY::ATTACH_ENTITY_TO_ENTITY(Object, Vehicle, 0, a.x, a.y, a.z, b.x, b.y, b.z, 0, false, 0, 0, 2, true);

			}

		}

	}
	if (strcmp((char*)hash.c_str(), "Special") == 0)
	{
		uint Me = PLAYER::PLAYER_PED_ID();
		Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		Vector3 null; null.x = 0, null.y = 0; null.z = 0;
		Vector3 a; a.x = 0, a.y = 0.17, a.z = -0.11;
		Vector3 b; b.x = 0, b.y = 0, b.z = 0;
		int hash = GAMEPLAY::GET_HASH_KEY("zentorno");
		int hash2 = GAMEPLAY::GET_HASH_KEY("kuruma2");

		STREAMING::REQUEST_MODEL(hash);
		STREAMING::REQUEST_MODEL(hash2);

		while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash2)) WAIT(1);

		if (STREAMING::HAS_MODEL_LOADED(hash) && STREAMING::HAS_MODEL_LOADED(hash2))
		{
			int Vehicle = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Me), 1, 0);
			int Object = VEHICLE::CREATE_VEHICLE(hash2, null.x, null.y, null.z, ENTITY::GET_ENTITY_HEADING(Me), 1, 0);
			if (ENTITY::DOES_ENTITY_EXIST(Vehicle) && ENTITY::DOES_ENTITY_EXIST(Object))
			{
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
				PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);
				ENTITY::SET_ENTITY_VISIBLE(Vehicle, true, true);
				VEHICLE::_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(Vehicle, 25);
				VEHICLE::SET_VEHICLE_MOD_KIT(Object, 0);
				VEHICLE::SET_VEHICLE_WINDOW_TINT(Object, 5);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Object, 0, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Object, 0, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Vehicle, 0, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Vehicle, 0, 0, 0);
				ENTITY::ATTACH_ENTITY_TO_ENTITY(Object, Vehicle, 0, a.x, a.y, a.z, b.x, b.y, b.z, 0, false, 0, 0, 2, true);
			}
		}
	}
	if (strcmp((char*)hash.c_str(), "Sub Copter") == 0)
	{
		uint Me = PLAYER::PLAYER_PED_ID();
		Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		Vector3 null; null.x = 0, null.y = 0; null.z = 0;
		Vector3 a; a.x = 0, a.y = 1, a.z = 0.8;
		Vector3 b; b.x = 0, b.y = 0, b.z = 0;
		int hash = GAMEPLAY::GET_HASH_KEY("buzzard");
		int hash2 = GAMEPLAY::GET_HASH_KEY("submersible");

		STREAMING::REQUEST_MODEL(hash);
		STREAMING::REQUEST_MODEL(hash2);

		while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash2)) WAIT(1);

		if (STREAMING::HAS_MODEL_LOADED(hash) && STREAMING::HAS_MODEL_LOADED(hash2))
		{
			int Vehicle = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Me), 1, 0);
			int Object = VEHICLE::CREATE_VEHICLE(hash2, null.x, null.y, null.z, ENTITY::GET_ENTITY_HEADING(Me), 1, 0);
			if (ENTITY::DOES_ENTITY_EXIST(Vehicle) && ENTITY::DOES_ENTITY_EXIST(Object))
			{
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
				PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);
				ENTITY::SET_ENTITY_VISIBLE(Vehicle, true, true);
				VEHICLE::SET_VEHICLE_MOD_KIT(Object, 0);
				VEHICLE::SET_VEHICLE_WINDOW_TINT(Object, 5);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Object, 0, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Object, 0, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Vehicle, 255, 149, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Vehicle, 0, 0, 0);
				ENTITY::ATTACH_ENTITY_TO_ENTITY(Object, Vehicle, 0, a.x, a.y, a.z, b.x, b.y, b.z, 0, false, 0, 0, 2, true);

			}

		}

	}
	if (strcmp((char*)hash.c_str(), "HotKnife 2.0") == 0)
	{
		uint Handle = PLAYER::PLAYER_PED_ID();
		Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		//hash2_PANTO
		Vector3 null; null.x = 0.0, null.y = 0.0; null.z = 0.0;
		Vector3 a; a.x = 0.0, a.y = 0.0, a.z = -0.2;//X Coord - Y Coord - Z Coord
		Vector3 b; b.x = 0.0, b.y = 0.0, b.z = 0.0;//Pitch - Roll - Yaw
		//hash3_MiniGun
		Vector3 null1; null1.x = 0.0, null1.y = 0.0; null1.z = 0.0;
		Vector3 a1; a1.x = -0.60, a1.y = 1.2, a1.z = 0.20;
		Vector3 b1; b1.x = 0.0, b1.y = 0.0, b1.z = 88.0;
		//hash3_MiniGun2
		Vector3 null0; null0.x = 0.0, null0.y = 0.0; null0.z = 0.0;
		Vector3 a0; a0.x = 0.60, a0.y = 1.2, a0.z = 0.20;
		Vector3 b0; b0.x = 0.0, b0.y = 0.0, b0.z = 88.0;
		//hash4_WindScreen _ VANISHED NEEDS FIXING
		Vector3 null2; null2.x = 0.0, null2.y = 0.0; null2.z = 0.0;
		Vector3 a2; a2.x = 0.24, a2.y = 0.41, a2.z = 0.50;
		Vector3 b2; b2.x = -45.3, b2.y = 0.0, b2.z = -179.0;//87
		//hash4_RLWheel1
		Vector3 null3; null3.x = 0.0, null3.y = 0.0; null3.z = 0.0;
		Vector3 a3; a3.x = -0.85, a3.y = -1.2, a3.z = 0.0; // -0.23
		Vector3 b3; b3.x = 0.0, b3.y = 0.0, b3.z = 86.0;
		//hash4_RFWindScreen 
		Vector3 null4; null4.x = 0.0, null4.y = 0.0; null4.z = 0.0;
		Vector3 a4; a4.x = -0.24, a4.y = 0.41, a4.z = 0.5;
		Vector3 b4; b4.x = -45.3, b4.y = 0.0, b4.z = -179.0;
		//hash4_RRWheel1
		Vector3 null5; null5.x = 0.0, null5.y = 0.0; null5.z = 0.0;
		Vector3 a5; a5.x = 0.85, a5.y = -1.2, a5.z = 0.01;
		Vector3 b5; b5.x = 0.0, b5.y = 0.0, b5.z = -92.0;
		//PUNTO -- FUCK UP NEEDS FIXING 
		Vector3 null6; null6.x = 0, null6.y = 0; null6.z = 0;
		Vector3 a6; a6.x = 0.0, a6.y = 0.0, a6.z = -0.2;
		Vector3 b6; b6.x = 0.0, b6.y = 0.0, b6.z = 0.0;

		int hash = GAMEPLAY::GET_HASH_KEY("hotknife");
		int hash3_MiniGun = GAMEPLAY::GET_HASH_KEY("prop_minigun_01");
		int hash4_Wheel = GAMEPLAY::GET_HASH_KEY("prop_wheel_01");
		int hashWindow = GAMEPLAY::GET_HASH_KEY("prop_ballistic_shield");

		STREAMING::REQUEST_MODEL(hashWindow);// WINDOW
		STREAMING::REQUEST_MODEL(hash3_MiniGun); // GUNS
		STREAMING::REQUEST_MODEL(hash4_Wheel); // back wheels
		STREAMING::REQUEST_MODEL(hash);

		while (!STREAMING::HAS_MODEL_LOADED(hashWindow)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash3_MiniGun)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash4_Wheel)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);

		if (STREAMING::HAS_MODEL_LOADED(hash) && STREAMING::HAS_MODEL_LOADED(hashWindow) && STREAMING::HAS_MODEL_LOADED(hash3_MiniGun) && STREAMING::HAS_MODEL_LOADED(hash4_Wheel))
		{
			int MiniGun = OBJECT::CREATE_OBJECT(hash3_MiniGun, null1.x, null1.y, null1.z, 1, 0, 1);
			int MiniGun2 = OBJECT::CREATE_OBJECT(hash3_MiniGun, null0.x, null0.y, null0.z, 1, 0, 1);
			int WINDOW_1 = OBJECT::CREATE_OBJECT(hashWindow, null2.x, null2.y, null2.z, 1, 0, 1);
			int RLWheel1 = OBJECT::CREATE_OBJECT(hash4_Wheel, null3.x, null3.y, null3.z, 1, 0, 1);
			int WINDOW_2 = OBJECT::CREATE_OBJECT(hashWindow, null4.x, null4.y, null4.z, 1, 0, 1);
			int RRWheel1 = OBJECT::CREATE_OBJECT(hash4_Wheel, null5.x, null5.y, null5.z, 1, 0, 1);


			if (ENTITY::DOES_ENTITY_EXIST(MiniGun) && ENTITY::DOES_ENTITY_EXIST(MiniGun2) && ENTITY::DOES_ENTITY_EXIST(WINDOW_1) && ENTITY::DOES_ENTITY_EXIST(RLWheel1) && ENTITY::DOES_ENTITY_EXIST(WINDOW_2) && ENTITY::DOES_ENTITY_EXIST(RRWheel1))
			{
				int Vehicle = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0);

				if (ENTITY::DOES_ENTITY_EXIST(Vehicle))
				{
					PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);
					STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
					ENTITY::SET_ENTITY_VISIBLE(Vehicle, true, true);
					ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), true, true);
					STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
					PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);
					ENTITY::SET_ENTITY_VISIBLE(Vehicle, true, true);
					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Vehicle, 0, 0, 0);
					VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Vehicle, 0, 0, 0);

					ENTITY::ATTACH_ENTITY_TO_ENTITY(MiniGun, Vehicle, 0, a1.x, a1.y, a1.z, b1.x, b1.y, b1.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(MiniGun2, Vehicle, 0, a0.x, a0.y, a0.z, b0.x, b0.y, b0.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(WINDOW_1, Vehicle, 0, a2.x, a2.y, a2.z, b2.x, b2.y, b2.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(RLWheel1, Vehicle, 0, a3.x, a3.y, a3.z, b3.x, b3.y, b3.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(WINDOW_2, Vehicle, 0, a4.x, a4.y, a4.z, b4.x, b4.y, b4.z, 0, 1, 0, 0, 2, 1);
					ENTITY::ATTACH_ENTITY_TO_ENTITY(RRWheel1, Vehicle, 0, a5.x, a5.y, a5.z, b5.x, b5.y, b5.z, 0, 1, 0, 0, 2, 1);
				}
			}
		}
	}
	if (strcmp((char*)hash.c_str(), "Train") == 0)
	{
		uint Me = PLAYER::PLAYER_PED_ID();
		Vector3 MyCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		Vector3 null; null.x = 0, null.y = 0; null.z = 0;
		Vector3 a; a.x = 0, a.y = 0, a.z = 0;
		Vector3 b; b.x = 0, b.y = 0, b.z = 0;
		int hash = GAMEPLAY::GET_HASH_KEY("bus");
		int hash2 = GAMEPLAY::GET_HASH_KEY("freight");

		STREAMING::REQUEST_MODEL(hash);
		STREAMING::REQUEST_MODEL(hash2);

		while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
		while (!STREAMING::HAS_MODEL_LOADED(hash2)) WAIT(1);

		if (STREAMING::HAS_MODEL_LOADED(hash) && STREAMING::HAS_MODEL_LOADED(hash2))
		{
			int Vehicle = VEHICLE::CREATE_VEHICLE(hash, MyCoords.x, MyCoords.y, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Me), 1, 0);
			int Object = VEHICLE::CREATE_VEHICLE(hash2, null.x, null.y, null.z, ENTITY::GET_ENTITY_HEADING(Me), 1, 0);
			if (ENTITY::DOES_ENTITY_EXIST(Vehicle) && ENTITY::DOES_ENTITY_EXIST(Object))
			{
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
				PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), Vehicle, -1);
				ENTITY::SET_ENTITY_VISIBLE(Vehicle, false, true);
				ENTITY::SET_ENTITY_ALPHA(Vehicle, 0, false);
				ENTITY::SET_ENTITY_VISIBLE(Object, true, true);
				VEHICLE::SET_VEHICLE_MOD_KIT(Object, 0);
				VEHICLE::SET_VEHICLE_WINDOW_TINT(Object, 5);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Object, 0, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Object, 0, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(Vehicle, 0, 0, 0);
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(Vehicle, 0, 0, 0);
				ENTITY::ATTACH_ENTITY_TO_ENTITY(Object, Vehicle, 0, a.x, a.y, a.z, b.x, b.y, b.z, 0, false, 0, 0, 2, true);

			}

		}

	}
}

Ped SpawnPed(int pedType, Hash pedModel, Vector3 spawnCoords)
{
	STREAMING::REQUEST_MODEL(pedModel);
	while (!STREAMING::HAS_MODEL_LOADED(pedModel)) WAIT(1);
	return PED::CREATE_PED(26, pedModel, spawnCoords.x, spawnCoords.y, spawnCoords.z + 0.4f, 0, true, true);
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(pedModel);
}

void StartPedGuardTask(Player toProtect, Ped guardPed, Hash weaponHash, int aimAccuracy)
{
	Ped pedToProtect = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(toProtect);
	if (ENTITY::DOES_ENTITY_EXIST(pedToProtect))
	{
		int playerGroup = PLAYER::GET_PLAYER_GROUP(toProtect);
		PED::SET_PED_AS_GROUP_LEADER(pedToProtect, playerGroup);
		PED::SET_PED_AS_GROUP_MEMBER(guardPed, playerGroup);
		//ENTITY::SET_ENTITY_CAN_BE_DAMAGED_BY_RELATIONSHIP_GROUP(guardPed, false, playerGroup);
		PED::SET_PED_NEVER_LEAVES_GROUP(guardPed, true);
		PED::SET_PED_COMBAT_ABILITY(guardPed, 100);
		PED::SET_PED_ACCURACY(guardPed, aimAccuracy);
		if (weaponHash != NULL)
		{
			WEAPON::GIVE_WEAPON_TO_PED(guardPed, weaponHash, 9999, false, true);
			//PED::SET_PED_CAN_SWITCH_WEAPON(bodyGuard, true);
		}
		PED::SET_GROUP_FORMATION(playerGroup, 3);
	}
}

void StartPedEnemyTask(Player toAttack, Ped enemyPed, Hash weaponHash, int aimAccuracy)
{
	Ped pedToAttack = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(toAttack);
	if (ENTITY::DOES_ENTITY_EXIST(pedToAttack))
	{
		PED::SET_PED_COMBAT_ABILITY(enemyPed, 100);
		PED::SET_PED_ACCURACY(enemyPed, aimAccuracy);
		if (weaponHash != NULL)
		{
			WEAPON::GIVE_WEAPON_TO_PED(enemyPed, weaponHash, 9999, false, true);
			//PED::SET_PED_CAN_SWITCH_WEAPON(enemyPed, true);
		}
		AI::TASK_COMBAT_PED(enemyPed, pedToAttack, 0, 16);
		PED::SET_PED_KEEP_TASK(enemyPed, true);
		PED::SET_PED_ALERTNESS(enemyPed, 1000);
		PED::SET_PED_COMBAT_RANGE(enemyPed, 1000);
	}
}

void StartFollowTaskOnVehicle(Player toFollow, Vehicle vehicle, Ped driverPed, bool fillUpVehicle)
{
	Ped playerPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(toFollow);
	if (ENTITY::DOES_ENTITY_EXIST(playerPed) && ENTITY::DOES_ENTITY_EXIST(vehicle) && ENTITY::DOES_ENTITY_EXIST(driverPed))
	{
		if (ENTITY::IS_ENTITY_A_VEHICLE(vehicle))
		{
			Hash vehicleModel = ENTITY::GET_ENTITY_MODEL(vehicle);

			VEHICLE::SET_VEHICLE_DOORS_LOCKED(vehicle, true);
			VEHICLE::SET_VEHICLE_ENGINE_ON(vehicle, true, true, true);

			if (VEHICLE::IS_THIS_MODEL_A_PLANE(vehicle))
			{
				AI::TASK_PLANE_CHASE(driverPed, playerPed, 0, 0, 50);
			}
			else if (VEHICLE::IS_THIS_MODEL_A_HELI(vehicle))
			{
				VEHICLE::SET_HELI_BLADES_FULL_SPEED(vehicle);
				AI::TASK_HELI_CHASE(driverPed, playerPed, 0, 0, 50);
			}
			else
			{
				AI::TASK_VEHICLE_CHASE(driverPed, playerPed);
			}

			PED::SET_PED_KEEP_TASK(driverPed, true);

			PED::SET_PED_INTO_VEHICLE(driverPed, vehicle, -1);

			if (fillUpVehicle && VEHICLE::GET_VEHICLE_MODEL_NUMBER_OF_SEATS(vehicleModel) > 1)
			{
				for (int i = 0; i < (VEHICLE::GET_VEHICLE_MODEL_NUMBER_OF_SEATS(vehicleModel) - 1); i++)
				{
					PED::SET_PED_INTO_VEHICLE(PED::CLONE_PED(driverPed, 0.0f, true, true), vehicle, i);
				}
			}
		}
	}
}

void AttachObjectToPlayer(char* name)
{
	Hash hash = GAMEPLAY::GET_HASH_KEY(name);
	STREAMING::REQUEST_MODEL(hash);
	while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
	Vector3 pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
	Object object = OBJECT::CREATE_OBJECT(hash, pos.x, pos.y, pos.z, true, 1, 0);
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
	ENTITY::SET_ENTITY_VISIBLE(object, true, 0);
	ENTITY::ATTACH_ENTITY_TO_ENTITY(object, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 2, 1);
}

void DetachObjectsFromPlayer(Player player)
{
	Ped playerPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player);
	if (player != PLAYER::PLAYER_ID())
	{
		ENTITY::DETACH_ENTITY(PLAYER::PLAYER_PED_ID(), true, true);
	}
	Vector3 coords = ENTITY::GET_ENTITY_COORDS(playerPed, false);
	char* Objects[205] = { "prop_acc_guitar_01_d1", "prop_xmas_ext", "prop_cs_katana_01", "prop_minigun_01", "prop_beachball_02", "prop_tv_flat_03", "prop_tv_flat_02", "prop_tv_flat_03b", "prop_tv_flat_02b", "prop_tv_flat_michael", "prop_tv_flat_01_screen", "prop_tv_flat_01", "prop_ld_ferris_wheel", "prop_bskball_01", "PROP_MP_RAMP_03", "PROP_MP_RAMP_02", "PROP_MP_RAMP_01", "PROP_JETSKI_RAMP_01", "PROP_WATER_RAMP_03", "PROP_VEND_SNAK_01", "PROP_TRI_START_BANNER", "PROP_TRI_FINISH_BANNER", "PROP_TEMP_BLOCK_BLOCKER", "PROP_SLUICEGATEL", "PROP_SKIP_08A", "PROP_SAM_01", "PROP_RUB_CONT_01B", "PROP_ROADCONE01A", "PROP_MP_ARROW_BARRIER_01", "PROP_HOTEL_CLOCK_01", "PROP_LIFEBLURB_02", "PROP_COFFIN_02B", "PROP_MP_NUM_1", "PROP_MP_NUM_2", "PROP_MP_NUM_3", "PROP_MP_NUM_4", "PROP_MP_NUM_5", "PROP_MP_NUM_6", "PROP_MP_NUM_7", "PROP_MP_NUM_8", "PROP_MP_NUM_9", "prop_xmas_tree_int", "prop_bumper_car_01", "prop_beer_neon_01", "prop_space_rifle", "prop_dummy_01", "prop_rub_trolley01a", "prop_wheelchair_01_s", "PROP_CS_KATANA_01", "PROP_CS_DILDO_01", "prop_armchair_01", "prop_bin_04a", "prop_chair_01a", "prop_dog_cage_01", "prop_dummy_plane", "prop_golf_bag_01", "prop_arcade_01", "hei_prop_heist_emp", "prop_alien_egg_01", "prop_air_towbar_01", "hei_prop_heist_tug", "prop_air_luggtrolley", "PROP_CUP_SAUCER_01", "prop_wheelchair_01", "prop_ld_toilet_01", "prop_acc_guitar_01", "prop_bank_vaultdoor", "p_v_43_safe_s", "p_spinning_anus_s", "prop_can_canoe", "prop_air_woodsteps", "Prop_weed_01", "prop_a_trailer_door_01", "prop_apple_box_01", "prop_air_fueltrail1", "prop_barrel_02a", "prop_barrel_float_1", "prop_barrier_wat_03b", "prop_air_fueltrail2", "prop_air_propeller01", "prop_windmill_01", "prop_Ld_ferris_wheel", "p_tram_crash_s", "p_oil_slick_01", "p_ld_stinger_s", "p_ld_soc_ball_01", "prop_juicestand", "p_oil_pjack_01_s", "prop_barbell_01", "prop_barbell_100kg", "p_parachute1_s", "p_cablecar_s", "prop_beach_fire", "prop_lev_des_barge_02", "prop_lev_des_barge_01", "prop_a_base_bars_01", "prop_beach_bars_01", "prop_air_bigradar", "prop_weed_pallet", "prop_artifact_01", "prop_attache_case_01", "prop_large_gold", "prop_roller_car_01", "prop_water_corpse_01", "prop_water_corpse_02", "prop_dummy_01", "prop_atm_01", "hei_prop_carrier_docklight_01", "hei_prop_carrier_liferafts", "hei_prop_carrier_ord_03", "hei_prop_carrier_defense_02", "hei_prop_carrier_defense_01", "hei_prop_carrier_radar_1", "hei_prop_carrier_radar_2", "hei_prop_hei_bust_01", "hei_prop_wall_alarm_on", "hei_prop_wall_light_10a_cr", "prop_afsign_amun", "prop_afsign_vbike", "prop_aircon_l_01", "prop_aircon_l_02", "prop_aircon_l_03", "prop_aircon_l_04", "prop_airhockey_01", "prop_air_bagloader", "prop_air_blastfence_01", "prop_air_blastfence_02", "prop_air_cargo_01a", "prop_air_chock_01", "prop_air_chock_03", "prop_air_gasbogey_01", "prop_air_generator_03", "prop_air_stair_02", "prop_amb_40oz_02", "prop_amb_40oz_03", "prop_amb_beer_bottle", "prop_amb_donut", "prop_amb_handbag_01", "prop_amp_01", "prop_anim_cash_pile_02", "prop_asteroid_01", "prop_arm_wrestle_01", "prop_ballistic_shield", "prop_bank_shutter", "prop_barier_conc_02b", "prop_barier_conc_05a", "prop_barrel_01a", "prop_bar_stool_01", "prop_basejump_target_01", "prop_flag_canada", "prop_flag_canada_s", "prop_flag_eu", "prop_flag_eu_s", "prop_flag_france", "prop_flag_france_s", "prop_flag_german", "prop_flag_german_s", "prop_flag_ireland", "prop_flag_ireland_s", "prop_flag_japan", "prop_flag_japan_s", "prop_flag_ls", "prop_flag_ls_s", "prop_flag_lsfd", "prop_flag_lsfd_s", "prop_flag_lsservices", "prop_flag_lsservices_s", "prop_flag_mexico", "prop_flag_mexico_s", "prop_flag_russia", "prop_flag_russia_s", "prop_flag_s", "prop_flag_sa", "prop_flag_sa_s", "prop_flag_sapd", "prop_flag_sapd_s", "prop_flag_sapd_s", "prop_flag_scotland", "prop_flag_scotland_s", "prop_flag_sheriff", "prop_flag_sheriff_s", "prop_flag_uk", "prop_flag_uk_s", "prop_flag_us", "prop_flag_us_r", "prop_flag_us_s", "prop_flag_usboat", "prop_flagpole_2b", "prop_flagpole_2c", "ind_prop_dlc_flag_01", "ind_prop_dlc_flag_02", "prop_beachflag_01", "prop_beachflag_le", "prop_golfflag", "pil_prop_fs_safedoor", "prop_ld_int_safe_01", "v_ilev_gangsafe", "v_ilev_gangsafedoor", "prop_big_shit_02", "prop_money_bag_01", "prop_gravestones_02a", "prop_beach_punchbag", "p_crahsed_heli_s" , "prop_crashed_heli", "prop_gascage01" };
	for (int i = 0; i < 204; i++) {
		Object obj = OBJECT::GET_CLOSEST_OBJECT_OF_TYPE(coords.x, coords.y, coords.z, 7.5, GAMEPLAY::GET_HASH_KEY(Objects[i]), 0, 0, 1);
		if (ENTITY::DOES_ENTITY_EXIST(obj) && ENTITY::IS_ENTITY_ATTACHED_TO_ENTITY(obj, playerPed)) {
			RequestControlOfEnt(obj);
			ENTITY::DETACH_ENTITY(obj, 1, 1);
			ENTITY::SET_ENTITY_COLLISION(obj, 0, true);
			ENTITY::SET_ENTITY_ALPHA(obj, 0, true);
			ENTITY::SET_ENTITY_VISIBLE(obj, false, false);
			ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&obj);
			ENTITY::DELETE_ENTITY(&obj);
			Vector3 objcoords = ENTITY::GET_ENTITY_COORDS(obj, false);
			GAMEPLAY::CLEAR_AREA_OF_OBJECTS(objcoords.x, objcoords.y, objcoords.z, 3.0f, 0);
			if (player == PLAYER::PLAYER_ID())
			{
				std::ostringstream Prot; Prot << "Detached Objects";
				iconNotify("Protection", (char*)Prot.str().c_str());
			}
			else
			{
				std::ostringstream Prot; Prot << "Detached Objects\nFrom: " << PLAYER::GET_PLAYER_NAME(player);
				iconNotify("Protection", (char*)Prot.str().c_str());
			}
		}
	}
}

void CrashPlayer(Player player)
{
	Ped playerPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player);
	if (ENTITY::DOES_ENTITY_EXIST(playerPed) && NETWORK::NETWORK_IS_PLAYER_CONNECTED(player) && !INTERIOR::IS_VALID_INTERIOR(INTERIOR::GET_INTERIOR_FROM_ENTITY(playerPed)))
	{
		Vector3 pos = ENTITY::GET_ENTITY_COORDS(playerPed, 1);
		Vector3 myPos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		if (GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(myPos.x, myPos.y, myPos.z, pos.x, pos.y, pos.z, true) > 1500.0f)
		{
			char* crashPeds[3] = { "slod_human", "slod_small_quadped", "slod_large_quadped" };
			for (int i = 0; i < 2; i++)
			{
				Hash hash = GAMEPLAY::GET_HASH_KEY(crashPeds[i]);
				STREAMING::REQUEST_MODEL(hash);
				while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
				Ped crashPed = PED::CREATE_PED(26, hash, pos.x, pos.y, pos.z, 0, true, 0);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
				ENTITY::ATTACH_ENTITY_TO_ENTITY(crashPed, playerPed, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 2, 1);
				WAIT(1);
				int deleteTrys = 5;
				for (int j = 1; j < deleteTrys; j++)
				{
					if (ENTITY::DOES_ENTITY_EXIST(crashPed))
					{
						RequestControlOfEnt(crashPed);
						ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&crashPed);
						ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&crashPed);
						ENTITY::DELETE_ENTITY(&crashPed);
					}
					else
						break;
				}
			}
		}
		else
		{
			iconNotify("Info", "to close to the Player,\nget 1,5km distance to the Player");
			//DWORD64 args[2] = { 1770262894, player };
			//Hooking::trigger_script_event(1, args, 2, 1 << player);
		}
	}
}

void OnlineStealth(int amount)
{
	if (amount >= 0 && amount <= 15000000)
	{
		/*
		int netShop = 2147483647;
		UNK3::_NETWORK_SHOP_BEGIN_SERVICE(&netShop, 1474183246, -1586170317, 1445302971, amount, flag);
		if (!UNK3::_NETWORK_SHOP_CHECKOUT_START(netShop)) {

		}
		*/

		globalHandle(4265412).At(1).As<int>() = 2147483646;
		globalHandle(4265412).At(2).As<int>() = amount;
		globalHandle(4265412).At(3).As<int>() = -1586170317;
		globalHandle(4265412).At(5).As<int>() = NULL;
		globalHandle(4265412).At(6).As<int>() = NULL;
		globalHandle(4265412).At(7).As<int>() = 2147483647;
	}
}

void VehicleStealth(int amount)
{
	if (amount >= 0 && amount <= 2100000000)
	{
		//globalHandle(26269).As<int>() = amount;
		globalHandle(99007 + 970).As<int>() = amount;
	}
}

void UnlockAll()
{
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_STAM"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_STRN"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_LUNG"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_DRIV"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_FLY"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_SHO"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_STL"), 100, 1);
	STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMRACEWORLDRECHOLDER"), 1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_ENEMYDRIVEBYKILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_USJS_COMPLETED"), 50, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_USJS_FOUND"), 50, 1);
	STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMWINALLRACEMODES"), 1, 1);
	STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMWINEVERYGAMEMODE"), 1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_DB_PLAYER_KILLS"), 1000, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_KILLS_PLAYERS"), 1000, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMHORDWAVESSURVIVE"), 21, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_CAR_BOMBS_ENEMY_KILLS"), 25, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_TDM_MVP"), 60, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_HOLD_UP_SHOPS"), 20, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_RACES_WON"), 101, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_NO_ARMWRESTLING_WINS"), 21, 1);
	STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMATTGANGHQ"), 1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMBBETWIN"), 50000, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_DM_WINS"), 51, 1);
	STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMFULLYMODDEDCAR"), 1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_DM_TOTALKILLS"), 500, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_DM_TOTAL_DEATHS"), 412, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TIMES_FINISH_DM_TOP_3"), 36, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_PLAYER_HEADSHOTS"), 623, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_DM_WINS"), 63, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_TDM_WINS"), 13, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_GTA_RACES_WON"), 12, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_GOLF_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_SHOOTRANG_TG_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_SHOOTRANG_RT_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_SHOOTRANG_CT_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_SHOOTRANG_GRAN_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_TENNIS_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TENNIS_MATCHES_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TOTAL_TDEATHMATCH_WON"), 63, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TOTAL_RACES_WON"), 101, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TOTAL_DEATHMATCH_LOST"), 23, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TOTAL_RACES_LOST"), 36, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_25_KILLS_STICKYBOMBS"), 50, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_50_KILLS_GRENADES"), 50, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_GRENADE_ENEMY_KILLS"), 50, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_20_KILLS_MELEE"), 50, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MOLOTOV_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CMBTPISTOL_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_PISTOL50_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_APPISTOL_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MICROSMG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SMG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_ASLTSMG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_ASLTRIFLE_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CRBNRIFLE_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_ADVRIFLE_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CMBTMG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_ASLTMG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_PUMP_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SAWNOFF_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_BULLPUP_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_ASLTSHTGN_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SNIPERRFL_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_HVYSNIPER_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_GRNLAUNCH_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_RPG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MINIGUNS_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_GRENADE_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SMKGRENADE_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_STKYBMB_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MOLOTOV_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_FEET_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_HAIR"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_HAIR_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_HAIR_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_HAIR_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_HAIR_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_HAIR_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_HAIR_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_HAIR_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_JBIB"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_JBIB_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_JBIB_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_JBIB_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_JBIB_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_JBIB_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_JBIB_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_JBIB_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_LEGS"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_LEGS_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_LEGS_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_LEGS_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_LEGS_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_LEGS_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_LEGS_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_LEGS_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_FEET"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_FEET_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_FEET_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_FEET_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_FEET_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_FEET_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_FEET_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_FEET_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_BERD"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_BERD_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_BERD_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_BERD_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_BERD_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_BERD_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_BERD_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_BERD_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_PROPS"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_PROPS_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_PROPS_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_PROPS_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_PROPS_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_PROPS_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_PROPS_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_PROPS_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_PROPS_8"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_PROPS_9"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_PROPS_10"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_OUTFIT"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_HAIR"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_HAIR_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_HAIR_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_HAIR_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_HAIR_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_HAIR_5"), -1, 1);;
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_HAIR_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_HAIR_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_JBIB"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_JBIB_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_JBIB_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_JBIB_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_JBIB_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_JBIB_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_JBIB_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_JBIB_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_LEGS"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_LEGS_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_LEGS_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_LEGS_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_LEGS_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_LEGS_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_LEGS_6"), -1, 1);;
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_LEGS_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_FEET"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_FEET_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_FEET_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_FEET_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_FEET_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_FEET_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_FEET_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_FEET_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_BERD"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_BERD_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_BERD_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_BERD_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_BERD_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_BERD_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_BERD_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_BERD_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_PROPS"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_PROPS_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_PROPS_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_PROPS_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_PROPS_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_PROPS_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_PROPS_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_PROPS_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_PROPS_8"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_PROPS_9"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_PROPS_10"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_OUTFIT"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_TORSO"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_SPECIAL"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_SPECIAL_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_SPECIAL_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_SPECIAL_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_SPECIAL_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_SPECIAL_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_SPECIAL_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_SPECIAL_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_SPECIAL2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_SPECIAL2_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_DECL"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_TEETH"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_TEETH_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_AVAILABLE_TEETH_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_TORSO"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_SPECIAL"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_SPECIAL_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_SPECIAL_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_SPECIAL_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_SPECIAL_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_SPECIAL_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_SPECIAL_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_SPECIAL_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_SPECIAL2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_SPECIAL2_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_DECL"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CLTHS_ACQUIRED_TEETH"), -1, 1);
	for (int i = 0; i < 2; i++)
	{
		char str[10];
		sprintf(str, "%d", i);
		std::ostringstream String; String << "MP0_CLTHS_ACQUIRED_TEETH_" << str;
		Hash hash = GAMEPLAY::GET_HASH_KEY(AddStrings("MP0_CLTHS_ACQUIRED_TEETH_", str));
		STATS::STAT_SET_INT(hash, -1, 1);
		Hash hashs = GAMEPLAY::GET_HASH_KEY(AddStrings("MP0_CLTHS_ACQUIRED_TEETH_2", str));
		STATS::STAT_SET_INT(hashs, -1, 1);
	}
	for (int i = 0; i < 140; i++)
	{
		char str[10];
		sprintf(str, "%d", i);

		Hash hash = GAMEPLAY::GET_HASH_KEY(AddStrings("MP0_DLC_APPAREL_ACQUIRED_", str));
		STATS::STAT_SET_INT(hash, -1, 1);
		Hash hashs = GAMEPLAY::GET_HASH_KEY(AddStrings("MP1_DLC_APPAREL_ACQUIRED_", str));
		STATS::STAT_SET_INT(hashs, -1, 1);
	}
	for (int i = 0; i < 26; i++)
	{
		char str[10];
		sprintf(str, "%d", i);

		Hash hash = GAMEPLAY::GET_HASH_KEY(AddStrings("MP0_ADMIN_CLOTHES_GV_BS_", str));
		STATS::STAT_SET_INT(hash, -1, 1);
		Hash hashs = GAMEPLAY::GET_HASH_KEY(AddStrings("MP1_ADMIN_CLOTHES_GV_BS_", str));
		STATS::STAT_SET_INT(hashs, -1, 1);
	}
	for (int i = 0; i < 38; i++)
	{
		char str[10];
		sprintf(str, "%d", i);

		Hash hash = GAMEPLAY::GET_HASH_KEY(AddStrings("MP0_TATTOO_FM_UNLOCKS_", str));
		STATS::STAT_SET_INT(hash, -1, 1);
		Hash hashs = GAMEPLAY::GET_HASH_KEY(AddStrings("MP1_TATTOO_FM_UNLOCKS_", str));
		STATS::STAT_SET_INT(hashs, -1, 1);
	}
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_STAM"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_STRN"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_LUNG"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_DRIV"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_FLY"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_SHO"), 100, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_STL"), 100, 1);
	STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMRACEWORLDRECHOLDER"), 1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_ENEMYDRIVEBYKILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_USJS_COMPLETED"), 50, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_USJS_FOUND"), 50, 1);
	STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMWINALLRACEMODES"), 1, 1);
	STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMWINEVERYGAMEMODE"), 1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_DB_PLAYER_KILLS"), 1000, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_KILLS_PLAYERS"), 1000, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMHORDWAVESSURVIVE"), 21, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_CAR_BOMBS_ENEMY_KILLS"), 25, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_TDM_MVP"), 60, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_HOLD_UP_SHOPS"), 20, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_RACES_WON"), 101, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_NO_ARMWRESTLING_WINS"), 21, 1);
	STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMATTGANGHQ"), 1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMBBETWIN"), 50000, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_DM_WINS"), 51, 1);
	STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMFULLYMODDEDCAR"), 1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_DM_TOTALKILLS"), 500, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_DM_TOTAL_DEATHS"), 412, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TIMES_FINISH_DM_TOP_3"), 36, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_PLAYER_HEADSHOTS"), 623, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_DM_WINS"), 63, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_TDM_WINS"), 13, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_GTA_RACES_WON"), 12, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_GOLF_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_SHOOTRANG_TG_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_SHOOTRANG_RT_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_SHOOTRANG_CT_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_SHOOTRANG_GRAN_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_TENNIS_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TENNIS_MATCHES_WON"), 2, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TOTAL_TDEATHMATCH_WON"), 63, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TOTAL_RACES_WON"), 101, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TOTAL_DEATHMATCH_LOST"), 23, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TOTAL_RACES_LOST"), 36, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_25_KILLS_STICKYBOMBS"), 50, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_50_KILLS_GRENADES"), 50, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_GRENADE_ENEMY_KILLS"), 50, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_20_KILLS_MELEE"), 50, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MOLOTOV_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CMBTPISTOL_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_PISTOL50_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_APPISTOL_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MICROSMG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SMG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_ASLTSMG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_ASLTRIFLE_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CRBNRIFLE_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_ADVRIFLE_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CMBTMG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_ASLTMG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_PUMP_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SAWNOFF_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_BULLPUP_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_ASLTSHTGN_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SNIPERRFL_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_HVYSNIPER_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_GRNLAUNCH_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_RPG_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MINIGUNS_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_GRENADE_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SMKGRENADE_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_STKYBMB_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MOLOTOV_ENEMY_KILLS"), 600, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_FEET_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_HAIR"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_HAIR_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_HAIR_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_HAIR_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_HAIR_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_HAIR_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_HAIR_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_HAIR_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_JBIB"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_JBIB_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_JBIB_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_JBIB_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_JBIB_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_JBIB_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_JBIB_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_JBIB_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_LEGS"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_LEGS_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_LEGS_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_LEGS_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_LEGS_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_LEGS_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_LEGS_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_LEGS_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_FEET"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_FEET_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_FEET_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_FEET_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_FEET_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_FEET_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_FEET_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_FEET_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_BERD"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_BERD_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_BERD_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_BERD_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_BERD_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_BERD_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_BERD_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_BERD_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_PROPS"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_PROPS_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_PROPS_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_PROPS_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_PROPS_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_PROPS_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_PROPS_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_PROPS_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_PROPS_8"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_PROPS_9"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_PROPS_10"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_OUTFIT"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_HAIR"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_HAIR_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_HAIR_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_HAIR_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_HAIR_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_HAIR_5"), -1, 1);;
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_HAIR_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_HAIR_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_JBIB"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_JBIB_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_JBIB_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_JBIB_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_JBIB_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_JBIB_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_JBIB_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_JBIB_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_LEGS"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_LEGS_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_LEGS_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_LEGS_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_LEGS_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_LEGS_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_LEGS_6"), -1, 1);;
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_LEGS_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_FEET"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_FEET_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_FEET_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_FEET_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_FEET_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_FEET_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_FEET_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_FEET_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_BERD"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_BERD_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_BERD_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_BERD_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_BERD_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_BERD_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_BERD_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_BERD_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_PROPS"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_PROPS_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_PROPS_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_PROPS_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_PROPS_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_PROPS_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_PROPS_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_PROPS_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_PROPS_8"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_PROPS_9"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_PROPS_10"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_OUTFIT"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_TORSO"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_SPECIAL"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_SPECIAL_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_SPECIAL_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_SPECIAL_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_SPECIAL_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_SPECIAL_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_SPECIAL_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_SPECIAL_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_SPECIAL2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_SPECIAL2_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_DECL"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_TEETH"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_TEETH_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_AVAILABLE_TEETH_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_TORSO"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_SPECIAL"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_SPECIAL_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_SPECIAL_2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_SPECIAL_3"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_SPECIAL_4"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_SPECIAL_5"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_SPECIAL_6"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_SPECIAL_7"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_SPECIAL2"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_SPECIAL2_1"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_DECL"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CLTHS_ACQUIRED_TEETH"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_REV_DA_IN_POSSESSION"), -1, 1);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_STAM"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_STRN"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_LUNG"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_DRIV"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_FLY"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_SHO"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_STL"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_STAM"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_STRN"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_LUNG"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_DRIV"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_FLY"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_SHO"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_STL"), 100, 0);
	STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MPPLY_VEHICLE_ID_ADMIN_WEB"), 117401876, 1);
}

int healthsave;
int sethealth = 0;
bool healthchange = false;
int setarmour = 0;
bool armourchange = false;
int wantedlevel = 0;
bool wantedlevelbool = false;
int visualwantedlevel = 0;
bool visualwantedlevelbool = false;
bool godmode = false;
bool neverwanted = false;
bool alwayswanted = false;
float fastrunspeed = 1.0f;
float fastswimspeed = 1.0f;
bool superrun = false;
float superrunspeed = 3.4f;
bool sliderun = false;
float sliderunspeed = 1.2f;
bool superjump = false;
bool noragdoll = false;
bool seatbelt = false;
int healincoverint = 3;
bool healincover = false;
int autohealint = 1;
bool autoheal = false;
bool alphachange = false;
int playeralpha = 255;
bool slowmotion = false;
float timescale = 1.0f;
bool firebreath = false;
bool vomit = false;
float noclipspeed = 1.5f;
bool noclip = false;
bool walkingnoclip = false;
bool invisible = false;
bool offradar = false;
bool offradartoggle = false;
bool offradar1 = false;
bool bullshark = false;
bool bypasschat = false;
bool badsport = false;
bool antiafk = false;
bool walkunderwater = false;
bool superman = false;
bool tinyplayer = false;
bool autoclean = false;
bool walkonwater = false;
bool forcefield = false;

//Outfit Stuff
bool outfitchange = false;
bool forceoutfit = false;

int face = 0;
int mask = 0;
int hair = 0;
int torso = 0;
int leg = 0;
int bag = 0;
int shoes = 0;
int accessory = 0;
int undershirt = 0;
int kevlar = 0;
int badge = 0;
int torso2 = 0;

void SelfOptions()
{
	
	//Get Player Outfit
	if (!outfitchange && !forceoutfit && Menu::Settings::currentMenu == outfitsmenu)
	{
		face = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 0);
		mask = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 1);
		hair = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 2);
		torso = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 3);
		leg = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 4);
		bag = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 5);
		shoes = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 6);
		accessory = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 7);
		undershirt = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 8);
		kevlar = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 9);
		badge = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 10);
		torso2 = PED::GET_PED_DRAWABLE_VARIATION(PLAYER::PLAYER_PED_ID(), 11);
	}
	if (forceoutfit)
	{
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 0, face, 0, 0);
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 1, mask, 0, 0);
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 2, hair, 0, 0);
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 3, torso, 0, 0);
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 4, leg, 0, 0);
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 5, bag, 0, 0);
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 6, shoes, 0, 0);
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 7, accessory, 0, 0);
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 8, undershirt, 0, 0);
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 9, kevlar, 0, 0);
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 10, badge, 0, 0);
		PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 11, torso2, 0, 0);
	}
	
	//Godmode
	ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::PLAYER_PED_ID(), godmode);
	//Set Health
	if (!healthchange)
		sethealth = ENTITY::GET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID());
	//Set Armor
	if (!armourchange)
		setarmour = PED::GET_PED_ARMOUR(PLAYER::PLAYER_PED_ID());
	//Never Wanted
	if (neverwanted) {
		PLAYER::CLEAR_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID());
		GAMEPLAY::SET_FAKE_WANTED_LEVEL(0);
	}
	PLAYER::SET_MAX_WANTED_LEVEL(neverwanted ? 0 : 5);
	PLAYER::SET_POLICE_IGNORE_PLAYER(PLAYER::PLAYER_ID(), neverwanted);
	//Always Wanted
	if (alwayswanted)
		Memory::set_value<int>({ 0x8, 0x10C8, 0x868 }, 5);
	//Wanted Level
	if (wantedlevelbool)
		Memory::set_value<int>({ 0x8, 0x10C8, 0x868 }, wantedlevel);
	else
		wantedlevel = Memory::get_value<int>({ 0x8, 0x10C8, 0x868 });
	//Visual Wanted Level
	if (visualwantedlevelbool)
		GAMEPLAY::SET_FAKE_WANTED_LEVEL(visualwantedlevel);
	else
		visualwantedlevel = GAMEPLAY::GET_FAKE_WANTED_LEVEL();
	//Tiny Player
	PED::SET_PED_CONFIG_FLAG(PLAYER::PLAYER_PED_ID(), 223, tinyplayer);
	//Slowmo stuff
	if (slowmotion)
		if (timescale != 0.3f) slowmotion = false;
	//Time Scale
	GAMEPLAY::SET_TIME_SCALE(timescale);
	//Player Alpha
	if (!alphachange) playeralpha = ENTITY::GET_ENTITY_ALPHA(PLAYER::PLAYER_PED_ID());
	//Invisible
	if (invisible)
	{
		ENTITY::SET_ENTITY_ALPHA(PLAYER::PLAYER_PED_ID(), 0, false);
		if (playeralpha != 0) invisible = false;
	}
	ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), !invisible, 0);
	//Fire Breath
	if (firebreath)
	{
		static int timer;
		if ((GetTickCount() - timer) > 700) {
			STREAMING::REQUEST_NAMED_PTFX_ASSET("core");
			GRAPHICS::_USE_PARTICLE_FX_ASSET_NEXT_CALL("core");
			GRAPHICS::_START_PARTICLE_FX_NON_LOOPED_ON_PED_BONE_2("ent_sht_flame", PLAYER::PLAYER_PED_ID(), 0.02, 0.2, 0.0, 90.0, -100.0, 90.0, SKEL_Head, 1, 1, 1, 1);
			STREAMING::REMOVE_PTFX_ASSET();
			timer = GetTickCount();
		}
	}
	//Vomit
	if (vomit)
	{
		float XPos = -0.035, YPos = 0.1, ZPos = 0.0, XOff = 0.0, YOff = -100.0, ZOff = 150.0;
		STREAMING::REQUEST_NAMED_PTFX_ASSET("scr_familyscenem");
		GRAPHICS::_USE_PARTICLE_FX_ASSET_NEXT_CALL("scr_familyscenem");
		GRAPHICS::_START_PARTICLE_FX_NON_LOOPED_ON_PED_BONE_2("scr_puke_in_car", PLAYER::PLAYER_PED_ID(), XPos, YPos, ZPos, XOff, YOff, ZOff, 0x796e, 1, 1, 1, 1);
		STREAMING::REMOVE_PTFX_ASSET();
	}
	//Auto-Clean
	if (autoclean)
	{
		PED::RESET_PED_VISIBLE_DAMAGE(PLAYER::PLAYER_PED_ID());
		PED::CLEAR_PED_BLOOD_DAMAGE(PLAYER::PLAYER_PED_ID());
	}
	//Forcefield
	if (forcefield)
	{
		Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
		FIRE::ADD_EXPLOSION(playerPos.x, playerPos.y, playerPos.z, 29, 5.0f, false, true, 0.0f);
	}
	ENTITY::SET_ENTITY_PROOFS(PLAYER::PLAYER_PED_ID(), false, false, forcefield, false, false, false, false, false);
	//Movement 
	//Fastrun
	//if (fastrun)
	//	PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER::PLAYER_ID(), 1.49);
	//else
	//	PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER::PLAYER_ID(), 1.00);
	Memory::set_value<float>({ 0x8, 0x10C8, 0xCD0 }, fastrunspeed);
	//Fastswim
	Memory::set_value<float>({ 0x8, 0x10C8, 0x150 }, fastswimspeed);
	//Slide Run
	if (sliderun)
	{
		if (!PED::IS_PED_RAGDOLL(PLAYER::PLAYER_PED_ID()))
		{
			if (CONTROLS::IS_CONTROL_PRESSED(0, INPUT_SPRINT))
			{
				ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::PLAYER_PED_ID(), 1, 0.0f, sliderunspeed, 0, 0.0f, 0.0f, 0.0f, 0, 1, 1, 1, 0, 1);
			}
		}
	}
	//Superrun
	if (superrun)
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		if (!ENTITY::IS_ENTITY_IN_AIR(playerPed) && !PED::IS_PED_RAGDOLL(playerPed))
		{
			if (CONTROLS::IS_CONTROL_PRESSED(0, INPUT_SPRINT))
				ENTITY::APPLY_FORCE_TO_ENTITY(playerPed, 1, 0.0, superrunspeed, 0.0, 0.0, 0.0, 0.0, 1, 1, 1, 1, 0, 1);

			else if (CONTROLS::IS_CONTROL_JUST_RELEASED(0, INPUT_SPRINT))
			{
				ENTITY::FREEZE_ENTITY_POSITION(playerPed, true);
				ENTITY::FREEZE_ENTITY_POSITION(playerPed, false);
			}
		}
	}
	//Superjump
	if (superjump)
	{
		GAMEPLAY::SET_SUPER_JUMP_THIS_FRAME(PLAYER::PLAYER_ID());
	}
	//No Ragdoll
	PED::SET_PED_CAN_RAGDOLL(PLAYER::PLAYER_PED_ID(), !noragdoll);
	PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(PLAYER::PLAYER_PED_ID(), !noragdoll);
	//Seatbelt
	PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(PLAYER::PLAYER_PED_ID(), seatbelt);
	if (!NETWORK::NETWORK_IS_SESSION_STARTED())
	{
		PED::SET_PED_CONFIG_FLAG(PLAYER::PLAYER_PED_ID(), 32, !seatbelt);
	}
	//Heal In Cover 
	if (healincover)
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		int health = ENTITY::GET_ENTITY_HEALTH(playerPed);
		if (PED::IS_PED_IN_COVER(playerPed, false) && !PED::IS_PED_AIMING_FROM_COVER(playerPed) && health < ENTITY::GET_ENTITY_MAX_HEALTH(playerPed))
		{
			static int timer;
			if ((GetTickCount() - timer) > 200)
			{
				ENTITY::SET_ENTITY_HEALTH(playerPed, health + healincoverint);
				timer = GetTickCount();
			}
		}
	}
	//Auto Heal
	if (autoheal)
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		int health = ENTITY::GET_ENTITY_HEALTH(playerPed);
		if (health < ENTITY::GET_ENTITY_MAX_HEALTH(playerPed))
		{
			static int timer;
			if ((GetTickCount() - timer) > 200)
			{
				ENTITY::SET_ENTITY_HEALTH(playerPed, health + autohealint);
				timer = GetTickCount();
			}
		}
	}
	//Online Functions
	if (NETWORK::NETWORK_IS_SESSION_STARTED())
	{
		//Offradar Lester
		if (offradar1)
		{
			globalHandle(2425869).At(1 + (PLAYER::PLAYER_ID() * 443)).At(204).As<int>() = true;
			globalHandle(2440049).At(70).As<int>() = NETWORK::GET_NETWORK_TIME() + 59;
		}
		//Bullshark Testosterone
		if (bullshark)
		{
			if (!PLAYER::IS_PLAYER_DEAD(PLAYER::PLAYER_ID()))
			{
				globalHandle(2440049).At(4006).As<int>() = 3;
			}
		}
		//Anti AFK
		if (antiafk)
		{
			globalHandle(1377236).At(1165).As<int>() = -1;
			globalHandle(1377236).At(1149).As<int>() = -1;
		}
		//Offradar Fake Death
		if (offradar)
		{
			if (!offradartoggle)
			{
				offradartoggle = true;
				healthsave = ENTITY::GET_ENTITY_MAX_HEALTH(PLAYER::PLAYER_PED_ID());
			}
			ENTITY::SET_ENTITY_MAX_HEALTH(PLAYER::PLAYER_PED_ID(), 0);
		}
		else
		{
			if (offradartoggle)
			{
				offradartoggle = false;
				ENTITY::SET_ENTITY_MAX_HEALTH(PLAYER::PLAYER_PED_ID(), healthsave);
			}
		}
		//Bypass Chat 
		NETWORK::NETWORK_OVERRIDE_RECEIVE_RESTRICTIONS(PLAYER::PLAYER_ID(), bypasschat);
		NETWORK::NETWORK_OVERRIDE_CHAT_RESTRICTIONS(PLAYER::PLAYER_ID(), bypasschat);
	}
	//No-Clip
	static int cam;

	if (noclip)
	{
		//Hide Radar
		UI::HIDE_HUD_AND_RADAR_THIS_FRAME();

		//Create a New Cam
		Vector3 rot = CAM::GET_GAMEPLAY_CAM_ROT(0);
		if (!CAM::DOES_CAM_EXIST(cam)) {
			cam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
			CAM::SET_CAM_ROT(cam, rot.x, rot.y, rot.z, 0);
			Vector3 camCoords = CAM::GET_GAMEPLAY_CAM_COORD();
			CAM::SET_CAM_COORD(cam, camCoords.x, camCoords.y, camCoords.z);
			ENTITY::SET_ENTITY_ALPHA(PLAYER::PLAYER_PED_ID(), 120, false);
		}

		//Set Entity Parameters
		Entity playerPed = PLAYER::PLAYER_PED_ID();
		if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false)) {
			playerPed = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
		}

		ENTITY::SET_ENTITY_INVINCIBLE(playerPed, true);
		//ENTITY::SET_ENTITY_HAS_GRAVITY(playerPed, false);
		PLAYER::DISABLE_PLAYER_FIRING(playerPed, true);
		ENTITY::SET_ENTITY_HEADING(playerPed, ENTITY::GET_ENTITY_HEADING(playerPed) + 3.0f);
		Vector3 Coords = ENTITY::GET_ENTITY_COORDS(playerPed, true);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, Coords.x, Coords.y, Coords.z, 0, 0, 0);

		float speed = 0;

		if (IsKeyPressed(VK_KEY_W) || CONTROLS::IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RT)) {
			speed = noclipspeed;
			Vector3 c = addVector(CAM::GET_CAM_COORD(cam), multiplyVector(RotationToDirection(rot), speed));
			CAM::SET_CAM_COORD(cam, c.x, c.y, c.z);
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, c.x, c.y, c.z - 1.7f, false, false, false);
		}

		if (IsKeyPressed(VK_KEY_S) || CONTROLS::IS_CONTROL_PRESSED(2, INPUT_FRONTEND_LT)) {
			speed = -noclipspeed;
			Vector3 c = addVector(CAM::GET_CAM_COORD(cam), multiplyVector(RotationToDirection(rot), speed));
			CAM::SET_CAM_COORD(cam, c.x, c.y, c.z);
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, c.x, c.y, c.z - 1.7f, false, false, false);
		}
	}
	else
	{
		if (CAM::DOES_CAM_EXIST(cam)) {
			CAM::RENDER_SCRIPT_CAMS(false, true, 700, 1, 1);
			CAM::SET_CAM_ACTIVE(cam, 0);
			CAM::DESTROY_CAM(cam, true);
			PLAYER::DISABLE_PLAYER_FIRING(PLAYER::PLAYER_PED_ID(), false);
			ENTITY::SET_ENTITY_ALPHA(PLAYER::PLAYER_PED_ID(), 255, false);
		}
	}
	//Walking No-Clip
	if (walkingnoclip)
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		Vector3 pos = ENTITY::GET_ENTITY_COORDS(playerPed, false);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, pos.x, pos.y, pos.z, true, true, true);
		if (IsKeyPressed(VK_KEY_W)) {
			float fivef = .5f;
			float heading = ENTITY::GET_ENTITY_HEADING(playerPed);
			float xVec = fivef * sin((heading * 0.017453292519943295)) * -1.0f;
			float yVec = fivef * cos((heading * 0.017453292519943295));
			ENTITY::SET_ENTITY_HEADING(playerPed, heading);

			pos.x += xVec, pos.y += yVec;
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, pos.x, pos.y, pos.z, true, true, true);
		}
		if (IsKeyPressed(VK_KEY_A)) {
			float heading = ENTITY::GET_ENTITY_HEADING(playerPed);
			ENTITY::SET_ENTITY_HEADING(playerPed, heading + 0.5f);
		}
		if (IsKeyPressed(VK_KEY_D)) {
			float heading = ENTITY::GET_ENTITY_HEADING(playerPed);
			ENTITY::SET_ENTITY_HEADING(playerPed, heading - 0.5f);
		}
		if (IsKeyPressed(VK_SHIFT)) {
			pos.z -= 0.5;
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, pos.x, pos.y, pos.z, false, false, false);
		}
		if (IsKeyPressed(VK_SPACE)) {
			pos.z += 0.5;
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, pos.x, pos.y, pos.z, false, false, false);
		}
	}
	//Walk Under Water
	if (walkunderwater)
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		if (ENTITY::IS_ENTITY_IN_WATER(playerPed))
		{
			PED::SET_PED_CONFIG_FLAG(playerPed, 65, false);
			PED::SET_PED_CONFIG_FLAG(playerPed, 66, false);
			PED::SET_PED_CONFIG_FLAG(playerPed, 168, false);

			Vector3 PlayerPos = ENTITY::GET_ENTITY_COORDS(playerPed, 0);
			GRAPHICS::_DRAW_LIGHT_WITH_RANGE_AND_SHADOW(PlayerPos.x, PlayerPos.y, (PlayerPos.z + 1.5f), 255, 255, 251, 100.0f, 1.5f, 0.0f);
			GRAPHICS::_DRAW_LIGHT_WITH_RANGE_AND_SHADOW(PlayerPos.x, PlayerPos.y, (PlayerPos.z + 50.0f), 255, 255, 251, 200.0f, 1.0f, 0.0f);

			if (PED::IS_PED_JUMPING(playerPed))
			{
				ENTITY::APPLY_FORCE_TO_ENTITY(playerPed, true, 0, 0, 0.7f, 0, 0, 0, true, true, true, true, false, true);
			}

			if (ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(playerPed) > 1.2f)
			{
				PED::SET_PED_CONFIG_FLAG(playerPed, 60, false);
				PED::SET_PED_CONFIG_FLAG(playerPed, 61, false);
				PED::SET_PED_CONFIG_FLAG(playerPed, 104, false);
				PED::SET_PED_CONFIG_FLAG(playerPed, 276, false);
				PED::SET_PED_CONFIG_FLAG(playerPed, 76, true);
				ENTITY::APPLY_FORCE_TO_ENTITY(playerPed, true, 0, 0, -0.7f, 0, 0, 0, true, true, true, true, false, true);
			}

			if (AI::GET_IS_TASK_ACTIVE(playerPed, 281) || PED::IS_PED_SWIMMING(playerPed) || PED::IS_PED_SWIMMING_UNDER_WATER(playerPed))
			{
				AI::CLEAR_PED_TASKS_IMMEDIATELY(playerPed);
			}
		}
	}
	//Superman
	if (superman) 
	{
		if (!WEAPON::HAS_PED_GOT_WEAPON(PLAYER::PLAYER_PED_ID(), GAMEPLAY::GET_HASH_KEY("GADGET_PARACHUTE"), false))
		{
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), GAMEPLAY::GET_HASH_KEY("GADGET_PARACHUTE"), 1, 1);
		}

		Vector3 pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 0);
		ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::PLAYER_PED_ID(), true);

		if (!PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false) && !PED::IS_PED_RAGDOLL(PLAYER::PLAYER_PED_ID()))
		{
			if (ENTITY::IS_ENTITY_IN_AIR(PLAYER::PLAYER_PED_ID()))
			{
				CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_PARACHUTE_DEPLOY, true);
				CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_PARACHUTE_DETACH, true);

				//Vanish Parachute
				char* Parachutes[] = { "p_parachute1_mp_dec", "p_parachute1_mp_s", "p_parachute1_s", "p_parachute1_sp_dec", "p_parachute1_sp_s", "prop_v_parachute", "p_parachute_fallen_s" };
				for (int i = 0; i < (sizeof(Parachutes) / sizeof(Parachutes[0])); i++)
				{
					Object Parachute = OBJECT::GET_CLOSEST_OBJECT_OF_TYPE(pos.x, pos.y, pos.z, 5.0, GAMEPLAY::GET_HASH_KEY(Parachutes[i]), 0, 0, 1);
					if (ENTITY::DOES_ENTITY_EXIST(Parachute))
					{
						if (ENTITY::GET_ENTITY_ALPHA(Parachute) != 0)
						{
							ENTITY::SET_ENTITY_ALPHA(Parachute, 0, true);
							ENTITY::SET_ENTITY_VISIBLE(Parachute, false, false);
						}
					}
				}

				if (CONTROLS::IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RT) || GetAsyncKeyState(VK_SHIFT))
				{
					ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::PLAYER_PED_ID(), 1, 0, 6, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1);
				}
			}
			if (!PED::IS_PED_CLIMBING(PLAYER::PLAYER_PED_ID()))
			{
				if (CONTROLS::IS_CONTROL_PRESSED(2, INPUT_JUMP))
				{
					if (ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(PLAYER::PLAYER_PED_ID()) < 1.5f)
					{
						FIRE::ADD_EXPLOSION(pos.x, pos.y, pos.z - 1.0f, 26, 0, true, true, 0);
					}
					ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::PLAYER_PED_ID(), 1, 0, 0, 6, 0, 0, 0, 0, 1, 1, 1, 0, 1);
				}
			}
		}
	}
	if (walkonwater && !walkingnoclip && !superman && !noclip)
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		Vector3 pos = ENTITY::GET_ENTITY_COORDS(playerPed, 0);
		float height = 0;
		if (WATER::GET_WATER_HEIGHT(pos.x, pos.y, pos.z, &height))
		{
			WATER::SET_DEEP_OCEAN_SCALER(0.0f);
			Object container = OBJECT::GET_CLOSEST_OBJECT_OF_TYPE(pos.x, pos.y, pos.z, 10.0f, GAMEPLAY::GET_HASH_KEY("prop_container_ld2"), 0, 0, 1);
			if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, false))
			{
				if (ENTITY::DOES_ENTITY_EXIST(container) && height > -50.0f) 
				{
					//RequestControlOfEnt(container);
					ENTITY::SET_ENTITY_COORDS(container, pos.x, pos.y, height - 2.8, 0, 0, 0, 1);
					ENTITY::SET_ENTITY_ROTATION(container, 0, 0, ENTITY::GET_ENTITY_ROTATION(playerPed, 0).z, 0, 1);
					Vector3 containerCoords = ENTITY::GET_ENTITY_COORDS(container, 1);
					if (pos.z < containerCoords.z)
					{
						ENTITY::SET_ENTITY_COORDS(playerPed, pos.x, pos.y, containerCoords.z + 2.0f, 0, 0, 0, 1);
					}
				}
				else 
				{
					if (GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(pos.x, pos.y, pos.z, pos.x, pos.y, height, true) < 5.0f)
					{
						Hash model = GAMEPLAY::GET_HASH_KEY("prop_container_ld2");
						STREAMING::REQUEST_MODEL(model);
						while (!STREAMING::HAS_MODEL_LOADED(model)) WAIT(1);
						container = OBJECT::CREATE_OBJECT(model, pos.x, pos.y, height - 2.8, 1, 1, 0);
						//RequestControlOfEnt(container);
						ENTITY::FREEZE_ENTITY_POSITION(container, 1);
						ENTITY::SET_ENTITY_ALPHA(container, 0, 1);
						ENTITY::SET_ENTITY_VISIBLE(container, 0, 0);
					}
				}
			}
		}
		else 
		{
			Object container = OBJECT::GET_CLOSEST_OBJECT_OF_TYPE(pos.x, pos.y, pos.z, 10.0f, GAMEPLAY::GET_HASH_KEY("prop_container_ld2"), 0, 0, 1);
			if (ENTITY::DOES_ENTITY_EXIST(container))
			{
				RequestControlOfEnt(container);
				ENTITY::SET_ENTITY_COORDS(container, 0, 0, -1000.0f, 0, 0, 0, 1);
				ENTITY::SET_ENTITY_COLLISION(container, false, true);
				ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&container);
				ENTITY::DELETE_ENTITY(&container);
				WATER::RESET_DEEP_OCEAN_SCALER();
			}
		}
	}
}

bool infammo = false;
bool noreload = false;
bool triggerbot = false;
int moneygunvalue = 2500;
int moneygunvalue2 = 2500;
bool moneygun = false;
bool moneygun2 = false;
bool rpgun = false;
bool rpgun2 = false;
int explosiveammoint = 0;
int explosiveammoint2 = 0;
bool explosiveammo = false;
bool explosiveammo2 = false;
bool explosivemelee = false;
bool explosivemelee2 = false;
bool aimassist = false;
bool airstrikegun = false;
bool airstrikegun2 = false;
bool newairstrikegun = false;
bool newairstrikegun2 = false;
bool rapidfire = false;
bool shootshockwave = false;
bool flamethrower = false;
bool shootrpg = false;
bool gravitygun = false;
bool teleportgun = false;
bool deletegun = false;
bool deletegun2 = false;
bool jetgun = false;

float GravityGunGoto(float aimX, float entityX)
{
	static float GravityGunVeloPos;

	if (aimX > 0 && entityX > 0) GravityGunVeloPos = aimX - (entityX);

	if (aimX < 0 && entityX < 0) GravityGunVeloPos = aimX - (entityX);

	if (aimX > 0 && entityX < 0) GravityGunVeloPos = aimX + (entityX);

	if (aimX < 0 && entityX > 0) GravityGunVeloPos = aimX + (entityX);

	return GravityGunVeloPos;
}

void WeaponOptions()
{
	//Infinite Ammo
	WEAPON::SET_PED_INFINITE_AMMO_CLIP(PLAYER::PLAYER_PED_ID(), infammo);
	//No Reload
	if (noreload)
		WEAPON::_PED_SKIP_NEXT_RELOADING(PLAYER::PLAYER_PED_ID());
	//Trigger Bot
	if (triggerbot)
	{
		Entity AimedAtEntity;
		if (PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(PLAYER::PLAYER_ID(), &AimedAtEntity))
			if (ENTITY::IS_ENTITY_A_PED(AimedAtEntity) && !PED::IS_PED_DEAD_OR_DYING(AimedAtEntity, false))
				if (CONTROLS::IS_CONTROL_PRESSED(2, INPUT_FRONTEND_LT) || IsKeyPressed(VK_RBUTTON))
				{
					Vector3 Mouth = PED::GET_PED_BONE_COORDS(AimedAtEntity, SKEL_Head, 0.1f, 0.0f, 0.0f);
					PED::SET_PED_SHOOTS_AT_COORD(PLAYER::PLAYER_PED_ID(), Mouth.x, Mouth.y, Mouth.z, true);
				}
	}
	//Explosive Ammo && Explosive Melee
	if (explosiveammo || explosivemelee)
	{
		if (explosivemelee) ENTITY::SET_ENTITY_PROOFS(PLAYER::PLAYER_PED_ID(), false, false, true, false, false, false, false, false);

		Hash weaponhash = NULL;

		WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID(), &weaponhash, false);
		if (weaponhash != NULL && ((explosiveammo && weaponhash != WeaponUnarmed) || (explosivemelee && weaponhash == WeaponUnarmed)))
		{
			Vector3 iCoord;
			if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &iCoord))
				FIRE::ADD_EXPLOSION(iCoord.x, iCoord.y, iCoord.z + 0.3f, (explosiveammo && weaponhash != WeaponUnarmed) ? explosiveammoint : 18, 100, true, false, 0);
		}
	}
	//Explosive Ammo && Explosive Melee (selectedPlayer)
	if (explosiveammo2 || explosivemelee2)
	{
		Hash weaponhash = NULL;

		WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), &weaponhash, false);
		if (weaponhash != NULL && ((explosiveammo2 && weaponhash != WeaponUnarmed) || (explosivemelee2 && weaponhash == WeaponUnarmed)))
		{
			Vector3 iCoord;
			if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), &iCoord))
				FIRE::ADD_EXPLOSION(iCoord.x, iCoord.y, iCoord.z + 0.3f, (explosiveammo2 && weaponhash != WeaponUnarmed) ? explosiveammoint2 : 1, 100, true, false, 0);
		}
	}
	//Aim Assist
	if (aimassist)
	{
		Hash weaponhash = NULL;

		if (!PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false) && WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID(), &weaponhash, false))
		{
			if (CONTROLS::IS_CONTROL_PRESSED(2, INPUT_FRONTEND_LT) || IsKeyPressed(VK_RBUTTON))
			{
				GRAPHICS::ANIMPOSTFX_PLAY("RaceTurbo", 0, 0);
				GAMEPLAY::SET_TIME_SCALE(0.2f);
			}
		}
	}
	//Money Gun
	if (moneygun)
	{
		Vector3 iCoord;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &iCoord))
		{
			static int timer;
			if ((GetTickCount() - timer) > 150) {
				STREAMING::REQUEST_MODEL(0x9CA6F755);
				while (!STREAMING::HAS_MODEL_LOADED(0x9CA6F755)) WAIT(1);
				OBJECT::CREATE_AMBIENT_PICKUP(0xDE78F17E, iCoord.x, iCoord.y, iCoord.z + 0.3f, 0, moneygunvalue, 0x9CA6F755, false, true);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(0x9CA6F755);
				timer = GetTickCount();
			}
		}
	}
	//Money Gun (selectedPlayer)
	if (moneygun2)
	{
		Vector3 iCoord;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), &iCoord))
		{
			static int timer;
			if ((GetTickCount() - timer) > 150) {
				STREAMING::REQUEST_MODEL(0x9CA6F755);
				while (!STREAMING::HAS_MODEL_LOADED(0x9CA6F755)) WAIT(1);
				OBJECT::CREATE_AMBIENT_PICKUP(0xDE78F17E, iCoord.x, iCoord.y, iCoord.z + 0.3f, 0, moneygunvalue2, 0x9CA6F755, false, true);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(0x9CA6F755);
				timer = GetTickCount();
			}
		}
	}
	//RP Gun
	if (rpgun)
	{
		Vector3 iCoord;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &iCoord))
		{
			static int timer;
			if ((GetTickCount() - timer) > 150) {
				Hash hash = GAMEPLAY::GET_HASH_KEY("vw_prop_vw_colle_alien");
				STREAMING::REQUEST_MODEL(hash);
				while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
				OBJECT::CREATE_AMBIENT_PICKUP(0xC3CD8B31, iCoord.x, iCoord.y, iCoord.z + 0.3f, 0, 0, hash, false, true);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
				timer = GetTickCount();
			}
		}
	}
	//RP Gun (selectedPlayer)
	if (rpgun2)
	{
		Vector3 iCoord;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), &iCoord))
		{
			static int timer;
			if ((GetTickCount() - timer) > 150) {
				Hash hash = GAMEPLAY::GET_HASH_KEY("vw_prop_vw_colle_alien");
				STREAMING::REQUEST_MODEL(hash);
				while (!STREAMING::HAS_MODEL_LOADED(hash)) WAIT(1);
				OBJECT::CREATE_AMBIENT_PICKUP(0xC3CD8B31, iCoord.x, iCoord.y, iCoord.z + 0.3f, 0, 0, hash, false, true);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
				timer = GetTickCount();
			}
		}
	}
	//Teleport Gun
	if (teleportgun)
	{
		Vector3 iCoord;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &iCoord))
			ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), iCoord.x, iCoord.y, iCoord.z + 1, 0, 0, 1, 1);
	}
	//Delete Gun
	if (deletegun)
	{
		Vector3 iCoord;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &iCoord))
			GAMEPLAY::_CLEAR_AREA_OF_EVERYTHING(iCoord.x, iCoord.y, iCoord.z, 2.85f, 0, 0, 0, 0);
	}
	//Delete Gun (selectedPlayer)
	if (deletegun2)
	{
		Vector3 iCoord;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), &iCoord))
			GAMEPLAY::_CLEAR_AREA_OF_EVERYTHING(iCoord.x, iCoord.y, iCoord.z, 2.85f, 0, 0, 0, 0);
	}
	//Airstrike Gun
	if (airstrikegun)
	{
		Vector3 iCoord;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &iCoord))
			GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(iCoord.x, iCoord.y, iCoord.z + 70, iCoord.x, iCoord.y, iCoord.z, 250, 1, GAMEPLAY::GET_HASH_KEY("VEHICLE_WEAPON_SPACE_ROCKET"), PLAYER::PLAYER_PED_ID(), 1, 1, 500);
	}
	//Airstrike Gun (selectedPlayer)
	if (airstrikegun2)
	{
		Vector3 iCoord;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), &iCoord))
			GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(iCoord.x, iCoord.y, iCoord.z + 70, iCoord.x, iCoord.y, iCoord.z, 250, 1, GAMEPLAY::GET_HASH_KEY("VEHICLE_WEAPON_SPACE_ROCKET"), PLAYER::PLAYER_PED_ID(), 1, 1, 500);
	}
	//New Airstrike Gun
	if (newairstrikegun)
	{
		Vector3 iCoord;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &iCoord))
		{
			FIRE::ADD_EXPLOSION(iCoord.x, iCoord.y, iCoord.z - 1, 22, 100, true, false, 0);
			for (int i = 1; i < 8; i++)
			{
				float spread = 2.5f;
				float spreadx = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-spread), spread);
				float spready = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-spread), spread);
				float height = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(-25, 25);
				GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(iCoord.x + (spreadx * 10.0f), iCoord.y + (spready * 10.0f), iCoord.z + (120 + height), iCoord.x + spreadx, iCoord.y + spready, iCoord.z, 250, 1, GAMEPLAY::GET_HASH_KEY("VEHICLE_WEAPON_SPACE_ROCKET"), PLAYER::PLAYER_PED_ID(), 1, 1, 450);
			}
		}
	}
	//New Airstrike Gun (selectedPlayer)
	if (newairstrikegun2)
	{
		Vector3 iCoord;
		if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &iCoord))
		{
			FIRE::ADD_EXPLOSION(iCoord.x, iCoord.y, iCoord.z - 1, 22, 100, true, false, 0);
			for (int i = 1; i < 8; i++)
			{
				float spread = 2.5f;
				float spreadx = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-spread), spread);
				float spready = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-spread), spread);
				float height = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(-25, 25);
				GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(iCoord.x + (spreadx * 10.0f), iCoord.y + (spready * 10.0f), iCoord.z + (120 + height), iCoord.x + spreadx, iCoord.y + spready, iCoord.z, 250, 1, GAMEPLAY::GET_HASH_KEY("VEHICLE_WEAPON_SPACE_ROCKET"), PLAYER::PLAYER_PED_ID(), 1, 1, 450);
			}
		}
	}
	//Jet Gun
	if (jetgun)
	{
		Vector3 dir = RotationToDirection(CAM::GET_GAMEPLAY_CAM_ROT(0));
		Vector3 camPosition = CAM::GET_GAMEPLAY_CAM_COORD();
		Vector3 playerPosition = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
		float spawnDistance = getDistance(camPosition, playerPosition);
		spawnDistance += 10;
		Vector3 spawnPosition = addVector(camPosition, multiplyVector(dir, spawnDistance));
		if (IsPedShooting(PLAYER::PLAYER_PED_ID()))
		{
			static int timer;
			if ((GetTickCount() - timer) > 200) {
				Hash vehmodel = GAMEPLAY::GET_HASH_KEY("Lazer");
				STREAMING::REQUEST_MODEL(vehmodel);
				while (!STREAMING::HAS_MODEL_LOADED(vehmodel)) WAIT(1);
				Vehicle veh = VEHICLE::CREATE_VEHICLE(vehmodel, spawnPosition.x, spawnPosition.y, spawnPosition.z, ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID()), true, 0);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(vehmodel);
				ENTITY::APPLY_FORCE_TO_ENTITY(veh, 1, dir.x * 1000000.0f, dir.y * 1000000.0f, dir.z * 1000000.0f, 0.0f, 0.0f, 0.0f, 0, 0, 1, 1, 0, 1);
				timer = GetTickCount();
			}
		}
	}
	//Rapidfire
	if (rapidfire)
	{
		Player playerPed = PLAYER::PLAYER_PED_ID();
		if (!PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
		{
			PLAYER::DISABLE_PLAYER_FIRING(PLAYER::PLAYER_PED_ID(), true);
			Vector3 gameplayCam = CAM::_GET_GAMEPLAY_CAM_COORDS();
			Vector3 gameplayCamRot = CAM::GET_GAMEPLAY_CAM_ROT(0);
			Vector3 gameplayCamDirection = RotationToDirection(gameplayCamRot);
			Vector3 startCoords = addVector(gameplayCam, (multiplyVector(gameplayCamDirection, 2.5f)));
			Vector3 endCoords = addVector(startCoords, multiplyVector(gameplayCamDirection, WEAPON::GET_MAX_RANGE_OF_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID())));
			Hash weaponhash;
			WEAPON::GET_CURRENT_PED_WEAPON(playerPed, &weaponhash, 1);
			if (CONTROLS::IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RT) || IsKeyPressed(VK_LBUTTON))
			{
				WEAPON::REQUEST_WEAPON_ASSET(weaponhash, 31, 0);
				while (!WEAPON::HAS_WEAPON_ASSET_LOADED(weaponhash)) WAIT(1);
				GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(startCoords.x, startCoords.y, startCoords.z, endCoords.x, endCoords.y, endCoords.z, 50, true, weaponhash, playerPed, 1, 1, 0xbf800000);
			}
		}
	}
	//Flame Thrower
	if (flamethrower)
	{
		Hash weaponhash = NULL;
		if (WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID(), &weaponhash, false))
		{
			PLAYER::DISABLE_PLAYER_FIRING(PLAYER::PLAYER_ID(), true);

			if (CONTROLS::IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RT) || IsKeyPressed(VK_LBUTTON))
			{
				static int timer;
				if ((GetTickCount() - timer) > 75) {
					STREAMING::REQUEST_NAMED_PTFX_ASSET("core");
					GRAPHICS::_USE_PARTICLE_FX_ASSET_NEXT_CALL("core");
					GRAPHICS::_START_PARTICLE_FX_LOOPED_ON_ENTITY_BONE("ent_sht_flame", WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(PLAYER::PLAYER_PED_ID()), 0.4f, 0.0f, 0.02f, 89.5f, 0.0f, 90.0f, ENTITY::GET_ENTITY_BONE_INDEX_BY_NAME(WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(PLAYER::PLAYER_PED_ID()), "Gun_Nuzzle"), 0.85f, 0, 0, 0);
					STREAMING::REMOVE_PTFX_ASSET();
					timer = GetTickCount();
				}
			}
		}
	}
	//Shoot Shockwave
	if (shootshockwave)
	{
		Player playerPed = PLAYER::PLAYER_PED_ID();
		if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, 1))
		{
			Vector3 gameplayCam = CAM::_GET_GAMEPLAY_CAM_COORDS();
			Vector3 gameplayCamRot = CAM::GET_GAMEPLAY_CAM_ROT(0);
			Vector3 gameplayCamDirection = RotationToDirection(gameplayCamRot);
			Vector3 startCoords = addVector(gameplayCam, (multiplyVector(gameplayCamDirection, 5.0f)));
			Vector3 endCoords = addVector(startCoords, multiplyVector(gameplayCamDirection, 1000.0f));
			Hash weaponhash = GAMEPLAY::GET_HASH_KEY("weapon_raypistol");
			if (IsPedShooting(playerPed))
			{
				WEAPON::REQUEST_WEAPON_ASSET(weaponhash, 31, 0);
				while (!WEAPON::HAS_WEAPON_ASSET_LOADED(weaponhash)) WAIT(1);
				GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(startCoords.x, startCoords.y, startCoords.z, endCoords.x, endCoords.y, endCoords.z, 250, 1, weaponhash, playerPed, 1, 0, 12500);
			}
		}
	}
	//Shoot RPG
	if (shootrpg)
	{
		Player playerPed = PLAYER::PLAYER_PED_ID();
		if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, 1))
		{
			Vector3 gameplayCam = CAM::_GET_GAMEPLAY_CAM_COORDS();
			Vector3 gameplayCamRot = CAM::GET_GAMEPLAY_CAM_ROT(0);
			Vector3 gameplayCamDirection = RotationToDirection(gameplayCamRot);
			Vector3 startCoords = addVector(gameplayCam, (multiplyVector(gameplayCamDirection, 5.0f)));
			Vector3 endCoords = addVector(startCoords, multiplyVector(gameplayCamDirection, 1000.0f));
			Hash weaponhash = GAMEPLAY::GET_HASH_KEY("WEAPON_VEHICLE_ROCKET");
			if (IsPedShooting(playerPed))
			{
				WEAPON::REQUEST_WEAPON_ASSET(weaponhash, 31, 0);
				while (!WEAPON::HAS_WEAPON_ASSET_LOADED(weaponhash)) WAIT(1);
				GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(startCoords.x, startCoords.y, startCoords.z, endCoords.x, endCoords.y, endCoords.z, 250, 1, weaponhash, playerPed, 1, 0, 12500);
			}
		}
	}
	if (gravitygun)
	{
		static int zoom = 15;
		static Entity currEnt{ NULL };

		if (CONTROLS::IS_CONTROL_PRESSED(2, INPUT_FRONTEND_LT) || IsKeyPressed(VK_RBUTTON))
		{
			if (currEnt == NULL)
				PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(PLAYER::PLAYER_ID(), &currEnt);
			else
			{
				if (ENTITY::IS_ENTITY_A_PED(currEnt) && PED::IS_PED_IN_ANY_VEHICLE(currEnt, true))
					currEnt = PED::GET_VEHICLE_PED_IS_IN(currEnt, 0);

				RequestControlOfEnt(currEnt);

				Vector3 rot = CAM::GET_GAMEPLAY_CAM_ROT(0);
				Vector3 dir = RotationToDirection(rot);
				Vector3 spawnPosition = addVector(CAM::GET_GAMEPLAY_CAM_COORD(), multiplyVector(dir, zoom));

				Vector3 getcoords = ENTITY::GET_ENTITY_COORDS(currEnt, 0);
				float getaimcoordsX = GravityGunGoto(spawnPosition.x, getcoords.x);
				float getaimcoordsY = GravityGunGoto(spawnPosition.y, getcoords.y);
				float getaimcoordsZ = GravityGunGoto(spawnPosition.z, getcoords.z);
				ENTITY::SET_ENTITY_VELOCITY(currEnt, getaimcoordsX * 2.0, getaimcoordsY * 2.0, getaimcoordsZ * 2.0);

				if (CONTROLS::IS_CONTROL_PRESSED(2, INPUT_FRONTEND_RT) || IsKeyPressed(VK_LBUTTON))
				{
					ENTITY::APPLY_FORCE_TO_ENTITY(currEnt, 1, dir.x * 25000.0f, dir.y * 25000.0f, dir.z * 25000.0f, 0.0f, 0.0f, 0.0f, false, false, true, true, false, true);
					PLAYER::DISABLE_PLAYER_FIRING(PLAYER::PLAYER_ID(), false);
					currEnt = NULL;
				}
			}
		}
		else
			currEnt = NULL;
	}
}

float rgbmulti = 2.5f;
float vehiclered = 0;
float vehicleblue = 255;
float vehiclegreen = 255;
float rgbmulti2 = 2.5f;
float vehiclered2 = 0;
float vehicleblue2 = 255;
float vehiclegreen2 = 255;
int powermultiplier = 1;
int torquemultiplier = 1;
int accelerationmultiplier = 1;
int brakemultiplier = 1;
int downforcemultiplier = 1;
int hornboostspeed = 35;
bool vehgodmode = false;
bool autofix = false;
int vehicleflyspeed = 90;
bool vehiclefly = false;
bool driftmode = false;
bool hornboost = false;
bool rainbowcolor = false;
bool rainbowtyresmoke = false;
bool rampvehicle = false;
bool sticktoground = false;
bool autoflip = false;
bool autopilotstart = false;
bool unlimitedboost = false;
bool instantboost = false;
bool driveonwater = false;
bool novehiclegravity = false;

float yaw, pitch, roll = 0.0f;

bool vehmodchange = false;

bool turbo; 
bool tiresmoke;
bool xenon;
int windowtint;
int vehicleMods[25];

void VehicleOptions()
{
	//Offline Spawn Bypass
	globalHandle(4269479).As<bool>() = !NETWORK::NETWORK_IS_SESSION_STARTED();
	
	Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
	//Get Vehicle Mods
	if (!vehmodchange && Menu::Settings::currentMenu == lossantoscustoms)
	{
		for (int i = 0; i < 24; i++)
		{
			/*if (vehicleMods[i] < 0) vehicleMods[i] = 0;*/

			if (i == 17 || i == 19 || i == 21)
				continue;

			if (i == 18 || i == 20 || i == 22)
			{
				if (i == 18) turbo = VEHICLE::IS_TOGGLE_MOD_ON(veh, i);
				if (i == 20) tiresmoke = VEHICLE::IS_TOGGLE_MOD_ON(veh, i);
				if (i == 22) xenon = VEHICLE::IS_TOGGLE_MOD_ON(veh, i);
				continue;
			}

			vehicleMods[i] = (VEHICLE::GET_VEHICLE_MOD(veh, i) + 1);
		}
		windowtint = (VEHICLE::GET_VEHICLE_WINDOW_TINT(veh) + 1);
	}

	//Vehicle Gravity 
	if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
	{
		VEHICLE::SET_VEHICLE_GRAVITY(veh, !novehiclegravity);
	}
	//Drift Mode
	if (driftmode)
		VEHICLE::SET_VEHICLE_REDUCE_GRIP(veh, CONTROLS::IS_CONTROL_PRESSED(2, INPUT_VEH_HANDBRAKE));
	else
		VEHICLE::SET_VEHICLE_REDUCE_GRIP(veh, false);
	//Stick to Ground
	if (sticktoground)
	{
		if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false) && ENTITY::GET_ENTITY_SPEED(veh) > 12.0f)
			VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(veh);
	}
	//Auto Flip
	if (autoflip)
	{
		if (ENTITY::IS_ENTITY_UPSIDEDOWN(veh) && (ENTITY::GET_ENTITY_ROLL(veh) > 150 || ENTITY::GET_ENTITY_ROLL(veh) < -150))
		{
			if (!ENTITY::IS_ENTITY_IN_AIR(veh) && !ENTITY::IS_ENTITY_IN_WATER(veh) && !VEHICLE::IS_THIS_MODEL_A_HELI(ENTITY::GET_ENTITY_MODEL(veh)) && !VEHICLE::IS_THIS_MODEL_A_PLANE(ENTITY::GET_ENTITY_MODEL(veh)))
			{
				RequestControlOfEnt(veh);
				ENTITY::SET_ENTITY_ROTATION(veh, 0, 0, ENTITY::GET_ENTITY_ROTATION(veh, 2).z, 2, 1);
			}
		}
	}
	//Auto Pilot
	if (autopilotstart)
	{
		if (!PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
			autopilotstart = false;
		Vector3 coords = GetBlipMarker();
		if (coords.x == 0 && coords.y == 0)
		{
			Vehicle vehicle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);
			AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::PLAYER_PED_ID());
			PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), vehicle, -1);
			float speed = ENTITY::GET_ENTITY_SPEED(vehicle);
			if (speed > 9.0f)
			{
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(vehicle, 7.0f);
			}
			autopilotstart = false;
		}
	}
	//Instant Boost
	if (instantboost && VEHICLE::_HAS_VEHICLE_ROCKET_BOOST(veh))
	{
		if (!VEHICLE::_IS_VEHICLE_ROCKET_BOOST_ACTIVE(veh))
		{
			VEHICLE::_SET_VEHICLE_ROCKET_BOOST_PERCENTAGE(veh, 100.0f);
		}
	}
	//Unlimted Boost
	if (unlimitedboost && VEHICLE::_HAS_VEHICLE_ROCKET_BOOST(veh))
	{
		if (VEHICLE::_IS_VEHICLE_ROCKET_BOOST_ACTIVE(veh))
		{
			VEHICLE::_SET_VEHICLE_ROCKET_BOOST_PERCENTAGE(veh, 100.0f);
		}
	}
	//Vehicle Godmode
	ENTITY::SET_ENTITY_INVINCIBLE(veh, vehgodmode);
	ENTITY::SET_ENTITY_PROOFS(veh, vehgodmode, vehgodmode, vehgodmode, vehgodmode, vehgodmode, vehgodmode, vehgodmode, vehgodmode);
	VEHICLE::SET_VEHICLE_STRONG(veh, vehgodmode);
	VEHICLE::SET_VEHICLE_CAN_BREAK(veh, !vehgodmode);
	VEHICLE::SET_VEHICLE_ENGINE_CAN_DEGRADE(veh, !vehgodmode);
	VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(veh, !vehgodmode);
	VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(veh, !vehgodmode);
	VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, !vehgodmode);
	if (!NETWORK::NETWORK_IS_SESSION_STARTED())
		VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK_OFF_WHEN_BLOW_UP(veh, !vehgodmode);
	if (godmode && VEHICLE::_IS_VEHICLE_DAMAGED(veh))
		for (int i = 0; i < 10; i++)
			VEHICLE::SET_VEHICLE_TYRE_FIXED(veh, i);
	//Horn Boost
	if (hornboost)
	{
		if (CONTROLS::IS_CONTROL_PRESSED(2, INPUT_VEH_HORN))
		{
			if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
			{
				RequestControlOfEnt(veh);
				int vehspeed = ENTITY::GET_ENTITY_SPEED(veh);
				if (vehspeed < 150)
				{
					static int timer;
					if ((GetTickCount() - timer) > 400) {
						GRAPHICS::ANIMPOSTFX_PLAY("RaceTurbo", 0, 0);
						AUDIO::SET_VEHICLE_BOOST_ACTIVE(veh, true);
						VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, (vehspeed + hornboostspeed));
						AUDIO::SET_VEHICLE_BOOST_ACTIVE(veh, false);
						timer = GetTickCount();
					}
				}
			}
		}
	}
	//Flying Car
	if (vehiclefly)
	{
		if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
		{
			if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(veh))
				RequestControlOfEnt(veh);

			VEHICLE::SET_VEHICLE_GRAVITY(veh, false);
			pitch = ENTITY::GET_ENTITY_PITCH(veh);
			float curSpeed = ENTITY::GET_ENTITY_SPEED(veh);

			if (IsKeyPressed(VK_KEY_W))
			{
				ENTITY::SET_ENTITY_ROTATION(veh, pitch, roll, yaw, 2, true);
				if (curSpeed < vehicleflyspeed)
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, curSpeed += 1);
				else if (curSpeed < (vehicleflyspeed + 2.0f))
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, vehicleflyspeed);
				else if (curSpeed > (vehicleflyspeed + 2.0f))
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, curSpeed -= 1);
			}
			else
			{
				ENTITY::SET_ENTITY_ROTATION(veh, pitch, roll, yaw, 2, true);
				int curSpeedInt = curSpeed;
				if (curSpeedInt > 0.1f)
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, curSpeedInt -= 1);
				else
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, 0);
			}
			if (IsKeyPressed(VK_KEY_S))
			{
				ENTITY::SET_ENTITY_ROTATION(veh, pitch, roll, yaw, 2, true);
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, 0);
			}

			if (IsKeyPressed(VK_KEY_A)) ENTITY::SET_ENTITY_ROTATION(veh, pitch, roll, yaw += 1.5f, 2, true);

			if (IsKeyPressed(VK_KEY_D)) ENTITY::SET_ENTITY_ROTATION(veh, pitch, roll, yaw -= 1.5f, 2, true);

			if (IsKeyPressed(VK_SHIFT)) ENTITY::SET_ENTITY_ROTATION(veh, pitch -= 1.5f, roll, yaw, 2, true);

			if (IsKeyPressed(VK_CONTROL)) ENTITY::SET_ENTITY_ROTATION(veh, pitch += 1.5f, roll, yaw, 2, true);
		}
	}
	else
	{
		//VEHICLE::SET_VEHICLE_GRAVITY(veh, true);
		pitch = ENTITY::GET_ENTITY_PITCH(veh);
		yaw = ENTITY::GET_ENTITY_ROTATION(veh, 2).z;
	}
	//Autofix
	if (autofix)
		if (ENTITY::GET_ENTITY_SPEED(veh) < 50)
		{
			VEHICLE::SET_VEHICLE_FIXED(veh);
			VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(veh);
			VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh, 0);
		}
	if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
	{
		if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(veh, -1) == PLAYER::PLAYER_PED_ID())
		{

			//Power Multiplier
			int PowerMultiplier;
			if (powermultiplier <= 1)
				PowerMultiplier = 1;
			else
				PowerMultiplier = (powermultiplier - 1.0f) * 13;
			VEHICLE::_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, PowerMultiplier);
			//Torque Multiplier
			VEHICLE::_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, torquemultiplier);

			if (!VEHICLE::IS_THIS_MODEL_A_HELI(ENTITY::GET_ENTITY_MODEL(veh)))
			{
				if (VEHICLE::IS_THIS_MODEL_A_PLANE(ENTITY::GET_ENTITY_MODEL(veh)) || VEHICLE::IS_VEHICLE_ON_ALL_WHEELS(veh))
				{
					//Acceleration Multiplier
					if (CONTROLS::IS_CONTROL_PRESSED(2, INPUT_VEH_ACCELERATE))
					{
						ENTITY::APPLY_FORCE_TO_ENTITY(veh, 1, 0.0, (float)(((accelerationmultiplier - 1.0f) * 2.1f) / 69.0f), 0.0, 0.0, 0.0, 0.0, 0, 1, 1, 1, 0, 1);
					}
					//Brake Multiplier
					if (CONTROLS::IS_CONTROL_PRESSED(2, INPUT_VEH_BRAKE))
					{
						ENTITY::APPLY_FORCE_TO_ENTITY(veh, 0, 0.0, (float)(0 - ((brakemultiplier - 1.0f) * 2.1f)), 0.0, 0.0, 0.0, 0.0, 0, 1, 1, 1, 0, 1);
					}
				}
				//Downforce Multiplier
				if (!VEHICLE::IS_THIS_MODEL_A_PLANE(ENTITY::GET_ENTITY_MODEL(veh)) && !VEHICLE::IS_THIS_MODEL_A_BOAT(ENTITY::GET_ENTITY_MODEL(veh)))
				{
					if ((ENTITY::GET_ENTITY_ROLL(veh) < 75 && ENTITY::GET_ENTITY_ROLL(veh) > -75 && ENTITY::GET_ENTITY_PITCH(veh) < 75 && ENTITY::GET_ENTITY_PITCH(veh) > -75 && !ENTITY::IS_ENTITY_UPSIDEDOWN(veh))/* || VEHICLE::IS_VEHICLE_ON_ALL_WHEELS(veh)*/)
					{
						ENTITY::APPLY_FORCE_TO_ENTITY(veh, 0, 0.0, 0.0, (float)(0 - ((downforcemultiplier > 0 ? (downforcemultiplier - 1.0f) : downforcemultiplier) * 1.9f)), 0.0, 0.0, 0.0, 0, 1, 1, 1, 0, 1);
					}
				}
			}
		}
	}
	if (rainbowcolor)
	{
		if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
		{
			if (vehiclered > 0 && vehicleblue == 0)
			{
				vehiclered -= rgbmulti;
				vehiclegreen += rgbmulti;
			}
			if (vehiclegreen > 0 && vehiclered == 0)
			{
				vehiclegreen -= rgbmulti;
				vehicleblue += rgbmulti;
			}
			if (vehicleblue > 0 && vehiclegreen == 0)
			{
				vehiclered += rgbmulti;
				vehicleblue -= rgbmulti;
			}

			//Reset Colors
			if (vehiclered > 255.0f) vehiclered = 255;
			if (vehiclegreen > 255.0f) vehiclegreen = 255;
			if (vehicleblue > 255.0f) vehicleblue = 255;
			if (vehiclered < 0.0f) vehiclered = 0;
			if (vehiclegreen < 0.0f) vehiclegreen = 0;
			if (vehicleblue < 0.0f) vehicleblue = 0;

			VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, (int)vehiclered, (int)vehiclegreen, (int)vehicleblue);
			VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, (int)vehiclered, (int)vehiclegreen, (int)vehicleblue);
		}
	}
	if (rainbowtyresmoke)
	{
		if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
		{
			if (vehiclered2 > 0 && vehicleblue2 == 0)
			{
				vehiclered2 -= rgbmulti2;
				vehiclegreen2 += rgbmulti2;
			}
			if (vehiclegreen2 > 0 && vehiclered2 == 0)
			{
				vehiclegreen2 -= rgbmulti2;
				vehicleblue2 += rgbmulti2;
			}
			if (vehicleblue2 > 0 && vehiclegreen2 == 0)
			{
				vehiclered2 += rgbmulti2;
				vehicleblue2 -= rgbmulti2;
			}

			//Reset Colors
			if (vehiclered2 > 255.0f) vehiclered2 = 255;
			if (vehiclegreen2 > 255.0f) vehiclegreen2 = 255;
			if (vehicleblue2 > 255.0f) vehicleblue2 = 255;
			if (vehiclered2 < 0.0f) vehiclered2 = 0;
			if (vehiclegreen2 < 0.0f) vehiclegreen2 = 0;
			if (vehicleblue2 < 0.0f) vehicleblue2 = 0;

			if (!VEHICLE::IS_TOGGLE_MOD_ON(veh, 20))
				VEHICLE::TOGGLE_VEHICLE_MOD(veh, 20, true);

			VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(veh, (int)vehiclered2, (int)vehiclegreen2, (int)vehicleblue2);
		}
	}
	if (driveonwater && !vehiclefly && !noclip)
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(playerPed, 0);
		Vector3 pos = ENTITY::GET_ENTITY_COORDS(veh, 0);
		float height = 0;
		if (!VEHICLE::IS_THIS_MODEL_A_PLANE(ENTITY::GET_ENTITY_MODEL(veh)) && !VEHICLE::IS_THIS_MODEL_A_HELI(ENTITY::GET_ENTITY_MODEL(veh)) && !VEHICLE::IS_THIS_MODEL_A_BOAT(ENTITY::GET_ENTITY_MODEL(veh)))
		{
			if (WATER::GET_WATER_HEIGHT(pos.x, pos.y, pos.z, &height))
			{
				if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, false))
				{
					WATER::SET_DEEP_OCEAN_SCALER(0.0f);
					Object container = OBJECT::GET_CLOSEST_OBJECT_OF_TYPE(pos.x, pos.y, pos.z, 10.0, GAMEPLAY::GET_HASH_KEY("prop_container_ld2"), 0, 0, 1);
					if (ENTITY::DOES_ENTITY_EXIST(container) && height > -50.0f)
					{
						//RequestControlOfEnt(container);
						ENTITY::SET_ENTITY_COORDS(container, pos.x, pos.y, height - 2.55, 0, 0, 0, 1);
						ENTITY::SET_ENTITY_ROTATION(container, 0, 0, ENTITY::GET_ENTITY_ROTATION(veh, 0).z, 0, 1);
						Vector3 containerCoords = ENTITY::GET_ENTITY_COORDS(container, 1);
						if (pos.z < containerCoords.z) 
						{
							RequestControlOfEnt(veh);
							ENTITY::SET_ENTITY_COORDS(veh, pos.x, pos.y, containerCoords.z + 2.0f, 0, 0, 0, 1);
						}
					}
					else
					{
						if (GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(pos.x, pos.y, pos.z, pos.x, pos.y, height, true) < 5.0f)
						{
							Hash model = GAMEPLAY::GET_HASH_KEY("prop_container_ld2");
							STREAMING::REQUEST_MODEL(model);
							while (!STREAMING::HAS_MODEL_LOADED(model)) WAIT(1);
							container = OBJECT::CREATE_OBJECT(model, pos.x, pos.y, height - 2.55f, 1, 1, 0);
							//RequestControlOfEnt(container);
							ENTITY::FREEZE_ENTITY_POSITION(container, 1);
							ENTITY::SET_ENTITY_ALPHA(container, 0, 1);
							ENTITY::SET_ENTITY_VISIBLE(container, 0, 0);
						}
					}
				}
			}
			else 
			{
				Object container = OBJECT::GET_CLOSEST_OBJECT_OF_TYPE(pos.x, pos.y, pos.z, 10.0, GAMEPLAY::GET_HASH_KEY("prop_container_ld2"), 0, 0, 1);
				if (ENTITY::DOES_ENTITY_EXIST(container)) 
				{
					RequestControlOfEnt(container);
					ENTITY::SET_ENTITY_COORDS(container, 0, 0, -1000.0f, 0, 0, 0, 1);
					ENTITY::SET_ENTITY_COLLISION(container, false, true);
					ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&container);
					ENTITY::DELETE_ENTITY(&container);
					WATER::RESET_DEEP_OCEAN_SCALER();
				}
			}
		}
	}
	//Ramp Car
	if (rampvehicle)
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, false))
		{
			if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(veh, -1) == playerPed)
			{
				Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(playerPed, false);
				if (!VEHICLE::IS_THIS_MODEL_A_PLANE(ENTITY::GET_ENTITY_MODEL(veh)) && !VEHICLE::IS_THIS_MODEL_A_HELI(ENTITY::GET_ENTITY_MODEL(veh)) && !VEHICLE::IS_THIS_MODEL_A_BOAT(ENTITY::GET_ENTITY_MODEL(veh)))
				{
					Vector3 coords = ENTITY::GET_ENTITY_COORDS(veh, false);
					Object obj = OBJECT::GET_CLOSEST_OBJECT_OF_TYPE(coords.x, coords.y, coords.z, 10.0f, GAMEPLAY::GET_HASH_KEY("prop_mp_ramp_01"), 0, 0, 1);
					if (!ENTITY::DOES_ENTITY_EXIST(obj) && !ENTITY::IS_ENTITY_ATTACHED_TO_ENTITY(obj, veh))
					{
						STREAMING::REQUEST_MODEL(GAMEPLAY::GET_HASH_KEY("prop_mp_ramp_01"));
						while (!STREAMING::HAS_MODEL_LOADED(GAMEPLAY::GET_HASH_KEY("prop_mp_ramp_01"))) WAIT(1);
						Entity Ramp = OBJECT::CREATE_OBJECT(GAMEPLAY::GET_HASH_KEY("prop_mp_ramp_01"), coords.x, coords.y, coords.z, 1, 1, 0);
						if (ENTITY::DOES_ENTITY_EXIST(Ramp))
						{
							ENTITY::SET_ENTITY_COLLISION(Ramp, true, true);
							ENTITY::SET_ENTITY_ALPHA(Ramp, 100, false);
							ENTITY::ATTACH_ENTITY_TO_ENTITY(Ramp, veh, 0, 0, 6.3, -0.3, 13.5, 0, 177.5, 0, 0, true, 0, 2, 1);
						}
					}
				}
			}
		}
	}
	else
	{
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, false))
		{
			if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(veh, -1) == playerPed)
			{
				Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(playerPed, false);
				if (!VEHICLE::IS_THIS_MODEL_A_PLANE(ENTITY::GET_ENTITY_MODEL(veh)) && !VEHICLE::IS_THIS_MODEL_A_HELI(ENTITY::GET_ENTITY_MODEL(veh)) && !VEHICLE::IS_THIS_MODEL_A_BOAT(ENTITY::GET_ENTITY_MODEL(veh)))
				{
					Vector3 coords = ENTITY::GET_ENTITY_COORDS(veh, false);
					Object obj = OBJECT::GET_CLOSEST_OBJECT_OF_TYPE(coords.x, coords.y, coords.z, 10.0f, GAMEPLAY::GET_HASH_KEY("prop_mp_ramp_01"), 0, 0, 1);
					if (ENTITY::DOES_ENTITY_EXIST(obj) && ENTITY::IS_ENTITY_ATTACHED_TO_ENTITY(obj, veh))
					{
						RequestControlOfEnt(obj);
						ENTITY::DETACH_ENTITY(obj, 1, 1);
						ENTITY::SET_ENTITY_COLLISION(obj, 0, true);
						ENTITY::SET_ENTITY_ALPHA(obj, 0, true);
						ENTITY::SET_ENTITY_VISIBLE(obj, false, false);
						ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&obj);
						ENTITY::DELETE_ENTITY(&obj);
					}
				}
			}
		}
	}
}

bool explodeprot = true;
bool fireprot = true;
bool autodetach = true;
bool autoplayerdetach = true;
bool deleteclones = true;
bool autouncage = true;
bool timeprot = false;
bool weatherprot = false;
bool freezeprot = true;
bool controllprot = true;
bool ptfxprot = true;

void Protections()
{
	//NetEventProt
	if (NETWORK::NETWORK_IS_SESSION_STARTED())
	{
		Hooking::defuseEvent(FIRE_EVENT, fireprot);
		Hooking::defuseEvent(EXPLOSION_EVENT, explodeprot);
		Hooking::defuseEvent(GAME_WEATHER_EVENT, weatherprot);
		Hooking::defuseEvent(GAME_CLOCK_EVENT, timeprot);
		Hooking::defuseEvent(NETWORK_PTFX_EVENT, ptfxprot);
		Hooking::defuseEvent(REQUEST_CONTROL_EVENT, controllprot);
		Hooking::defuseEvent(NETWORK_CLEAR_PED_TASKS_EVENT, freezeprot);
		Hooking::defuseEvent(GIVE_PED_SCRIPTED_TASK_EVENT, freezeprot);
		Hooking::defuseEvent(GIVE_PED_SEQUENCE_TASK_EVENT, freezeprot);
		//Money Drop Self Report Prot
		//Hooking::defuseEvent(REPORT_CASH_SPAWN_EVENT, true);
		//Hooking::defuseEvent(REPORT_MYSELF_EVENT, true);
	}
	if (autodetach)
	{
		static int timer;
		if ((GetTickCount() - timer) > 600) {
			DetachObjectsFromPlayer(PLAYER::PLAYER_ID());
			timer = GetTickCount();
		}
	}
	if (deleteclones)
	{
		static int timer;
		if ((GetTickCount() - timer) > 200) {

			if (!CUTSCENE::IS_CUTSCENE_ACTIVE() && !CUTSCENE::IS_CUTSCENE_PLAYING() && !STREAMING::IS_NETWORK_LOADING_SCENE() && !NETWORK::NETWORK_IS_IN_MP_CUTSCENE() && !INTERIOR::IS_INTERIOR_SCENE())
			{
				Ped Closest;
				Vector3 MyPos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false);
				if (PED::GET_CLOSEST_PED(MyPos.x, MyPos.y, MyPos.z, 15.0f, true, true, &Closest, false, true, -1))
				{
					if (ENTITY::GET_ENTITY_MODEL(Closest) == ENTITY::GET_ENTITY_MODEL(PLAYER::PLAYER_PED_ID()))
					{
						if (!PED::IS_PED_A_PLAYER(Closest))
						{
							if (ENTITY::DOES_ENTITY_EXIST(Closest))
							{
								ENTITY::DETACH_ENTITY(Closest, 1, 1);
								ENTITY::SET_ENTITY_COLLISION(Closest, 0, true);
								ENTITY::SET_ENTITY_ALPHA(Closest, 0, true);
								ENTITY::SET_ENTITY_VISIBLE(Closest, false, false);
								RequestControlOfEnt(Closest);
								ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&Closest);
								ENTITY::SET_ENTITY_AS_MISSION_ENTITY(Closest, 1, 1);
								PED::DELETE_PED(&Closest);
								ENTITY::DELETE_ENTITY(&Closest);
							}
						}
					}
				}
			}
			timer = GetTickCount();
		}
	}
	if (autoplayerdetach)
	{
		static int timer;
		if ((GetTickCount() - timer) > 200) {

			if (!CUTSCENE::IS_CUTSCENE_ACTIVE() && !CUTSCENE::IS_CUTSCENE_PLAYING() && !STREAMING::IS_NETWORK_LOADING_SCENE() && !NETWORK::NETWORK_IS_IN_MP_CUTSCENE() && !INTERIOR::IS_INTERIOR_SCENE())
			{
				Ped Closest;
				Vector3 pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false);
				for (int i = 0; i < onlinePlayerCount(); i++)
				{
					if (PED::GET_CLOSEST_PED(pos.x, pos.y, pos.z, 15.0f, true, true, &Closest, false, true, -1))
					{
						if (ENTITY::GET_ENTITY_MODEL(Closest) == ENTITY::GET_ENTITY_MODEL(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i)))
						{
							if (ENTITY::DOES_ENTITY_EXIST(Closest))
							{
								if (PED::IS_PED_A_PLAYER(Closest))
								{
									if (ENTITY::IS_ENTITY_ATTACHED_TO_ENTITY(Closest, PLAYER::PLAYER_PED_ID()))
									{
										ENTITY::DETACH_ENTITY(Closest, 1, 1);
										//ENTITY::SET_ENTITY_COLLISION(Closest, 0, true);
										//ENTITY::SET_ENTITY_ALPHA(Closest, 0, true);
										//ENTITY::SET_ENTITY_VISIBLE(Closest, false, false);
										//RequestControlOfEnt(Closest);
										//ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&Closest);
										//ENTITY::SET_ENTITY_AS_MISSION_ENTITY(Closest, 1, 1);
										//PED::DELETE_PED(&Closest);
										//ENTITY::DELETE_ENTITY(&Closest);
									}
								}
							}
						}
					}
				}
			}
			timer = GetTickCount();
		}
	}
	if (autouncage)
	{
		static int timer;
		if ((GetTickCount() - timer) > 200) {

			Ped playerPed = PLAYER::PLAYER_PED_ID();
			Vector3 pos = ENTITY::GET_ENTITY_COORDS(playerPed, 0);
			char* cages[9] = { "p_cablecar_s_door_r", "p_cablecar_s_door_l", "p_cablecar_s_door_l" ,"p_cablecar_s", "prop_rub_cage01a", "prop_gold_cont_01", "prop_gold_cont_01b", "prop_dog_cage_01", "prop_dog_cage_02" };
			for (int i = 0; i < 9; i++)
			{
				Object cage = OBJECT::GET_CLOSEST_OBJECT_OF_TYPE(pos.x, pos.y, pos.z, 3.0f, GAMEPLAY::GET_HASH_KEY(cages[i]), 0, 0, 1);
				if (ENTITY::DOES_ENTITY_EXIST(cage))
				{
					if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
					{
						ENTITY::SET_ENTITY_COLLISION(cage, false, true);
						ENTITY::SET_ENTITY_ALPHA(cage, 0, true);
						ENTITY::SET_ENTITY_VISIBLE(cage, false, false);
						Vector3 objcoords = ENTITY::GET_ENTITY_COORDS(cage, false);
						STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(GAMEPLAY::GET_HASH_KEY(cages[i]));
						RequestControlOfEnt(cage);
						ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&cage);
						ENTITY::DELETE_ENTITY(&cage);
						GAMEPLAY::_CLEAR_AREA_OF_EVERYTHING(objcoords.x, objcoords.y, objcoords.z, 3.0f, 0, 0, 0, 0);
					}
				}
			}
			timer = GetTickCount();
		}
	}
}

bool autoteleport = false;

void TeleportOptions()
{
	//Auto Teleport
	if (autoteleport)
	{
		Vector3 Blip = GetBlipMarker();
		if (Blip.x != 0 && Blip.y != 0)
		{
			TeleportToMarker();
		}
	}

	//Teleport To Waypoint hotkey (F5)
	if (IsKeyPressed(VK_F5))
	{
		TeleportToMarker();
	}
}

bool freeze = false;
bool spectate = false;
bool pukick = false;
int pukickcount = 0;
bool playeresp = false;
bool pedesp = false;

void OnlineFunctions()
{
	Vector3 myCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);
	Vector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), 1);
	float distance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(myCoords.x, myCoords.y, myCoords.z, coords.x, coords.y, coords.z, true);
	bool inside = INTERIOR::IS_VALID_INTERIOR(INTERIOR::GET_INTERIOR_FROM_ENTITY(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer)));

	if (spectate)
	{
		if (NETWORK::NETWORK_IS_PLAYER_CONNECTED(selectedPlayer) && ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer)))
		{
			if (!inside)
			{
				if (distance > 20.0f)
				{
					NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(true, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
				}
				else
				{
					spectate = false;
					iconNotify("Info", "Spectate turned Off\nReason: To close To the Player");
				}
			}
			else
			{
				spectate = false;
				iconNotify("Info", "Spectate turned Off\nReason: Player is in a Building");
			}
		}
		else
		{
			spectate = false;
			iconNotify("Info", "Spectate turned Off\nReason: Player Not Found");
		}
	}
	else
	{
		NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(false, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
	}
	if (freeze)
	{
		if (ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer)))
		{
			AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
			AI::CLEAR_PED_SECONDARY_TASK(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
			AI::CLEAR_PED_TASKS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
		}
	}
	if (pukick)
	{
		if (NETWORK::NETWORK_IS_PLAYER_CONNECTED(selectedPlayer) && ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer)) && distance > 4.00f && pukickcount < 400)
		{

			AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
			AI::CLEAR_PED_SECONDARY_TASK(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
			AI::CLEAR_PED_TASKS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));

			static int timer;
			if ((GetTickCount() - timer) > 5)
			{
				pukickcount++;

				CreateDrop(0xC3CD8B31, GAMEPLAY::GET_HASH_KEY("vw_prop_vw_colle_alien"), ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false), 0.3f, 0, 2500);
				timer = GetTickCount();
			}
			static int timer2;
			if ((GetTickCount() - timer2) > 50)
			{
				Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false);
				GAMEPLAY::_CLEAR_AREA_OF_EVERYTHING(playerPos.x, playerPos.y, playerPos.z, 7.00f, 0, 0, 0, 0);
				timer2 = GetTickCount();
			}
		}
		else
		{
			Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false);
			GAMEPLAY::_CLEAR_AREA_OF_EVERYTHING(playerPos.x, playerPos.y, playerPos.z, 15.00f, 0, 0, 0, 0);
			pukick = false;
		}
	}

	if (playeresp)
	{
		for (int i = 0; i < 32; i++)
		{
			if (i != PLAYER::PLAYER_ID())
			{
				Ped playerPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i);
				if (ENTITY::DOES_ENTITY_EXIST(playerPed))
				{
					RGB color;
					if (ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(PLAYER::PLAYER_PED_ID(), playerPed, 17)) {
						color.r = 0; color.g = 255;
					}
					else {
						color.r = 255; color.g = 0;
					}

					Vector3 Coords = ENTITY::GET_ENTITY_COORDS(playerPed, true);
					Vector3 CamCoords = CAM::GET_GAMEPLAY_CAM_COORD();
					Vector3 HeadCoords = PED::GET_PED_BONE_COORDS(playerPed, SKEL_Head, 0.0f, 0.0f, -0.1f);
					Vector3 FootCoords = PED::GET_PED_BONE_COORDS(playerPed, SKEL_ROOT, 0.0f, 0.0f, -1.6f);

					float distanceToCam = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(CamCoords.x, CamCoords.y, CamCoords.z, Coords.x, Coords.y, Coords.z, false);

					if (distanceToCam > 0.0f)
					{
						float HeadX, HeadY;
						GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(HeadCoords.x, HeadCoords.y, HeadCoords.z, &HeadX, &HeadY);

						float FootX, FootY;
						GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(FootCoords.x, FootCoords.y, FootCoords.z, &FootX, &FootY);

						float ScreenX, ScreenY; //Screen Coords of the Player
						if (GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(Coords.x, Coords.y, Coords.z, &ScreenX, &ScreenY))
						{
							//If Player is visible on screen
							float Size = HeadY - FootY;
							if (Size <= 1.0f && Size >= -1.0f)
							{
								//Menu::Drawing::Rect({ color.r, color.g, 0, 255 }, { ScreenX, ScreenY }, { Size / 3.0f, Size });
								Menu::Drawing::Assets();
								Menu::Drawing::Spriter("Assets", "ESP_BOX", ScreenX, ScreenY, Size / 3.0f, Size, 0.0f, color.r, color.g, 0, 255);
							}
						}
					}
				}
			}
		}
	}

	if (pedesp)
	{
		int pedCount = 150;
		int ArrSize = pedCount * 2 + 2;

		Ped* peds = new Ped[ArrSize];
		peds[0] = pedCount;

		int PedFound = PED::GET_PED_NEARBY_PEDS(PLAYER::PLAYER_PED_ID(), peds, -1);

		for (int i = 0; i < PedFound; i++)
		{
			int OffsetID = i * 2 + 2;
			Ped espPed = peds[OffsetID];

			if (ENTITY::DOES_ENTITY_EXIST(espPed) && !PED::IS_PED_A_PLAYER(espPed))
			{
				RGB color;
				if (ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(PLAYER::PLAYER_PED_ID(), espPed, 17)) {
					color.r = 0; color.g = 255;
				}
				else {
					color.r = 255; color.g = 0;
				}

				Vector3 Coords = ENTITY::GET_ENTITY_COORDS(espPed, true);
				Vector3 CamCoords = CAM::GET_GAMEPLAY_CAM_COORD();
				Vector3 HeadCoords = PED::GET_PED_BONE_COORDS(espPed, SKEL_Head, 0.0f, 0.0f, -0.1f);
				Vector3 FootCoords = PED::GET_PED_BONE_COORDS(espPed, SKEL_ROOT, 0.0f, 0.0f, -1.6f);

				float distanceToCam = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(CamCoords.x, CamCoords.y, CamCoords.z, Coords.x, Coords.y, Coords.z, false);

				if (distanceToCam > 0.0f)
				{
					float HeadX, HeadY;
					GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(HeadCoords.x, HeadCoords.y, HeadCoords.z, &HeadX, &HeadY);

					float FootX, FootY;
					GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(FootCoords.x, FootCoords.y, FootCoords.z, &FootX, &FootY);

					float ScreenX, ScreenY; //Screen Coords of the Player
					if (GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(Coords.x, Coords.y, Coords.z, &ScreenX, &ScreenY))
					{
						//If Player is visible on screen
						float Size = HeadY - FootY;
						if (Size <= 1.0f && Size >= -1.0f)
						{
							//Menu::Drawing::Rect({ color.r, color.g, 0, 255 }, { ScreenX, ScreenY }, { Size / 3.0f, Size });
							Menu::Drawing::Assets();
							Menu::Drawing::Spriter("Assets", "ESP_BOX", ScreenX, ScreenY, Size / 3.0f, Size, 0.0f, color.r, color.g, 0, 255);
						}
					}
				}
			}
		}
	}
}

float dropspread = 0;
float dropheight = 1.5f;
int dropdelay = 500;
int dropamount = 2500;
bool moneydrop = false;
bool rpdrop = false;
bool medidrop = false;
bool armourdrop = false;
bool ammodrop = false;

void CreateDrop(Hash pickupHash, Hash modelHash, Vector3 droppos, float drophight, float dropspread, int value)
{
	if (modelHash != 0)
	{
		STREAMING::REQUEST_MODEL(modelHash);
		while (!STREAMING::HAS_MODEL_LOADED(modelHash)) WAIT(1);
	}
	float dropspreadx = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-dropspread), dropspread);
	float dropspready = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-dropspread), dropspread);
	OBJECT::CREATE_AMBIENT_PICKUP(pickupHash, droppos.x + dropspreadx, droppos.y + dropspready, droppos.z += drophight, 0, value, modelHash, false, false);
	if (modelHash != 0)
	{
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
	}
}

void RecoveryOptions()
{
	if (moneydrop)
	{
		static int timer;
		if ((GetTickCount() - timer) > dropdelay)
		{
			CreateDrop(0xDE78F17E, 0x9CA6F755, ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false), dropheight, dropspread, dropamount); // 2.5k
			timer = GetTickCount();
		}
	}
	if (rpdrop)
	{
		static int timer;
		if ((GetTickCount() - timer) > dropdelay)
		{
			CreateDrop(0xC3CD8B31, GAMEPLAY::GET_HASH_KEY("vw_prop_vw_colle_alien"), ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false), dropheight, dropspread);
			timer = GetTickCount();
		}
	}
	if (medidrop)
	{
		static int timer;
		if ((GetTickCount() - timer) > dropdelay)
		{
			CreateDrop(GAMEPLAY::GET_HASH_KEY("PICKUP_HEALTH_STANDARD"), NULL, ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false), dropheight, dropspread);
			timer = GetTickCount();
		}
	}
	if (armourdrop)
	{
		static int timer;
		if ((GetTickCount() - timer) > dropdelay)
		{
			CreateDrop(GAMEPLAY::GET_HASH_KEY("PICKUP_ARMOUR_STANDARD"), NULL, ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false), dropheight, dropspread);
			timer = GetTickCount();
		}
	}
	if (ammodrop)
	{
		static int timer;
		if ((GetTickCount() - timer) > dropdelay)
		{
			CreateDrop(GAMEPLAY::GET_HASH_KEY("PICKUP_AMMO_BULLET_MP"), NULL, ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false), dropheight, dropspread);
			CreateDrop(GAMEPLAY::GET_HASH_KEY("PICKUP_AMMO_MISSILE_MP"), NULL, ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false), dropheight, dropspread);
			timer = GetTickCount();
		}
	}
}

float spawnspread = 1.5f;
int bodyguardhealth = 300;
int bodyguardaccuracy = 90;
bool bodyguardinvincible = false;
bool bodyguardnoragdoll = false;

void MiscOptions()
{

}

bool hourchange = false;
int worldhours = 0;
bool minutechange = false;
int worldminutes = 0;
bool secondchange = false;
int worldseconds = 0;
bool wavechange = false;
float waveintensity = 0.0f;
bool windchange = false;
float windspeed = 0.0f;
int gravitylevel = 0;
bool pauseclock = false;
bool timelaps = false;
int timelapsscale = 10;

void WorldOptions()
{
	if (!hourchange && Menu::Settings::currentMenu == worldtime)
		worldhours = TIME::GET_CLOCK_HOURS();
	if (!minutechange && Menu::Settings::currentMenu == worldtime)
		worldminutes = TIME::GET_CLOCK_MINUTES();
	if (!secondchange && Menu::Settings::currentMenu == worldtime)
		worldseconds = TIME::GET_CLOCK_SECONDS();
	if (!wavechange && Menu::Settings::currentMenu == world)
		waveintensity = WATER::GET_DEEP_OCEAN_SCALER();
	if (!windchange && Menu::Settings::currentMenu == world)
		windspeed = GAMEPLAY::GET_WIND_SPEED();

	TIME::PAUSE_CLOCK(pauseclock);
	//Timelaps
	if (timelaps)
	{
		TIME::PAUSE_CLOCK(true);
		TIME::ADD_TO_CLOCK_TIME(0, 0, timelapsscale);
	}

	GAMEPLAY::SET_GRAVITY_LEVEL(gravitylevel);
}

void FeaturesMain()
{
	SelfOptions();
	WeaponOptions();
	VehicleOptions();
	Protections();
	TeleportOptions();
	OnlineFunctions();
	RecoveryOptions();
	MiscOptions();
	WorldOptions();
}
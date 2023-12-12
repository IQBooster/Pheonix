#pragma once
#include "stdafx.h"
#include "Features.h"
#include "Memory.h" 
#include "Lists.h"
#include "Structs.h"
#include <experimental/filesystem>

namespace filesystem = std::experimental::filesystem;

int menuX = 20;
int menuY = 1;
int selectedPlayer;
bool spawnerlistmode = true;

//HUD

int FPS;
int AvgFPS;
float frametime;
uint64_t count = 0;
uint64_t framecount = 0;

void MenuHUD()
{
    time_t timestandard;
    time(&timestandard);

    struct tm* gettime;
    gettime = localtime(&timestandard);

    char timefinal[80];
    strftime(timefinal, sizeof(timefinal), "%H:%M:%S", gettime);

    static int timer;
    if ((GetTickCount() - timer) > 300) {
        frametime = GAMEPLAY::GET_FRAME_TIME();
        FPS = (1.0f / frametime);
        count++;
        framecount += FPS;
        AvgFPS = (framecount / count);
        timer = GetTickCount();
    }
    std::ostringstream AvgFPSDraw; AvgFPSDraw << "AVG FPS: " << AvgFPS;
    std::ostringstream FPSDraw; FPSDraw << "FPS: " << FPS;
    //Clock 
    Menu::Drawing::Text(timefinal, { 255, 255, 255, 255, 4 }, { 0.500f, 0.004f }, { 0.50f, 0.50f }, true, true);
    //Avg FPS
    Menu::Drawing::Text((char*)AvgFPSDraw.str().c_str(), { 255, 255, 255, 255, 4 }, { 0.007f , 0.004f }, { 0.50f, 0.50f }, false, true);
    //FPS
    Menu::Drawing::Text((char*)FPSDraw.str().c_str(), { 255, 255, 255, 255, 4 }, { 0.007f , 0.029f }, { 0.50f, 0.50f }, false, true);

    if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
    {
        float vehSpeed = ENTITY::GET_ENTITY_SPEED(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false));
        std::ostringstream SpeedoDraw; SpeedoDraw << "SPEED: " << round(vehSpeed * 3.6f) << " ~s~KMH";
        //Speedo
        Menu::Drawing::Text((char*)SpeedoDraw.str().c_str(), { 255, 255, 255, 255, 4 }, { 0.500f, 0.952f }, { 0.63f, 0.63f }, true, true);
    }

    ////Mouse 
    //VECTOR2 cursor = Cursor::cursorPos();
    //std::ostringstream Mouse; Mouse << "X: " << cursor.x << " Y: " << cursor.y;
    //Menu::Drawing::Text((char*)Mouse.str().c_str(), { 255, 255, 255, 255, 4 }, { 0.007f , 0.054f }, { 0.50f, 0.50f }, false, true);
    ////Checks Probs
    //bool onheader = Cursor::onHeader();
    //std::ostringstream OnHeader; OnHeader << "On Header: " << (onheader ? "true" : "false");
    //Menu::Drawing::Text((char*)OnHeader.str().c_str(), { 255, 255, 255, 255, 4 }, { 0.007f , 0.079f }, { 0.50f, 0.50f }, false, true);
    //bool onmenu = Cursor::onMenu();
    //std::ostringstream OnMenu; OnMenu << "On Menu: " << (onmenu ? "true" : "false");
    //Menu::Drawing::Text((char*)OnMenu.str().c_str(), { 255, 255, 255, 255, 4 }, { 0.007f , 0.104f }, { 0.50f, 0.50f }, false, true);
    //bool onoption = Cursor::onOption();
    //std::ostringstream OnOption; OnOption << "On Option: " << (onoption ? "true" : "false");
    //Menu::Drawing::Text((char*)OnOption.str().c_str(), { 255, 255, 255, 255, 4 }, { 0.007f , 0.129f }, { 0.50f, 0.50f }, false, true);
    //int curoption = Cursor::currentOption();
    //std::ostringstream CurOption; CurOption << "Cur Option: " << curoption;
    //Menu::Drawing::Text((char*)CurOption.str().c_str(), { 255, 255, 255, 255, 4 }, { 0.007f , 0.154f }, { 0.50f, 0.50f }, false, true);
}

void TPto(Vector3 Coords)
{
    int Handle = PLAYER::PLAYER_PED_ID();
    if (PED::IS_PED_IN_ANY_VEHICLE(Handle, 0))
    {
        RequestControlOfEnt(PED::GET_VEHICLE_PED_IS_IN(Handle, false));
        ENTITY::SET_ENTITY_COORDS(PED::GET_VEHICLE_PED_IS_IN(Handle, false), Coords.x, Coords.y, Coords.z, 0, 0, 0, 1);
    }
    else
        ENTITY::SET_ENTITY_COORDS(Handle, Coords.x, Coords.y, Coords.z, 0, 0, 0, 1);
}

//MAIN

void main() {
    system("start https://discord.gg/YFCzZj59G9"); //Anim Discord Link

    std::ostringstream WelcomeName; WelcomeName << "Welcome: " << PLAYER::GET_PLAYER_NAME(PLAYER::PLAYER_ID());
    iconNotify(NULL, (char*)WelcomeName.str().c_str());

    for (int AssetTrys = 0; AssetTrys <= 1000; AssetTrys++)
    {
        //if (AssetTrys == 0)
        //    notifyMap("Trying to Load Menu Assets");

        filesystem::path m_FilePath;

        try
        {
            m_FilePath.append(DocumentsPath());
            if (!filesystem::exists(m_FilePath))
                filesystem::create_directory(m_FilePath);
            m_FilePath.append("Anim.ytd");
        }
        catch (filesystem::filesystem_error const&)
        {

        }

        if (FileExists(m_FilePath.u8string().c_str()))
        {
            if (!Menu::Settings::assetsLoaded)
            {
                Menu::Settings::assetsLoaded = true;
                notifyMap("~g~Succeded Loading Assets!");
            }
        }

        if (AssetTrys == 1000 && !Menu::Settings::assetsLoaded)
            notifyMap("~r~Failed Loading Assets.ytd, check Anim Folder! Loading Emergency Assets");
    }

	while (true) {
        MenuHUD();
        FeaturesMain();
        Menu::Scroller(); //Drawn in the Last Frame
        Menu::Checks::Controlls();

		switch (Menu::Settings::currentMenu) {

		case mainmenu:
		{
			Menu::Title(MENU_NAME);
            Menu::SubTitle("MAIN");
            Menu::MenuOption("Self", selfmenu);
            Menu::MenuOption("Weapon", weaponmenu);
            Menu::MenuOption("Player list", playerlist);
            Menu::MenuOption("Vehicle Spawner", vehiclespawner);
            Menu::MenuOption("Vehicle Options", vehicleoptions);
            Menu::MenuOption("Protections", protections);
            Menu::MenuOption("Teleports", teleports);
            Menu::MenuOption("Recovery", recovery);
            Menu::MenuOption("World", world);
            Menu::MenuOption("Settings", settings);
            //Menu::MenuOption("Test", test);
		}
		break;
		case selfmenu:
		{
            Menu::Title(MENU_NAME);
            Menu::SubTitle("SELF");
            Menu::MenuOption("Common", commonmenu);
            Menu::MenuOption("Health", healthmenu);
            Menu::MenuOption("Wanted", wantedmenu);
			Menu::MenuOption("Animations", animmenu);
			Menu::MenuOption("Outfits", outfitsmenu);
            Menu::MenuOption("Visuals", visualsmenu);
            Menu::MenuOption("Movement", movementmenu);
			Menu::Toggle("Off Radar [Fake Death~s~]", offradar);
			Menu::Toggle("Off Radar [Lester~s~]", offradar1);
			Menu::Toggle("Bullshark [Instand~s~]", bullshark);
			Menu::Toggle("Bypass Chat", bypasschat);
			if (Menu::Toggle("Badsport", badsport))
			{
				STATS::STAT_SET_FLOAT(GAMEPLAY::GET_HASH_KEY("MPPLY_OVERALL_BADSPORT"), badsport ? 1000 : 0, true);
				STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MPPLY_DESTROYED_PVEHICLES"), badsport ? 1000 : 0, true);
				STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MPPLY_BADSPORT_MESSAGE"), badsport ? 1000 : 0, true);
				if (!badsport) iconNotify("Stats", "Badsport Removed!\nPls Change Session!");
			}
			Menu::Toggle("Anti AFK", antiafk);
			Menu::Break("Merryweather");
			if (Menu::Option("Ammo Drop"))
			{
				globalHandle(2540384).At(871).As<int>() = 833;
			}
			if (Menu::Option("Backup Heli"))
			{
				globalHandle(2540384).At(4450).As<int>() = 4389;
			}
			if (Menu::Option("Airstrike"))
			{
				globalHandle(2540384).At(4451).As<int>() = 4390;
			}
			Menu::Toggle("Forcefield [Explosive~s~]", forcefield);
			Menu::Break("Bodyguards & Enemys");
			Menu::Float("Spawn Spread", spawnspread, 0.0f, 10.0f);
			Menu::Int("Aim Accuracy", bodyguardaccuracy, 0, 100);
			Menu::Int("Ped Health", bodyguardhealth, 100, 5000, 50);
			Menu::Toggle("Ped Godmode", bodyguardinvincible);
			Menu::Toggle("Ped No Ragdoll", bodyguardnoragdoll);
			Menu::StringVector("Ped Model", BodyGuards, BodyGuardsPos);
			Menu::StringVector("Ped Weapon", BodyGuardsWeapon, BodyGuardsWeaponPos);
			if (Menu::Option("Spawn Bodyguard"))
			{
				Vector3 spawnCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
				spawnCoords.x += GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-spawnspread), spawnspread);
				spawnCoords.y += GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-spawnspread), spawnspread);
				Ped guardPed = SpawnPed(26, GAMEPLAY::GET_HASH_KEY((char*)BodyGuardsHash[BodyGuardsPos].c_str()), spawnCoords);
				PED::SET_PED_MAX_HEALTH(guardPed, bodyguardhealth);
				ENTITY::SET_ENTITY_HEALTH(guardPed, bodyguardhealth);
				ENTITY::SET_ENTITY_INVINCIBLE(guardPed, bodyguardinvincible);
				PED::SET_PED_CAN_RAGDOLL(guardPed, !bodyguardnoragdoll);
				PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(guardPed, !bodyguardnoragdoll);
				StartPedGuardTask(PLAYER::PLAYER_ID(), guardPed, GAMEPLAY::GET_HASH_KEY((char*)BodyGuardsWeaponHash[BodyGuardsWeaponPos].c_str()), bodyguardaccuracy);
			}
			if (Menu::Option("Spawn Enemy"))
			{
				Vector3 spawnCoords = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
				spawnCoords.x += GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-spawnspread), spawnspread);
				spawnCoords.y += GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-spawnspread), spawnspread);
				Ped enemyPed = SpawnPed(26, GAMEPLAY::GET_HASH_KEY((char*)BodyGuardsHash[BodyGuardsPos].c_str()), spawnCoords);
				PED::SET_PED_MAX_HEALTH(enemyPed, bodyguardhealth);
				ENTITY::SET_ENTITY_HEALTH(enemyPed, bodyguardhealth);
				ENTITY::SET_ENTITY_INVINCIBLE(enemyPed, bodyguardinvincible);
				PED::SET_PED_CAN_RAGDOLL(enemyPed, !bodyguardnoragdoll);
				PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(enemyPed, !bodyguardnoragdoll);
				StartPedEnemyTask(PLAYER::PLAYER_ID(), enemyPed, GAMEPLAY::GET_HASH_KEY((char*)BodyGuardsWeaponHash[BodyGuardsWeaponPos].c_str()), bodyguardaccuracy);
			}
		}
		break;
        case test:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("TEST");
            std::uint64_t Kicks[] = { -442306200, 1352706024, -1252906024, -1890951223, -1559754940, -1243454584, -891346918 };
            for (int i = 0; i < (sizeof(Kicks) / sizeof(Kicks[0])); i++)
            {
                std::ostringstream Text; Text << "Kick " << i;
                if (Menu::Option((char*)Text.str().c_str()))
                {
                    std::uint64_t args[2] = { Kicks[i], PLAYER::PLAYER_ID() };
                    Hooking::trigger_script_event(1, args, 2, 1 << PLAYER::PLAYER_ID());
                }
            }
        }
        break;
      
        case commonmenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("COMMON");
            Menu::Toggle("Godmode", godmode);
            if (Menu::Toggle("Never Wanted", neverwanted)) { if (neverwanted) { alwayswanted = false; visualwantedlevelbool = false; wantedlevelbool = false; } }
            Menu::Toggle("No Ragdoll", noragdoll);
            Menu::Float("Fast Run", fastrunspeed, 1.0f, 10.0f);
            Menu::Toggle("Super Jump", superjump);
        }
        break;
        case healthmenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("HEALTH");
            Menu::Toggle("Godmode", godmode);
            Menu::ToggleInt("Auto Heal", autoheal, autohealint, 1, 15);
            Menu::ToggleInt("Heal in Cover", healincover, healincoverint, 1, 15);
            if (Menu::Int("Set Health", sethealth, 100, ENTITY::GET_ENTITY_MAX_HEALTH(PLAYER::PLAYER_PED_ID())))
            {
                 if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                 {
                     healthchange = true;
                     ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), sethealth);
                     healthchange = false;
                 }
            }
            if (Menu::Int("Set Armour", setarmour, 0, PLAYER::GET_PLAYER_MAX_ARMOUR(PLAYER::PLAYER_ID())))
            {
                if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                {
                    armourchange = true;
                    PED::SET_PED_ARMOUR(PLAYER::PLAYER_PED_ID(), setarmour);
                    armourchange = false;
                }
            }
            if (Menu::Option("Max Health"))
            {
                ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), ENTITY::GET_ENTITY_MAX_HEALTH(PLAYER::PLAYER_PED_ID()));
            }
            if (Menu::Option("Max Armour"))
            {
                PED::SET_PED_ARMOUR(PLAYER::PLAYER_PED_ID(), PLAYER::GET_PLAYER_MAX_ARMOUR(PLAYER::PLAYER_ID()));
            }
            if (Menu::Option("Suicide"))
            {
                ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), 0);
            }
        }
        break;
        case wantedmenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("WANTED");
            if (Menu::Option("Clear Wanted Level"))
            {
                alwayswanted = false;
                wantedlevelbool = false;
                visualwantedlevelbool = false;
                for (int i = 1; i < 10; i++)
                {
                    PLAYER::CLEAR_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID());
                    GAMEPLAY::SET_FAKE_WANTED_LEVEL(0);
                }
            }
            if (Menu::Option("Six Star Wanted Level"))
            {
                neverwanted = false;
                visualwantedlevel = 6;
                GAMEPLAY::SET_FAKE_WANTED_LEVEL(6);
            }
            if (Menu::ToggleInt("Fake Wanted Level", visualwantedlevelbool, visualwantedlevel, 0, 6, true)) { if (visualwantedlevelbool) { neverwanted = false; alwayswanted = false; } }
            if (Menu::ToggleInt("Wanted Level", wantedlevelbool, wantedlevel, 0, 5, true)) { if (wantedlevelbool) { neverwanted = false; alwayswanted = false; } }
            if (Menu::Toggle("Never Wanted", neverwanted)) { if (neverwanted) { alwayswanted = false; visualwantedlevelbool = false; wantedlevelbool = false; } }
            if (Menu::Toggle("Always Wanted", alwayswanted)) { if (alwayswanted) { neverwanted = false; visualwantedlevelbool = false; wantedlevelbool = false; } else Memory::set_value<int>({ 0x8, 0x10C8, 0x868 }, 0); }
        }
        break;
        case visualsmenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("VISUALS");
            if (Menu::Toggle("Invisible", invisible)) { if (invisible) playeralpha = 0; else ENTITY::SET_ENTITY_ALPHA(PLAYER::PLAYER_PED_ID(), 255, false); }
            if (Menu::Int("Player Alpha [Local~s~]", playeralpha, 0, 255, 51))
            {
                if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                {
                    alphachange = true;
                    ENTITY::SET_ENTITY_ALPHA(PLAYER::PLAYER_PED_ID(), playeralpha, false);
                    alphachange = false;
                }
            }
            if (Menu::Toggle("Slow Motion", slowmotion)) { if (slowmotion) { timescale = 0.3f; } else { timescale = 1.0f; } }
            Menu::Float("Time Scale", timescale, 0.1f, 1.0f);
            Menu::Toggle("Auto Clean", autoclean);
            Menu::Toggle("Vomit", vomit);
            if (Menu::Toggle("Fire Breath", firebreath)) { ENTITY::SET_ENTITY_PROOFS(PLAYER::PLAYER_PED_ID(), false, firebreath, false, false, false, false, false, false); }
            Menu::Toggle("Tiny Player", tinyplayer);
            Menu::Toggle("Player ESP", playeresp);
            Menu::Toggle("Ped ESP", pedesp);
        }
        break;
        case movementmenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("MOVEMENT");
            Menu::Toggle("No Ragdoll", noragdoll);
            Menu::Toggle("Seatbelt", seatbelt);
            Menu::Float("Fast Run", fastrunspeed, 0.5f, 10.0f);
            Menu::Float("Fast Swim", fastswimspeed, 0.5f, 10.0f);
            Menu::ToggleFloat("Slide Run", sliderun, sliderunspeed, 0.5f, 2.5f);
            Menu::ToggleFloat("Super Run", superrun, superrunspeed, 1.5f, 7.5f);
            Menu::Toggle("Super Jump", superjump);
            Menu::Toggle("Superman", superman);
            if (Menu::ToggleFloat("Mouse-Clip", noclip, noclipspeed, 0.5f, 10.0f)) if (noclip) walkingnoclip = false;
            if (Menu::Toggle("Walking-Clip", walkingnoclip)) if (walkingnoclip) noclip = false;
            if (Menu::Toggle("Walk On Water", walkonwater)) if (walkonwater) walkunderwater = false;
            if (Menu::Toggle("Walk Under Water", walkunderwater)) if (walkunderwater) walkonwater = false;
        }
        break;
        case onlinemenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("ONLINE");
            Menu::Toggle("Off Radar [Fake Death~s~]", offradar);
            Menu::Toggle("Off Radar [Lester~s~]", offradar1);
            Menu::Toggle("Bullshark [Instand~s~]", bullshark);
            Menu::Toggle("Bypass Chat", bypasschat);
            if (Menu::Toggle("Badsport", badsport)) 
            {
                STATS::STAT_SET_FLOAT(GAMEPLAY::GET_HASH_KEY("MPPLY_OVERALL_BADSPORT"), badsport ? 1000 : 0, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MPPLY_DESTROYED_PVEHICLES"), badsport ? 1000 : 0, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MPPLY_BADSPORT_MESSAGE"), badsport ? 1000 : 0, true);
                if (!badsport) iconNotify("Stats", "Badsport Removed!\nPls Change Session!");
            }
            Menu::Toggle("Anti AFK", antiafk);    
            Menu::Break("Merryweather");
            if (Menu::Option("Ammo Drop"))
            {
                globalHandle(2540384).At(871).As<int>() = 833;
            }
            //if (Menu::Option("Boat Taxi"))
            //{
            //    globalHandle(2540384).At(834).As<int>() = 1;
            //}
            //if (Menu::Option("Heli Taxi"))
            //{
            //    globalHandle(2540384).At(835).As<int>() = 1;
            //}
            if (Menu::Option("Backup Heli"))
            {
                globalHandle(2540384).At(4450).As<int>() = 4389;
            }
            if (Menu::Option("Airstrike"))
            {
                globalHandle(2540384).At(4451).As<int>() = 4390;
            }
        }
        break;
        case animmenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("ANIMATIONS");

            PED::SET_PED_CAN_PLAY_AMBIENT_ANIMS(PLAYER::PLAYER_PED_ID(), true);
            PED::SET_PED_CAN_PLAY_AMBIENT_BASE_ANIMS(PLAYER::PLAYER_PED_ID(), true);
            PED::SET_PED_CAN_PLAY_GESTURE_ANIMS(PLAYER::PLAYER_PED_ID(), true);
            PED::SET_PED_CAN_PLAY_VISEME_ANIMS(PLAYER::PLAYER_PED_ID(), true, true);

            if (Menu::Option("Clear Animation")) AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::PLAYER_PED_ID()); 
            Menu::Break("Animations");
            if (Menu::Option("Sex Receiver")) DoAnimation(PLAYER::PLAYER_ID(), "rcmpaparazzo_2", "shag_loop_poppy"); 
            if (Menu::Option("Sex Giver")) DoAnimation(PLAYER::PLAYER_ID(), "rcmpaparazzo_2", "shag_loop_a"); 
            if (Menu::Option("Private Dance")) DoAnimation(PLAYER::PLAYER_ID(), "mini@strip_club@private_dance@part1", "priv_dance_p1"); 
            if (Menu::Option("Pole Dance")) DoAnimation(PLAYER::PLAYER_ID(), "mini@strip_club@pole_dance@pole_dance1", "pd_dance_01"); 
            if (Menu::Option("Push Ups")) DoAnimation(PLAYER::PLAYER_ID(), "amb@world_human_push_ups@male@base", "base"); 
            if (Menu::Option("Sit Ups")) DoAnimation(PLAYER::PLAYER_ID(), "amb@world_human_sit_ups@male@base", "base"); 
            if (Menu::Option("Celebrate")) DoAnimation(PLAYER::PLAYER_ID(), "rcmfanatic1celebrate", "celebrate"); 
            if (Menu::Option("Electrocution")) DoAnimation(PLAYER::PLAYER_ID(), "ragdoll@human", "electrocute"); 
            if (Menu::Option("Suicide")) DoAnimation(PLAYER::PLAYER_ID(), "mp_suicide", "pistol"); 
            if (Menu::Option("Showering")) DoAnimation(PLAYER::PLAYER_ID(), "mp_safehouseshower@male@", "male_shower_idle_b"); 
            if (Menu::Option("Hood Dance")) DoAnimation(PLAYER::PLAYER_ID(), "missfbi3_sniping", "dance_m_default");
            if (Menu::Option("Burning")) DoAnimation(PLAYER::PLAYER_ID(), "ragdoll@human", "on_fire");
            if (Menu::Option("Wave Yo' Arms")) DoAnimation(PLAYER::PLAYER_ID(), "random@car_thief@victimpoints_ig_3", "arms_waving");
            if (Menu::Option("Give BJ to Driver")) DoAnimation(PLAYER::PLAYER_ID(), "mini@prostitutes@sexnorm_veh", "bj_loop_prostitute");
            if (Menu::Option("Pleasure Driver")) DoAnimation(PLAYER::PLAYER_ID(), "mini@prostitutes@sexnorm_veh", "sex_loop_prostitute");
            if (Menu::Option("Mime")) DoAnimation(PLAYER::PLAYER_ID(), "special_ped@mime@monologue_8@monologue_8a", "08_ig_1_wall_ba_0");
            if (Menu::Option("Mime 2")) DoAnimation(PLAYER::PLAYER_ID(), "special_ped@mime@monologue_7@monologue_7a", "11_ig_1_run_aw_0");
            if (Menu::Option("Throw")) DoAnimation(PLAYER::PLAYER_ID(), "switch@franklin@throw_cup", "throw_cup_loop");
            if (Menu::Option("Smoke Coughing")) DoAnimation(PLAYER::PLAYER_ID(), "timetable@gardener@smoking_joint", "idle_cough");
            if (Menu::Option("Chilling with Friends")) DoAnimation(PLAYER::PLAYER_ID(), "friends@laf@ig_1@base", "base");
            if (Menu::Option("They Think We Dumb")) DoAnimation(PLAYER::PLAYER_ID(), "timetable@ron@they_think_were_stupid", "they_think_were_stupid");
            if (Menu::Option("Come Here")) DoAnimation(PLAYER::PLAYER_ID(), "gestures@m@standing@fat", "gesture_come_here_hard");
            if (Menu::Option("No Way")) DoAnimation(PLAYER::PLAYER_ID(), "gestures@m@standing@fat", "gesture_no_way");
            if (Menu::Option("They're Gonna Kill Me")) DoAnimation(PLAYER::PLAYER_ID(), "random@bicycle_thief@ask_help", "my_dads_going_to_kill_me");
            if (Menu::Option("You Gotta Help Me")) DoAnimation(PLAYER::PLAYER_ID(), "random@bicycle_thief@ask_help", "please_man_you_gotta_help_me");
            if (Menu::Option("Sleep")) DoAnimation(PLAYER::PLAYER_ID(), "savecouch@", "t_sleep_loop_couch");
            if (Menu::Option("Sleep 2")) DoAnimation(PLAYER::PLAYER_ID(), "savem_default@", "m_sleep_r_loop");
            if (Menu::Option("Sleep 3")) DoAnimation(PLAYER::PLAYER_ID(), "timetable@tracy@sleep@", "idle_c");
            if (Menu::Option("Meditate")) DoAnimation(PLAYER::PLAYER_ID(), "rcmcollect_paperleadinout@", "meditiate_idle");
            if (Menu::Option("Fap")) DoAnimation(PLAYER::PLAYER_ID(), "switch@trevor@jerking_off", "trev_jerking_off_loop");
            if (Menu::Option("Yeah Yeah Yeah")) DoAnimation(PLAYER::PLAYER_ID(), "special_ped@jessie@michael_1@michael_1b", "jessie_ig_2_yeahyeahyeah_1");
            if (Menu::Option("Idle On Laptop")) DoAnimation(PLAYER::PLAYER_ID(), "switch@franklin@on_laptop", "001927_01_fras_v2_4_on_laptop_idle");
            if (Menu::Option("Hands Up")) DoAnimation(PLAYER::PLAYER_ID(), "random@arrests", "idle_2_hands_up");
            if (Menu::Option("Stand Still, Arms Spread")) DoAnimation(PLAYER::PLAYER_ID(), "mp_sleep", "bind_pose_180");
            //Scenarios Here
            Menu::Break("Scenarios");
            if (Menu::Option("Paparizzi")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_PAPARAZZI");
            if (Menu::Option("Drug Dealer")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_DRUG_DEALER_HARD");
            if (Menu::Option("Drinking Coffee")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_AA_COFFEE");
            if (Menu::Option("Playing Instruments")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_MUSICIAN");
            if (Menu::Option("Flexing")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_MUSCLE_FLEX");
            if (Menu::Option("Jogging")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_JOG_STANDING");
            if (Menu::Option("Binoculars")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_BINOCULARS");
            if (Menu::Option("Clipboard")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_CLIPBOARD");
            if (Menu::Option("Bench Press")) DoScenario(PLAYER::PLAYER_ID(), "PROP_HUMAN_SEAT_MUSCLE_BENCH_PRESS");
            if (Menu::Option("Chin Ups")) DoScenario(PLAYER::PLAYER_ID(), "PROP_HUMAN_MUSCLE_CHIN_UPS");
            if (Menu::Option("BBQ")) DoScenario(PLAYER::PLAYER_ID(), "PROP_HUMAN_BBQ");
            if (Menu::Option("Fishing")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_STAND_FISHING");
            if (Menu::Option("Security")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_SECURITY_SHINE_TORCH");
            if (Menu::Option("Leaf Blower")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_GARDENER_LEAF_BLOWER");
            if (Menu::Option("Film Shocked")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_MOBILE_FILM_SHOCKING");
            if (Menu::Option("Idle Cop")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_COP_IDLES");
            if (Menu::Option("Drinking")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_DRINKING");
            if (Menu::Option("Golf Player")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_GOLF_PLAYER");
            if (Menu::Option("Welding")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_WELDING");
            if (Menu::Option("Hammering")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_HAMMERING");
            if (Menu::Option("Tennis")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_TENNIS_PLAYER");
            if (Menu::Option("Drilling")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_CONST_DRILL");
            if (Menu::Option("Mechanic")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_VEHICLE_MECHANIC");
            if (Menu::Option("Hang Out")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_HANG_OUT_STREET");
            if (Menu::Option("Smoking")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_SMOKING");
            if (Menu::Option("Smoking Weed")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_SMOKING_POT");
            if (Menu::Option("Standing With Phone")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_STAND_MOBILE");
            if (Menu::Option("Standing With Phone 2")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_STAND_MOBILE_UPRIGHT");
            if (Menu::Option("Standing Guard")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_GUARD_STAND");
            if (Menu::Option("Standing Impatiently")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_STAND_IMPATIENT");
            if (Menu::Option("Standing Impatiently 2")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_STAND_IMPATIENT_UPRIGHT");
            if (Menu::Option("Soldier Stand")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_GUARD_STAND_ARMY");
            if (Menu::Option("Hobo Stand")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_BUM_STANDING");
            if (Menu::Option("Doing Pushups")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_PUSH_UPS");
            if (Menu::Option("Lifting Weights")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_MUSCLE_FREE_WEIGHTS");
            if (Menu::Option("Hooker")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_PROSTITUTE_LOW_CLASS");
            if (Menu::Option("Hooker 2")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_PROSTITUTE_HIGH_CLASS");
            if (Menu::Option("Drunk")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_STUPOR");
            if (Menu::Option("Medic")) DoScenario(PLAYER::PLAYER_ID(), "CODE_HUMAN_MEDIC_KNEEL");
            if (Menu::Option("Window Shopping")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_WINDOW_SHOP_BROWSE");
            if (Menu::Option("Cleaning")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_MAID_CLEAN");
            if (Menu::Option("Doing Yoga")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_YOGA");
            if (Menu::Option("Tourist Map")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_TOURIST_MAP");
            if (Menu::Option("Sunbathing")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_SUNBATHE");
            if (Menu::Option("Sunbathing 2")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_SUNBATHE_BACK");
            if (Menu::Option("Picnic")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_PICNIC");
            if (Menu::Option("Partying")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_PARTYING");
            if (Menu::Option("Leaning")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_LEANING");
            if (Menu::Option("Jog Standing")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_JOG_STANDING");
            if (Menu::Option("Human Statue")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_HUMAN_STATUE");
            if (Menu::Option("Hanging Out (Street)")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_HANG_OUT_STREET");
            if (Menu::Option("Gardening")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_GARDENER_PLANT");
            if (Menu::Option("Cheering")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_CHEERING");
            if (Menu::Option("Parking Attendant")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_CAR_PARK_ATTENDANT");
            if (Menu::Option("Wash")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_BUM_WASH");
            if (Menu::Option("Holding Sign")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_BUM_FREEWAY");
            if (Menu::Option("Laying Down (Hobo)")) DoScenario(PLAYER::PLAYER_ID(), "WORLD_HUMAN_BUM_SLUMPED");
        }
        break;
        case modelsmenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("MODELS");
            for (int i = 0; i < 11; i++)
            {
                if (Menu::Option((char*)BodyGuards[i].c_str()))
                {
                    setPlayerModel(GAMEPLAY::GET_HASH_KEY((char*)BodyGuardsHash[i].c_str()));
                }
            }
        }
        break;
        case outfitsmenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("OUTFITS");
            Menu::Toggle("Force Outfit", forceoutfit);
            if (Menu::Int("Face", face, 0, (PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 0) - 1)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 0, face, 0, 0);
                    outfitchange = false;
                }
            }
            if (Menu::Int(NETWORK::NETWORK_IS_SESSION_STARTED() ? "Masks" : "Beards", mask, 0, (PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 1) - 1)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 1, mask, 0, 0);
                    outfitchange = false;
                }
            }
            if (Menu::Int("Hair", hair, 0, (PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 2) - 1)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 2, hair, 0, 0);
                    outfitchange = false;
                }
            }
            if (Menu::Int(NETWORK::NETWORK_IS_SESSION_STARTED() ? "Torso" : "Jackets", torso, 0, (PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 3) - 1)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 3, torso, 0, 0);
                    outfitchange = false;
                }
            }
            if (Menu::Int("Pants", leg, 0, (PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 4) - 1)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 4, leg, 0, 0);
                    outfitchange = false;
                }
            }
            if (Menu::Int(NETWORK::NETWORK_IS_SESSION_STARTED() ? "Bags" : "Hands" , bag, 0, (PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 5) - 1)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 5, bag, 0, 0);
                    outfitchange = false;
                }
            }
            if (Menu::Int("Shoes", shoes, 0, (PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 6) - 1)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 6, shoes, 0, 0);
                    outfitchange = false;
                }
            }
            if (Menu::Int("Chains", accessory, 0, (PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 7) - 1)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 7, accessory, 0, 0);
                    outfitchange = false;
                }
            }
            if (Menu::Int(NETWORK::NETWORK_IS_SESSION_STARTED() ? "Shirts" : "Tasks", undershirt, 0, (PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 8) - 1)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 8, undershirt, 0, 0);
                    outfitchange = false;
                }
            }
            if (Menu::Int(NETWORK::NETWORK_IS_SESSION_STARTED() ? "Kevlar" : "Tasks", kevlar, 0, PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 9)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 9, kevlar, 0, 0);
                    outfitchange = false;
                }
            }
            if (Menu::Int("Badge", badge, 0, (PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 10) - 1)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 10, badge, 0, 0);
                    outfitchange = false;
                }
            }
            if (Menu::Int(NETWORK::NETWORK_IS_SESSION_STARTED() ? "Jakets" : "Shirts", torso2, 0, (PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(PLAYER::PLAYER_PED_ID(), 11) - 1)))
            {
                if ((Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed) && !forceoutfit)
                {
                    outfitchange = true;
                    PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), 11, torso2, 0, 0);
                    outfitchange = false;
                }
            }
        }
        break;
        case weaponmenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("WEAPON");
            if (Menu::Option("Give all Weapons"))
            {
                for (int i = 0; i < (sizeof(WeaponIDs) / sizeof(WeaponIDs[0])); i++)
                    WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), GAMEPLAY::GET_HASH_KEY(WeaponIDs[i]), 9999, 1);
            }
            if (Menu::Option("Remove all Weapons"))
            {
                WEAPON::REMOVE_ALL_PED_WEAPONS(PLAYER::PLAYER_PED_ID(), true);
                WEAPON::REMOVE_ALL_PED_WEAPONS(PLAYER::PLAYER_PED_ID(), false);
            }
            Menu::MenuOption("Ammo Options", ammoweaponmenu);
			Menu::MenuOption("Object Weapons", propweaponmenu);
			Menu::Toggle("Triggerbot", triggerbot);
			Menu::Toggle("Aim Assist", aimassist);
			Menu::Toggle("Airstrike Gun", airstrikegun);
			Menu::Toggle("New Airstrike Gun", newairstrikegun);
			Menu::Toggle("Shoot RPG", shootrpg);
			Menu::ToggleInt("Explosive Ammo", explosiveammo, explosiveammoint, 0, 72);
			Menu::Toggle("Explosive Melee", explosivemelee);
			Menu::Toggle("Gravity Gun", gravitygun);
			Menu::Toggle("Rapidfire", rapidfire);
			Menu::Toggle("Teleport Gun", teleportgun);
        }
        break;
        case propweaponmenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("OBJECT WEAPON");
            Menu::Toggle("RP Gun", rpgun);
            Menu::ToggleInt("Money Gun", moneygun, moneygunvalue, 0, 2500, 100, false);
            if (Menu::Toggle("Shoot Lazer", jetgun)) if (jetgun) rapidfire = false;
            Menu::Toggle("Delete Gun", deletegun);
            Menu::Toggle("Gravity Gun", gravitygun);
        }
        break;
        case ammoweaponmenu:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("AMMO OPTIONS");            
            if (Menu::Option("Max all Weapon Ammo"))
            {
                for (int i = 0; i < (sizeof(WeaponIDs) / sizeof(WeaponIDs[0])); i++)
                    WEAPON::ADD_AMMO_TO_PED(PLAYER::PLAYER_PED_ID(), GAMEPLAY::GET_HASH_KEY(WeaponIDs[i]), 9999);
            }
            if (Menu::Option("Max Weapon Ammo"))
            {
                Hash weaponhash = NULL;
                if (WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID(), &weaponhash, false))
                    WEAPON::ADD_AMMO_TO_PED(PLAYER::PLAYER_PED_ID(), weaponhash, 9999);
            }
            Menu::Toggle("Infinite Ammo", infammo);
            Menu::ToggleInt("Explosive Ammo", explosiveammo, explosiveammoint, 0, 72);
            Menu::Toggle("No Reload", noreload);
            Menu::Toggle("Shoot Shockwave", shootshockwave);
            Menu::Toggle("Shoot RPG", shootrpg);
        }
        break;
     
		case playerlist:
		{
            Menu::Title(MENU_NAME);
			Menu::SubTitle("PLAYER LIST");
            LoadPlayerInfo(selectedPlayer);
            DrawPlayerMarker(selectedPlayer);
            //Reset Looped Options
            if (pukick) pukick = false;
            if (spectate) spectate = false;
            if (rpdrop) rpdrop = false;
            if (moneydrop) moneydrop = false;
            if (freeze) freeze = false;
            //Weapon Mods
            if (moneygun2) moneygun2 = false;
            if (rpgun2) rpgun2 = false;
            if (deletegun2) deletegun2 = false;
            if (explosiveammo2) explosiveammo2 = false;
            if (explosivemelee2) explosivemelee2 = false;
            if (airstrikegun2) airstrikegun2 = false;
            if (newairstrikegun2) newairstrikegun2 = false;
            //
			for (int i = 0; i < 32; ++i)
			{
				if (ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i)) && NETWORK::NETWORK_IS_PLAYER_CONNECTED(i) && NETWORK::NETWORK_IS_PLAYER_ACTIVE(i))
                {

                    bool isfriend;
                    int NETWORK_HANDLE[76];
                    NETWORK::NETWORK_HANDLE_FROM_PLAYER(i, &NETWORK_HANDLE[0], 13);
                    if (NETWORK::NETWORK_IS_HANDLE_VALID(&NETWORK_HANDLE[0], 13))
                        isfriend = NETWORK::NETWORK_IS_FRIEND(&NETWORK_HANDLE[0]);
                    bool offradar = globalHandle(2425869).At(1 + (i * 443)).At(204).As<int>();
                    int rank = globalHandle(1590682).At(i, 883).At(211).At(6).As<int>();
                    int kd = globalHandle(1590682).At(i, 883).At(211).At(26).As<float>();
                    int wallet = globalHandle(1590682).At(i, 883).At(211).At(3).As<int>();
                    int earnings = globalHandle(1590682).At(i, 883).At(211).At(56).As<int>();
                    int bank = earnings - wallet;
                    bool inside = INTERIOR::IS_VALID_INTERIOR(INTERIOR::GET_INTERIOR_FROM_ENTITY(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i)));
                    bool godmode;
                    float runspeed;
                    float swimspeed;
                    auto CPed = Hooking::get_player_address(i);
                    if (CPed)
                    {
                        godmode = *reinterpret_cast<bool*>(CPed + 0x189);

                        auto CPlayerInfo = *(uintptr_t*)(CPed + 0x10C8);
                        if (CPlayerInfo)
                        {
                            runspeed = *reinterpret_cast<float*>(CPlayerInfo + 0xCD0);
                            swimspeed = *reinterpret_cast<float*>(CPlayerInfo + 0x150);
                        }
                    }
					std::ostringstream PlayerTitle; PlayerTitle << PLAYER::GET_PLAYER_NAME(i);
					if (PLAYER::PLAYER_ID() == i)
					{
						PlayerTitle << " ~o~[Me]";
					}
                    if (NETWORK::NETWORK_GET_HOST_OF_SCRIPT("Freemode", -1, 0) == i)
                    {
                        PlayerTitle << " ~y~[SH]";
                    }
                    if (godmode && !inside)
                    {
                        PlayerTitle << " ~r~[G]";
                    }
                    if (PLAYER::PLAYER_ID() != i)
                    {
                        if (runspeed > 1.0f || swimspeed > 1.0f)
                            PlayerTitle << " ~r~[M]";
                        else if (rank > 1500 || wallet > 1000000 || bank > 100000000 || kd > 5.0f)
                            PlayerTitle << " ~p~[PM]";
                    }
                    if (offradar)
                    {
                        PlayerTitle << " ~c~[OR]";
                    }
                    if (isfriend)
                    {
                        PlayerTitle << " ~g~[F]";
                    }
                    if (inside)
                    {
                        PlayerTitle << " ~c~[I]";
                    }
                    Menu::MenuOption((char*)PlayerTitle.str().c_str(), playermenu) ? selectedPlayer = i : NULL;
				}
			}
		}
		break;
		case playermenu:
		{
            Menu::Title(MENU_NAME);
            Menu::SubTitle(PLAYER::GET_PLAYER_NAME(selectedPlayer));
            LoadPlayerInfo(selectedPlayer);
            DrawPlayerMarker(selectedPlayer);
            Menu::MenuOption("Peaceful", playerpeaceful);
            Menu::MenuOption("Trolls", playertrolls);
            Menu::MenuOption("Drops", playerdrops);
			Menu::MenuOption("Attachments", playerattachments);
			if (Menu::Option("Crash Player"))
			{
				CrashPlayer(selectedPlayer);
			}
			Menu::Toggle("Spectate", spectate);
			if (Menu::Option("Teleport to Player"))
			{
				if (selectedPlayer != PLAYER::PLAYER_ID())
				{
					Ped playerPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer);
					Vector3 Coords = ENTITY::GET_ENTITY_COORDS(playerPed, 1);
					if (!PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
					{
						if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, false))
						{
							Vehicle vehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);
							if (VEHICLE::ARE_ANY_VEHICLE_SEATS_FREE(vehicle))
							{
								if (VEHICLE::IS_VEHICLE_SEAT_FREE(vehicle, -1)) { PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), vehicle, -1); }
								else if (VEHICLE::IS_VEHICLE_SEAT_FREE(vehicle, 0)) { PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), vehicle, 0); }
								else for (int i = 0; i < 16; i++)
									if (VEHICLE::IS_VEHICLE_SEAT_FREE(vehicle, i)) { PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), vehicle, i); }
							}
							else
								ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), Coords.x, Coords.y, Coords.z + 0.8f, 1, 0, 0, 1);
						}
						else
							ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), Coords.x, Coords.y, Coords.z + 0.3f, 1, 0, 0, 1);
					}
					else
						ENTITY::SET_ENTITY_COORDS(PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()), Coords.x, Coords.y, Coords.z + 0.8f, 1, 0, 0, 1);
				}
			}
		}
		break;
        case playerpeaceful:
        {
            Menu::Title(PLAYER::GET_PLAYER_NAME(selectedPlayer));
            Menu::SubTitle("PEACEFUL");
            LoadPlayerInfo(selectedPlayer);
            DrawPlayerMarker(selectedPlayer);
            if (Menu::Option("Give All Weapons"))
            {
                for (int i = 1; i < 5; i++)
                {
                    for (int j = 0; j < (sizeof(WeaponIDs) / sizeof(WeaponIDs[0])); j++)
                    {
                        RequestControlOfEnt(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
                        WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), GAMEPLAY::GET_HASH_KEY(WeaponIDs[j]), 9999, 1);
                    }
                }
            }
            if (Menu::Option("Max all Weapon Ammo"))
            {
                for (int i = 1; i < 5; i++)
                {
   
                    for (int j = 0; j < (sizeof(WeaponIDs) / sizeof(WeaponIDs[0])); j++)
                    {
                        RequestControlOfEnt(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
                        WEAPON::ADD_AMMO_TO_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), GAMEPLAY::GET_HASH_KEY(WeaponIDs[j]), 9999);
                    }
                }
            }
            if (Menu::Option("Max Weapon Ammo"))
            {
                for (int i = 1; i < 5; i++)
                {
                    Hash weaponhash = NULL;

                    if (WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), &weaponhash, false))
                    {
                        RequestControlOfEnt(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
                        WEAPON::ADD_AMMO_TO_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), weaponhash, 9999);
                    }
                }
            }
            Menu::Toggle("RP Gun", rpgun2);
            Menu::ToggleInt("Money Gun", moneygun2, moneygunvalue2, 0, 2500, 100, false);
            Menu::Toggle("Delete Gun", deletegun);
            Menu::Toggle("Airstrike Gun", airstrikegun2);
            Menu::Toggle("New Airstrike Gun", newairstrikegun2);
            Menu::ToggleInt("Explosive Ammo", explosiveammo2, explosiveammoint2, 0, 72);
            Menu::Toggle("Explosive Melee", explosivemelee2);
        }
        break;
        case playertrolls:
        { 
            Menu::Title(PLAYER::GET_PLAYER_NAME(selectedPlayer));
            Menu::SubTitle("TROLLS");
            LoadPlayerInfo(selectedPlayer);
            DrawPlayerMarker(selectedPlayer);
            Menu::Toggle("Freeze", freeze);
            if (Menu::Option("Non Host Kick"))
            {
                pukickcount = 0;
                pukick = true;

                for (int i = 1; i < 5; i++)
                {
                    std::uint64_t Kicks[] = { -442306200, 1352706024, -1252906024, -1890951223, -1559754940, -1243454584, -891346918 };
                    for (int j = 0; j < (sizeof(Kicks) / sizeof(Kicks[0])); j++)
                    {
                        std::uint64_t args[2] = { Kicks[j], selectedPlayer };
                        Hooking::trigger_script_event(1, args, 2, 1 << selectedPlayer);
                    }
                }
            }
            if (Menu::Option("Ceo Ban"))
            {
                std::uint64_t args[4] = { -738295409, selectedPlayer, 1, 5 };
                Hooking::trigger_script_event(1, args, 4, 1 << selectedPlayer);
            }
            if (Menu::Option("Ceo Kick"))
            {
                std::uint64_t args[4] = { -1648921703, selectedPlayer, 1, 5 };
                Hooking::trigger_script_event(1, args, 4, 1 << selectedPlayer);
            }
            if (Menu::Option("Cage"))
            {
                Ped playerPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer);
                AI::CLEAR_PED_TASKS_IMMEDIATELY(playerPed);
                Vector3 pos = ENTITY::GET_ENTITY_COORDS(playerPed, true);
                OBJECT::CREATE_OBJECT(GAMEPLAY::GET_HASH_KEY("prop_gold_cont_01"), pos.x, pos.y, pos.z - 1.f, true, false, false);
            }
            if (Menu::Option("Clone"))
            {
                PED::CLONE_PED(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), 0, true, false);
            }
            if (Menu::Option("Blame"))
            {
                for (int i = 0; i < 32; ++i)
                {
                    if (ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i)))
                    {
                        if (i != PLAYER::PLAYER_ID() && i != selectedPlayer)
                        {
                            Vector3 Coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i), true);
                            FIRE::ADD_OWNED_EXPLOSION(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), Coords.x, Coords.y, Coords.z, 29, 20.f, true, false, 5.f);
                        }
                    }
                }
            }
            if (Menu::Option("Airstrike"))
            {
                Vector3 Coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), 1);
                GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(Coords.x, Coords.y, Coords.z + 70, Coords.x, Coords.y, Coords.z, 250, 1, GAMEPLAY::GET_HASH_KEY("VEHICLE_WEAPON_SPACE_ROCKET"), PLAYER::PLAYER_PED_ID(), 1, 1, 500);
            }
            if (Menu::Option("New Airstrike"))
            {
                Vector3 Coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), 1);
                FIRE::ADD_EXPLOSION(Coords.x, Coords.y, Coords.z - 1, 22, 100, true, false, 0);
                for (int i = 1; i < 8; i++)
                {
                    float spread = 2.5f;
                    float spreadx = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-spread), spread);
                    float spready = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE((-spread), spread);
                    float height = GAMEPLAY::GET_RANDOM_FLOAT_IN_RANGE(-30, 40);
                    GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(Coords.x + (spreadx * 10.0f), Coords.y + (spready * 10.0f), Coords.z + (120 + height), Coords.x + spreadx, Coords.y + spready, Coords.z, 250, 1, GAMEPLAY::GET_HASH_KEY("VEHICLE_WEAPON_SPACE_ROCKET"), PLAYER::PLAYER_PED_ID(), 1, 1, 450);
                }
            }
            if (Menu::Option("Steal Outfit")) 
            {
                Ped playerPed = PLAYER::PLAYER_PED_ID();
                Ped theirPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer);
                for (int i = 0; i < 12; i++)
                    PED::SET_PED_COMPONENT_VARIATION(playerPed, i, PED::GET_PED_DRAWABLE_VARIATION(theirPed, i), PED::GET_PED_TEXTURE_VARIATION(theirPed, i), PED::GET_PED_PALETTE_VARIATION(theirPed, i));
            }
            if (Menu::Option("Explode"))
            {
                Vector3 Coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), 1);
                FIRE::ADD_EXPLOSION(Coords.x, Coords.y, Coords.z - 0.5f, 1, 100, true, false, 0);
            }
            if (Menu::Option("Silent Kill"))
            {
                Vector3 Coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), 1);
                FIRE::ADD_EXPLOSION(Coords.x, Coords.y, Coords.z - 0.5f, 1, 100, false, true, 0);
            }
            if (Menu::Option("Gentle Vehicle Kick"))
            {
                std::uint64_t args[2] = { -1333236192, selectedPlayer };
                Hooking::trigger_script_event(1, args, 2, 1 << selectedPlayer);
            }
            if (Menu::Option("Hard Vehicle Kick"))
            {
                if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false))
                    AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
            }
            if (Menu::Option("Blow Car Tyres"))
            {
                if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false))
                {
                    for (int i = 1; i < 3; i++)
                    {
                        Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false);
                        RequestControlOfEnt(veh);
                        VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(veh, true);
                        VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, true);
                        for (int j = 0; j < 7; j++)
                        {
                            RequestControlOfEnt(veh);
                            VEHICLE::SET_VEHICLE_TYRE_BURST(veh, j, true, 1000.0f);
                        }
                    }
                }
            }
            if (Menu::Option("Kill Vehicle Engine"))
            {
                for (int i = 1; i < 3; i++)
                {
                    Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), false);
                    RequestControlOfEnt(veh);
                    VEHICLE::SET_VEHICLE_ENGINE_HEALTH(veh, -1.0f);
                    VEHICLE::SET_VEHICLE_UNDRIVEABLE(veh, 1);
                }
            }
            if (Menu::Option("Ram With Vehicle"))
            {
                float offset;
                Hash vehmodel = GAMEPLAY::GET_HASH_KEY("SANDKING");
                STREAMING::REQUEST_MODEL(vehmodel);
                while (!STREAMING::HAS_MODEL_LOADED(vehmodel)) WAIT(0);
                Vector3 pCoords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), 0.0, -10.0, 0.0);
                if (STREAMING::IS_MODEL_IN_CDIMAGE(vehmodel) && STREAMING::IS_MODEL_A_VEHICLE(vehmodel))
                {
                    Vector3 dim1, dim2;
                    GAMEPLAY::GET_MODEL_DIMENSIONS(vehmodel, &dim1, &dim2);
                    offset = dim2.y * 1.6;
                    Vector3 dir = ENTITY::GET_ENTITY_FORWARD_VECTOR(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer));
                    float rot = (ENTITY::GET_ENTITY_ROTATION(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), 0)).z;
                    Vehicle veh = VEHICLE::CREATE_VEHICLE(vehmodel, pCoords.x + (dir.x * offset), pCoords.y + (dir.y * offset), pCoords.z, rot, 1, 1);
                    VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(veh);
                    ENTITY::SET_ENTITY_VISIBLE(veh, true, true);
                    VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, 150.0f);
                }
            }
        }
        break;
        case playerattachments:
        {
            Menu::Title(PLAYER::GET_PLAYER_NAME(selectedPlayer));
            Menu::SubTitle("ATTACHMENTS");
            LoadPlayerInfo(selectedPlayer);
            DrawPlayerMarker(selectedPlayer);
            if (Menu::Option("Detach"))
            {
                DetachObjectsFromPlayer(selectedPlayer);
            }
            if (Menu::Option("Attach Yourself"))
            {
                if (selectedPlayer != PLAYER::PLAYER_ID())
                {
                    ENTITY::ATTACH_ENTITY_TO_ENTITY(PLAYER::PLAYER_PED_ID(), PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(selectedPlayer), 0, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, true, true, false, true, 2, true);
                }
            }
            if (Menu::Option("Attach Ball")) AttachObjectToPlayer("prop_juicestand");
            if (Menu::Option("Attach Campfire")) AttachObjectToPlayer("prop_beach_fire");
            if (Menu::Option("Attach Weed")) AttachObjectToPlayer("prop_weed_01");
            if (Menu::Option("Attach Xmas Tree")) AttachObjectToPlayer("prop_xmas_tree_int");
            if (Menu::Option("Attach UFO")) AttachObjectToPlayer("p_spinning_anus_s");
            if (Menu::Option("Attach Safe")) AttachObjectToPlayer("prop_ld_int_safe_01");
        }
        break;
        case playerdrops:
        {
            Menu::Title(PLAYER::GET_PLAYER_NAME(selectedPlayer));
            Menu::SubTitle("DROPS");
            LoadPlayerInfo(selectedPlayer);
            DrawPlayerMarker(selectedPlayer);
            Menu::Toggle("RP Drop", rpdrop);
            Menu::Toggle("Medi Drop", medidrop);
            Menu::Toggle("Ammo Drop", ammodrop);
            Menu::Toggle("Armour Drop", armourdrop);
            Menu::ToggleInt("Money Drop", moneydrop, dropamount, 0, 2500, 100, false);
            Menu::Break("Drop Settings");
            Menu::Float("Drop Height", dropheight, 0.0f, 10.0f);
            Menu::Float("Drop Spread", dropspread, 0.0f, 10.0f);
            Menu::Int("Drop Delay", dropdelay, 0, 2000, 100);
        }
        break;
        case vehiclespawner:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("VEHICLE SPAWNER");
            Menu::Toggle("Spawn Maxed", spawnmaxed);
            if (Menu::Option("Spawn By Name"))
            {
                char* carName = CharKeyboard();
                SpawnVehicleOnPlayer(carName);
                //SpawnModVehicleOnPlayer(carName);
            }
            Menu::Toggle("List Mode", spawnerlistmode);
            if (spawnerlistmode)
            {
                Menu::MenuOption("Mod Vehicles", modvehicles);
                Menu::MenuOption("New Vehicles", updatevehicles);
                Menu::MenuOption("Super Sport", supersport);
                Menu::MenuOption("Sports", sports);
                Menu::MenuOption("Sports Classic", sportsclassic);
                Menu::MenuOption("Offroad", offroad);
                Menu::MenuOption("Sedans", sedans);
                Menu::MenuOption("Coupes", coupes);
                Menu::MenuOption("Muscle", muscle);
                Menu::MenuOption("Boats", boats);
                Menu::MenuOption("Commercials", commercials);
                Menu::MenuOption("Compacts", compacts);
                Menu::MenuOption("Cycles", cycles);
                Menu::MenuOption("Emergency", emergency);
                Menu::MenuOption("Helicopters", helicopters);
                Menu::MenuOption("Industrial", industrial);
                Menu::MenuOption("Military", military);
                Menu::MenuOption("Motorcycles", motorcycles);
                Menu::MenuOption("Planes", planes);
                Menu::MenuOption("SUV", suvs);
                //Menu::MenuOption("Trailer", trailers);
                //Menu::MenuOption("Trains", trains);
                Menu::MenuOption("Utility", utility);
                Menu::MenuOption("Vans", vans);
            }
            else
            {
                if (Menu::StringVector("Mod Vehicles", ModVehs, ModVehsPos)) SpawnModVehicleOnPlayer(ModVehs[ModVehsPos]);
                if (Menu::StringVector("New Vehicles", NewUpdate, NewUpdatePos))  SpawnVehicleOnPlayer(NewUpdate[NewUpdatePos]);
                if (Menu::StringVector("Super Sport", Super, SuperPos))  SpawnVehicleOnPlayer(Super[SuperPos]);
                if (Menu::StringVector("Sports", Sports, SportsPos)) SpawnVehicleOnPlayer(Sports[SportsPos]);
                if (Menu::StringVector("Sports Classic", SportsClassics, SportsClassicsPos)) SpawnVehicleOnPlayer(SportsClassics[SportsClassicsPos]);
                if (Menu::StringVector("Offroad", OffRoad, OffRoadPos)) SpawnVehicleOnPlayer(OffRoad[OffRoadPos]);
                if (Menu::StringVector("Sedans", Sedans, SedansPos)) SpawnVehicleOnPlayer(Sedans[SedansPos]);
                if (Menu::StringVector("Coupes", Coupes, CoupesPos)) SpawnVehicleOnPlayer(Coupes[CoupesPos]);
                if (Menu::StringVector("Muscle", Muscle, MusclePos)) SpawnVehicleOnPlayer(Muscle[MusclePos]);
                if (Menu::StringVector("Boats", Boats, BoatsPos)) SpawnVehicleOnPlayer(Boats[BoatsPos]);
                if (Menu::StringVector("Commercials", Commercials, CommercialsPos)) SpawnVehicleOnPlayer(Commercials[CommercialsPos]);
                if (Menu::StringVector("Compacts", Compacts, CompactsPos)) SpawnVehicleOnPlayer(Compacts[CompactsPos]);
                if (Menu::StringVector("Cycles", Cycles, CyclesPos)) SpawnVehicleOnPlayer(Cycles[CyclesPos]);
                if (Menu::StringVector("Emergency", Emergency, EmergencyPos)) SpawnVehicleOnPlayer(Emergency[EmergencyPos]);
                if (Menu::StringVector("Helicopters", Helicopters, HelicoptersPos)) SpawnVehicleOnPlayer(Helicopters[HelicoptersPos]);
                if (Menu::StringVector("Industrial", Industrial, IndustrialPos)) SpawnVehicleOnPlayer(Industrial[IndustrialPos]);
                if (Menu::StringVector("Military", Cycles, CyclesPos)) SpawnVehicleOnPlayer(Cycles[CyclesPos]);
                if (Menu::StringVector("Motorcycles", Motorcycles, MotorcyclesPos)) SpawnVehicleOnPlayer(Motorcycles[MotorcyclesPos]);
                if (Menu::StringVector("Planes", Planes, PlanesPos)) SpawnVehicleOnPlayer(Planes[PlanesPos]);
                if (Menu::StringVector("SUV", SUVs, SUVsPos)) SpawnVehicleOnPlayer(SUVs[SUVsPos]);
                //if (Menu::StringVector("Trailer", Trailers, TrailersPos)) SpawnVehicleOnPlayer(Trailers[TrailersPos]);
                //if (Menu::StringVector("Trains", Trains, TrainsPos)) SpawnVehicleOnPlayer(Trains[TrainsPos]);
                if (Menu::StringVector("Utility", Utility, UtilityPos)) SpawnVehicleOnPlayer(Utility[UtilityPos]);
                if (Menu::StringVector("Vans", Vans, VansPos)) SpawnVehicleOnPlayer(Vans[VansPos]);
            }
        }
        break;

        //Modded Vehicles
        case modvehicles:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("MOD VEHICLES");
            for (int i = 0; i < ModVehs.size(); i++)
                if (Menu::Option((char*)ModVehs[i].c_str()))
                    SpawnModVehicleOnPlayer(ModVehs[i]);
        }
        break;

        //Vehicle Spawner List Mode 

        case updatevehicles:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("NEW VEHICLES");
            for (int i = 0; i < NewUpdate.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)NewUpdate[i].c_str())))))
                    SpawnVehicleOnPlayer(NewUpdate[i]);
        }
        break;
        case supersport:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("SUPER SPORT");
            for (int i = 0; i < Super.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Super[i].c_str())))))
                    SpawnVehicleOnPlayer(Super[i]);
        }
        break;
        case sports:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("SPORTS");
            for (int i = 0; i < Sports.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Sports[i].c_str())))))
                    SpawnVehicleOnPlayer(Sports[i]);
        }
        break;
        case sportsclassic:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("SPORTS CLASSICS");
            for (int i = 0; i < SportsClassics.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)SportsClassics[i].c_str())))))
                    SpawnVehicleOnPlayer(SportsClassics[i]);
        }
        break;
        case offroad:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("OFFROAD");
            for (int i = 0; i < OffRoad.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)OffRoad[i].c_str())))))
                    SpawnVehicleOnPlayer(OffRoad[i]);
        }
        break;
        case sedans:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("SEDANS");
            for (int i = 0; i < Sedans.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Sedans[i].c_str())))))
                    SpawnVehicleOnPlayer(Sedans[i]);
        }
        break;
        case coupes:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("COUPES");
            for (int i = 0; i < Coupes.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Coupes[i].c_str())))))
                    SpawnVehicleOnPlayer(Coupes[i]);
        }
        break;
        case muscle:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("MUSCLE");
            for (int i = 0; i < Muscle.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Muscle[i].c_str())))))
                    SpawnVehicleOnPlayer(Muscle[i]);
        }
        break;
        case boats:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("BOATS");
            for (int i = 0; i < Boats.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Boats[i].c_str())))))
                    SpawnVehicleOnPlayer(Boats[i]);
        }
        break;
        case commercials:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("COMMERCIALS");
            for (int i = 0; i < Commercials.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Commercials[i].c_str())))))
                    SpawnVehicleOnPlayer(Commercials[i]);
        }
        break;
        case compacts:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("COMPACTS");
            for (int i = 0; i < Compacts.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Compacts[i].c_str())))))
                    SpawnVehicleOnPlayer(Compacts[i]);
        }
        break;
        case cycles:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("CYCLES");
            for (int i = 0; i < Cycles.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Cycles[i].c_str())))))
                    SpawnVehicleOnPlayer(Cycles[i]);
        }
        break;
        case emergency:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("EMERGENCY");
            for (int i = 0; i < Emergency.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Emergency[i].c_str())))))
                    SpawnVehicleOnPlayer(Emergency[i]);
        }
        break;
        case helicopters:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("HELICOPTERS");
            for (int i = 0; i < Helicopters.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Helicopters[i].c_str())))))
                    SpawnVehicleOnPlayer(Helicopters[i]);
        }
        break;
        case industrial:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("INDUSTRIAL");
            for (int i = 0; i < Industrial.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Industrial[i].c_str())))))
                    SpawnVehicleOnPlayer(Industrial[i]);
        }
        break;
        case military:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("MILITARY");
            for (int i = 0; i < Military.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Military[i].c_str())))))
                    SpawnVehicleOnPlayer(Military[i]);
        }
        break;
        case motorcycles:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("MOTORCYCLES");
            for (int i = 0; i < Motorcycles.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Motorcycles[i].c_str())))))
                    SpawnVehicleOnPlayer(Motorcycles[i]);
        }
        break;
        case planes:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("PLANES");
            for (int i = 0; i < Planes.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Planes[i].c_str())))))
                    SpawnVehicleOnPlayer(Planes[i]);
        }
        break;
        case suvs:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("SUVs");
            for (int i = 0; i < SUVs.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)SUVs[i].c_str())))))
                    SpawnVehicleOnPlayer(SUVs[i]);
        }
        break;
        case utility:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("UTILITY");
            for (int i = 0; i < Utility.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Utility[i].c_str())))))
                    SpawnVehicleOnPlayer(Utility[i]);
        }
        break;
        case vans:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("VANS");
            for (int i = 0; i < Vans.size(); i++)
                if (Menu::Option(UI::_GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(GAMEPLAY::GET_HASH_KEY((char*)Vans[i].c_str())))))
                    SpawnVehicleOnPlayer(Vans[i]);
        }
        break;

        case vehicleoptions:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("VEHICLE OPTIONS");
            Menu::MenuOption("Los Santos Customs", lossantoscustoms);
            Menu::Toggle("Godmode", vehgodmode);
            Menu::Toggle("Auto Fix", autofix);
            Menu::Toggle("Auto Flip", autoflip);
            Menu::Toggle("Ramp Vehicle", rampvehicle);
            if (Menu::Toggle("No Gravity", novehiclegravity)) if (novehiclegravity) if (downforcemultiplier != 1) downforcemultiplier = 1;
            if (Menu::Toggle("Drift Mode", driftmode)) if (driftmode) notifyMap("Press Handbreak! [Space~s~]");
            Menu::ToggleInt("Vehicle Fly", vehiclefly, vehicleflyspeed, 0, 150, 5, false);
            Menu::ToggleFloat("Rainbow Color", rainbowcolor, rgbmulti, 0.5f, 15.0f);
            Menu::ToggleFloat("Rainbow Tyresmoke", rainbowtyresmoke, rgbmulti2, 0.5f, 15.0f);
            //Menu::Toggle("Enter Locked", enterlocked);
            Menu::Toggle("Stick to Ground", sticktoground);
            Menu::Toggle("Unlimited Boost", unlimitedboost);
            Menu::Toggle("Instand Boost", instantboost);
            Menu::Toggle("Drive On Water", driveonwater);
            Menu::ToggleInt("Hornboost", hornboost, hornboostspeed, 0, 150, 5, false);
            if (Menu::Option("Max Vehicle"))
            {
                Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);
                SetVehicleMaxed(veh);
            }
            if (Menu::Option("Fix Vehicle"))
            {
                Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);
                VEHICLE::SET_VEHICLE_FIXED(veh);
                VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(veh);
                VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh, 0);
            }
            if (Menu::Option("Set Numberplate"))
            {
                char* plateText = CharKeyboard();
                Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);
                VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(veh, plateText);
            }
            if (Menu::Option("Auto Pilot [Car Only~s~]"))
            {
                Vector3 coords = GetBlipMarker();
                Player playerPed = PLAYER::PLAYER_PED_ID();
                if (coords.x != 0 && coords.y != 0)
                {
                    if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, false))
                    {
                        Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
                        if (!PED::IS_PED_IN_ANY_BOAT(playerPed) && !PED::IS_PED_IN_ANY_HELI(playerPed) && !PED::IS_PED_IN_ANY_PLANE(playerPed) && !PED::IS_PED_IN_ANY_TAXI(playerPed) && !PED::IS_PED_IN_ANY_SUB(playerPed) && !PED::IS_PED_IN_ANY_TRAIN(playerPed) && !PED::IS_PED_ON_ANY_BIKE(playerPed))
                        {
                            AI::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(playerPed, veh, coords.x, coords.y, 0, 150.0f, 1074528293, 1.0f);
                            autopilotstart = true;
                        }
                    }
                }
                else
                {
                    notifyMap("~r~No Destination set!");
                }
            }
            if (Menu::Option("Stop Auto Pilot"))
            {
                if (autopilotstart)
                {
                    Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);
                    AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::PLAYER_PED_ID());
                    PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), veh, -1);
                    autopilotstart = false;
                }
                else
                {
                    notifyMap("~r~No Auto Pilot Running!");
                }
            }
            Menu::Int("Power Multiplier", powermultiplier, 1, 100);
            Menu::Int("Torque Multiplier", torquemultiplier, 1, 100);
            Menu::Int("Acceleration Multiplier", accelerationmultiplier, 1, 100);
            Menu::Int("Downforce Multiplier", downforcemultiplier, -5, 100);
            Menu::Int("Brake Multiplier", brakemultiplier, 1, 100);
        }
        break;
        case lossantoscustoms:
        {
            Menu::Header("shopui_title_carmod", "shopui_title_carmod");
            Menu::SubTitle("LOS SANTOS CUSTOMS");

            Vehicle vehicle = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
            //Enable Los Santos Customs

            if (VEHICLE::GET_VEHICLE_MOD_KIT(vehicle) == 65535 && PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
            {
                VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
            }

            char* slotNames[25] = { 
                "Spoilers", 
                "Front Bumper", 
                "Rear Bumper", 
                "Side Skirt", 
                "Exhaust", 
                "Frame", 
                "Grille", 
                "Hood", 
                "Fender",
                "Right Fender",
                "Roof", 
                "Engine",
                "Brakes",
                "Transmission",
                "Horns",
                "Suspension",
                "Armor", 
                "",
                "Turbo", 
                "",
                "Tire Smoke",
                "",
                "Xenon",
                VEHICLE::IS_THIS_MODEL_A_BIKE(ENTITY::GET_ENTITY_MODEL(vehicle)) ? "Front Wheels" : "Wheels", 
                "Back Wheels" 
            };

            for (int i = 0; i < 24; i++)
            {
                if (i == 17 || i == 19 || i == 21)
                    continue;

                if (i == 24 && !VEHICLE::IS_THIS_MODEL_A_BIKE(ENTITY::GET_ENTITY_MODEL(vehicle)))
                    continue;

                if (i == 18)
                {
                    //Apply Turbo
                    if (Menu::Toggle(slotNames[i], turbo))
                    {
                        if (Menu::Settings::selectPressed)
                        {
                            VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
                            vehmodchange = true;
                            VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, i, turbo);
                            vehmodchange = false;
                        }
                    }
                    continue;
                }
                if (i == 20)
                {
                    //Enable Tire Smoke Coloring
                    if (Menu::Toggle(slotNames[i], tiresmoke))
                    {
                        if (Menu::Settings::selectPressed)
                        {
                            VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
                            vehmodchange = true;
                            VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, i, tiresmoke);
                            vehmodchange = false;
                        }
                    }
                    continue;
                }
                if (i == 22)
                {
                    //Apply Xenon Headlights 
                    if (Menu::Toggle(slotNames[i], xenon))
                    {
                        if (Menu::Settings::selectPressed)
                        {
                            VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
                            vehmodchange = true;
                            VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, i, xenon);
                            vehmodchange = false;
                        }
                    }
                    continue;
                }
                //Apply Other Vehicle Mods
                if (VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, i) > 0 || !PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
                {
                    if (Menu::Int(slotNames[i], vehicleMods[i], 0, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, i)))
                    {
                        if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                        {
                            VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
                            vehmodchange = true;
                            VEHICLE::SET_VEHICLE_MOD(vehicle, i, (vehicleMods[i] - 1), true);
                            vehmodchange = false;
                        }
                    }
                }
            }

            //Vehicle Window Tint
            if (Menu::Int("Window Tint", windowtint, 0, 3))
            {
                if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                {
                    VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
                    vehmodchange = true;
                    VEHICLE::SET_VEHICLE_WINDOW_TINT(vehicle, (windowtint - 1));
                    vehmodchange = false;
                }
            }

        }
        break;

        case teleports:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("TELEPORTS");
            if (Menu::Option("Teleport to Waypoint [F5~s~]"))
            {
                TeleportToMarker();
            }
            Menu::Toggle("Auto Teleport", autoteleport);
            Menu::Break("Locations");
            if (Menu::Option("Mount Chiliad")) {
                Vector3 Coords;
                Coords.x = 496.75f; Coords.y = 5591.17f; Coords.z = 795.03f;
                TPto(Coords);
            }
            if (Menu::Option("Maze Bank")) {
                Vector3 Coords;
                Coords.x = -74.94243f; Coords.y = -818.63446f; Coords.z = 326.174347f;
                TPto(Coords);
            }
            if (Menu::Option("Military Base")) {
                Vector3 Coords;
                Coords.x = -2012.8470f; Coords.y = 2956.5270f; Coords.z = 32.8101f;
                TPto(Coords);
            }
            if (Menu::Option("Zancudo Tower")) {
                Vector3 Coords;
                Coords.x = -2356.0940; Coords.y = 3248.645; Coords.z = 101.4505;
                TPto(Coords);
            }
            if (Menu::Option("Mask Shop")) {
                Vector3 Coords;
                Coords.x = -1338.16; Coords.y = -1278.11; Coords.z = 4.87;
                TPto(Coords);
            }
            if (Menu::Option("LSC")) {
                Vector3 Coords;
                Coords.x = -373.01; Coords.y = -124.91; Coords.z = 38.31;
                TPto(Coords);
            }
            if (Menu::Option("Ammunation")) {
                Vector3 Coords;
                Coords.x = 247.3652; Coords.y = -45.8777; Coords.z = 69.9411;
                TPto(Coords);
            }
            if (Menu::Option("Airport")) {
                Vector3 Coords;
                Coords.x = -1102.2910f; Coords.y = -2894.5160f; Coords.z = 13.9467f;
                TPto(Coords);
            }
            if (Menu::Option("Clothes Store")) {
                Vector3 Coords;
                Coords.x = -718.91; Coords.y = -158.16; Coords.z = 37.00;
                TPto(Coords);
            }
            if (Menu::Option("Waterfall")) {
                Vector3 Coords;
                Coords.x = -597.9525f; Coords.y = 4475.2910f; Coords.z = 25.6890f;
                TPto(Coords);
            }
            if (Menu::Option("FIB")) {
                Vector3 Coords;
                Coords.x = 135.5220f; Coords.y = -749.0003f; Coords.z = 260.0000f;
                TPto(Coords);
            }
            if (Menu::Option("Human Labs")) {
                Vector3 Coords;
                Coords.x = 3617.231f; Coords.y = 3739.871f; Coords.z = 28.6901f;
                TPto(Coords);
            }
            if (Menu::Option("MMI")) {
                Vector3 Coords;
                Coords.x = -222.1977; Coords.y = -1185.8500; Coords.z = 23.0294;
                TPto(Coords);
            }
            if (Menu::Option("Sandy Shores Airfield")) {
                Vector3 Coords;
                Coords.x = 1741.4960f; Coords.y = 3269.2570f; Coords.z = 41.6014f;
                TPto(Coords);
            }
            if (Menu::Option("Cayo Perico"))
            {
                STREAMING::REQUEST_IPL("h4_islandx_terrain_01_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_01_slod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_02");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_02_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_02_slod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_03");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_03_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_04");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_04_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_04_slod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_05");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_05_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_05_slod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_06");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_01_grass_0");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_01_grass_1");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_02_grass_0");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_02_grass_1");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_02_grass_2");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_02_grass_3");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_04_grass_0");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_04_grass_1");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_05_grass_0");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_06_grass_0");
                //STREAMING::REQUEST_IPL("h4_islandx_terrain_01"); Triggers teleport
                STREAMING::REQUEST_IPL("h4_islandx_terrain_06_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_06_slod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_a");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_a_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_b");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_b_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_c");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_c_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_d");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_d_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_d_slod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_e");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_e_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_e_slod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_f");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_f_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_05_f_slod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_06_a");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_06_a_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_06_a_slod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_06_b");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_06_b_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_06_b_slod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_06_c");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_06_c_lod");
                STREAMING::REQUEST_IPL("h4_islandx_terrain_props_06_c_slod");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_01");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_01_long_0");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_02");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_03");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_04");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_05");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_06");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_06_strm_0");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_lod");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_occ_00");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_occ_01");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_occ_02");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_occ_03");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_occ_04");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_occ_05");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_occ_06");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_occ_07");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_occ_08");
                STREAMING::REQUEST_IPL("h4_mph4_terrain_occ_09");
                STREAMING::REQUEST_IPL("h4_boatblockers");
                STREAMING::REQUEST_IPL("h4_islandx");
                STREAMING::REQUEST_IPL("h4_islandx_disc_strandedshark");
                STREAMING::REQUEST_IPL("h4_islandx_disc_strandedshark_lod");
                STREAMING::REQUEST_IPL("h4_islandx_disc_strandedwhale");
                STREAMING::REQUEST_IPL("h4_islandx_disc_strandedwhale_lod");
                STREAMING::REQUEST_IPL("h4_islandx_props");
                STREAMING::REQUEST_IPL("h4_islandx_props_lod");
                STREAMING::REQUEST_IPL("h4_islandx_sea_mines");
                STREAMING::REQUEST_IPL("h4_mph4_island");
                STREAMING::REQUEST_IPL("h4_mph4_island_long_0");
                STREAMING::REQUEST_IPL("h4_mph4_island_strm_0");
                STREAMING::REQUEST_IPL("h4_aa_guns");
                STREAMING::REQUEST_IPL("h4_aa_guns_lod");
                STREAMING::REQUEST_IPL("h4_beach");
                STREAMING::REQUEST_IPL("h4_beach_bar_props");
                STREAMING::REQUEST_IPL("h4_beach_lod");
                STREAMING::REQUEST_IPL("h4_beach_party");
                STREAMING::REQUEST_IPL("h4_beach_party_lod");
                STREAMING::REQUEST_IPL("h4_beach_props");
                STREAMING::REQUEST_IPL("h4_beach_props_lod");
                STREAMING::REQUEST_IPL("h4_beach_props_party");
                STREAMING::REQUEST_IPL("h4_beach_props_slod");
                STREAMING::REQUEST_IPL("h4_beach_slod");
                STREAMING::REQUEST_IPL("h4_islandairstrip");
                STREAMING::REQUEST_IPL("h4_islandairstrip_doorsclosed");
                STREAMING::REQUEST_IPL("h4_islandairstrip_doorsclosed_lod");
                STREAMING::REQUEST_IPL("h4_islandairstrip_doorsopen");
                STREAMING::REQUEST_IPL("h4_islandairstrip_doorsopen_lod");
                STREAMING::REQUEST_IPL("h4_islandairstrip_hangar_props");
                STREAMING::REQUEST_IPL("h4_islandairstrip_hangar_props_lod");
                STREAMING::REQUEST_IPL("h4_islandairstrip_hangar_props_slod");
                STREAMING::REQUEST_IPL("h4_islandairstrip_lod");
                STREAMING::REQUEST_IPL("h4_islandairstrip_props");
                STREAMING::REQUEST_IPL("h4_islandairstrip_propsb");
                STREAMING::REQUEST_IPL("h4_islandairstrip_propsb_lod");
                STREAMING::REQUEST_IPL("h4_islandairstrip_propsb_slod");
                STREAMING::REQUEST_IPL("h4_islandairstrip_props_lod");
                STREAMING::REQUEST_IPL("h4_islandairstrip_props_slod");
                STREAMING::REQUEST_IPL("h4_islandairstrip_slod");
                STREAMING::REQUEST_IPL("h4_islandxcanal_props");
                STREAMING::REQUEST_IPL("h4_islandxcanal_props_lod");
                STREAMING::REQUEST_IPL("h4_islandxcanal_props_slod");
                STREAMING::REQUEST_IPL("h4_islandxdock");
                STREAMING::REQUEST_IPL("h4_islandxdock_lod");
                STREAMING::REQUEST_IPL("h4_islandxdock_props");
                STREAMING::REQUEST_IPL("h4_islandxdock_props_2");
                STREAMING::REQUEST_IPL("h4_islandxdock_props_2_lod");
                STREAMING::REQUEST_IPL("h4_islandxdock_props_2_slod");
                STREAMING::REQUEST_IPL("h4_islandxdock_props_lod");
                STREAMING::REQUEST_IPL("h4_islandxdock_props_slod");
                STREAMING::REQUEST_IPL("h4_islandxdock_slod");
                STREAMING::REQUEST_IPL("h4_islandxdock_water_hatch");
                STREAMING::REQUEST_IPL("h4_islandxtower");
                STREAMING::REQUEST_IPL("h4_islandxtower_lod");
                STREAMING::REQUEST_IPL("h4_islandxtower_slod");
                STREAMING::REQUEST_IPL("h4_islandxtower_veg");
                STREAMING::REQUEST_IPL("h4_islandxtower_veg_lod");
                STREAMING::REQUEST_IPL("h4_islandxtower_veg_slod");
                STREAMING::REQUEST_IPL("h4_islandx_barrack_hatch");
                STREAMING::REQUEST_IPL("h4_islandx_barrack_props");
                STREAMING::REQUEST_IPL("h4_islandx_barrack_props_lod");
                STREAMING::REQUEST_IPL("h4_islandx_barrack_props_slod");
                STREAMING::REQUEST_IPL("h4_islandx_checkpoint");
                STREAMING::REQUEST_IPL("h4_islandx_checkpoint_lod");
                STREAMING::REQUEST_IPL("h4_islandx_checkpoint_props");
                STREAMING::REQUEST_IPL("h4_islandx_checkpoint_props_lod");
                STREAMING::REQUEST_IPL("h4_islandx_checkpoint_props_slod");
                STREAMING::REQUEST_IPL("h4_islandx_maindock");
                STREAMING::REQUEST_IPL("h4_islandx_maindock_lod");
                STREAMING::REQUEST_IPL("h4_islandx_maindock_props");
                STREAMING::REQUEST_IPL("h4_islandx_maindock_props_2");
                STREAMING::REQUEST_IPL("h4_islandx_maindock_props_2_lod");
                STREAMING::REQUEST_IPL("h4_islandx_maindock_props_2_slod");
                STREAMING::REQUEST_IPL("h4_islandx_maindock_props_lod");
                STREAMING::REQUEST_IPL("h4_islandx_maindock_props_slod");
                STREAMING::REQUEST_IPL("h4_islandx_maindock_slod");
                STREAMING::REQUEST_IPL("h4_islandx_mansion");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_b");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_b_lod");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_b_side_fence");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_b_slod");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_entrance_fence");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_guardfence");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_lights");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_lockup_01");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_lockup_01_lod");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_lockup_02");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_lockup_02_lod");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_lockup_03");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_lockup_03_lod");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_lod");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_office");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_office_lod");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_props");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_props_lod");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_props_slod");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_slod");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_vault");
                STREAMING::REQUEST_IPL("h4_islandx_mansion_vault_lod");
                STREAMING::REQUEST_IPL("h4_island_padlock_props");
                STREAMING::REQUEST_IPL("h4_mansion_gate_broken");
                STREAMING::REQUEST_IPL("h4_mansion_gate_closed");
                STREAMING::REQUEST_IPL("h4_mansion_remains_cage");
                STREAMING::REQUEST_IPL("h4_mph4_airstrip");
                STREAMING::REQUEST_IPL("h4_mph4_airstrip_interior_0_airstrip_hanger");
                STREAMING::REQUEST_IPL("h4_mph4_beach");
                STREAMING::REQUEST_IPL("h4_mph4_dock");
                STREAMING::REQUEST_IPL("h4_mph4_island_lod");
                STREAMING::REQUEST_IPL("h4_mph4_island_ne_placement");
                STREAMING::REQUEST_IPL("h4_mph4_island_nw_placement");
                STREAMING::REQUEST_IPL("h4_mph4_island_se_placement");
                STREAMING::REQUEST_IPL("h4_mph4_island_sw_placement");
                STREAMING::REQUEST_IPL("h4_mph4_mansion");
                STREAMING::REQUEST_IPL("h4_mph4_mansion_b");
                STREAMING::REQUEST_IPL("h4_mph4_mansion_b_strm_0");
                STREAMING::REQUEST_IPL("h4_mph4_mansion_strm_0");
                STREAMING::REQUEST_IPL("h4_mph4_wtowers");
                STREAMING::REQUEST_IPL("h4_ne_ipl_00");
                STREAMING::REQUEST_IPL("h4_ne_ipl_00_lod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_00_slod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_01");
                STREAMING::REQUEST_IPL("h4_ne_ipl_01_lod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_01_slod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_02");
                STREAMING::REQUEST_IPL("h4_ne_ipl_02_lod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_02_slod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_03");
                STREAMING::REQUEST_IPL("h4_ne_ipl_03_lod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_03_slod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_04");
                STREAMING::REQUEST_IPL("h4_ne_ipl_04_lod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_04_slod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_05");
                STREAMING::REQUEST_IPL("h4_ne_ipl_05_lod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_05_slod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_06");
                STREAMING::REQUEST_IPL("h4_ne_ipl_06_lod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_06_slod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_07");
                STREAMING::REQUEST_IPL("h4_ne_ipl_07_lod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_07_slod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_08");
                STREAMING::REQUEST_IPL("h4_ne_ipl_08_lod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_08_slod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_09");
                STREAMING::REQUEST_IPL("h4_ne_ipl_09_lod");
                STREAMING::REQUEST_IPL("h4_ne_ipl_09_slod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_00");
                STREAMING::REQUEST_IPL("h4_nw_ipl_00_lod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_00_slod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_01");
                STREAMING::REQUEST_IPL("h4_nw_ipl_01_lod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_01_slod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_02");
                STREAMING::REQUEST_IPL("h4_nw_ipl_02_lod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_02_slod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_03");
                STREAMING::REQUEST_IPL("h4_nw_ipl_03_lod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_03_slod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_04");
                STREAMING::REQUEST_IPL("h4_nw_ipl_04_lod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_04_slod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_05");
                STREAMING::REQUEST_IPL("h4_nw_ipl_05_lod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_05_slod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_06");
                STREAMING::REQUEST_IPL("h4_nw_ipl_06_lod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_06_slod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_07");
                STREAMING::REQUEST_IPL("h4_nw_ipl_07_lod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_07_slod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_08");
                STREAMING::REQUEST_IPL("h4_nw_ipl_08_lod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_08_slod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_09");
                STREAMING::REQUEST_IPL("h4_nw_ipl_09_lod");
                STREAMING::REQUEST_IPL("h4_nw_ipl_09_slod");
                STREAMING::REQUEST_IPL("h4_se_ipl_00");
                STREAMING::REQUEST_IPL("h4_se_ipl_00_lod");
                STREAMING::REQUEST_IPL("h4_se_ipl_00_slod");
                STREAMING::REQUEST_IPL("h4_se_ipl_01");
                STREAMING::REQUEST_IPL("h4_se_ipl_01_lod");
                STREAMING::REQUEST_IPL("h4_se_ipl_01_slod");
                STREAMING::REQUEST_IPL("h4_se_ipl_02");
                STREAMING::REQUEST_IPL("h4_se_ipl_02_lod");
                STREAMING::REQUEST_IPL("h4_se_ipl_02_slod");
                STREAMING::REQUEST_IPL("h4_se_ipl_03");
                STREAMING::REQUEST_IPL("h4_se_ipl_03_lod");
                STREAMING::REQUEST_IPL("h4_se_ipl_03_slod");
                STREAMING::REQUEST_IPL("h4_se_ipl_04");
                STREAMING::REQUEST_IPL("h4_se_ipl_04_lod");
                STREAMING::REQUEST_IPL("h4_se_ipl_04_slod");
                STREAMING::REQUEST_IPL("h4_se_ipl_05");
                STREAMING::REQUEST_IPL("h4_se_ipl_05_lod");
                STREAMING::REQUEST_IPL("h4_se_ipl_05_slod");
                STREAMING::REQUEST_IPL("h4_se_ipl_06");
                STREAMING::REQUEST_IPL("h4_se_ipl_06_lod");
                STREAMING::REQUEST_IPL("h4_se_ipl_06_slod");
                STREAMING::REQUEST_IPL("h4_se_ipl_07");
                STREAMING::REQUEST_IPL("h4_se_ipl_07_lod");
                STREAMING::REQUEST_IPL("h4_se_ipl_07_slod");
                STREAMING::REQUEST_IPL("h4_se_ipl_08");
                STREAMING::REQUEST_IPL("h4_se_ipl_08_lod");
                STREAMING::REQUEST_IPL("h4_se_ipl_08_slod");
                STREAMING::REQUEST_IPL("h4_se_ipl_09");
                STREAMING::REQUEST_IPL("h4_se_ipl_09_lod");
                STREAMING::REQUEST_IPL("h4_se_ipl_09_slod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_00");
                STREAMING::REQUEST_IPL("h4_sw_ipl_00_lod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_00_slod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_01");
                STREAMING::REQUEST_IPL("h4_sw_ipl_01_lod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_01_slod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_02");
                STREAMING::REQUEST_IPL("h4_sw_ipl_02_lod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_02_slod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_03");
                STREAMING::REQUEST_IPL("h4_sw_ipl_03_lod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_03_slod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_04");
                STREAMING::REQUEST_IPL("h4_sw_ipl_04_lod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_04_slod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_05");
                STREAMING::REQUEST_IPL("h4_sw_ipl_05_lod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_05_slod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_06");
                STREAMING::REQUEST_IPL("h4_sw_ipl_06_lod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_06_slod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_07");
                STREAMING::REQUEST_IPL("h4_sw_ipl_07_lod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_07_slod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_08");
                STREAMING::REQUEST_IPL("h4_sw_ipl_08_lod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_08_slod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_09");
                STREAMING::REQUEST_IPL("h4_sw_ipl_09_lod");
                STREAMING::REQUEST_IPL("h4_sw_ipl_09_slod");
                STREAMING::REQUEST_IPL("h4_underwater_gate_closed");
                STREAMING::REQUEST_IPL("h4_islandx_placement_01");
                STREAMING::REQUEST_IPL("h4_islandx_placement_02");
                STREAMING::REQUEST_IPL("h4_islandx_placement_03");
                STREAMING::REQUEST_IPL("h4_islandx_placement_04");
                STREAMING::REQUEST_IPL("h4_islandx_placement_05");
                STREAMING::REQUEST_IPL("h4_islandx_placement_06");
                STREAMING::REQUEST_IPL("h4_islandx_placement_07");
                STREAMING::REQUEST_IPL("h4_islandx_placement_08");
                STREAMING::REQUEST_IPL("h4_islandx_placement_09");
                STREAMING::REQUEST_IPL("h4_islandx_placement_10");
                STREAMING::REQUEST_IPL("h4_mph4_island_placement");
                Vector3 Coords;
                Coords.x = 4892.064f; Coords.y = -4923.567; Coords.z = 3.500f;
                TPto(Coords);
            }
            if (Menu::Option("North Yankton")) {
                STREAMING::REQUEST_IPL("prologue01");
                STREAMING::REQUEST_IPL("Prologue01c");
                STREAMING::REQUEST_IPL("Prologue01d");
                STREAMING::REQUEST_IPL("Prologue01e");
                STREAMING::REQUEST_IPL("Prologue01f");
                STREAMING::REQUEST_IPL("Prologue01g");
                STREAMING::REQUEST_IPL("prologue01h");
                STREAMING::REQUEST_IPL("prologue01i");
                STREAMING::REQUEST_IPL("prologue01j");
                STREAMING::REQUEST_IPL("prologue01k");
                STREAMING::REQUEST_IPL("prologue01z");
                STREAMING::REQUEST_IPL("prologue02");
                STREAMING::REQUEST_IPL("prologue03");
                STREAMING::REQUEST_IPL("prologue03b");
                STREAMING::REQUEST_IPL("prologue03_grv_cov");
                STREAMING::REQUEST_IPL("prologue03_grv_dug");
                STREAMING::REQUEST_IPL("prologue03_grv_fun");
                STREAMING::REQUEST_IPL("prologue04");
                STREAMING::REQUEST_IPL("prologue04b");
                STREAMING::REQUEST_IPL("prologue04_cover");
                STREAMING::REQUEST_IPL("prologue05");
                STREAMING::REQUEST_IPL("prologue05b");
                STREAMING::REQUEST_IPL("prologue06");
                STREAMING::REQUEST_IPL("prologue06b");
                STREAMING::REQUEST_IPL("prologue06_int");
                STREAMING::REQUEST_IPL("prologuerd");
                STREAMING::REQUEST_IPL("prologuerdb");
                STREAMING::REQUEST_IPL("prologue_DistantLights");
                STREAMING::REQUEST_IPL("prologue_grv_torch");
                STREAMING::REQUEST_IPL("prologue_m2_door");
                STREAMING::REQUEST_IPL("prologue_LODLights");
                STREAMING::REQUEST_IPL("DES_ProTree_start");
                Vector3 Coords;
                Coords.x = 3595.39673f; Coords.y = -4893.727f; Coords.z = 115.838394f;
                TPto(Coords);
            }
            if (Menu::Option("Porn Yacht")) {
                STREAMING::REQUEST_IPL("smboat");
                Vector3 Coords;
                Coords.x = -2045.8f; Coords.y = -1031.2f; Coords.z = 11.9f;
                TPto(Coords);
            }
            if (Menu::Option("Aircraft Carrier")) {
                STREAMING::REQUEST_IPL("hei_carrier");
                STREAMING::REQUEST_IPL("hei_carrier_DistantLights");
                STREAMING::REQUEST_IPL("hei_Carrier_int1");
                STREAMING::REQUEST_IPL("hei_Carrier_int2");
                STREAMING::REQUEST_IPL("hei_Carrier_int3");
                STREAMING::REQUEST_IPL("hei_Carrier_int4");
                STREAMING::REQUEST_IPL("hei_Carrier_int5");
                STREAMING::REQUEST_IPL("hei_Carrier_int6");
                STREAMING::REQUEST_IPL("hei_carrier_LODLights");
                Vector3 Coords;
                Coords.x = 3069.330f; Coords.y = -4632.4f; Coords.z = 15.043f;
                TPto(Coords);
            }
            if (Menu::Option("Sunken Cargoship")) {
                STREAMING::REQUEST_IPL("sunkcargoship");
                Vector3 Coords;
                Coords.x = -162.8918f; Coords.y = -2365.769f; Coords.z = 0.0f;
                TPto(Coords);
            }
            if (Menu::Option("Hospital")) {
                STREAMING::REQUEST_IPL("RC12B_HospitalInterior");
                STREAMING::REQUEST_IPL("RC12B_Destroyed");
                Vector3 Coords;
                Coords.x = 356.8f; Coords.y = -590.1f; Coords.z = 43.3f;
                TPto(Coords);
            }
            if (Menu::Option("Oneil Farm")) {
                STREAMING::REMOVE_IPL("farm_burnt");
                STREAMING::REMOVE_IPL("farm_burnt_props");
                STREAMING::REQUEST_IPL("farm");
                STREAMING::REQUEST_IPL("farm_props");
                STREAMING::REQUEST_IPL("farmint");
                STREAMING::REQUEST_IPL("farmint_cap");
                Vector3 Coords;
                Coords.x = 2441.2f; Coords.y = 4968.5f; Coords.z = 51.7f;
                TPto(Coords);
            }
            if (Menu::Option("Life Invader Office")) {
                STREAMING::REQUEST_IPL("facelobby");
                STREAMING::REQUEST_IPL("facelobbyfake");
                Vector3 Coords;
                Coords.x = -1047.9f; Coords.y = -233.0f; Coords.z = 39.0f;
                TPto(Coords);
            }
            if (Menu::Option("Cargoship")) {
                STREAMING::REQUEST_IPL("cargoship");
                Vector3 Coords;
                Coords.x = -162.8918f; Coords.y = -2365.769f; Coords.z = 9.3192f;
                TPto(Coords);
            }
            if (Menu::Option("Jewelry Store")) {
                STREAMING::REQUEST_IPL("jewel2fake");
                STREAMING::REQUEST_IPL("post_hiest_unload");
                STREAMING::REQUEST_IPL("bh1_16_refurb");
                Vector3 Coords;
                Coords.x = -630.4f; Coords.y = -236.7f; Coords.z = 40.0f;
                TPto(Coords);
            }
            if (Menu::Option("Morgue")) {
                STREAMING::REQUEST_IPL("Coroner_Int_on");
                Vector3 Coords;
                Coords.x = 244.9f; Coords.y = -1374.7f; Coords.z = 39.5f;
                TPto(Coords);
            }
        }
        break;
        case protections:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("PROTECTIONS");
            Menu::Toggle("Block All Remotes [Expert~s~]", scripteventprot);
            Menu::Toggle("Non Host Kick", nonhostkickprot);
            Menu::Toggle("Force to Mission", missionprot);
            Menu::Toggle("Apartment Teleport", inviteprot);
            Menu::Toggle("Vehicle Kick", vehkickprot);
            Menu::Toggle("Ceo Kick Prot", ceokickprot);
            Menu::Toggle("Ceo Ban Prot", ceobanprot);
            Menu::Toggle("Sound Spam", soundspamprot);
            Menu::Toggle("Controll Events", controllprot);
            Menu::Toggle("Freeze Events", freezeprot);
            Menu::Toggle("Time Change", timeprot);
            Menu::Toggle("Weather Change", weatherprot);
            Menu::Toggle("Explode Events", explodeprot);
            Menu::Toggle("PTFX Events", ptfxprot);
            Menu::Toggle("Burn Events", fireprot);
            Menu::Toggle("Clone Protection", deleteclones);
            Menu::Toggle("Block Player Attach", autoplayerdetach);
            Menu::Toggle("Pickup Protection", pickupprot);
            Menu::Toggle("Transaction Error", transactionerrorprot);
            Menu::Toggle("Remove Attachments", autodetach);
            Menu::Toggle("Remove Cages", autouncage);
        }
        break;
        case recovery:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("RECOVERY");
            Menu::Break("Offline");
            if (Menu::Option("Max Single Player Money"))
            {
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("SP0_TOTAL_CASH"), INT_MAX, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("SP1_TOTAL_CASH"), INT_MAX, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("SP2_TOTAL_CASH"), INT_MAX, true);
            }
            Menu::Break("Level Recovery");
            Menu::Int("Choose Level", LevelsPos, 1, 8000);
            if (LevelsPos <= 600)
            {
                if (Menu::Option("Set Level [Instant]"))
                {
                    if (LevelsPos > 0 && LevelsPos <= 8000)
                    {
                        STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CHAR_XP_FM"), Levels[(LevelsPos - 1)], 0);
                        STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CHAR_XP_FM"), Levels[(LevelsPos - 1)], 0);
                    }
                }
            }
            if (Menu::Option("Set Level [Correction]"))
            {
                if (LevelsPos > 0 && LevelsPos <= 8000)
                {
                    STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_CHAR_SET_RP_GIFT_ADMIN"), Levels[(LevelsPos - 1)], 1);
                    STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_CHAR_SET_RP_GIFT_ADMIN"), Levels[(LevelsPos - 1)], 1);
                    iconNotify("Recovery", "Level Changed!\nPls Change Session!");
                }
            }
            Menu::Break("Vehicle Stealth");
            if (Menu::Option("Reset Vehicle Sell time"))
            {
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MPPLY_VEHICLE_SELL_TIME"), 0, true);
            }
            Menu::Break("Unlocks & Stats");
            if (Menu::Option("Unlock All"))
            {
                UnlockAll();
            }
            if (Menu::Option("Clear Orbital Cooldown"))
            {
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_ORBITAL_CANNON_COOLDOWN"), 0, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_ORBITAL_CANNON_COOLDOWN"), 0, true);
            }
            if (Menu::Option("Max All Skills")) 
            {
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_STAM"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_STRN"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_LUNG"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_DRIV"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_FLY"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_SHO"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_SCRIPT_INCREASE_STL"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_STAM"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_LUNG"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_DRIV"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_FLY"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_SHO"), 100, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_SCRIPT_INCREASE_STL"), 100, true);
            }
            if (Menu::Option("Unlock LSC Parts")) 
            {
                STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMRACEWORLDRECHOLDER"), 1, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_ENEMYDRIVEBYKILLS"), 600, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_USJS_COMPLETED"), 50, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_USJS_FOUND"), 50, true);
                STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMWINALLRACEMODES"), 1, true);
                STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMWINEVERYGAMEMODE"), 1, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_DB_PLAYER_KILLS"), 1000, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_KILLS_PLAYERS"), 1000, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMHORDWAVESSURVIVE"), 21, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_CAR_BOMBS_ENEMY_KILLS"), 25, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_TDM_MVP"), 60, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_HOLD_UP_SHOPS"), 20, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_RACES_WON"), 101, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_NO_ARMWRESTLING_WINS"), 21, true);
                STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMATTGANGHQ"), 1, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMBBETWIN"), 50000, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_DM_WINS"), 51, true);
                STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FMFULLYMODDEDCAR"), 1, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_DM_TOTALKILLS"), 500, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_DM_TOTAL_DEATHS"), 412, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TIMES_FINISH_DM_TOP_3"), 36, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_PLAYER_HEADSHOTS"), 623, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_DM_WINS"), 63, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_TDM_WINS"), 13, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_GTA_RACES_WON"), 12, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_GOLF_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_SHOOTRANG_TG_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_SHOOTRANG_RT_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_SHOOTRANG_CT_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_SHOOTRANG_GRAN_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FM_TENNIS_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TENNIS_MATCHES_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TOTAL_TDEATHMATCH_WON"), 63, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TOTAL_RACES_WON"), 101, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TOTAL_DEATHMATCH_LOST"), 23, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_MPPLY_TOTAL_RACES_LOST"), 36, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_25_KILLS_STICKYBOMBS"), 50, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_50_KILLS_GRENADES"), 50, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_GRENADE_ENEMY_KILLS "), 50, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_20_KILLS_MELEE"), 50, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_WIN_CAPTURES"), 25, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_DROPOFF_CAP_PACKAGES"), 100, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_KILL_CARRIER_CAPTURE"), 100, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FINISH_HEISTS"), 50, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FINISH_HEIST_SETUP_JOB"), 50, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_NIGHTVISION_KILLS, 100"), 1, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_WIN_LAST_TEAM_STANDINGS"), 50, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_ONLY_PLAYER_ALIVE_LTS"), 50, 1);
                STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMRACEWORLDRECHOLDER"), 1, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_ENEMYDRIVEBYKILLS"), 600, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_USJS_COMPLETED"), 50, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_USJS_FOUND"), 50, true);
                STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMWINALLRACEMODES"), 1, true);
                STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMWINEVERYGAMEMODE"), 1, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_DB_PLAYER_KILLS"), 1000, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_KILLS_PLAYERS"), 1000, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMHORDWAVESSURVIVE"), 21, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_CAR_BOMBS_ENEMY_KILLS"), 25, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_TDM_MVP"), 60, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_HOLD_UP_SHOPS"), 20, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_RACES_WON"), 101, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_NO_ARMWRESTLING_WINS"), 21, true);
                STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMATTGANGHQ"), 1, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMBBETWIN"), 50000, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_DM_WINS"), 51, true);
                STATS::STAT_SET_BOOL(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FMFULLYMODDEDCAR"), 1, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_DM_TOTALKILLS"), 500, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_DM_TOTAL_DEATHS"), 412, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TIMES_FINISH_DM_TOP_3"), 36, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_PLAYER_HEADSHOTS"), 623, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_DM_WINS"), 63, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_TDM_WINS"), 13, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_GTA_RACES_WON"), 12, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_GOLF_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_SHOOTRANG_TG_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_SHOOTRANG_RT_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_SHOOTRANG_CT_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_SHOOTRANG_GRAN_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FM_TENNIS_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TENNIS_MATCHES_WON"), 2, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TOTAL_TDEATHMATCH_WON"), 63, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TOTAL_RACES_WON"), 101, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TOTAL_DEATHMATCH_LOST"), 23, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_MPPLY_TOTAL_RACES_LOST"), 36, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_25_KILLS_STICKYBOMBS"), 50, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_50_KILLS_GRENADES"), 50, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_GRENADE_ENEMY_KILLS "), 50, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_20_KILLS_MELEE"), 50, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_WIN_CAPTURES"), 25, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_DROPOFF_CAP_PACKAGES"), 100, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_KILL_CARRIER_CAPTURE"), 100, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FINISH_HEISTS"), 50, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_FINISH_HEIST_SETUP_JOB"), 50, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_NIGHTVISION_KILLS, 100"), 1, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_WIN_LAST_TEAM_STANDINGS"), 50, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_AWD_ONLY_PLAYER_ALIVE_LTS"), 50, 1);
            }
            if (Menu::Option("Unlock Chrome Rims"))
            {
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_WIN_CAPTURES"), 25, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_DROPOFF_CAP_PACKAGES"), 100, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_KILL_CARRIER_CAPTURE"), 100, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FINISH_HEISTS"), 50, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_FINISH_HEIST_SETUP_JOB"), 50, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_NIGHTVISION_KILLS, 100"), 1, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_WIN_LAST_TEAM_STANDINGS"), 50, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_AWD_ONLY_PLAYER_ALIVE_LTS"), 50, 1);
            }
            if (Menu::Option("Clear Badsport")) {
                STATS::STAT_SET_FLOAT(GAMEPLAY::GET_HASH_KEY("MPPLY_OVERALL_BADSPORT"), 0.0f, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MPPLY_DESTROYED_PVEHICLES"), 0, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MPPLY_BADSPORT_MESSAGE"), 0, true);
            }
            if (Menu::Option("Redesign Character")) {
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_FM_CHANGECHAR_ASKED"), 0, 1);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_FM_CHANGECHAR_ASKED"), 0, 1);
            }
            if (Menu::Option("Allow Gender Change"))
            {
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP0_ALLOW_GENDER_CHANGE"), 52, true);
                STATS::STAT_SET_INT(GAMEPLAY::GET_HASH_KEY("MP1_ALLOW_GENDER_CHANGE"), 52, true);
            }

        }
        break;
        case world:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("WORLD");
            Menu::MenuOption("Weather", weather);
            Menu::MenuOption("Time", worldtime);
            if (Menu::Float("Wave Intensity", waveintensity, 0.0f, 100.0f))
            {
                if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                {
                    wavechange = true; 
                    WATER::SET_DEEP_OCEAN_SCALER(waveintensity);
                    wavechange = false;
                }
            }
            if (Menu::Float("Wind Speed", windspeed, 0.0f, 12.0f))
            {
                if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                {
                    windchange = true;
                    GAMEPLAY::SET_WIND_SPEED(windspeed);
                    windchange = false;
                }
            }
            Menu::Int("Gravity Level", gravitylevel, 0, 3);
        }
        break;
        case weather:
        {
            Menu::Title(MENU_NAME);
            if (NETWORK::NETWORK_IS_SESSION_STARTED())
            {
                Menu::SubTitle("ONLINE WEATHER");
                if (Menu::Option("Extra Sunny")) SetGameWeather(0); 
                if (Menu::Option("Clear")) SetGameWeather(1); 
                if (Menu::Option("Clouds")) SetGameWeather(2);
                if (Menu::Option("Smog")) SetGameWeather(3); 
                if (Menu::Option("Foggy")) SetGameWeather(4); 
                if (Menu::Option("Rain")) SetGameWeather(6); 
                if (Menu::Option("Thunder")) SetGameWeather(7); 
                if (Menu::Option("Clearing")) SetGameWeather(8); 
                if (Menu::Option("Neutral")) SetGameWeather(9); 
                if (Menu::Option("Snow")) SetGameWeather(10); 
                if (Menu::Option("Blizzard")) SetGameWeather(11); 
                if (Menu::Option("Light Snow")) SetGameWeather(12); 
                if (Menu::Option("Cristmas")) SetGameWeather(13);
                if (Menu::Option("Halloween")) SetGameWeather(14); 
                if (Menu::Option("Black Screen")) SetGameWeather(15); 
            }
            else
            {
                Menu::SubTitle("OFFLINE WEATHER");
                if (Menu::Option("Extra Sunny")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Extrasunny"); 
                if (Menu::Option("Clear")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Clear"); 
                if (Menu::Option("Clouds")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Clouds"); 
                if (Menu::Option("Smog")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Smog"); 
                if (Menu::Option("Foggy")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Foggy");
                if (Menu::Option("Rain")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Rain"); 
                if (Menu::Option("Thunder")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Thunder"); 
                if (Menu::Option("Clearing")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Clearing"); 
                if (Menu::Option("Neutral")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Neutral"); 
                if (Menu::Option("Snow")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Snow"); 
                if (Menu::Option("Blizzard")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Blizzard"); 
                if (Menu::Option("Light Snow")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Snowlight"); 
                if (Menu::Option("Cristmas")) GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST("Xmas");
            }
        }
        break;
        case worldtime:
        {
            Menu::Title(MENU_NAME);
            if (NETWORK::NETWORK_IS_SESSION_STARTED() && onlinePlayerCount() > 1)
            {
                Menu::SubTitle("ONLINE TIME");
                if (Menu::Option("Morning")) { SetGameTime(5); }
                if (Menu::Option("Noon")) { SetGameTime(10); }
                if (Menu::Option("Afternoon")) { SetGameTime(15); }
                if (Menu::Option("Night")) { SetGameTime(20); }
                if (Menu::Option("Midnight")) { SetGameTime(23, 59); }
                Menu::Break("Custom Time");
                if (Menu::Toggle("Stop Time [Local]", pauseclock)) if (pauseclock) if (timelaps) timelaps = false;
                if (Menu::Int("Set Hour", worldhours, 0, 23))
                {
                    if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                    {
                        hourchange = true;
                        SetGameTime(worldhours, worldminutes, worldseconds);
                        hourchange = false;
                    }
                }
                if (Menu::Int("Set Minutes", worldminutes, 0, 59))
                {
                    if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                    {
                        minutechange = true;
                        SetGameTime(worldhours, worldminutes, worldseconds);
                        minutechange = false;
                    }
                }
                if (Menu::Int("Set Seconds", worldseconds, 0, 59))
                {
                    if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                    {
                        secondchange = true;
                        SetGameTime(worldhours, worldminutes, worldseconds);
                        secondchange = false;
                    }
                }
            }
            else
            {
                Menu::SubTitle("OFFLINE TIME");
                if (Menu::Option("Morning")) { TIME::SET_CLOCK_TIME(5, 0, 0); }
                if (Menu::Option("Noon")) { TIME::SET_CLOCK_TIME(10, 0, 0); }
                if (Menu::Option("Afternoon")) { TIME::SET_CLOCK_TIME(15, 0, 0); }
                if (Menu::Option("Night")) { TIME::SET_CLOCK_TIME(20, 0, 0); }
                if (Menu::Option("Midnight")) { TIME::SET_CLOCK_TIME(23, 59, 0); }
                Menu::Break("Custom Time");
                if (Menu::Toggle("Stop Time [Local]", pauseclock)) if (pauseclock) if (timelaps) timelaps = false;
                if (Menu::Int("Set Hour", worldhours, 0, 23))
                {
                    if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                    {
                        hourchange = true;
                        TIME::SET_CLOCK_TIME(worldhours, worldminutes, worldseconds);
                        hourchange = false;
                    }
                }
                if (Menu::Int("Set Minutes", worldminutes, 0, 59))
                {
                    if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                    {
                        minutechange = true;
                        TIME::SET_CLOCK_TIME(worldhours, worldminutes, worldseconds);
                        minutechange = false;
                    }
                }
                if (Menu::Int("Set Seconds", worldseconds, 0, 59))
                {
                    if (Menu::Settings::leftPressed || Menu::Settings::rightPressed || Menu::Settings::selectPressed)
                    {
                        secondchange = true;
                        TIME::SET_CLOCK_TIME(worldhours, worldminutes, worldseconds);
                        secondchange = false;
                    }
                }
            }
            Menu::Break("Time Laps");
            if (Menu::ToggleInt("Time Laps [Local]", timelaps, timelapsscale, 0, 1000, 10, false)) if (timelaps) if (pauseclock) pauseclock = false;
        }
        break;
		case settings:
		{
            Menu::Title(MENU_NAME);
			Menu::SubTitle("SETTINGS");
			Menu::MenuOption("Menu Visuals", menuvisuals);
            Menu::Int("Scroll Delay", Menu::Settings::keyPressDelay2, 75, 250, 5);
            Menu::Int("Int Delay", Menu::Settings::keyPressDelay3, 75, 250, 5);
            Menu::MenuOption("~r~EXIT GTA", exitgta);
		}
		break;
        case menuvisuals:
        {
            Menu::Title(MENU_NAME);
            Menu::SubTitle("MENU VISUALS");
            Menu::Toggle("Smooth Scroll", Menu::Settings::smoothScroll);
            if (Menu::Option("Menu Pos Right")) {
                Menu::Settings::menuX = 0.88f;
                menuX = 20;
            }
            if (Menu::Option("Menu Pos Left")) {
                Menu::Settings::menuX = 0.12f;
                menuX = 1;
            }
            if (Menu::Int("Menu X", menuX, 1, 20)) {
                Menu::Settings::menuX = (0.08f + (((float)menuX) / 25));
            }
            if (Menu::Int("Menu Y", menuY, 1, 20)) {
                Menu::Settings::menuY = (0.08f + (((float)menuY) / 25));
            }
        }
        break;
		case exitgta:
		{
            Menu::Title(MENU_NAME);
			Menu::SubTitle("EXIT GTA");
			if (Menu::Option("~r~Yes")) exit(0);
            if (Menu::Option("No"))
            {
                Menu::Settings::currentMenu = mainmenu;
                Menu::Settings::currentOption = 1;
            }
		}
		break;
		}
        Menu::Scrollbar();
        Menu::End();
        WAIT(0);
	}
}

void ScriptMain() {
	srand(GetTickCount());
	main();
}
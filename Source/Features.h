#pragma once

bool FileExists(const std::string& fileName); 
std::string DocumentsPath();
void setPlayerModel(Hash modelHash);
int onlinePlayerCount();
Vector3 addVector(Vector3 vector, Vector3 vector2);
Vector3 RotationToDirection(Vector3 rot);
Vector3 multiplyVector(Vector3 vector, float inc);
float getDistance(Vector3 pointA, Vector3 pointB);
char* CharKeyboard(char* windowName = "", int maxInput = 21, char* defaultText = "");
double NumberKeyboard(double originalValue, double min, double max);
double NumberKeyboard(double originalValue, double min, double max, double step);
void CreateDrop(Hash pickupHash, Hash modelHash, Vector3 droppos, float drophight = 1.5f, float dropspread = 0.0f, int value = 0);
void iconNotify(char* subject, char* message);
void notifyMap(char* fmt, ...);
void notifyMap(std::string str);
Vector3 GetBlipMarker();
void TeleportToMarker();
void DrawPlayerMarker(Player player);
void LoadPlayerInfo(Player player);
void SetGameWeather(int weatherID);
void SetGameTime(int h, int m = 0, int s = 0);
void RequestControlOfid(Entity netid);
void RequestControlOfEnt(Entity entity);
void DoAnimation(Player player, char* anim, char* animid);
void DoScenario(Player player, char* scenario);
void CrashPlayer(Player player);
void OnlineStealth(int amount);
void UnlockAll();
Ped SpawnPed(int pedType, Hash pedModel, Vector3 spawnCoords);
void StartPedGuardTask(Player toProtect, Ped guardPed, Hash weaponHash = 0x0, int aimAccuracy = 80);
void StartPedEnemyTask(Player toAttack, Ped enemyPed, Hash weaponHash = 0x0, int aimAccuracy = 80);
void StartFollowTaskOnVehicle(Player toFollow, Vehicle vehicle, Ped driverPed, bool fillUpVehicle = false);
void AttachObjectToPlayer(char* name);
void DetachObjectsFromPlayer(Player player);
Vehicle SpawnVehicle(Hash hash, float x, float y, float z);
extern bool spawnmaxed;
void OnlineKickBypassToVehicle(Vehicle vehicle);
void SetVehicleMaxed(Vehicle vehicle);
Vehicle SpawnVehicleOnPlayer(std::string hash);
void SpawnModVehicleOnPlayer(std::string hash);

//Toggled Functions

extern int healthsave;
extern int sethealth;
extern bool healthchange;
extern bool sethealthbool;
extern int setarmour;
extern bool armourchange;
extern bool setarmourbool;
extern int wantedlevel;
extern bool wantedlevelbool;
extern int visualwantedlevel;
extern bool visualwantedlevelbool;
extern bool godmode;
extern bool neverwanted;
extern bool alwayswanted;
extern float fastrunspeed;
extern float fastswimspeed;
extern bool superrun;
extern float superrunspeed;
extern bool sliderun;
extern float sliderunspeed;
extern bool superjump;
extern bool noragdoll;
extern bool seatbelt;
extern int healincoverint;
extern bool healincover;
extern int autohealint;
extern bool autoheal;
extern bool slowmotion;
extern float timescale;
extern bool vomit;
extern bool firebreath;
extern float noclipspeed;
extern bool noclip;
extern bool walkingnoclip;
extern bool invisible;
extern bool alphachange;
extern int playeralpha;
extern bool offradar;
extern bool walkunderwater;
extern bool superman;
extern bool offradar1;
extern bool antiafk;
extern bool bullshark;
extern bool bypasschat;
extern bool badsport;
extern bool tinyplayer;
extern bool autoclean;
extern bool walkonwater;
extern bool forcefield;

//Outfit Stuff
extern bool outfitchange;
extern bool forceoutfit; 

extern int face;
extern int mask;
extern int hair;
extern int torso;
extern int leg;
extern int bag;
extern int shoes;
extern int accessory;
extern int undershirt;
extern int kevlar;
extern int badge;
extern int torso2;

void SelfOptions();

extern bool infammo;
extern bool noreload;
extern bool triggerbot;
extern int moneygunvalue;
extern int moneygunvalue2;
extern bool moneygun;
extern bool moneygun2;
extern bool rpgun;
extern bool rpgun2;
extern int explosiveammoint;
extern int explosiveammoint2;
extern bool explosiveammo;
extern bool explosiveammo2;
extern bool explosivemelee;
extern bool explosivemelee2;
extern bool aimassist;
extern bool airstrikegun;
extern bool airstrikegun2;
extern bool newairstrikegun;
extern bool newairstrikegun2;
extern bool rapidfire;
extern bool shootshockwave;
extern bool flamethrower;
extern bool shootrpg;
extern bool gravitygun;
extern bool teleportgun;
extern bool deletegun;
extern bool deletegun2;
extern bool jetgun;

void WeaponOptions();

extern float rgbmulti;
extern float rgbmulti2;
extern int powermultiplier;
extern int torquemultiplier;
extern int accelerationmultiplier;
extern int brakemultiplier;
extern int downforcemultiplier;
extern int hornboostspeed;
extern bool vehgodmode;
extern bool autofix;
extern int vehicleflyspeed;
extern bool vehiclefly;
extern bool driftmode;
extern bool hornboost;
extern bool rainbowcolor;
extern bool rainbowtyresmoke;
extern bool rampvehicle;
extern bool sticktoground;
extern bool autoflip;
extern bool autopilotstart;
extern bool instantboost;
extern bool unlimitedboost;
extern bool driveonwater;
extern bool novehiclegravity;

extern bool vehmodchange;

extern bool turbo;
extern bool tiresmoke;
extern bool xenon;
extern int windowtint;
extern int vehicleMods[25];

void VehicleOptions();

extern bool scripteventprot;
extern bool missionprot;
extern bool inviteprot;
extern bool soundspamprot;
extern bool nonhostkickprot;
extern bool ceobanprot;
extern bool ceokickprot;
extern bool bountyprot;
extern bool vehkickprot;
extern bool transactionerrorprot;
extern bool pickupprot;
extern bool explodeprot;
extern bool fireprot;
extern bool ptfxprot;
extern bool autodetach;
extern bool autoplayerdetach; 
extern bool deleteclones;
extern bool autouncage;
extern bool timeprot;
extern bool weatherprot;
extern bool freezeprot;
extern bool controllprot;

void Protections();

extern bool autoteleport;

void TeleportOptions();

extern bool freeze;
extern bool spectate;
extern bool pukick;
extern int pukickcount;
extern bool playeresp;
extern bool pedesp;

void OnlineFunctions();

extern float dropspread;
extern float dropheight;
extern int dropdelay;
extern int dropamount;
extern bool moneydrop;
extern bool rpdrop;
extern bool medidrop;
extern bool armourdrop;
extern bool ammodrop;

void RecoveryOptions();

extern float spawnspread;
extern int bodyguardhealth;
extern int bodyguardaccuracy;
extern bool bodyguardinvincible;
extern bool bodyguardnoragdoll; 

void MiscOptions();

extern bool hourchange;
extern int worldhours;
extern bool minutechange;
extern int worldminutes;
extern bool secondchange;
extern int worldseconds;
extern bool wavechange;
extern float waveintensity;
extern bool windchange; 
extern float windspeed;
extern int gravitylevel;
extern bool pauseclock;
extern bool timelaps;
extern int timelapsscale;

void WorldOptions();

void FeaturesMain();

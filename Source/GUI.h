#pragma once

enum SubMenus { //Add Sub Menus in here
    nomenu,
	mainmenu,
	test,
    selfmenu,
	commonmenu,
	healthmenu,
	wantedmenu,
	visualsmenu,
    movementmenu,
	onlinemenu,
	animmenu,
	modelsmenu,
	outfitsmenu,
	miscmenu,
	weaponmenu,
	explosiveweaponmenu,
	propweaponmenu,
	ammoweaponmenu,
	aimweaponmenu,
	miscweaponmenu,
    playerlist,
    playermenu,
	playerpeaceful,
	playertrolls,
	playerattachments,
	playerdrops,
	vehiclespawner,
	vehicleoptions,
	lossantoscustoms,
	protections,
	teleports,
	recovery,
	world,
	weather,
	worldtime,
    settings,
    menuvisuals,
    exitgta,

	//Spawner Submenus

	modvehicles,
	updatevehicles,
	supersport,
	sports,
    sportsclassic,
	offroad,
	sedans,
	coupes,
	muscle,
	boats,
	commercials, 
	compacts,
	cycles,
	emergency,
	helicopters,
	industrial,
	military,
	motorcycles,
	planes,
	suvs, 
	trailers,
	trains,
	utility,
	vans,
};
extern enum SubMenus;

typedef struct VECTOR2 {
	float x, y;
};
typedef struct VECTOR2_2 {
	float w, h;
};
typedef struct RGBAF {
	int r, g, b, a, f;
};
typedef struct RGBA {
	int r, g, b, a;
};
typedef struct RGB {
	int r, g, b;
};
namespace Cursor {
	int getScrolling();
	VECTOR2 cursorPos();
	bool onHeader();
	bool onMenu();
	bool onOption();
	int currentOption();
	void moveScroller();
}
namespace Menu {
	namespace Drawing {
		void Assets();
		void Text(const char* text, RGBAF rgbaf, VECTOR2 position, VECTOR2_2 size, bool center, bool outline = false);
		void Rect(RGBA rgba, VECTOR2 position, VECTOR2_2 size);
		void Spriter(std::string Streamedtexture, std::string textureName, float x, float y, float width, float height, float rotation, int r, int g, int b, int a);
	}
	namespace Settings {
		extern bool showCursor;
		extern bool vipUser;
		extern bool smoothScroll;
		extern bool assetsLoaded;
		extern bool selectPressed;
		extern bool leftPressed;
		extern bool rightPressed;
		extern bool center;
		extern bool controllerinput;
		extern bool movingMenu;
        extern bool scrollbar;

		extern int maxOptions;
		extern int currentOption;
		extern int currentMouseOption;
		extern float scrolledOption;
		extern int optionCount;

		extern SubMenus currentMenu;
		extern int menuLevel;
		extern int optionsArray[1000];
		extern SubMenus menusArray[1000];

		extern float menuX;
		extern float menuY;
		extern RGBAF titleText;
		extern RGBA titleRect;
		extern RGBAF breakText;
		extern RGBAF selectedBreak;
        extern RGBA headerRect;
		extern RGBA footerRect;
		extern RGBAF optionText;
		extern RGBAF selectedText;
        extern RGBAF optionArrow;
		extern RGBAF selectedArrow;
		extern RGBAF arrow;
		extern RGBAF integre;
		extern RGBA optionRect;
		extern RGBA scroller;
		extern RGBA line;

		extern int keyPressDelay;
		extern int keyPressPreviousTick;
		extern int keyPressDelay2;
		extern int keyPressPreviousTick2;
		extern int keyPressDelay3;
		extern int keyPressPreviousTick3;
	}
	namespace MenuLevelHandler {
		void MoveMenu(SubMenus menu);
        void ExitMenu();
		void BackMenu();
	}
	namespace Checks {
		void Controlls();
	}
	namespace Tools {
		char* StringToChar(std::string string);
	}
	namespace Files {
		void WriteStringToIni(std::string string, std::string file, std::string app, std::string key);
		std::string ReadStringFromIni(std::string file, std::string app, std::string key);
		void WriteIntToIni(int intValue, std::string file, std::string app, std::string key);
		int ReadIntFromIni(std::string file, std::string app, std::string key);
		void WriteFloatToIni(float floatValue, std::string file, std::string app, std::string key);
		float ReadFloatFromIni(std::string file, std::string app, std::string key);
		void WriteBoolToIni(bool b00l, std::string file, std::string app, std::string key);
		bool ReadBoolFromIni(std::string file, std::string app, std::string key);
	}
	VECTOR2 GetWindowResolution();
	void Title(const char* title);
	void Header(char* Streamedtexture, char* textureName);
	void info(const char * title);
	void Vehicle(std::string texture1, std::string texture2);
	void AddSmallTitle(char* text);
	void AddSmallInfo(char* text, short line);
	void AddSmallEnd(short line);
	bool Break(const char* option);
	void Scroller();
    void Scrollbar();
	bool Option(const char* option);
	bool Option(const char* option, std::function<void()> function);
	bool MenuOption(const char* option, SubMenus newSub);
	bool MenuOption(const char* option, SubMenus newSub, std::function<void()> function);
    void SubTitle(const char* title);
	bool Toggle(const char* option, bool& b00l);
	bool Toggle(const char* option, bool& b00l, std::function<void()> function);
	bool ToggleInt(const char* option, bool& b00l, int& _int, int min, int max, bool inputwhenactive = false);
	bool ToggleInt(const char* option, bool& b00l, int& _int, int min, int max, int step, bool inputwhenactive);
	bool ToggleFloat(const char* option, bool& b00l, float& _float, float min, float max, bool inputwhenactive = false);
	bool ToggleFloat(const char* option, bool& b00l, float& _float, float min, float max, float step, bool inputwhenactive);
	bool Int(const char* option, int& _int, int min, int max);
	bool Int(const char* option, int& _int, int min, int max, int step);
	bool Int(const char* option, int& _int, int min, int max, std::function<void()> function);
	bool Int(const char* option, int& _int, int min, int max, int step, std::function<void()> function);
	bool Float(const char* option, float& _float, float min, float max);
	bool Float(const char* option, float& _float, float min, float max, float step);
	bool Float(const char* option, float& _float, float min, float max, std::function<void()> function);
	bool Float(const char* option, float& _float, float min, float max, float step, std::function<void()> function);
	bool IntVector(const char* option, std::vector<int> Vector, int& position);
	bool IntVector(const char* option, std::vector<int> Vector, int& position, std::function<void()> function);
	bool FloatVector(const char* option, std::vector<float> Vector, int& position);
	bool FloatVector(const char* option, std::vector<float> Vector, int& position, std::function<void()> function);
	bool StringVector(const char* option, std::vector<std::string> Vector, int& position);
	bool StringVector(const char* option, std::vector<std::string> Vector, int& position, std::function<void()> function);
	bool StringVector(const char* option, std::vector<char*> Vector, int& position);
	bool StringVector(const char* option, std::vector<char*> Vector, int& position, std::function<void()> function);
    void End();
}

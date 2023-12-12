#include "stdafx.h"
#include "Features.h"
#include <experimental/filesystem>

namespace filesystem = std::experimental::filesystem;


VECTOR2 Menu::GetWindowResolution()
{
	RECT desktop;
	const HWND hDesktop = FindWindowA("grcWindow", "Grand Theft Auto V"); /*GetDesktopWindow();*/
	GetClientRect(hDesktop, &desktop);
	VECTOR2 resVec;
	resVec.x = (float)desktop.right;
	resVec.y = (float)desktop.bottom;
	return resVec;
}

int Cursor::getScrolling()
{
	int result = 0;
	if (CONTROLS::GET_DISABLED_CONTROL_NORMAL(2, 241)) //Up
		result = 1;
	if (CONTROLS::GET_DISABLED_CONTROL_NORMAL(2, 242)) //Down
		result = -1;
	return result;
}

VECTOR2 Cursor::cursorPos()
{
	
	//Using Win-API (OLD)
	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(FindWindowA("grcWindow", "Grand Theft Auto V"), &cursor);
	VECTOR2 screenVec = Menu::GetWindowResolution();

	VECTOR2 cursorVec;
	cursorVec.x = ((float)cursor.x) / screenVec.x;
	cursorVec.y = ((float)cursor.y) / screenVec.y;

	//Using Native
	//cursorVec.x = CONTROLS::GET_DISABLED_CONTROL_NORMAL(2, 239);
	//cursorVec.y = CONTROLS::GET_DISABLED_CONTROL_NORMAL(2, 240);

	if (Menu::Settings::showCursor) {
		return cursorVec;
	}
	else {
		VECTOR2 zero = { 0.0f, 0.0f };
		return zero;
	}
}

bool Cursor::onHeader()
{
	float cursorX = cursorPos().x;
	float cursorY = cursorPos().y;
	if (cursorX > Menu::Settings::menuX - (0.21f / 2.0f) && cursorX < Menu::Settings::menuX + (0.21f / 2.0f) && cursorY >(Menu::Settings::menuY - (0.085f / 2.0f)) && cursorY < (Menu::Settings::menuY + (0.085f / 2.0f)))
	{
		return true;
	}
	return false;
}

bool Cursor::onMenu()
{
	float cursorX = cursorPos().x;
	float cursorY = cursorPos().y;
	if (cursorX > Menu::Settings::menuX - (0.21f / 2.0f) && cursorX < Menu::Settings::menuX + (0.21f / 2.0f) && cursorY > (Menu::Settings::menuY - (0.085f / 2.0f)) && cursorY < Menu::Settings::menuY + 0.059f + ((Menu::Settings::optionCount <= Menu::Settings::maxOptions ? (Menu::Settings::optionCount + 1) : (Menu::Settings::maxOptions + 1)) * 0.035f) + (0.035f / 2.0f))
	{
		return true;
	}
	return false;
}

bool Cursor::onOption()
{
	float cursorX = cursorPos().x;
	float cursorY = cursorPos().y;
	if (cursorX > Menu::Settings::menuX - (0.21f / 2.0f) && cursorX < Menu::Settings::menuX + (0.21f / 2.0f) && cursorY > Menu::Settings::menuY + 0.059f + (0.035f / 2.0f) && cursorY < Menu::Settings::menuY + 0.059f + ((Menu::Settings::optionCount < Menu::Settings::maxOptions ? Menu::Settings::optionCount : Menu::Settings::maxOptions) * 0.035f) + (0.035f / 2.0f))
	{
		return true;
	}
	return false;
}

float optionHeight(int Option)
{
	return (Menu::Settings::menuY + 0.059f + (0.035f * Option)) - 0.035f / 2.0f;
}

int Cursor::currentOption()
{
	if (onOption() && Menu::Settings::optionCount > 1)
	{
		float cursorY = cursorPos().y;
		for (int i = 1; i < (Menu::Settings::optionCount < Menu::Settings::maxOptions ? Menu::Settings::optionCount : Menu::Settings::maxOptions); i++)
		{
			if (!onOption() || Menu::Settings::optionCount < 2)
				return 1;

			if (cursorY > optionHeight(i) && cursorY < optionHeight(i + 1))
			{
				return i;
			}
		}
	}
	else
		return 1;
}

void Cursor::moveScroller()
{
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		if ((GetTickCount() - Menu::Settings::keyPressPreviousTick2) > Menu::Settings::keyPressDelay2) {
			if (Menu::Settings::currentOption > Menu::Settings::maxOptions)
				Menu::Settings::currentOption = (Menu::Settings::currentOption + (Menu::Settings::currentMouseOption - Menu::Settings::maxOptions));
			else
				Menu::Settings::currentOption = Menu::Settings::currentMouseOption;
			Menu::Settings::keyPressPreviousTick2 = GetTickCount();
		}
	}

	Menu::Settings::currentMouseOption = currentOption();
}

void Menu::Drawing::Text(const char * text, RGBAF rgbaf, VECTOR2 position, VECTOR2_2 size, bool center, bool outline)
{
	UI::SET_TEXT_CENTRE(center);
	UI::SET_TEXT_COLOUR(rgbaf.r, rgbaf.g, rgbaf.b, rgbaf.a);
	UI::SET_TEXT_FONT(rgbaf.f);
	UI::SET_TEXT_SCALE(size.w, size.h);
	if (outline)
	{
		//UI::SET_TEXT_DROPSHADOW(1, 0, 0, 0, 0);
		//UI::SET_TEXT_EDGE(1, 0, 0, 0, 0);
		UI::SET_TEXT_OUTLINE();
	}
	UI::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
	UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME((char*)text);
	UI::END_TEXT_COMMAND_DISPLAY_TEXT(position.x, position.y);
}

void Menu::Drawing::Spriter(std::string Streamedtexture, std::string textureName, float x, float y, float width, float height, float rotation, int r, int g, int b, int a)
{
	if (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED((char*)Streamedtexture.c_str()))
	{
		GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT((char*)Streamedtexture.c_str(), false);
	}
	else
	{
		GRAPHICS::DRAW_SPRITE((char*)Streamedtexture.c_str(), (char*)textureName.c_str(), x, y, width, height, rotation, r, g, b, a);
	}
}

void Menu::Drawing::Rect(RGBA rgba, VECTOR2 position, VECTOR2_2 size)
{
	GRAPHICS::DRAW_RECT(position.x, position.y, size.w, size.h, rgba.r, rgba.g, rgba.b, rgba.a);
}

float Menu::Settings::menuX = 0.88f;
float Menu::Settings::menuY = 0.12f;
bool Menu::Settings::showCursor = false;
bool Menu::Settings::vipUser = false;
bool Menu::Settings::smoothScroll = true;
bool Menu::Settings::assetsLoaded = false;
bool Menu::Settings::selectPressed = false;
bool Menu::Settings::leftPressed = false;
bool Menu::Settings::rightPressed = false;
bool firstopen = true;
bool Menu::Settings::scrollbar = false;
int Menu::Settings::maxOptions = 13;
int Menu::Settings::currentOption = 0;
int Menu::Settings::currentMouseOption = 0;
float Menu::Settings::scrolledOption = 1.0f;
int Menu::Settings::optionCount = 0;
SubMenus Menu::Settings::currentMenu;
int Menu::Settings::menuLevel = 0;
int Menu::Settings::optionsArray[1000];
SubMenus Menu::Settings::menusArray[1000];

RGBAF Menu::Settings::titleText{ 0, 255, 187, 255, 1 };
RGBA Menu::Settings::titleRect{ 38, 36, 57, 255 };
RGBAF Menu::Settings::optionText{ 255, 255, 255, 255, 0 };
RGBAF Menu::Settings::selectedText{ 38, 36, 57, 255, 0 };
RGBAF Menu::Settings::breakText{ 255, 255, 255, 255, 1 };
RGBAF Menu::Settings::selectedBreak{ 15, 0, 30, 255, 1 };
RGBA Menu::Settings::headerRect{ 20, 0, 40, 255 };
RGBA Menu::Settings::footerRect{ 20, 0, 40, 255 };
RGBAF Menu::Settings::optionArrow{ 255, 255, 255, 255, 2 };
RGBAF Menu::Settings::selectedArrow{ 38, 36, 57, 255, 2 };		
RGBA Menu::Settings::optionRect{ 44, 42, 66, 255 };
RGBA Menu::Settings::scroller{ 0, 0, 0, 245 };
RGBAF Menu::Settings::integre{ 255, 255, 255, 255, 4 };
RGBA Menu::Settings::line{ 100, 0, 170, 255 };

void Menu::Drawing::Assets()
{
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

	int textureID;
	if (FileExists(m_FilePath.u8string().c_str()))
		Hooking::file_register(&textureID, m_FilePath.u8string().c_str(), true, "Pepega.ytd", false);
}

void Menu::Title(const char * title)
{	

	Drawing::Assets();
	//ChangeFrames();
	if (Settings::assetsLoaded)
	{
		Drawing::Spriter("Textures", "header", Settings::menuX, Settings::menuY, 0.21f, 0.085f, 0, 255, 255, 255, 255);
	}
	else
	{
		Drawing::Text(title, Settings::titleText, { Settings::menuX, Settings::menuY - 0.0325f }, { 1.17f, 1.17f }, true);
		Drawing::Rect(Settings::titleRect, { Settings::menuX, Settings::menuY }, { 0.21f, 0.085f });
	}

	// Disable Controls
	UI::HIDE_HELP_TEXT_THIS_FRAME();
	CAM::SET_CINEMATIC_BUTTON_ACTIVE(0);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(10);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(6);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(7);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(9);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(8);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_NEXT_CAMERA, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_CHARACTER_WHEEL, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_MELEE_ATTACK_LIGHT, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_MELEE_ATTACK_HEAVY, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_MULTIPLAYER_INFO, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_PHONE, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_MELEE_ATTACK_ALTERNATE, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_VEH_CIN_CAM, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_MAP_POI, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_VEH_RADIO_WHEEL, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_VEH_HEADLIGHT, true);
}

void Menu::Header(char* Streamedtexture, char* textureName)
{
	Drawing::Assets();
	//ChangeFrames();
	Drawing::Spriter(Streamedtexture, textureName, Settings::menuX, Settings::menuY, 0.21f, 0.085f, 0, 255, 255, 255, 255);

	// Disable Controls
	UI::HIDE_HELP_TEXT_THIS_FRAME();
	CAM::SET_CINEMATIC_BUTTON_ACTIVE(0);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(10);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(6);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(7);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(9);
	UI::HIDE_HUD_COMPONENT_THIS_FRAME(8);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_NEXT_CAMERA, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_CHARACTER_WHEEL, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_MELEE_ATTACK_LIGHT, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_MELEE_ATTACK_HEAVY, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_MULTIPLAYER_INFO, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_PHONE, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_MELEE_ATTACK_ALTERNATE, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_VEH_CIN_CAM, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_MAP_POI, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_VEH_RADIO_WHEEL, true);
	CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_VEH_HEADLIGHT, true);
}

void Menu::SubTitle(const char* title)
{
	Drawing::Text(title, Settings::optionText, { Settings::menuX, Settings::menuY + 0.0455f }, { 0.365f, 0.365f }, true);
}

float getScrollSpeed(int scrollDelayMS)
{
	return (1.0f / ((1.0f / GAMEPLAY::GET_FRAME_TIME()) / 1000 * scrollDelayMS));
}

float getDistance(float x, float y)
{
	return x > y ? x - y : y - x;
}

float getSpeedIncrease()
{
	return ((((getDistance((float)Menu::Settings::currentOption, Menu::Settings::scrolledOption) < Menu::Settings::maxOptions ? getDistance((float)Menu::Settings::currentOption, Menu::Settings::scrolledOption) : Menu::Settings::maxOptions) / 25) / 60/*60 Frames Base*/) * (1.0f / GAMEPLAY::GET_FRAME_TIME()));
}

void Menu::Scroller()
{
	if (Settings::optionCount > 0 && !Settings::movingMenu)
	{
		//Drawing Background Rect
		if (Settings::optionCount < Settings::maxOptions)
			Drawing::Rect(Menu::Settings::optionRect, { Settings::menuX, ((float)Settings::optionCount / 2.0f) * 0.035f + Settings::menuY + 0.0765f }, { 0.21f, ((float)Settings::optionCount * 0.035f) });
		else
			Drawing::Rect(Menu::Settings::optionRect, { Settings::menuX, ((float)Settings::maxOptions / 2.0f) * 0.035f + Settings::menuY + 0.0765f }, { 0.21f, (float)Settings::maxOptions * 0.035f });

		//Moving the Scroller
		if (!Settings::smoothScroll)
			Settings::scrolledOption = (float)Settings::currentOption;
		else
		{
			//Correcting Over End Bug
			if (Settings::scrolledOption > (float)Settings::optionCount)
				Settings::scrolledOption = (float)Settings::optionCount;

			//Correcting little Offset
			if (Settings::scrolledOption >= (float)Settings::currentOption - getScrollSpeed(Settings::keyPressDelay2) && Settings::scrolledOption <= (float)Settings::currentOption + getScrollSpeed(Settings::keyPressDelay2))
				Settings::scrolledOption = (float)Settings::currentOption;

			//Scrolling
			if (Settings::scrolledOption < (float)Settings::currentOption && Settings::scrolledOption < (float)Settings::optionCount)
				Settings::scrolledOption += (getScrollSpeed(Settings::keyPressDelay2) + getSpeedIncrease());
			if (Settings::scrolledOption > (float)Settings::currentOption && Settings::scrolledOption > 1.0f)
				Settings::scrolledOption -= (getScrollSpeed(Settings::keyPressDelay2) + getSpeedIncrease());
		}

		//Mouse Hovered Option
		if (Settings::showCursor)
		{
			if (Settings::currentMouseOption != (int)Settings::scrolledOption)
				Drawing::Rect(Settings::headerRect, { Settings::menuX, (Settings::currentMouseOption < Settings::maxOptions ? Settings::currentMouseOption : Settings::maxOptions) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f });
		}

		//Drawing Scroller on Top
		Drawing::Rect(Settings::scroller, { Settings::menuX, (Settings::scrolledOption < (float)Settings::maxOptions ? Settings::scrolledOption : Settings::maxOptions) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f });
	}
}

void Menu::Scrollbar()
{
	if (Settings::optionCount > 1 && Settings::optionCount < 150 && Settings::scrollbar)
	{
		if (Settings::optionCount <= Settings::maxOptions)
		{
			if (Menu::Settings::menuX < 0.5f)
			{
				Drawing::Rect(Settings::optionRect, { Settings::menuX + 0.1165f, ((float)Settings::optionCount / 2.0f) * 0.035f + Settings::menuY + 0.0765f }, { 0.009f, ((Settings::optionCount) * 0.035f) + 0.001f });
				Drawing::Rect(Settings::headerRect, { Settings::menuX + 0.1165f, Settings::menuY + 0.0665f }, { 0.009f, 0.019f });
				Drawing::Rect(Settings::footerRect, { Settings::menuX + 0.1165f, (Settings::optionCount + 1) * 0.035f + Settings::menuY + 0.0515f }, { 0.009f, 0.019f });
				Drawing::Spriter("commonmenu", "arrowright", Settings::menuX + 0.1165f, Settings::menuY + 0.0665f, 0.012f, 0.020f, 270, Settings::line.r, Settings::line.g, Settings::line.b, Settings::line.a);
				Drawing::Spriter("commonmenu", "arrowright", Settings::menuX + 0.1165f, (Settings::optionCount + 1) * 0.035f + Settings::menuY + 0.0515f, 0.012f, 0.020f, 90, Settings::line.r, Settings::line.g, Settings::line.b, Settings::line.a);
				Drawing::Rect(Settings::scroller, { Settings::menuX + 0.1165f, (Settings::scrolledOption) * 0.035f + Settings::menuY + 0.059f }, { 0.006f, 0.035f });
			}
			else
			{
				Drawing::Rect(Settings::optionRect, { Settings::menuX - 0.1165f, ((float)Settings::optionCount / 2.0f) * 0.035f + Settings::menuY + 0.0765f }, { 0.009f, ((Settings::optionCount) * 0.035f) + 0.001f });
				Drawing::Rect(Settings::headerRect, { Settings::menuX - 0.1165f, Settings::menuY + 0.0665f }, { 0.009f, 0.019f });
				Drawing::Rect(Settings::footerRect, { Settings::menuX - 0.1165f, (Settings::optionCount + 1) * 0.035f + Settings::menuY + 0.0515f }, { 0.009f, 0.019f });
				Drawing::Spriter("commonmenu", "arrowright", Settings::menuX - 0.1165f, Settings::menuY + 0.0665f, 0.012f, 0.020f, 270, Settings::line.r, Settings::line.g, Settings::line.b, Settings::line.a);
				Drawing::Spriter("commonmenu", "arrowright", Settings::menuX - 0.1165f, (Settings::optionCount + 1) * 0.035f + Settings::menuY + 0.0515, 0.012f, 0.020f, 90, Settings::line.r, Settings::line.g, Settings::line.b, Settings::line.a);
				Drawing::Rect(Settings::scroller, { Settings::menuX - 0.1165f, (Settings::scrolledOption) * 0.035f + Settings::menuY + 0.059f }, { 0.006f, 0.035f });
			}
		}
		else
		{
			if (Menu::Settings::menuX < 0.5f)
			{
				Drawing::Rect(Settings::optionRect, { Settings::menuX + 0.1165f, ((float)Settings::maxOptions / 2.0f) * 0.035f + Settings::menuY + 0.0765f }, { 0.009f, (Settings::maxOptions * 0.035f) + 0.001f });
				Drawing::Rect(Settings::headerRect, { Settings::menuX + 0.1165f, Settings::menuY + 0.0665f }, { 0.009f, 0.019f });
				Drawing::Rect(Settings::footerRect, { Settings::menuX + 0.1165f, (Settings::maxOptions + 1) * 0.035f + Settings::menuY + 0.0515f }, { 0.009f, 0.019f });
				Drawing::Spriter("commonmenu", "arrowright", Settings::menuX + 0.1165f, Settings::menuY + 0.0665f, 0.012f, 0.020f, 270, Settings::line.r, Settings::line.g, Settings::line.b, Settings::line.a);
				Drawing::Spriter("commonmenu", "arrowright", Settings::menuX + 0.1165f, (Settings::maxOptions + 1) * 0.035f + Settings::menuY + 0.0515, 0.012f, 0.020f, 90, Settings::line.r, Settings::line.g, Settings::line.b, Settings::line.a);
				Drawing::Rect(Settings::scroller, { Settings::menuX + 0.1165f, (Settings::scrolledOption) * (Settings::maxOptions * 0.035f / Settings::optionCount) + (Settings::menuY + 0.0765f - ((Settings::maxOptions * 0.035f / Settings::optionCount) / 2.0f)) }, { 0.006f, (Settings::maxOptions * 0.035f / Settings::optionCount) });
			}
			else
			{
				Drawing::Rect(Settings::optionRect, { Settings::menuX - 0.1165f, ((float)Settings::maxOptions / 2.0f) * 0.035f + Settings::menuY + 0.0765f }, { 0.009f, (Settings::maxOptions * 0.035f) + 0.001f });
				Drawing::Rect(Settings::headerRect, { Settings::menuX - 0.1165f, Settings::menuY + 0.0665f }, { 0.009f, 0.019f });
				Drawing::Rect(Settings::footerRect, { Settings::menuX - 0.1165f, (Settings::maxOptions + 1) * 0.035f + Settings::menuY + 0.0515f }, { 0.009f, 0.019f });
				Drawing::Spriter("commonmenu", "arrowright", Settings::menuX - 0.1165f, Settings::menuY + 0.0665f, 0.012f, 0.020f, 270, Settings::line.r, Settings::line.g, Settings::line.b, Settings::line.a);
				Drawing::Spriter("commonmenu", "arrowright", Settings::menuX - 0.1165f, (Settings::maxOptions + 1) * 0.035f + Settings::menuY + 0.0515, 0.012f, 0.020f, 90, Settings::line.r, Settings::line.g, Settings::line.b, Settings::line.a);
				Drawing::Rect(Settings::scroller, { Settings::menuX - 0.1165f, (Settings::scrolledOption) * (Settings::maxOptions * 0.035f / Settings::optionCount) + (Settings::menuY + 0.0765f - ((Settings::maxOptions * 0.035f / Settings::optionCount) / 2.0f)) }, { 0.006f, (Settings::maxOptions * 0.035f / Settings::optionCount) });
			}
		}
	}
}

bool Menu::Option(const char * option)
{
	Settings::optionCount++;

	bool onThis = false;
	if (Settings::scrolledOption >= (float)Settings::optionCount - 0.5f && Settings::scrolledOption <= (float)Settings::optionCount + 0.5f)
		onThis = true;

	if (Settings::currentOption <=Settings::maxOptions && Settings::optionCount <=Settings::maxOptions)
	{
		Drawing::Text(option, /*onThis ? Settings::selectedText : */Settings::optionText, { Settings::menuX - 0.100f, (Settings::optionCount) * 0.035f + Settings::menuY + 0.0455f }, { 0.34f, 0.34f }, false);
		if (Settings::movingMenu) {
			Drawing::Rect(Settings::optionRect, { Settings::menuX, (Settings::optionCount) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f });
			Settings::optionCount == 1 ? Drawing::Rect(Settings::scroller, { Settings::menuX, (Settings::optionCount) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f }) : NULL;
		}
	}
	else if (Settings::optionCount > (Settings::currentOption -Settings::maxOptions) && Settings::optionCount <= Settings::currentOption)
	{
		Drawing::Text(option, /*onThis ? Settings::selectedText : */Settings::optionText, { Settings::menuX - 0.100f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions))*0.035f + Settings::menuY + 0.0455f }, { 0.34f, 0.34f }, false);
		if (Settings::movingMenu) {
			Drawing::Rect(Settings::optionRect, { Settings::menuX,  (Settings::optionCount - (Settings::currentOption - Settings::maxOptions)) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f });
			Settings::optionCount == 1 ? Drawing::Rect(Settings::scroller, { Settings::menuX, (Settings::optionCount - (Settings::currentOption - Settings::maxOptions)) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f }) : NULL;
		}
	}
	if (Settings::currentOption == Settings::optionCount)
	{
		if (Settings::selectPressed)
		{
			return true;
		}
	}
	return false;
}



bool Menu::Break(const char* option)
{
	Settings::optionCount++;

	bool onThis = false;
	if (Settings::scrolledOption >= (float)Settings::optionCount - 0.5f && Settings::scrolledOption <= (float)Settings::optionCount + 0.5f)
		onThis = true;

	if (Settings::currentOption <=Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
	{
		Drawing::Text(option, /*onThis ? Settings::selectedBreak : */Settings::breakText, { Settings::menuX, (Settings::optionCount) * 0.035f + Settings::menuY + 0.0455f }, { 0.47f, 0.47f }, true);
		if (Settings::movingMenu) {
			Drawing::Rect(Settings::optionRect, { Settings::menuX, (Settings::optionCount) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f });
			Settings::optionCount == 1 ? Drawing::Rect(Settings::scroller, { Settings::menuX, (Settings::optionCount) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f }) : NULL;
		}
	}
	else if (Settings::optionCount > (Settings::currentOption -Settings::maxOptions) && Settings::optionCount <= Settings::currentOption)
	{
		Drawing::Text(option, /*onThis ? Settings::selectedBreak : */Settings::breakText, { Settings::menuX, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.0455f }, { 0.47f, 0.47f }, true);
		if (Settings::movingMenu) {
			Drawing::Rect(Settings::optionRect, { Settings::menuX,  (Settings::optionCount - (Settings::currentOption - Settings::maxOptions)) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f });
			Settings::optionCount == 1 ? Drawing::Rect(Settings::scroller, { Settings::menuX, (Settings::optionCount - (Settings::currentOption - Settings::maxOptions)) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f }) : NULL;
		}
	}
	return false;
}

bool Menu::MenuOption(const char * option, SubMenus newSub)
{
	Option(option);

	bool onThis = false;
	if (Settings::scrolledOption >= (float)Settings::optionCount - 0.5f && Settings::scrolledOption <= (float)Settings::optionCount + 0.5f)
		onThis = true;

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
		Drawing::Text(">", /*onThis ? Settings::selectedArrow : */Settings::optionArrow, { Settings::menuX + 0.095f, Settings::optionCount * 0.035f + Settings::menuY + 0.0465f }, { 0.36f, 0.36f }, true);
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
		Drawing::Text(">", /*onThis ? Settings::selectedArrow : */Settings::optionArrow, { Settings::menuX + 0.095f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions))*0.035f + Settings::menuY + 0.0465f }, { 0.36f, 0.36f }, true);

	if (Settings::optionCount == Settings::currentOption && Settings::selectPressed) {
		MenuLevelHandler::MoveMenu(newSub);
		return true;
	}
	else if (Settings::optionCount == Settings::currentOption && Menu::Settings::currentMenu == playerlist)
	{
		return true;
	}
	return false;
}
bool Menu::Toggle(const char * option, bool & b00l)
{
	Option(option);
	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
		/*Drawing::Text(b00l ? "~g~ON" : "~r~OFF", Settings::optionText, { Settings::menuX + 0.088f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.34f, 0.34f }, true);*/
		Drawing::Spriter("commonmenu",b00l ?  "shop_box_tick" : "shop_box_blank", Settings::menuX + 0.091f, (Settings::optionCount * 0.035f + Settings::menuY + 0.059f), 0.024f, 0.039f, 0, 255, 255, 255, 255);
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
		/*Drawing::Text(b00l ? "~g~ON" : "~r~OFF", Settings::optionText, { Settings::menuX + 0.088f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.0455f }, { 0.34f, 0.34f }, true);*/
		Drawing::Spriter("commonmenu", b00l ? "shop_box_tick" : "shop_box_blank", Settings::menuX + 0.091f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.059f, 0.024f, 0.039f, 0, 255, 255, 255, 255);
	if (Settings::optionCount == Settings::currentOption && Settings::selectPressed) {
		b00l ^= 1;
		return true;
	}
	return false;
}
bool Menu::ToggleInt(const char* option, bool& b00l, int& _int, int min, int max, bool inputwhenactive)
{
	Option(option);

	if ((inputwhenactive ? b00l : true) && (Settings::optionCount == Settings::currentOption)) {
		if (Settings::leftPressed) {
			_int < max ? _int++ : _int = min;
		}
		if (Settings::rightPressed) {
			_int > min ? _int-- : _int = max;
		}
	}

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
	{
		Drawing::Spriter("commonmenu", b00l ? "shop_box_tick" : "shop_box_blank", Settings::menuX + 0.091f, (Settings::optionCount * 0.035f + Settings::menuY + 0.059f), 0.024f, 0.039f, 0, 255, 255, 255, 255);
		Drawing::Text(Tools::StringToChar((_int > min ? "< " : "    ") + std::to_string(_int) + (_int < max ? " >" : "    ")), Settings::integre, { Settings::menuX + 0.065f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	}
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
	{
		Drawing::Spriter("commonmenu", b00l ? "shop_box_tick" : "shop_box_blank", Settings::menuX + 0.091f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.059f, 0.024f, 0.039f, 0, 255, 255, 255, 255);
		Drawing::Text(Tools::StringToChar((_int > min ? "< " : "    ") + std::to_string(_int) + (_int < max ? " >" : "    ")), Settings::integre, { Settings::menuX + 0.065f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	}
	if (Settings::optionCount == Settings::currentOption && Settings::selectPressed) {
		b00l ^= 1;
		return true;
	}
	return false;
}
bool Menu::ToggleInt(const char* option, bool& b00l, int& _int, int min, int max, int step, bool inputwhenactive)
{
	Option(option);

	if ((inputwhenactive ? b00l : true) && (Settings::optionCount == Settings::currentOption)) {
		if (Settings::leftPressed) {
			_int < max ? _int += step : _int = min;
		}
		if (Settings::rightPressed) {
			_int > min ? _int -= step : _int = max;
		}
	}

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
	{
		Drawing::Spriter("commonmenu", b00l ? "shop_box_tick" : "shop_box_blank", Settings::menuX + 0.091f, (Settings::optionCount * 0.035f + Settings::menuY + 0.059f), 0.024f, 0.039f, 0, 255, 255, 255, 255);
		Drawing::Text(Tools::StringToChar((_int > min ? "< " : "     ") + std::to_string(_int) + (_int < max ? " >" : "    ")), Settings::integre, { Settings::menuX + 0.065f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	}
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
	{
		Drawing::Spriter("commonmenu", b00l ? "shop_box_tick" : "shop_box_blank", Settings::menuX + 0.091f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.059f, 0.024f, 0.039f, 0, 255, 255, 255, 255);
		Drawing::Text(Tools::StringToChar((_int > min ? "< " : "    ") + std::to_string(_int) + (_int < max ? " >" : "    ")), Settings::integre, { Settings::menuX + 0.065f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	}
	if (Settings::optionCount == Settings::currentOption && Settings::selectPressed) {
		b00l ^= 1;
		return true;
	}
	return false;
}
bool Menu::ToggleFloat(const char* option, bool & b00l, float& _float, float min, float max, bool inputwhenactive)
{
	Option(option);

	if ((inputwhenactive ? b00l : true) && (Settings::optionCount == Settings::currentOption)) {
		if (Settings::leftPressed) {
			_float < max ? _float += 0.1f : _float = min;
		}
		if (Settings::rightPressed) {
			_float > min ? _float -= 0.1f : _float = max;
		}
	}

	std::ostringstream Text; Text << (_float > min ? "< " : "    ") << (round(_float * 100) / 100) << (_float < max ? " >" : "    ");

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
	{
		Drawing::Spriter("commonmenu", b00l ? "shop_box_tick" : "shop_box_blank", Settings::menuX + 0.091f, (Settings::optionCount * 0.035f + Settings::menuY + 0.059f), 0.024f, 0.039f, 0, 255, 255, 255, 255);
		Drawing::Text(Tools::StringToChar(Text.str()), Settings::integre, { Settings::menuX + 0.065f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	}
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
	{
		Drawing::Spriter("commonmenu", b00l ? "shop_box_tick" : "shop_box_blank", Settings::menuX + 0.091f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.059f, 0.024f, 0.039f, 0, 255, 255, 255, 255);
		Drawing::Text(Tools::StringToChar(Text.str()), Settings::integre, { Settings::menuX + 0.065f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	}
	if (Settings::optionCount == Settings::currentOption && Settings::selectPressed) {
		b00l ^= 1;
		return true;
	}
	return false;
}
bool Menu::ToggleFloat(const char* option, bool& b00l, float& _float, float min, float max, float step, bool inputwhenactive)
{
	Option(option);

	if ((inputwhenactive ? b00l : true) && (Settings::optionCount == Settings::currentOption)) {
		if (Settings::leftPressed) {
			_float < max ? _float += step : _float = min;
		}
		if (Settings::rightPressed) {
			_float > min ? _float -= step : _float = max;
		}
	}

	std::ostringstream Text; Text << (_float > min ? "< " : "    ") << (round(_float * 100) / 100) << (_float < max ? " >" : "    ");

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
	{
		Drawing::Spriter("commonmenu", b00l ? "shop_box_tick" : "shop_box_blank", Settings::menuX + 0.091f, (Settings::optionCount * 0.035f + Settings::menuY + 0.059f), 0.024f, 0.039f, 0, 255, 255, 255, 255);
		Drawing::Text(Tools::StringToChar(Text.str()), Settings::integre, { Settings::menuX + 0.065f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	}
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
	{
		Drawing::Spriter("commonmenu", b00l ? "shop_box_tick" : "shop_box_blank", Settings::menuX + 0.091f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.059f, 0.024f, 0.039f, 0, 255, 255, 255, 255);
		Drawing::Text(Tools::StringToChar(Text.str()), Settings::integre, { Settings::menuX + 0.065f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	}
	if (Settings::optionCount == Settings::currentOption && Settings::selectPressed) {
		b00l ^= 1;
		return true;
	}
	return false;
}
bool Menu::Int(const char * option, int & _int, int min, int max)
{
	Option(option);

	if (Settings::optionCount == Settings::currentOption) {
		if (Settings::leftPressed) {
			_int < max ? _int++ : _int = min;
		}
		if (Settings::rightPressed) {
			_int > min ? _int-- : _int = max;
		}
		if (Settings::selectPressed) {
			_int = NumberKeyboard(_int, min, max);
		}
	}

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
		Drawing::Text(Tools::StringToChar((_int > min ? "< " : "    ") + std::to_string(_int) + (_int < max ? " >" : "    ")), Settings::integre, { Settings::menuX + 0.085f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
		Drawing::Text(Tools::StringToChar((_int > min ? "< " : "    ") + std::to_string(_int) + (_int < max ? " >" : "    ")), Settings::integre, { Settings::menuX + 0.085f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions))*0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);

	if (Settings::optionCount == Settings::currentOption) return true;
	return false;
}
bool Menu::Int(const char * option, int & _int, int min, int max, int step)
{
	Option(option);

	if (Settings::optionCount == Settings::currentOption) {
		if (Settings::leftPressed) {
			_int = round(_int / step);
			_int = (_int * step);
			_int < max ? _int += step : _int = min;
		}
		if (Settings::rightPressed) {
			_int = round(_int / step);
			_int = (_int * step);
			_int > min ? _int -= step : _int = max;
		}
		if (Settings::selectPressed) {
			_int = NumberKeyboard(_int, min, max, step);
		}
	}

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
		Drawing::Text(Tools::StringToChar((_int > min ? "< " : "    ") + std::to_string(_int) + (_int < max ? " >" : "    ")), Settings::integre, { Settings::menuX + 0.085f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
		Drawing::Text(Tools::StringToChar((_int > min ? "< " : "    ") + std::to_string(_int) + (_int < max ? " >" : "    ")), Settings::integre, { Settings::menuX + 0.085f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions))*0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);

	if (Settings::optionCount == Settings::currentOption) return true;
	return false;
}
#pragma warning(disable: 4244)
bool Menu::Float(const char * option, float & _float, float min, float max)
{
	Option(option);

	if (Settings::optionCount == Settings::currentOption) {
		if (Settings::leftPressed) {
			_float = (round(_float * 10) / 10);
			_float < max ? _float += 0.1f : _float = min;
		}
		if (Settings::rightPressed) {
			_float = (round(_float * 10) / 10);
			_float > min ? _float -= 0.1f : _float = max;
		}
		if (Settings::selectPressed) {
			_float = NumberKeyboard(_float, min, max);
			_float = (round(_float * 10) / 10);
		}
	}

	std::ostringstream Text; Text << (_float > min ? "< " : "    ") << (round(_float * 10) / 10) << (_float < max ? " >" : "    ");

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
		Drawing::Text(Tools::StringToChar(Text.str()), Settings::integre, { Settings::menuX + 0.085f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
		Drawing::Text(Tools::StringToChar(Text.str()), Settings::integre, { Settings::menuX + 0.085f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);

	if (Settings::optionCount == Settings::currentOption) return true;
	return false;
}
bool Menu::Float(const char * option, float & _float, float min, float max, float step)
{
	Option(option);

	if (Settings::optionCount == Settings::currentOption) {
		if (Settings::leftPressed) {
			_float < max ? _float += step : _float = min;
		}
		if (Settings::rightPressed) {
			_float > min ? _float -= step : _float = max;
		}
	}

	std::ostringstream Text; Text << (_float > min ? "< " : "    ") << (round(_float * 100) / 100) << (_float < max ? " >" : "    ");

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
		Drawing::Text(Tools::StringToChar(Text.str()), Settings::integre, { Settings::menuX + 0.085f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
		Drawing::Text(Tools::StringToChar(Text.str()), Settings::integre, { Settings::menuX + 0.085f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions)) * 0.035f + Settings::menuY + 0.0455f }, { 0.38f, 0.38f }, true);

	if (Settings::optionCount == Settings::currentOption) return true;
	return false;
}
#pragma warning(default: 4244)
#pragma warning(disable: 4267)
bool Menu::IntVector(const char * option, std::vector<int> Vector, int & position)
{
	Option(option);

	if (Settings::optionCount == Settings::currentOption) {
		int max = Vector.size() - 1;
		int min = 0;
		if (Settings::rightPressed) {
			position >= 1 ? position-- : position = max;
		}
		if (Settings::leftPressed) {
			position < max ? position++ : position = min;
		}
	}

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
		Drawing::Text(Tools::StringToChar(std::to_string(Vector[position])), Settings::optionText, { Settings::menuX + 0.090f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.5f, 0.5f }, true);
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
		Drawing::Text(Tools::StringToChar(std::to_string(Vector[position])), Settings::optionText, { Settings::menuX + 0.090f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions))*0.035f + Settings::menuY + 0.0455f }, { 0.5f, 0.5f }, true);

	if (Settings::optionCount == Settings::currentOption && Settings::selectPressed) return true;
	else if (Settings::optionCount == Settings::currentOption && Settings::leftPressed) return true;
	else if (Settings::optionCount == Settings::currentOption && Settings::rightPressed) return true;
	return false;
}
bool Menu::FloatVector(const char * option, std::vector<float> Vector, int & position)
{
	Option(option);

	if (Settings::optionCount == Settings::currentOption) {
		size_t max = Vector.size() - 1;
		int min = 0;
		if (Settings::rightPressed) {
			position >= 1 ? position-- : position = max;
		}
		if (Settings::leftPressed) {
			position < max ? position++ : position = min;
		}
	}

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
		Drawing::Text(Tools::StringToChar(std::to_string(Vector[position])), Settings::optionText, { Settings::menuX + 0.090f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.5f, 0.5f }, true);
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
		Drawing::Text(Tools::StringToChar(std::to_string(Vector[position])), Settings::optionText, { Settings::menuX + 0.090f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions))*0.035f + Settings::menuY + 0.0455f }, { 0.5f, 0.5f }, true);

	if (Settings::optionCount == Settings::currentOption && Settings::selectPressed) return true;
	else if (Settings::optionCount == Settings::currentOption && Settings::leftPressed) return true;
	else if (Settings::optionCount == Settings::currentOption && Settings::rightPressed) return true;
	return false;
}
bool Menu::StringVector(const char * option, std::vector<std::string> Vector, int & position)
{
	Option(option);

	if (Settings::optionCount == Settings::currentOption) {
		size_t max = Vector.size() - 1;
		int min = 0;
		if (Settings::rightPressed) {
			position >= 1 ? position-- : position = max;
		}
		if (Settings::leftPressed) {
			position < max ? position++ : position = min;
		}
	}

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
		Drawing::Text(Tools::StringToChar((Vector[position])), Settings::optionText, { Settings::menuX + (0.088f - (float)((float)((Vector[position]).size()) / 480.0f)), Settings::optionCount * 0.035f + Settings::menuY + 0.0475f }, { 0.31f, 0.31f }, true);
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
		Drawing::Text(Tools::StringToChar((Vector[position])), Settings::optionText, { Settings::menuX + (0.088f - (float)((float)((Vector[position]).size()) / 480.0f)), (Settings::optionCount - (Settings::currentOption -Settings::maxOptions))*0.035f + Settings::menuY + 0.0475f }, { 0.31f, 0.31f }, true);

	if (Settings::optionCount == Settings::currentOption && Settings::selectPressed) return true;
	return false;
}
bool Menu::StringVector(const char * option, std::vector<char*> Vector, int & position)
{
	Option(option);

	if (Settings::optionCount == Settings::currentOption) {
		size_t max = Vector.size() - 1;
		int min = 0;
		if (Settings::leftPressed) {
			position >= 1 ? position-- : position = max;
		}
		if (Settings::leftPressed) {
			position < max ? position++ : position = min;
		}
	}

	if (Settings::currentOption <= Settings::maxOptions && Settings::optionCount <= Settings::maxOptions)
		Drawing::Text(((Vector[position])), Settings::optionText, { Settings::menuX + 0.090f, Settings::optionCount * 0.035f + Settings::menuY + 0.0455f }, { 0.5f, 0.5f }, true);
	else if (Settings::optionCount > Settings::currentOption - Settings::maxOptions && Settings::optionCount <= Settings::currentOption)
		Drawing::Text(((Vector[position])), Settings::optionText, { Settings::menuX + 0.090f, (Settings::optionCount - (Settings::currentOption -Settings::maxOptions))*0.035f + Settings::menuY + 0.0455f }, { 0.5f, 0.5f }, true);

	if (Settings::optionCount == Settings::currentOption && Settings::selectPressed) return true;
	else if (Settings::optionCount == Settings::currentOption && Settings::leftPressed) return true;
	else if (Settings::optionCount == Settings::currentOption && Settings::rightPressed) return true;
	return false;
}
#pragma warning(default: 4267)

void Menu::End()
{
	int opcount = Settings::optionCount;
	int currop = Settings::currentOption;
	if (opcount >=Settings::maxOptions) {
        //Drawing::Text(Tools::StringToChar(std::to_string(currop) + " / " + std::to_string(opcount)), Settings::optionText, { Settings::menuX + 0.078f, Settings::menuY + 0.0455f }, { 0.34f, 0.34f }, true);
        Drawing::Rect(Settings::headerRect, { Settings::menuX, Settings::menuY + 0.059f }, { 0.21f, 0.035f });
		Drawing::Rect(Settings::footerRect, { Settings::menuX, (Settings::maxOptions + 1) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f });
        Drawing::Rect(Settings::line, { Settings::menuX, (Settings::maxOptions + 1) * 0.035f + 0.1610f }, { 0.21f, 0.0015f });
        Drawing::Rect(Settings::line, { Settings::menuX, 0.1920f }, { 0.21f, 0.0015f });
		Drawing::Spriter("", "", Settings::menuX, ((Settings::maxOptions + 1) * 0.035f + Settings::menuY + 0.059f), 0.020f, 0.035f, 180, Settings::line.r, Settings::line.g, Settings::line.b, Settings::line.a);
	}
	else if (opcount > 0) {
        //Drawing::Text(Tools::StringToChar(std::to_string(currop) + " / " + std::to_string(opcount)), Settings::optionText, { Settings::menuX + 0.078f, Settings::menuY + 0.0455f }, { 0.34f, 0.34f }, true);
        Drawing::Rect(Settings::headerRect, { Settings::menuX, Settings::menuY + 0.059f }, { 0.21f, 0.035f });
		Drawing::Rect(Settings::footerRect, { Settings::menuX, (Settings::optionCount + 1) * 0.035f + Settings::menuY + 0.059f }, { 0.21f, 0.035f });
        Drawing::Rect(Settings::line, { Settings::menuX, (Settings::optionCount + 1) * 0.035f + 0.1610f }, { 0.21f, 0.0015f });
        Drawing::Rect(Settings::line, { Settings::menuX, 0.1920f }, { 0.21f, 0.0015f });
		Drawing::Spriter("", "", Settings::menuX, ((Settings::optionCount + 1) * 0.035f + Settings::menuY + 0.059f), 0.020f, 0.035f, 180, Settings::line.r, Settings::line.g, Settings::line.b, Settings::line.a);
	}
}

int Menu::Settings::keyPressDelay = 200;
int Menu::Settings::keyPressPreviousTick = GetTickCount();
int Menu::Settings::keyPressDelay2 = 130;
int Menu::Settings::keyPressPreviousTick2 = GetTickCount();
int Menu::Settings::keyPressDelay3 = 110;
int Menu::Settings::keyPressPreviousTick3 = GetTickCount();

bool Menu::Settings::controllerinput = true;

bool Menu::Settings::movingMenu = false;

void Menu::Checks::Controlls()
{
	Settings::selectPressed = false;
	Settings::leftPressed = false;
	Settings::rightPressed = false;


	//Komplexe Maus Scheiße
	if (GetAsyncKeyState(VK_F4))
	{
		if ((GetTickCount() - Settings::keyPressPreviousTick) > Settings::keyPressDelay) {
			if (!Settings::showCursor)
				Settings::showCursor = true;
			else
				Settings::showCursor = false;

			Settings::keyPressPreviousTick = GetTickCount();
		}
	}

	if (Settings::showCursor && Settings::menuLevel > 0)
	{
		CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(0);
		CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(1);
		CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(2);
		CONTROLS::ENABLE_CONTROL_ACTION(2, INPUT_MOVE_UP_ONLY, true);
		CONTROLS::ENABLE_CONTROL_ACTION(2, INPUT_MOVE_DOWN_ONLY, true);
		CONTROLS::ENABLE_CONTROL_ACTION(2, INPUT_MOVE_LEFT_ONLY, true);
		CONTROLS::ENABLE_CONTROL_ACTION(2, INPUT_MOVE_RIGHT_ONLY, true);
		UI::_SHOW_CURSOR_THIS_FRAME();

		if (!Cursor::onHeader() && !Settings::movingMenu)
		{
			UI::_SET_CURSOR_SPRITE(1);

			if (Cursor::onOption())
			{
				Cursor::moveScroller();
				switch (Cursor::getScrolling())
				{
					if ((GetTickCount() - Settings::keyPressPreviousTick2) > Settings::keyPressDelay2) 
					{
						case 0:
							break;

						case 1:
						{
							Settings::currentOption > 1 ? Settings::currentOption-- : Settings::currentOption = Settings::optionCount;
							break;
						}

						case -1:
						{
							Settings::currentOption < Settings::optionCount ? Settings::currentOption++ : Settings::currentOption = 1;
							break;
						}
						Settings::keyPressPreviousTick2 = GetTickCount();
					}
				}

				if (GetAsyncKeyState(VK_LBUTTON)) {
					if ((GetTickCount() - Settings::keyPressPreviousTick) > Settings::keyPressDelay) {
						if (Settings::currentMouseOption == Settings::scrolledOption)
						{
							Settings::selectPressed = true;
							AUDIO::PLAY_SOUND_FRONTEND(-1, "SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
						}
						Settings::keyPressPreviousTick = GetTickCount();
					}
				}
				else if (GetAsyncKeyState(VK_RBUTTON)) {
					if ((GetTickCount() - Settings::keyPressPreviousTick) > Settings::keyPressDelay) {
						MenuLevelHandler::BackMenu();
						AUDIO::PLAY_SOUND_FRONTEND(-1, "BACK", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
						
						Settings::keyPressPreviousTick = GetTickCount();
					}
				}
			}	
		}

		//Show Pre Grabbing Cursor
		if (Cursor::onHeader())
		{
			if (!GetAsyncKeyState(VK_LBUTTON))
			{
				UI::_SET_CURSOR_SPRITE(3);
			}
		}

		//Move Menu
		if (GetAsyncKeyState(VK_LBUTTON))
		{
			if (!Settings::movingMenu)
			{
				if (Cursor::onHeader())
				{
					Settings::movingMenu = true;
				}
			}
			else
			{
				UI::_SET_CURSOR_SPRITE(4);

				Settings::menuX = Cursor::cursorPos().x;
				Settings::menuY = Cursor::cursorPos().y;
			}
		}
		else
			Settings::movingMenu = false; //Stop the Moving State
	}
	
	if (GetTickCount() - Settings::keyPressPreviousTick > Settings::keyPressDelay) {
		if (GetTickCount() - Settings::keyPressPreviousTick2 > Settings::keyPressDelay2) {
			if (GetTickCount() - Settings::keyPressPreviousTick3 > Settings::keyPressDelay3) {
				if (IsKeyPressed(VK_MULTIPLY) || IsKeyPressed(VK_F8) || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlScriptRB) && CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneRight) && Settings::controllerinput) {
					Settings::menuLevel == 0 ? MenuLevelHandler::MoveMenu(SubMenus::mainmenu) : Settings::menuLevel > 0 ? MenuLevelHandler::ExitMenu() : NULL;
					if (firstopen)
					{
						firstopen = false;
						AUDIO::PLAY_SOUND_FRONTEND(-1, "Hack_Success", "DLC_HEIST_BIOLAB_PREP_HACKING_SOUNDS", 0);
					}
					Settings::keyPressPreviousTick = GetTickCount();
				}
				else if (!Settings::showCursor && (IsKeyPressed(VK_NUMPAD0) || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendCancel) && Settings::controllerinput)) {
					Settings::menuLevel > 0 ? MenuLevelHandler::BackMenu() : NULL;
					if (Settings::menuLevel > 0)
						AUDIO::PLAY_SOUND_FRONTEND(-1, "BACK", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);

					Settings::keyPressPreviousTick = GetTickCount();
				}
				else if (!Settings::showCursor && (IsKeyPressed(VK_NUMPAD8) || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendUp) && Settings::controllerinput)) {
					Settings::currentOption > 1 ? Settings::currentOption-- : Settings::currentOption = Settings::optionCount;
					if (Settings::menuLevel > 0)
						AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);

					Settings::keyPressPreviousTick2 = GetTickCount();
				}
				else if (!Settings::showCursor && (IsKeyPressed(VK_NUMPAD2) || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendDown) && Settings::controllerinput)) {
					Settings::currentOption < Settings::optionCount ? Settings::currentOption++ : Settings::currentOption = 1;
					if (Settings::menuLevel > 0)
						AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);

					Settings::keyPressPreviousTick2 = GetTickCount();
				}
				else if (!Settings::showCursor && (IsKeyPressed(VK_NUMPAD6) || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneRight) && Settings::controllerinput)) {
					Settings::leftPressed = true;
					if (Settings::menuLevel > 0)
						AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);

					Settings::keyPressPreviousTick3 = GetTickCount();
				}
				else if (!Settings::showCursor && (IsKeyPressed(VK_NUMPAD4) || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlPhoneLeft) && Settings::controllerinput)) {
					Settings::rightPressed = true;
					if (Settings::menuLevel > 0)
						AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);

					Settings::keyPressPreviousTick3 = GetTickCount();
				}
				else if (!Settings::showCursor && (IsKeyPressed(VK_NUMPAD5) || CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, ControlFrontendAccept) && Settings::controllerinput)) {
					Settings::selectPressed = true;
					if (Settings::menuLevel > 0)
						AUDIO::PLAY_SOUND_FRONTEND(-1, "SELECT", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);

					Settings::keyPressPreviousTick = GetTickCount();
				}
			}
		}
	}
	Settings::optionCount = 0;
}
#pragma warning(default : 4018)
void Menu::MenuLevelHandler::MoveMenu(SubMenus menu)
{
	Settings::menusArray[Settings::menuLevel] = Settings::currentMenu;
	Settings::optionsArray[Settings::menuLevel] = Settings::currentOption;
	Settings::menuLevel++;
	Settings::currentMenu = menu;
	Settings::currentOption = 1;
}

void Menu::MenuLevelHandler::ExitMenu()
{
    Settings::menuLevel = 0;
    Settings::currentMenu = Settings::menusArray[Settings::menuLevel];
    Settings::currentOption = Settings::optionsArray[Settings::menuLevel];
}

void Menu::MenuLevelHandler::BackMenu()
{
	Settings::menuLevel--;
	Settings::currentMenu = Settings::menusArray[Settings::menuLevel];
	Settings::currentOption = Settings::optionsArray[Settings::menuLevel];
}

char * Menu::Tools::StringToChar(std::string string)
{
	return _strdup(string.c_str());
}

void Menu::Files::WriteStringToIni(std::string string, std::string file, std::string app, std::string key)
{
	WritePrivateProfileStringA(app.c_str(), key.c_str(), string.c_str(), file.c_str());
}

std::string Menu::Files::ReadStringFromIni(std::string file, std::string app, std::string key)
{
	char buf[100];
	GetPrivateProfileStringA(app.c_str(), key.c_str(), "NULL", buf, 100, file.c_str());
	return (std::string)buf;
}

void Menu::Files::WriteIntToIni(int intValue, std::string file, std::string app, std::string key)
{
	WriteStringToIni(std::to_string(intValue), file, app, key);
}

int Menu::Files::ReadIntFromIni(std::string file, std::string app, std::string key)
{
	return std::stoi(ReadStringFromIni(file, app, key));
}

void Menu::Files::WriteFloatToIni(float floatValue, std::string file, std::string app, std::string key)
{
	WriteStringToIni((std::to_string(floatValue)), file, app, key);
}

float Menu::Files::ReadFloatFromIni(std::string file, std::string app, std::string key)
{
	return std::stof(ReadStringFromIni(file, app, key));
}

void Menu::Files::WriteBoolToIni(bool b00l, std::string file, std::string app, std::string key)
{
	WriteStringToIni(b00l ? "true" : "false", file, app, key);
}

bool Menu::Files::ReadBoolFromIni(std::string file, std::string app, std::string key)
{
	return ReadStringFromIni(file, app, key) == "true" ? true : false;
}

void Menu::Vehicle(std::string texture1, std::string texture2)
{
	if (Menu::Settings::menuX < 0.5f)
	{
		if (Menu::Settings::optionCount == Menu::Settings::currentOption) { Menu::Drawing::Spriter(texture1, texture2, Menu::Settings::menuX + 0.24f, 0.2f, 0.11f, 0.11f, 0, 255, 255, 255, 255); }
	}
	else { if (Menu::Settings::optionCount == Menu::Settings::currentOption) { Menu::Drawing::Spriter(texture1, texture2, Menu::Settings::menuX - 0.24f, 0.2f, 0.11f, 0.11f, 0, 255, 255, 255, 255); } }
}

void Menu::AddSmallTitle(char* text)
{
	if (Menu::Settings::menuX < 0.5f)
	{
		Drawing::Text(text, Settings::titleText, { Settings::menuX + 0.182f, Settings::menuY - 0.0355f }, { 0.50f, 0.50f }, true);
        Drawing::Rect(Settings::titleRect, { Settings::menuX + 0.182f, Settings::menuY - 0.019f }, { 0.115f, 0.045f });
	}
	else
	{
		Drawing::Text(text, Settings::titleText, { Settings::menuX - 0.182f, Settings::menuY - 0.0355f }, { 0.50f, 0.50f }, true);
        Drawing::Rect(Settings::titleRect, { Settings::menuX - 0.182f, Settings::menuY - 0.019f }, { 0.115f, 0.045f });
	}
}

void Menu::AddSmallInfo(char* text, short line)
{
    if (Menu::Settings::menuX < 0.5f)
    {
        Drawing::Rect(Settings::optionRect, { Settings::menuX + 0.182f, (line * 0.022f) + Settings::menuY + 0.0145f }, { 0.115f, 0.022f });
        Drawing::Text(text, Settings::optionText, { Settings::menuX + 0.129f, (line * 0.022f) + Settings::menuY + 0.0035f }, { 0.26f, 0.26f }, false);
    }
    else
    {
        Drawing::Rect(Settings::optionRect, { Settings::menuX - 0.182f, (line * 0.022f) + Settings::menuY + 0.0145f }, { 0.115f, 0.022f });
        Drawing::Text(text, Settings::optionText, { Settings::menuX - 0.235f, (line * 0.022f) + Settings::menuY + 0.0035f }, { 0.26f, 0.26f }, false);
    }
}

void Menu::AddSmallEnd(short line)
{
	if (Menu::Settings::menuX < 0.5f)
	{
		Drawing::Rect(Settings::footerRect, { Settings::menuX + 0.182f, (line * 0.022f) + Settings::menuY + 0.0145f }, { 0.115f, 0.022f });
		Drawing::Spriter("commonmenu", "shop_arrows_upanddown", Settings::menuX + 0.182f, (line * 0.022f) + Settings::menuY + 0.0145f, 0.017f, 0.026f, 0, 255, 255, 255, 255);
	}
	else
	{
		Drawing::Rect(Settings::footerRect, { Settings::menuX - 0.182f, (line * 0.022f) + Settings::menuY + 0.0145f }, { 0.115f, 0.022f });
		Drawing::Spriter("commonmenu", "shop_arrows_upanddown", Settings::menuX - 0.182f, (line * 0.022f) + Settings::menuY + 0.0145f, 0.017f, 0.026f, 0, 255, 255, 255, 255);
	}
}
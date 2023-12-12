//Hooking.cpp
#pragma once
#include "stdafx.h"
#include "Features.h"

using namespace Memory;

ScriptThread*(*GetActiveThread)() = nullptr;
HMODULE _hmoduleDLL;
HANDLE mainFiber;
DWORD wakeAt;

//Bypass
twoBytes*	owned_explosion_bypass;
//twoBytes*	model_spawn_bypass;
//Bytes24*	model_check_bypass;

//Return Spoofing
//char*		m_NativeRetn;
//char		m_OriginalNativeRetn[2];

std::vector<LPVOID>		Hooking::m_hooks;
uint64_t*				Hooking::m_frameCount;
fpIsDLCPresent			Hooking::is_DLC_present;
SetSessionWeather       Hooking::set_session_weather;
PickupProtSig			Hooking::pickup_prot_sig;
SetWarningMessage2		Hooking::set_warning_message_2;
SetSessionTime          Hooking::set_session_time;
TriggerScriptEvent      Hooking::trigger_script_event;
GetEventData			Hooking::get_event_data;
FileRegister            Hooking::file_register;
GetLabelText			Hooking::get_label_text;
GetPlayerAddress		Hooking::get_player_address;
clockTime*				Hooking::ClockTime;

static eGameState* 											m_gameState;
static uint64_t												m_worldPtr;
static BlipList*											m_blipList;
static Hooking::NativeRegistrationNew**						m_registrationTable;
static std::unordered_map<uint64_t, Hooking::NativeHandler>	m_handlerCache;
static std::vector<LPVOID>									m_hookedNative;
static __int64**                                            m_globalPtr;

const int EVENT_COUNT = 85;
static std::vector<void*> EventPtr;
static char EventRestore[EVENT_COUNT] = {};

bool Bytes24::empty()
{
	bool	r = true;
	for (int i = 0; i < 24; ++i)
		if (this->byte[i] != 0)
		{
			r = false;
			break;
		}
	return r;
}

bool twoBytes::empty()
{
	bool	r = true;
	for (int i = 0; i < 2; ++i)
		if (this->byte[i] != 0)
		{
			r = false;
			break;
		}
	return r;
}

/* Start Hooking */
void Hooking::Start(HMODULE hmoduleDLL)
{
	_hmoduleDLL = hmoduleDLL;
	Logger::Init();
	FindPatterns();
	if (!InitializeHooks()) Cleanup();

	//User Login Here


	Logger::Info("Anim Loaded");
}
BOOL Hooking::InitializeHooks()
{
	BOOL returnVal = TRUE;

	if (!iHook.Initialize()) {
		Logger::Error("Failed to initialize InputHook");
		returnVal = FALSE;
	}

	if (MH_Initialize() != MH_OK) {
		Logger::Error("MinHook failed to initialize");
		returnVal = FALSE;
	}

	if (!HookNatives()) {

		Logger::Error("Failed to initialize NativeHooks");
		returnVal = FALSE;
	}

	return returnVal;
}

template <typename T>
bool Native(DWORD64 hash, LPVOID hookFunction, T** trampoline)
{
	if (*reinterpret_cast<LPVOID*>(trampoline) != NULL)
		return true;
	auto originalFunction = Hooking::GetNativeHandler(hash);
	if (originalFunction != 0) {
		MH_STATUS createHookStatus = MH_CreateHook(originalFunction, hookFunction, reinterpret_cast<LPVOID*>(trampoline));
		if (((createHookStatus == MH_OK) || (createHookStatus == MH_ERROR_ALREADY_CREATED)) && (MH_EnableHook(originalFunction) == MH_OK))
		{
			m_hookedNative.push_back((LPVOID)originalFunction);
			//DEBUGMSG("Hooked Native 0x%#p", hash);
			return true;
		}
	}
	return false;
}

uint64_t CMetaData::m_begin = 0;
uint64_t CMetaData::m_end = 0;
DWORD CMetaData::m_size = 0;

uint64_t CMetaData::begin()
{
	return m_begin;
}
uint64_t CMetaData::end()
{
	return m_end;
}
DWORD CMetaData::size()
{
	return m_size;
}

void CMetaData::init()
{
	if (m_begin && m_size)
		return;

	m_begin = (uint64_t)GetModuleHandleA(nullptr);
	const IMAGE_DOS_HEADER*	headerDos = (const IMAGE_DOS_HEADER*)m_begin;
	const IMAGE_NT_HEADERS*	headerNt = (const IMAGE_NT_HEADERS64*)((const BYTE*)headerDos + headerDos->e_lfanew);
	m_size = headerNt->OptionalHeader.SizeOfCode;
	m_end = m_begin + m_size;
	return;
}

fpIsDLCPresent	OG_IS_DLC_PRESENT = nullptr;
BOOL __cdecl HK_IS_DLC_PRESENT()
{
	static uint64_t	last = 0;
	uint64_t		cur = *Hooking::m_frameCount;
	if (last != cur)
	{
		last = cur;
		Hooking::onTickInit();
	}
	return OG_IS_DLC_PRESENT();
}

void ScriptEventBlockMessage(const char* type, Player name)
{
	static int timer;
	if ((GetTickCount() - timer) > 1) {
		std::ostringstream TEXT; TEXT << "~r~Blocked Remote\n~s~Type: " << type << "\nFrom: " << PLAYER::GET_PLAYER_NAME(name);
		UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
		UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME((char*)TEXT.str().c_str());
		UI::_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_BLOCKED", "CHAR_BLOCKED", true, 8, "Anim", "Protection", 0.5f, "___Menu", 9, 0);
		UI::_DRAW_NOTIFICATION(FALSE, FALSE);
		timer = GetTickCount();
	}
}

bool missionprot = false;
bool inviteprot = false;
bool soundspamprot = true;
bool nonhostkickprot = true;
bool ceobanprot = false;
bool ceokickprot = false;
bool scripteventprot = false;
bool vehkickprot = false;

GetEventData OG_GET_EVENT_DATA = nullptr;
BOOL __cdecl HK_GET_EVENT_DATA(std::int32_t eventGroup, std::int32_t eventIndex, std::int64_t* args, std::uint32_t argCount)
{
	auto result = static_cast<decltype(&HK_GET_EVENT_DATA)>(OG_GET_EVENT_DATA)(eventGroup, eventIndex, args, argCount);

	bool dontreturn = false;

	if (result)
	{
		//Block Scripted Game Events here

		if (args[0] == -171207973 && (scripteventprot || inviteprot))
		{
			ScriptEventBlockMessage("Apartment Invite", args[1]);
			dontreturn = true;
		}
		std::uint64_t ForceMission[] = { 1115000764, -545396442 };
		for (int i = 0; i < (sizeof(ForceMission) / sizeof(ForceMission[0])); i++)
		{
			if (args[0] == ForceMission[i] && (scripteventprot || missionprot))
			{
				ScriptEventBlockMessage("Force to Mission", args[1]);
				dontreturn = true;
			}
		}
		if (args[0] == -1333236192 && (scripteventprot || vehkickprot))
		{
			ScriptEventBlockMessage("Vehicle Kick", args[1]);
			dontreturn = true;
		}
		if (args[0] == -1648921703 && (scripteventprot || ceokickprot))
		{
			ScriptEventBlockMessage("Ceo Kick", args[1]);
			dontreturn = true;
		}
		if (args[0] == -738295409 && (scripteventprot || ceobanprot))
		{
			ScriptEventBlockMessage("Ceo Ban", args[1]);
			dontreturn = true;
		}
		std::uint64_t Soundspam[2] = { 1097312011, -1162153263 };
		for (int i = 0; i < 1; i++)
		{
			if (args[0] == Soundspam[i] && (scripteventprot || soundspamprot))
			{
				ScriptEventBlockMessage("Sound Spam", args[1]);
				dontreturn = true;
			}
		}
		std::uint64_t NotifiedKicks[] = { -442306200, 1352706024, -1252906024, -1890951223, -1559754940, -1243454584, -891346918 };
		for (int i = 0; i < (sizeof(NotifiedKicks) / sizeof(NotifiedKicks[0])); i++)
		{
			if (args[0] == NotifiedKicks[i] && (scripteventprot || nonhostkickprot))
			{
				ScriptEventBlockMessage("Non Host Kick", args[1]);
				dontreturn = true;
			}
		}
		//std::uint64_t NonNotifiedKicks[] = { 848786118 };
		//for (int i = 0; i < (sizeof(NonNotifiedKicks) / sizeof(NonNotifiedKicks[0])); i++)
		//{
		//	if (args[0] == NonNotifiedKicks[i] && (scripteventprot || nonhostkickprot))
		//	{
		//		dontreturn = true;
		//	}
		//}
	}
	if (scripteventprot || dontreturn)
		return false;
	return result;
}


bool transactionerrorprot = true;

SetWarningMessage2 OG_SET_WARNING_MESSAGE_2 = nullptr;
void HK_SET_WARNING_MESSAGE_2(char* entryHeader, char* entryLine1, int instructionalKey, char* entryLine2, BOOL p4, Any p5, Any* p6, Any* p7, BOOL background)
{
	if (transactionerrorprot)
	{
		if (SCRIPT::GET_HASH_OF_THIS_SCRIPT_NAME() == GAMEPLAY::GET_HASH_KEY("shop_controller"))
		{
			return;
		}
	}
	return static_cast<decltype(&HK_SET_WARNING_MESSAGE_2)>(OG_SET_WARNING_MESSAGE_2)(entryHeader, entryLine1, instructionalKey, entryLine2, p4, p5, p6, p7, background);
}


bool pickupprot = false;

PickupProtSig OG_PICKUP_PROT_SIG = nullptr;
bool __cdecl HK_PICKUP_PROT_SIG(void* classidonthave, void* unk, CPed* ped)
{
	if (pickupprot)
	{
		return false;
	}
	return static_cast<decltype(&HK_PICKUP_PROT_SIG)>(OG_PICKUP_PROT_SIG)(classidonthave, unk, ped);
}

GetLabelText OG_GET_LABEL_TEXT = nullptr;
const char* __cdecl HK_GET_LABEL_TEXT(void* unk, const char* label)
{
	//Add Custom Game Texts here

	if (strcmp("HUD_JOINING", label) == 0)
		return "Isn't " MENU_NAME " the fucking best?";

	return static_cast<decltype(&HK_GET_LABEL_TEXT)>(OG_GET_LABEL_TEXT)(unk, label);
}

bool Hooking::HookNatives()
{
	MH_STATUS status = MH_CreateHook(Hooking::is_DLC_present, HK_IS_DLC_PRESENT, (void**)&OG_IS_DLC_PRESENT);
	if ((status != MH_OK && status != MH_ERROR_ALREADY_CREATED) || MH_EnableHook(Hooking::is_DLC_present) != MH_OK)
		return false;
	else Logger::Custom("Hooking", "Hooked --> is_dlc_present");
	Hooking::m_hooks.push_back(Hooking::is_DLC_present);

	status = MH_CreateHook(Hooking::get_event_data, HK_GET_EVENT_DATA, (void**)&OG_GET_EVENT_DATA);
	if ((status != MH_OK && status != MH_ERROR_ALREADY_CREATED) || MH_EnableHook(Hooking::get_event_data) != MH_OK)
		return false;
	else Logger::Custom("Hooking", "Hooked --> get_event_data");
	Hooking::m_hooks.push_back(Hooking::get_event_data);

	status = MH_CreateHook(Hooking::set_warning_message_2, HK_SET_WARNING_MESSAGE_2, (void**)&OG_SET_WARNING_MESSAGE_2);
	if ((status != MH_OK && status != MH_ERROR_ALREADY_CREATED) || MH_EnableHook(Hooking::set_warning_message_2) != MH_OK)
		return false;
	else Logger::Custom("Hooking", "Hooked --> set_warning_message_2");
	Hooking::m_hooks.push_back(Hooking::set_warning_message_2);

	status = MH_CreateHook(Hooking::pickup_prot_sig, HK_PICKUP_PROT_SIG, (void**)&OG_PICKUP_PROT_SIG);
	if ((status != MH_OK && status != MH_ERROR_ALREADY_CREATED) || MH_EnableHook(Hooking::pickup_prot_sig) != MH_OK)
		return false;
	else Logger::Custom("Hooking", "Hooked --> pickup_prot_sig");
	Hooking::m_hooks.push_back(Hooking::pickup_prot_sig);

	status = MH_CreateHook(Hooking::get_label_text, HK_GET_LABEL_TEXT, (void**)&OG_GET_LABEL_TEXT);
	if ((status != MH_OK && status != MH_ERROR_ALREADY_CREATED) || MH_EnableHook(Hooking::get_label_text) != MH_OK)
		return false;
	else Logger::Custom("Hooking", "Hooked --> get_label_text");
	Hooking::m_hooks.push_back(Hooking::get_label_text);

	return true;
}

void __stdcall ScriptFunction(LPVOID lpParameter)
{
	try
	{
		ScriptMain();
	}
	catch (...)
	{
		Logger::Fatal("Failed scriptFiber");
	}
}

void Hooking::onTickInit()
{
	if (mainFiber == nullptr)
		mainFiber = ConvertThreadToFiber(nullptr);

	if (timeGetTime() < wakeAt)
		return;

	static HANDLE scriptFiber;
	if (scriptFiber)
		SwitchToFiber(scriptFiber);
	else
		scriptFiber = CreateFiber(NULL, ScriptFunction, nullptr);
}

void Hooking::FailPatterns(const char* name)
{
	char buf[4096];
	sprintf_s(buf, "finding %s", name);
	Logger::Error(buf);
	Cleanup();
}

/*
//CPatternResult
*/

CPatternResult::CPatternResult(void* pVoid) :
	m_pVoid(pVoid)
{}
CPatternResult::CPatternResult(void* pVoid, void* pBegin, void* pEnd) :
	m_pVoid(pVoid),
	m_pBegin(pBegin),
	m_pEnd(pEnd)
{}
CPatternResult::~CPatternResult() {}

/*
//CPattern Public
*/

CPattern::CPattern(char* szByte, char* szMask) :
	m_szByte(szByte),
	m_szMask(szMask),
	m_bSet(false)
{}
CPattern::~CPattern() {}

CPattern&	CPattern::find(int i, uint64_t startAddress)
{
	match(i, startAddress, false);
	if (m_result.size() <= i)
		m_result.push_back(nullptr);
	return *this;
}

CPattern&	CPattern::virtual_find(int i, uint64_t startAddress)
{
	match(i, startAddress, true);
	if (m_result.size() <= i)
		m_result.push_back(nullptr);
	return *this;
}

CPatternResult	CPattern::get(int i)
{
	if (m_result.size() > i)
		return m_result[i];
	return nullptr;
}

/*
//CPattern Private
*/
bool	CPattern::match(int i, uint64_t startAddress, bool virt)
{
	if (m_bSet)
		return false;
	uint64_t	begin = 0;
	uint64_t	end = 0;
	if (!virt)
	{
		CMetaData::init();
		begin = CMetaData::begin() + startAddress;
		end = CMetaData::end();
		if (begin >= end)
			return false;
	}
	int		j = 0;
	do
	{
		if (virt)
			begin = virtual_find_pattern(startAddress, (BYTE*)m_szByte, m_szMask) + 1;
		else
			begin = find_pattern(begin, end, (BYTE*)m_szByte, m_szMask) + 1;
		if (begin == NULL)
			break;
		j++;
	} while (j < i);

	m_bSet = true;
	return true;
}

bool	CPattern::byte_compare(const BYTE* pData, const BYTE* btMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++btMask)
		if (*szMask == 'x' && *pData != *btMask)
			break;
	if ((*szMask) != 0)
		return false;
	return true;
}


uint64_t	CPattern::find_pattern(uint64_t address, uint64_t end, BYTE *btMask, char *szMask)
{
	size_t len = strlen(szMask) + 1;
	for (uint64_t i = 0; i < (end - address - len); i++)
	{
		BYTE*	ptr = (BYTE*)(address + i);
		if (byte_compare(ptr, btMask, szMask))
		{
			m_result.push_back(CPatternResult((void*)(address + i)));
			return address + i;
		}
	}
	return NULL;
}

uint64_t	CPattern::virtual_find_pattern(uint64_t address, BYTE *btMask, char *szMask)
{
	MEMORY_BASIC_INFORMATION mbi;
	char*	pStart = nullptr;
	char*	pEnd = nullptr;
	char*	res = nullptr;
	size_t	maskLen = strlen(szMask);

	while (res == nullptr && sizeof(mbi) == VirtualQuery(pEnd, &mbi, sizeof(mbi)))
	{
		pStart = pEnd;
		pEnd += mbi.RegionSize;
		if (mbi.Protect != PAGE_READWRITE || mbi.State != MEM_COMMIT)
			continue;

		for (int i = 0; pStart < pEnd - maskLen && res == nullptr; ++pStart)
		{
			if (byte_compare((BYTE*)pStart, btMask, szMask))
			{
				m_result.push_back(CPatternResult((void*)pStart, mbi.BaseAddress, pEnd));
				res = pStart;
			}
		}

		mbi = {};
	}
	return (uint64_t)res;
}

void	failPat(const char* name)
{
	Logger::Fatal("Failed to find %s pattern.", name);
}

template <typename T>
void	setPat
(
	const char*	name,
	char*		pat,
	char*		mask,
	T**			out,
	bool		rel,
	int			offset = 0,
	int			deref = 0,
	int			skip = 0
)
{
	T*	ptr = nullptr;

	CPattern pattern(pat, mask);
	pattern.find(1 + skip);
	if (rel)
		ptr = pattern.get(skip).get_rel<T>(offset);
	else
		ptr = pattern.get(skip).get<T>(offset);

	while (true)
	{
        if (ptr == nullptr)
            failPat(name);
        else
            Logger::Custom("Pattern", "Variable: %s", name);

		if (deref <= 0)
			break;
		ptr = *(T**)ptr;
		--deref;
	}

	*out = ptr;
	return;
}

template <typename T>
void	setFn
(
	const char*	name,
	char*		pat,
	char*		mask,
	T*			out,
	int			offset = 0,
	int			skip = 0,
	bool		rel = false
)
{
	char*	ptr = nullptr;

	CPattern pattern(pat, mask);
	pattern.find(1 + skip);
	if (rel)
		ptr = pattern.get(skip).get_rel<char>(offset);
	else
		ptr = pattern.get(skip).get<char>(offset);

    if (ptr == nullptr)
        failPat(name);
    else
        Logger::Custom("Pattern", "Function: %s", name);

	*out = (T)ptr;
	return;
}

void Hooking::FindPatterns()
{
	HANDLE steam = GetModuleHandleA("steam_api64.dll");

	auto p_activeThread = pattern("E8 ? ? ? ? 48 8B 88 10 01 00 00");
	auto p_blipList = pattern("4C 8D 05 ? ? ? ? 0F B7 C1");
	auto p_fixVector3Result = pattern("83 79 18 00 48 8B D1 74 4A FF 4A 18");
	auto p_gameLegals = pattern("72 1F E8 ? ? ? ? 8B 0D");
	auto p_gameLogos = pattern("70 6C 61 74 66 6F 72 6D 3A");
	auto p_gameState = pattern("83 3D ? ? ? ? ? 8A D9 74 0A");
	//auto p_modelCheck = pattern("48 85 C0 0F 84 ? ? ? ? 8B 48 50");
	//auto p_modelSpawn = pattern("48 8B C8 FF 52 30 84 C0 74 05 48");
	auto p_nativeTable = pattern("76 32 48 8B 53 40 48 8D 0D");
	auto p_worldPtr = pattern("48 8B 05 ? ? ? ? 45 ? ? ? ? 48 8B 48 08 48 85 C9 74 07");
	auto p_globalPtr = pattern("4C 8D 05 ? ? ? ? 4D 8B 08 4D 85 C9 74 11");
	auto p_eventHook = pattern("48 83 EC 28 E8 ? ? ? ? 48 8B 0D ? ? ? ? 4C 8D 0D ? ? ? ? 4C 8D 05 ? ? ? ? BA 03");

	setPat<uint64_t>("frame_count",
		"\x8B\x15\x00\x00\x00\x00\x41\xFF\xCF",
		"xx????xxx",
		&Hooking::m_frameCount,
		true,
		2);

	Sleep(100);

	setFn<fpIsDLCPresent>("is_dlc_present",
		"\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x20\x81\xF9\x00\x00\x00\x00",
		"xxxx?xxxxxxx????",
		&Hooking::is_DLC_present);

	Sleep(100);

	/*
	setPat<char>("native_return",
		"\x44\x89\x75\x9F\x49\x8D\x0C\xC1",
		"xxxxxxxx",
		&Hooking::m_NativeRetn,
		false,
		19);

	Sleep(100);
	*/
	setFn<PickupProtSig>("pickup_prot_sig",
		"\x49\x8B\x80\x00\x00\x00\x00\x48\x85\xC0\x74\x0C\xF6\x80\x00\x00\x00\x00\x00\x75\x03\x32\xC0\xC3",
		"xxx????xxxxxxx?????xxxxx",
		&Hooking::pickup_prot_sig);

	Sleep(100);

	setFn<SetWarningMessage2>("set_warning_message_2",
		"\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x41\x56\x41\x57\x48\x83\xEC\x60\x4C\x8B\xF2\x48\x8B\x94\x24\x00\x00\x00\x00\x33\xDB",
		"xxxx?xxxx?xxxx?xxxxxxxxxxxxxxxx????xx",
		&Hooking::set_warning_message_2);

	Sleep(100);

	setFn<SetSessionWeather>("set_session_weather",
		"\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x30\x40\x8A\xE9",
		"xxxx?xxxx?xxxx?xxxxxxxx",
		&Hooking::set_session_weather);

	Sleep(100);

	setFn<SetSessionTime>("session_time_set",
		"\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x20\x8B\xF9\x48\x8B\x0D\x00\x00\x00\x00\x48\x8B\xDA\x33\xD2\xE9\x00\x00\x00\x00",
		"xxxxxxxxxxxxxxx????xxxxxx????",
		&Hooking::set_session_time);

	Sleep(100);

	setPat<twoBytes>("owned_explosion_bypass",
		"\x3B\xF8\x40\x0F\x94\xC7",
		"xxxxxx",
		&owned_explosion_bypass,
		false);

	Sleep(100);

	//setPat<twoBytes>("model_spawn_bypass",
	//	"\xD8\x48\x85\xC0\x74\x12\x48\x8B\x10\x48\x8B\xC8\xFF\x52\x30\x84\xC0\x74\x05",
	//	"xxxxxxxxxxxxxxxxxxx",
	//	&model_spawn_bypass,
	//	false,
	//	17);

	//Sleep(100);

	//setPat<Bytes24>("model_check_bypass",
	//	"\xCE\x33\xC1\x0F\xBA\xF0\x1D\x8B\xC8\x33\x4D\x67",
	//	"xxxxxxxxxxxx",
	//	&model_check_bypass,
	//	false,
	//	40);

	//Sleep(100);

	setPat<clockTime>("clock_time",
		"\x48\x8D\x0D\x00\x00\x00\x00\x8B\xFA\xE8\x00\x00\x00\x00\x44\x8D\x0C\x5B",
		"xxx????xxx????xxxx",
		&Hooking::ClockTime,
		true,
		3);

	Sleep(100);

	setFn<GetPlayerAddress>("get_player_address",
		"\x40\x53\x48\x83\xEC\x20\x33\xDB\x38\x1D\x00\x00\x00\x00\x74\x1C",
		"xxxxxxxxxx????xx",
		&Hooking::get_player_address);

	Sleep(100);

	setFn<TriggerScriptEvent>("trigger_script_event", 
		"\x48\x8B\xC4\x48\x89\x58\x08\x48\x89\x68\x10\x48\x89\x70\x18\x48\x89\x78\x20\x41\x56\x48\x81\xEC\x00\x00\x00\x00\x45\x8B\xF0\x41\x8B\xF9",
		"xxxxxxxxxxxxxxxxxxxxxxxx????xxxxxx",
		&Hooking::trigger_script_event);

	Sleep(100);

	setFn<GetEventData>("get_event_data",
		"\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x00\x49\x8B\xF8\x4C\x8D\x05",
		"xxxx?xxxx?xxxxxx",
		&Hooking::get_event_data);

	Sleep(100);

	setFn<FileRegister>("file_register",
		"\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x7C\x24\x00\x41\x54\x41\x56\x41\x57\x48\x83\xEC\x50\x48\x8B\xEA\x4C\x8B\xFA\x48\x8B\xD9\x4D\x85\xC9",
		"xxxx?xxxx?xxxx?xxxxxxxxxxxxxxxxxxxxxx",
		&Hooking::file_register);

	Sleep(100);

	setFn<GetLabelText>("get_label_text",
		"\x75\x00\xE8\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x65\x48\x8B\x04\x25\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x48\x8B\x04\xC8\x8B\x0C\x02\xD1\xE9",
		"x?x????xx????xxxxx????x????xxxxxxxxx",
		&Hooking::get_label_text, -19);

	char * c_location = nullptr;
	void * v_location = nullptr;

	Sleep(5000);

	//Logger::Custom("Spoofing", "Initializing Return Spoof...");
	//std::copy_n(m_NativeRetn, 2, m_OriginalNativeRetn);

	//Logger::Info("Getting Game State...");
	c_location = p_gameState.count(1).get(0).get<char>(2);
	c_location == nullptr ? FailPatterns("gameState") : m_gameState = reinterpret_cast<decltype(m_gameState)>(c_location + *(int32_t*)c_location + 5);

	//Logger::Info("Getting vector3 result fixer func...");
	v_location = p_fixVector3Result.count(1).get(0).get<void>(0);
	if (v_location != nullptr) scrNativeCallContext::SetVectorResults = (void(*)(scrNativeCallContext*))(v_location);

	//Logger::Info("Getting native registration table...");
	c_location = p_nativeTable.count(1).get(0).get<char>(9);
	c_location == nullptr ? FailPatterns("native registration table") : m_registrationTable = reinterpret_cast<decltype(m_registrationTable)>(c_location + *(int32_t*)c_location + 4);

	//Logger::Info("Getting World Pointer...");
	c_location = p_worldPtr.count(1).get(0).get<char>(0);
	c_location == nullptr ? FailPatterns("world Pointer") : m_worldPtr = reinterpret_cast<uint64_t>(c_location) + *reinterpret_cast<int*>(reinterpret_cast<uint64_t>(c_location) + 3) + 7;

	//Logger::Info("Getting Blip List...");
	c_location = p_blipList.count(1).get(0).get<char>(0);
	c_location == nullptr ? FailPatterns("blip List") : m_blipList = (BlipList*)(c_location + *reinterpret_cast<int*>(c_location + 3) + 7);

	//Logger::Info("Bypassing Object restrictions...");

	//if (p_modelCheck.size() > 0) {

	//	v_location = p_modelCheck.count(1).get(0).get<void>(0);
	//	Memory::nop(v_location, 24);
	//}

	//if (p_modelSpawn.size() > 0) {

	//	v_location = p_modelSpawn.count(1).get(0).get<void>(8);
	//	Memory::nop(v_location, 2);
	//}

	//Logger::Info("Getting active script thread...");
	c_location = p_activeThread.count(1).get(0).get<char>(1);
	c_location == nullptr ? FailPatterns("Active Script Thread") : GetActiveThread = reinterpret_cast<decltype(GetActiveThread)>(c_location + *(int32_t*)c_location + 4);

	//Logger::Info("Getting World Pointer...");
	c_location = p_globalPtr.count(1).get(0).get<char>(0);
	__int64 patternAddr = NULL;
	c_location == nullptr ? FailPatterns("globalTable") : patternAddr = reinterpret_cast<decltype(patternAddr)>(c_location);
	m_globalPtr = (__int64**)(patternAddr + *(int*)(patternAddr + 3) + 7);

	//Logger::Info("Getting Event Hook...");
	if ((c_location = p_eventHook.count(1).get(0).get<char>(0)))
	{
		int i = 0, j = 0, matches = 0, found = 0;
		char* pattern = "\x4C\x8D\x05";
		while (found != EVENT_COUNT)
		{
			if (c_location[i] == pattern[j])
			{
				if (++matches == 3)
				{
					EventPtr.push_back((void*)(reinterpret_cast<uint64_t>(c_location + i - j) + *reinterpret_cast<int*>(c_location + i + 1) + 7));
					found++;
					j = matches = 0;
				}
				j++;
			}
			else
			{
				matches = j = 0;
			}
			i++;
		}
	}

	//Logger::Info("Initializing natives...");
	CrossMapping::initNativeMap();

	Sleep(250);

	char* rockstarID;
	char* AcceptedRockstarIDs[] = { /*"107125159"*/"99" };
	int NETWORK_HANDLE[76];
	NETWORK::NETWORK_HANDLE_FROM_PLAYER(PLAYER::PLAYER_ID(), &NETWORK_HANDLE[0], 13);
	if (NETWORK::NETWORK_IS_HANDLE_VALID(&NETWORK_HANDLE[0], 13))
	{
		rockstarID = NETWORK::NETWORK_MEMBER_ID_FROM_GAMER_HANDLE(&NETWORK_HANDLE[0]);
	}
	for (int i = 0; i < (sizeof(AcceptedRockstarIDs) / sizeof(AcceptedRockstarIDs[0])); i++)
	{
		if (strcmp(AcceptedRockstarIDs[i], rockstarID) == 0) { Menu::Settings::vipUser = true; }
	}

	//Logger::Info("Checking if GTA V is ready...");
	while (*m_gameState != GameStatePlaying) {
		Sleep(200);
	}
	/*Logger::Info("GTA V ready!");*/

	Sleep(100);

}

static Hooking::NativeHandler _Handler(uint64_t origHash)
{
	uint64_t newHash = CrossMapping::MapNative(origHash);
	if (newHash == 0)
	{
		return nullptr;
	}

	Hooking::NativeRegistrationNew * table = m_registrationTable[newHash & 0xFF];

	for (; table; table = table->getNextRegistration())
	{
		for (uint32_t i = 0; i < table->getNumEntries(); i++)
		{
			if (newHash == table->getHash(i))
			{
				return table->handlers[i];
			}
		}
	}
	return nullptr;
}

Hooking::NativeHandler Hooking::GetNativeHandler(uint64_t origHash)
{
	auto& handler = m_handlerCache[origHash];

	if (handler == nullptr)
	{
		handler = _Handler(origHash);
	}

	return handler;
}

eGameState Hooking::GetGameState()
{
	return *m_gameState;
}

uint64_t Hooking::getWorldPtr()
{
	return m_worldPtr;
}
void WAIT(DWORD ms)
{
	wakeAt = timeGetTime() + ms;
	SwitchToFiber(mainFiber);
}

void Hooking::Cleanup()
{
	Logger::Info("Cleaning up hooks");
	for (int i = 0; i < m_hooks.size(); i++)
		if (MH_DisableHook(m_hooks[i]) != MH_OK && MH_RemoveHook(m_hooks[i]) != MH_OK)
			Logger::Error("Failed to unhook %p", (void*)m_hooks[i]);
	MH_Uninitialize();

	FreeLibraryAndExitThread(static_cast<HMODULE>(_hmoduleDLL), 0);
}

/*
void* Hooking::GetReturnAddress()
{
	return Hooking::m_NativeRetn;
}

void Hooking::NativePatch(bool toggle)
{
	if (toggle)
	{
		std::uint8_t data[2] = { (std::uint8_t)0xFF, (std::uint8_t)0x23 };
		std::copy(std::begin(data), std::end(data), (std::uint8_t*)Hooking::m_NativeRetn);
	}
	else
		std::copy(std::begin(Hooking::m_OriginalNativeRetn), std::end(Hooking::m_OriginalNativeRetn), Hooking::m_NativeRetn);
}
*/

void Hooking::ownedExplosionBypass (bool toggle)
{
	constexpr twoBytes patched = { 0x39, 0xDB };
	constexpr twoBytes restore = { 0x3B, 0xF8 };
	if (owned_explosion_bypass == nullptr)
		return;
	*owned_explosion_bypass = toggle ? patched : restore;
}

void Hooking::modelSpawnBypass(bool toggle)
{
	static void* stsins;
	if (!stsins) stsins = Memory::pattern("74 6B 48 85 D2").count(1).get(0).get<void>(0x222);
	*(PUINT16)stsins = toggle ? 0x9090 : 0x0574;

	//Detected

	//constexpr twoBytes patched = { 0x90, 0x90 };
	//constexpr twoBytes restore = { 0x74, 0x05 };
	//if (model_spawn_bypass == nullptr)
	//	return;
	//*model_spawn_bypass = toggle ? patched : restore;
}

void Hooking::modelCheckBypass(bool toggle)
{
	//Detected

	/*
	constexpr Bytes24 patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
		0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
		0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	constexpr Bytes24 restore = { 0x48, 0x85, 0xC0, 0x0F, 0x84, 0x99,
		0x02, 0x00, 0x00, 0x8B, 0x48, 0x50, 0xC1, 0xE9, 0x04, 0xF6,
		0xC1, 0x01, 0x0F, 0x84, 0x8A, 0x02, 0x00, 0x00 };
	if (model_check_bypass == nullptr)
		return;
	*model_check_bypass = toggle ? patched : restore;
	*/
}

void Hooking::defuseEvent(RockstarEvent e, bool toggle)
{
	static const unsigned char retn = 0xC3;
	char* p = (char*)EventPtr[e];
	if (toggle)
	{
        if (EventRestore[e] == 0)
            EventRestore[e] = p[0];
		*p = retn;
	}
	else
	{
        if (EventRestore[e] != 0)
            *p = EventRestore[e];
	}
}

__int64** Hooking::getGlobalPtr()
{
	return m_globalPtr;
}
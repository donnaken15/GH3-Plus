
#define IM_SUICIDAL true
// i would be regardless
// since im adding support
// for this stupid app
// and quote unquote

#include "gh3\GH3Keys.h"
#include "core\Patcher.h"
#include "RichPresence.h"
#if !IM_SUICIDAL
#include "discord_game_sdk.h"
#else
#include "discord_rpc.h"
#endif
#include "gh3\GH3Functions.h"
#include "gh3\GH3GlobalAddresses.h"
#include "gh3\malloc.h"
#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#if !IM_SUICIDAL
#pragma comment(lib, "E:\\DiscordGameSDK\\lib\\x86\\discord_game_sdk.dll.lib")
static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

struct Application {
	struct IDiscordCore* core;
	struct IDiscordUserManager* users;
	struct IDiscordAchievementManager* achievements;
	struct IDiscordActivityManager* activities;
	struct IDiscordRelationshipManager* relationships;
	struct IDiscordApplicationManager* application;
	struct IDiscordLobbyManager* lobbies;
	DiscordUserId user_id;
};
Application app;
IDiscordUserEvents users_events;
IDiscordActivityEvents activities_events;
IDiscordRelationshipEvents relationships_events;
DiscordCreateParams params;
DiscordActivity pres;
#else
static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);
#pragma comment(lib, "discord-rpc.lib")

DiscordRichPresence RPC;
char state[128];
char details[128];
char smltxt[128];
char smlimage[128];
char lrgtxt[128];
char lrgimage[128];
int starttime = -1;
int endtime = -1;
#endif

int UpdatePresence(QbStruct*str, QbScript*scr)
{
	// show params (logger)
	//ExecuteScript2(QbKey("PrintStruct"), str, QbKey((uint32_t)0), 0, 0, 0, 0, 0, 0, 0);
#if !IM_SUICIDAL
	char* state = "";
	char* details = "";
	char* smltxt = "";
	char* smlimage = "";
	char* lrgtxt = "";
	char* lrgimage = "slash_hat";
	int starttime = 0;
	int endtime = 0;
	// should it be this way?
	// thought of doing it this way so
	// the same params wouldnt have
	// to be entered and gotten
	// multiple times per second,
	// so the already existing
	// presence details can exist
	// when executing this func
	// multiple times until a
	// blank param is thrown in
	pres.type = EDiscordActivityType::DiscordActivityType_Playing;
	if (str->GetString(QbKey("state"), state))
		strcpy_s(pres.state, state);
	if (str->GetString(QbKey("details"), details))
		strcpy_s(pres.details, details);
	if (str->GetString(QbKey("smltxt"), smltxt))
		strcpy_s(pres.assets.small_text, smltxt);
	if (str->GetString(QbKey("smlimage"), smlimage))
		strcpy_s(pres.assets.small_image, smlimage);
	if (str->GetString(QbKey("lrgtxt"), lrgtxt))
		strcpy_s(pres.assets.large_text, lrgtxt);
	if (str->GetString(QbKey("lrgimage"), lrgimage))
		strcpy_s(pres.assets.large_image, lrgimage);
	//strcpy_s(pres.secrets.join, "421"); // discord fails to invite anyone through this
	str->GetInt(QbKey("starttime"), starttime);
	bool endtimepassed = str->GetInt(QbKey("endtime"), endtime);
	if (endtimepassed)
	{
		if (endtime)
		{
			time(&pres.timestamps.start);
			pres.timestamps.end = endtime - starttime + time(0);
		}
		else
		{
			pres.timestamps.start = 0;
			pres.timestamps.end = 0;
		}
	}

	app.activities->update_activity(app.activities, &pres, 0, 0);
#else
	char*buf;
	if (str->GetString(QbKey("state"), buf))
	{
		strcpy_s(state, sizeof(state), buf);
		RPC.state = state;
	}
	if (str->GetString(QbKey("details"), buf))
	{
		strcpy_s(details, sizeof(details), buf);
		RPC.details = details;
	}
	if (str->GetString(QbKey("smltxt"), buf))
	{
		strcpy_s(smltxt, sizeof(smltxt), buf);
		RPC.smallImageText = smltxt;
	}
	if (str->GetString(QbKey("smlimage"), buf))
	{
		strcpy_s(smlimage, sizeof(smlimage), buf);
		RPC.smallImageKey = smlimage;
	}
	if (str->GetString(QbKey("lrgtxt"), buf))
	{
		strcpy_s(lrgtxt, sizeof(lrgtxt), buf);
		RPC.largeImageText = lrgtxt;
	}
	if (str->GetString(QbKey("lrgimage"), buf))
	{
		strcpy_s(lrgimage, sizeof(lrgimage), buf);
		RPC.largeImageKey = lrgimage;
	}
	RPC.instance = 1;
	bool starttimepassed = str->GetInt(QbKey("starttime"), starttime);
	bool endtimepassed = str->GetInt(QbKey("endtime"), endtime);
	if (endtimepassed)
	{
		if (endtime >= 0)
		{
			time(&RPC.startTimestamp);
			RPC.endTimestamp = endtime - starttime + time(0);
		}
		else
		{
			RPC.startTimestamp = 0;
			RPC.endTimestamp = 0;
		}
	}
	else if (starttimepassed && endtime == -1)
	{
		RPC.startTimestamp = time(0) + starttime;
		RPC.endTimestamp = 0;
	}
	if (starttime == -1)
		RPC.startTimestamp = 0;
	Discord_UpdatePresence(&RPC);
#endif
	return 1;
}

static void *gameFrameDetour = (void *)0x0048452C;
void DiscordCallbacks()
{
#if !IM_SUICIDAL
	app.core->run_callbacks(app.core); // do i need to do this every frame
#else
	Discord_RunCallbacks();
#endif
}

#define RPC_FASTGH3 true
#if !IM_SUICIDAL
constexpr unsigned long long RPC_ID = RPC_FASTGH3 ? 385161862586695686 : 940793683144507492;
#else
constexpr char*RPC_ID_STR = RPC_FASTGH3 ? "385161862586695686" : "940793683144507492";
#endif

int why;

#define handletest false

#if IM_SUICIDAL
void Shutdown();

#if handletest
void runScript(QbKey script, QbStruct*params)
{
	ExecuteScript2(script, params,
		((QbKey)(uint32_t)0), 0, 0, 0, 0, 0, 0, 0);
}
#define STUPID(x,y) InsertCStringItem(x,const_cast<char*>y)

bool DiscordRespond(QbStruct*params, QbScript*_this)
{
	char*user = 0;
	int reply = 0;
	if (!params->GetString(QbKey("user"), user) ||
		!params->GetInt(QbKey("reply"), reply))
		return 0;
	Discord_Respond(user, reply);
	return 1;
}

QbStruct*InitCallbackData()
{
	return (QbStruct*)memset(qbMalloc(sizeof(QbStruct), 1), 0, sizeof(QbStruct));
}

void DE_joinReq(const DiscordUser* user)
{
	QbStruct* callbackData = InitCallbackData();
	callbackData->STUPID(QbKey("username"), (user->username));
	callbackData->STUPID(QbKey("userid"), (user->userId));
	callbackData->STUPID(QbKey("discriminator"), (user->discriminator));
	runScript(QbKey("discord_event_join_req"), callbackData);
}

#endif

void init()
{
#if handletest
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = 0; // don't know when and how i'm meant
	handlers.errored = 0; // to HANDLE these functions badum tss
	handlers.disconnected = 0;
	handlers.joinGame = 0;
	handlers.spectateGame = 0;
	handlers.joinRequest = DE_joinReq;
	Discord_Initialize(RPC_ID_STR, &handlers, 1, 0);
#else
	Discord_Initialize(RPC_ID_STR, 0, 1, 0);
#endif
	//atexit(Shutdown);
}
#endif

void ApplyHack()
{

#if !IM_SUICIDAL
#else
#endif
#if !IM_SUICIDAL
	// ?????
	memset(&app, 0, sizeof(Application));
	memset(&users_events, 0, sizeof(users_events));
	memset(&activities_events, 0, sizeof(activities_events));
	memset(&relationships_events, 0, sizeof(relationships_events));

	DiscordCreateParamsSetDefault(&params);
	params.client_id = RPC_ID;
	params.flags = DiscordCreateFlags_NoRequireDiscord;
	params.event_data = &app;
	params.activity_events = &activities_events;
	params.relationship_events = &relationships_events;
	params.user_events = &users_events;
	why = DiscordCreate(DISCORD_VERSION, &params, &app.core);
	// trash app

	/*if (why)
	{
		MessageBoxA(0, "Failed to init RPC", "", 0);
	}*/

	if (!why)
	{
		app.users = app.core->get_user_manager(app.core);
		app.achievements = app.core->get_achievement_manager(app.core);
		app.activities = app.core->get_activity_manager(app.core);
		app.application = app.core->get_application_manager(app.core);
		app.lobbies = app.core->get_lobby_manager(app.core);

		g_patcher.WriteCall(gameFrameDetour, DiscordCallbacks);
		g_patcher.WriteJmp((void*)0x004CDF43, UpdatePresence);
	}
	//g_patcher.WritePointer((void*)0x00957094, UpdatePresence);
	// ^ use if i can get ahead of cfunc init without breaking something
	// and not have to depend on an already patched EXE to change
#else
	g_patcher.WriteCall(gameFrameDetour, DiscordCallbacks);
	g_patcher.WriteCall((void*)0x0047B346, init);
	g_patcher.WriteJmp((void*)0x004CDF43, UpdatePresence);
#endif

}

void Shutdown()
{
#if !IM_SUICIDAL
	if (!why)
		app.activities->clear_activity(app.activities, 0, 0);
#else
	//Discord_Shutdown();
	// WHY DOES INCLUDING THIS MAKE IT NOT WORK
#endif
}
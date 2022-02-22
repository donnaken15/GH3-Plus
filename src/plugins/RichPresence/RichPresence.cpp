
#include "gh3\GH3Keys.h"
#include "core\Patcher.h"
#include "RichPresence.h"
#include "discord_game_sdk.h"
#include "gh3\GH3Functions.h"
#include "gh3\GH3GlobalAddresses.h"
#include "gh3\malloc.h"
#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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
//QbStruct*nullParams;

int UpdatePresence(QbStruct*str, QbScript*scr)
{
	memset(&pres, 0, sizeof(pres));
	//memset(nullParams, 0, sizeof(QbStruct));

	char*state = "";
	char*details = state;
	char*smltxt = "";
	char*smlimage = "";
	char*lrgtxt = "";
	char*lrgimage = "slash_hat";
	str->GetString(QbKey("state"), state);
	str->GetString(QbKey("details"), details);
	strcpy_s(pres.state, 128, state);
	strcpy_s(pres.details, 128, details);
	float starttime = 0;
	//int starttime_ = 0;
	int endtime = 0;
	str->GetFloat(QbKey("starttime"), starttime);
	str->GetInt(QbKey("endtime"), endtime);
	str->GetString(QbKey("smltxt"), smltxt);
	str->GetString(QbKey("smlimage"), smlimage);
	str->GetString(QbKey("lrgtxt"), lrgtxt);
	str->GetString(QbKey("lrgimage"), lrgimage);
	strcpy_s(pres.assets.small_text, 128, smltxt);
	strcpy_s(pres.assets.small_image, 128, smlimage);
	strcpy_s(pres.assets.large_text, 128, lrgtxt);
	strcpy_s(pres.assets.large_image, 128, lrgimage);
	if (endtime)
	{
		//time(&pres.timestamps.start);
		//time(&pres.timestamps.end);
		//starttime_ = starttime;
		pres.timestamps.start = /*starttime_ +*/ time(0);//prestest
		pres.timestamps.end = endtime - starttime + time(0);
	}
	else
	{
		pres.timestamps.start = 0;
		pres.timestamps.end = 0;
	}

	app.activities->update_activity(app.activities, &pres, 0, 0);
	return 1;
}

static void *gameFrameDetour = (void *)0x0048452C;
void DiscordCallbacks()
{
	//UpdatePresence();
	app.core->run_callbacks(app.core);
}

void ApplyHack()
{


	// ?????
	memset(&app, 0, sizeof(Application));
	memset(&users_events, 0, sizeof(users_events));
	memset(&activities_events, 0, sizeof(activities_events));
	memset(&relationships_events, 0, sizeof(relationships_events));

	DiscordCreateParamsSetDefault(&params);
	params.client_id = 940793683144507492UL;
	params.flags = DiscordCreateFlags_NoRequireDiscord;
	params.event_data = &app;
	params.activity_events = &activities_events;
	params.relationship_events = &relationships_events;
	params.user_events = &users_events;
	DiscordCreate(DISCORD_VERSION, &params, &app.core);

	app.users = app.core->get_user_manager(app.core);
	app.achievements = app.core->get_achievement_manager(app.core);
	app.activities = app.core->get_activity_manager(app.core);
	app.application = app.core->get_application_manager(app.core);
	app.lobbies = app.core->get_lobby_manager(app.core);

	//songname = "\"Neversoft - Amazing\"";

	//g_patcher.WriteJmp(testDetour, nakedFunction);
	g_patcher.WriteCall(gameFrameDetour, DiscordCallbacks);
	//time(&timestart);
	g_patcher.WriteJmp((void*)0x004CDF43, UpdatePresence);

	//nullParams = (QbStruct *)qbMalloc(sizeof(QbStruct), 1);
}
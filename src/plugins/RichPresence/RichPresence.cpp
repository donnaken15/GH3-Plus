
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

int UpdatePresence(QbStruct*str, QbScript*scr)
{
	// show params (logger)
	//ExecuteScript2(QbKey("PrintStruct"), str, QbKey((uint32_t)0), 0, 0, 0, 0, 0, 0, 0);

	char*state = "";
	char*details = "";
	char*smltxt = "";
	char*smlimage = "";
	char*lrgtxt = "";
	char*lrgimage = "slash_hat";
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
	if (str->GetString(QbKey("state"), state))
		strcpy_s(pres.state, 128, state);
	if (str->GetString(QbKey("details"), details))
		strcpy_s(pres.details, 128, details);
	if (str->GetString(QbKey("smltxt"), smltxt))
		strcpy_s(pres.assets.small_text, 128, smltxt);
	if (str->GetString(QbKey("smlimage"), smlimage))
		strcpy_s(pres.assets.small_image, 128, smlimage);
	if (str->GetString(QbKey("lrgtxt"), lrgtxt))
		strcpy_s(pres.assets.large_text, 128, lrgtxt);
	if (str->GetString(QbKey("lrgimage"), lrgimage))
		strcpy_s(pres.assets.large_image, 128, lrgimage);
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
	return 1;
}

static void *gameFrameDetour = (void *)0x0048452C;
void DiscordCallbacks()
{
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

	g_patcher.WriteCall(gameFrameDetour, DiscordCallbacks);
	g_patcher.WriteJmp((void*)0x004CDF43, UpdatePresence);
}
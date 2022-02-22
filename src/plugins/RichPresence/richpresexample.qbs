script() {

// scripts\discord\activity.qb

/*SetRichPresenceMode({
	string state    = 'ABC 123';
	string details  = 'DEF 456';
	int starttime   = 0;
	int endtime     = 0;
});*/

mode = *game_mode;
song = *current_song;
GetSongTime();
time = (%time + *current_starttime);
//MathFloor(%time); // NOT WORKING!!! out of order
get_song_end_time(song=%song);
total_end_time = (%total_end_time * 1000);
get_song_title (song=%song);
get_song_artist(song=%song,with_year=0);
FormatText(textname=text, '%a - %t', t = %song_title, a = %song_artist);
modetext = 'Unknown mode';
smlimage = '';
lrgimage = 'slash_hat';
smltxt = '';
lrgtxt = 'Expert'; // set automatically
//get_game_mode_ui_string(game_mode=%mode);
// TODO: use global struct
// actually theres already a struct in globaltags
if (%mode == p1_career)
{
	modetext = 'Career';
}
elseif (%mode == p1_quickplay)
{
	modetext = 'Quickplay';
}
elseif (%mode == training)
{
	modetext = 'Practice';
}
elseif (%mode == p2_faceoff)
{
	modetext = 'Face-Off';
	smlimage = 'faceoff';
}
elseif (%mode == p2_pro_faceoff)
{
	modetext = 'Pro Face-Off';
	smlimage = 'faceoff';
}
elseif (%mode == p2_battle)
{
	modetext = 'Battle';
	smlimage = 'battle';
}
elseif (%mode == p2_career)
{
	modetext = 'Co-op Career';
	smlimage = 'coop';
}
elseif (%mode == p2_coop)
{
	modetext = 'Co-op';
	smlimage = 'coop';
}

SetRichPresenceMode(
	state     =%modetext,
	details   =%text,
	starttime =%songtime,
	endtime   =%total_end_time);

}
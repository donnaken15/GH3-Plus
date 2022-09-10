script() {

// scripts\discord\activity.qb

change(rp_song_active=1);

mode = *game_mode;
song = *current_song;
get_song_title (song=%song);
get_song_artist(song=%song,with_year=0);
FormatText(textname=text, '%a - %t', t = %song_title, a = %song_artist);
modetext = 'Unknown mode';
smlimage = '';
lrgimage = 'slash_hat'; // fastgh3: main_icon // gh3+: slash_hat
smltxt = '';
get_difficulty_text(difficulty=(*current_difficulty));
FormatText(textname=lrgtxt,'%d',d=%difficulty_text);
//get_game_mode_ui_string(game_mode=%mode);
// TODO: use global struct
// actually theres already a struct in globaltags
//modetext = (*richpres_game_modes).(%mode); // ??
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
if (*boss_battle == 1)
{
	modetext = 'Boss Battle';
	smlimage = 'battle';
}
// trust me, i'd love to use switch
// but it's not working for some reason
// V     V     V
/*switch (%mode)
{
	case p1_career:
		modetext = 'Career';
		break;
	case p1_quickplay:
		modetext = 'Quickplay';
		break;
	case training:
		modetext = 'Practice';
		break;
	case p2_faceoff:
		modetext = 'Face-Off';
		smlimage = 'faceoff';
		break;
	case p2_pro_faceoff:
		modetext = 'Pro Face-Off';
		smlimage = 'faceoff';
		break;
	case p2_battle:
		modetext = 'Battle';
		smlimage = 'battle';
		break;
	case p2_career:
		modetext = 'Co-op Career';
		smlimage = 'coop';
		break;
	case p2_coop:
		modetext = 'Co-op';
		smlimage = 'coop';
		break;
}*/

SetRichPresenceMode(
	state     =%modetext,
	details   =%text,
	smltxt    =%smltxt,
	smlimage  =%smlimage,
	lrgtxt    =%lrgtxt,
	lrgimage  =%lrgimage);

SpawnScriptNow(richpres_update_song);

}
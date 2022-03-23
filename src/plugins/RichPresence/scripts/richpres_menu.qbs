script() {

//mode = *rich_presence_context;
flow_state = (*ui_flow_manager_state[0]);
flowstr = '';

/*flownum = 1;
flowkeys = [quickplay_setlist_fs];
flownames = ['Quickplay: Setlist'];

i = 0;
repeat (%flownum)
{
	printf('%d',d=%i);
	if (ChecksumEquals(a=%flow_state,b=%flownames[%i]))
	{
		printf('found');
	}
	i = (%i + 1);
}*/

/*flownames = (*richpres_flownames);
flowstr = (%flownames.%flow_state);
if (StructureContains(structure=richpres_flownames,name=%flow_state))
{
	flowstr = *richpres_flownames.quickplay_setlist_fs;
}*/

if (ChecksumEquals(a=%flow_state,b=main_menu_fs))
{
	flowstr = 'Main Menu';
}
elseif (ChecksumEquals(a=%flow_state,b=debug_menu_fs))
{
	flowstr = 'Debug Menu';
}
elseif (ChecksumEquals(a=%flow_state,b=$303AE628))
{
	flowstr = 'Custom Menu';
}
elseif (ChecksumEquals(a=%flow_state,b=career_enter_band_name_fs))
{
	flowstr = 'Career';
}
// || crashed it >:(
elseif (ChecksumEquals(a=%flow_state,b=career_choose_band_fs))
{
	flowstr = 'Career';
}
elseif (ChecksumEquals(a=%flow_state,b=career_setlist_fs))
{
	flowstr = 'Career: Setlist';
}
elseif (ChecksumEquals(a=%flow_state,b=quickplay_select_difficulty_fs))
{
	flowstr = 'Quickplay';
}
elseif (ChecksumEquals(a=%flow_state,b=quickplay_setlist_fs))
{
	flowstr = 'Quickplay: Setlist';
}
elseif (ChecksumEquals(a=%flow_state,b=coop_career_select_controllers_fs))
{
	flowstr = 'Co-op Career';
}
elseif (ChecksumEquals(a=%flow_state,b=coop_career_setlist_fs))
{
	flowstr = 'Co-op Career: Setlist';
}
elseif (ChecksumEquals(a=%flow_state,b=mp_select_controller_fs))
{
	flowstr = 'Multiplayer';
}
elseif (ChecksumEquals(a=%flow_state,b=mp_faceoff_setlist_fs))
{
	flowstr = 'Multiplayer: Setlist';
}
elseif (ChecksumEquals(a=%flow_state,b=options_select_option_fs))
{
	flowstr = 'Options';
}
elseif (ChecksumEquals(a=%flow_state,b=practice_select_mode_fs))
{
	flowstr = 'Training';
}
elseif (ChecksumEquals(a=%flow_state,b=practice_setlist_fs))
{
	flowstr = 'Training: Setlist';
}
printstruct(<...>);
if (%flowstr != '' && *rp_song_active == 0)
{
	SetRichPresenceMode(
		state	=%flowstr,
		details	='Menu',
		lrgimage='slash_hat');
}

}
script() {

song = *current_song;
GetSongTime();
time = (%time + *current_starttime);
get_song_end_time(song=%song);
total_end_time = (%total_end_time * 1000);
CastToInteger(songtime);

SetRichPresenceMode(
	starttime =%songtime,
	endtime   =%total_end_time);

}
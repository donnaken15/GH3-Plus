script() {

// TODO: find out how to run asynchronous scripts while the song is paused
// so it looks like the time left display is paused

SetRichPresenceMode(
	starttime =0,
	endtime   =5);

Wait(0.5,Seconds);

spawnscriptnow(richpres_timeleft_pauselock);

}
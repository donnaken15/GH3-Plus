script() {

// don't use this particular version of
// the script if you want to use solos
if (GuitarEvent_HitNotes_CFunc())
{
	UpdateGuitarVolume();
}
if (GotParam(open))
{
	Open_NoteFX(player=%player,player_status=%player_status);
}

}
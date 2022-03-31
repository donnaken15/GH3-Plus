script({
	int player = 1;
}) {

Wait(*button_sink_time,Seconds);
GetSongTimeMs();

open_color1 = [240,199,255];
open_color2 = [212,  0,255];

if (*%player_status.star_power_used == 1)
{
	open_color1 = [199,252,255];
	open_color2 = [  0,247,255];
}

fxprefix = 'open_particle';
FormatText(checksumName=player_status, 'player%d_status', d = %player);
// changed id keys to ones with existing names just because
FormatText(checksumName=container_id, 'gem_container%p', p = (*%player_status.text));
/*fxidparams = {
	checksumName: fx_id,
	$00000000: '%f%np%p_%t',
	n: 1,
	f: %fxprefix,
	p: %player,
	t: %time
};
FormatText(%fxidparams);*/
FormatText(checksumName=fx_id, '%f1p%p_%t', f = %fxprefix, p = %player, t = %time);
FormatText(checksumName=fx2_id, '%f2p%p_%t', f = %fxprefix, p = %player, t = %time);
createscreenelement({ // fret flash
	type: SpriteElement,
	parent: %container_id,
	id: %fx_id,
	scale: (1.0, 1.0),
	rgba: %open_color1,
	just: [center, center],
	z_priority: 30,
	pos: (640.0, 619.0),
	alpha: 1,
	material: sys_openfx1_sys_openfx1
});
createscreenelement({ // open shape
	type: SpriteElement,
	parent: %container_id,
	id: %fx2_id,
	scale: (2.2, 2.4),
	rgba: %open_color2,
	just: [center, center],
	z_priority: 30,
	pos: (640.0, 630.0),
	alpha: 1,
	material: sys_openfx2_sys_openfx2
});

time = (0.085 * (*current_speedfactor));
//multiply and divide operators are swapped in the compiler
//thanks adituv

id = %fx_id; // wtf
DoScreenElementMorph(
	id=%id,
	time=%time,
	alpha=0,
	scale=(1.0, 1.7)
);
id = %fx2_id;
DoScreenElementMorph(
	id=%id,
	time=%time,
	alpha=0,
	scale=1.4,
	relative_scale
);

Wait(%time,Seconds);

id = %fx_id;
if (screenelementexists({id: %fx_id})) {
    destroyscreenelement({id: %fx_id});
}
id = %fx2_id;
if (screenelementexists({id: %fx2_id})) {
    destroyscreenelement({id: %fx2_id});
}

}
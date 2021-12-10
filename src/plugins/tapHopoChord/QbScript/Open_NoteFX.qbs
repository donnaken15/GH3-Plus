script({
	int player = 1;
}) {

FormatText(checksumName=player_status, 'player%d_status', d = %player);

Wait(*button_sink_time,Seconds);
GetSongTimeMs();

open_color1 = [240,199,255,255];
open_color2 = [212,  0,255,255];

if (*%player_status.star_power_used == 1)
{
	open_color1 = [199,252,255,255];
	open_color2 = [0, 247, 255];
}

FormatText(checksumName=container_id, 'gem_container%p', p = (*%player_status.text));
FormatText(checksumName=id, 'open_particle1_%t', t = %time);
id_1 = %id;
createscreenelement({ // fret overlay
	type: SpriteElement,
	parent: %container_id,
	id: %id,
	scale: 1,
	rgba: %open_color1,
	just: [center, center],
	z_priority: 20,
	pos: (640.0, 623.0),
	alpha: 1,
	material: sys_openfx1_sys_openfx1
});
FormatText(checksumName=id, 'open_particle2_%t', t = %time);
id_2 = %id;
createscreenelement({ // open shape
	type: SpriteElement,
	parent: %container_id,
	id: %id,
	scale: 2.1,
	rgba: %open_color2,
	just: [center, center],
	z_priority: 20,
	pos: (640.0, 627.0),
	alpha: 1,
	material: sys_openfx2_sys_openfx2
});

time = (0.12 * (*current_speedfactor));
//multiply and divide operators are swapped in the compiler
//thanks adituv

id = %id_1;
DoScreenElementMorph(
	id=%id,
	time=%time,
	alpha=0
);
id = %id_2;
DoScreenElementMorph(
	id=%id,
	time=%time,
	alpha=0,
	scale=1.5,
	relative_scale
);

Wait(%time,Seconds);

id = %id_1;
if (screenelementexists({id: %id})) {
    destroyscreenelement({id: %id});
}
id = %id_2;
if (screenelementexists({id: %id})) {
    destroyscreenelement({id: %id});
}

}
script({
	int player = 1;
}) {

Wait(*button_sink_time,Seconds);
GetSongTimeMs();

%open_color1 = [240,199,255,255];
%open_color2 = [212,  0,255,255];
//%open_color1 = [199,252,255,255];
//%open_color2 = [0, 247, 255];
// TODO: get starpower color working ^

FormatText(checksum=%container_id, 'gem_container%p', p = *(player1_status.text));
FormatText(checksum=%id, 'open_particle1_%t', t = %time);
%id_1 = %id;
createscreenelement({
	type: SpriteElement,
	parent: gem_containerp1,
	id: %id,
	font: fontgrid_title_gh3,
	scale: 1,
	rgba: %open_color1,
	just: [center, center],
	z_priority: 20,
    pos: (640.0, 623.0),
	alpha: 1,
	material: sys_openfx1_sys_openfx1
});
FormatText(checksum=%id, 'open_particle2_%t', t = %time);
%id_2 = %id;
createscreenelement({
	type: SpriteElement,
	parent: gem_containerp1,
	id: %id,
	font: fontgrid_title_gh3,
	scale: 2,
	rgba: %open_color2,
	just: [center, center],
	z_priority: 20,
    pos: (640.0, 627.0),
	alpha: 1,
	material: sys_openfx2_sys_openfx2
});

%time = (0.175 / (*current_speedfactor));
//useless syncing with slomo
//change a byte here to change multiply to division
//33 33 33 3E 0C 0E 4B 16 C1 1B D9 16 0F
//thanks adituv

%id = %id_1;
DoScreenElementMorph(
	id=%id,
	time=%time,
	alpha=0,
	scale=2,
	relative_scale
);
%id = %id_2; // why is this not animating or hiding right
DoScreenElementMorph(
	id=%id,
	time=%time,
	alpha=0,
	scale=2,
	relative_scale
);

Wait(%time,Seconds);

%id = %id_1;
if (screenelementexists({id: %id})) {
    destroyscreenelement({id: %id});
}
%id = %id_2;
if (screenelementexists({id: %id})) {
    destroyscreenelement({id: %id});
}

}
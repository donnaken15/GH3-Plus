script({
	int player = 1;
	qbkey player_status = player1_status;
}) {

if (*toggle_particles > 1)
{
	return;
}

Wait(*button_sink_time,Seconds);
GetSongTimeMs();

//FormatText(checksumName=player_status, 'player%d_status', d = %player);

open_color1 = [240,199,255,255];
open_color2 = [212,  0,255,255];
// WOR COLOR(-ish)
//open_color1 = [255,178, 89,255];
//open_color2 = [207,107,  0,255];

if (*%player_status.star_power_used == 1)
{
	open_color1 = [199,252,255,255];
	open_color2 = [  0,247,255,255];
}

fxprefix = 'open_particle'; // hardcore optimization
fxformat = '%f%dp%p_%t';
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
FormatText(checksumName=fx_id , %fxformat, f = %fxprefix, d = 1, p = %player, t = %time);
FormatText(checksumName=fx2_id, %fxformat, f = %fxprefix, d = 2, p = %player, t = %time);
//highway_width = (1.0 / (*nowbar_scale_x * 0.8));
//amount = (*%player_status.star_power_amount * 100.0);
//pos = (%amount / (-5.0,-200.0));
//fx1_scale = ( (%highway_width / (1.0,0.0)) + (0.0,1.0) );
//now_scale = ((*nowbar_scale_x / (1.0,0.0)) + (*nowbar_scale_y / (0.0,1.0)));
//fx1_scale = (((0-%now_scale/(1.0,0.0))/(1.0,0.0))+(%now_scale/(0.0,1.0)/(0.0,1.0)));

//stupid
fx1_scale = (1.0, 1.0);
fx2_scale = (2.2, 2.4);
if (*current_num_players == 2)
{
	fx1_scale = (0.76, 0.9);
	fx2_scale = (1.7, 2.4);
}

createscreenelement({ // fret flash
	type: SpriteElement,
	parent: %container_id,
	id: %fx_id,
	scale: %fx1_scale,
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
	scale: %fx2_scale,
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
	scale=(1.0, 1.7),
	relative_scale
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
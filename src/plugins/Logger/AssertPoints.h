#pragma once

typedef struct {
	void* rangeStart;
	void* rangeEnd; // on ret
	char* funcName;
} _ASSERT_INFO_FUNC;
#define AIF(S,E,N) { (void*)S,(void*)E,N }
_ASSERT_INFO_FUNC assert_functions[] = {
	AIF(0x00404D60,0x00404DEA,"sub_404D60"),
	AIF(0x004075B0,0x00407748,"SendStructure"),
	AIF(0x004100B0,0x00410110,"InitCutsceneInfo"),
	AIF(0x00410800,0x004108B6,"CProgressionSave::method_410800"),
	AIF(0x00411290,0x004112CC,"SetLeakCheck"),
	AIF(0x00412860,0x004128B5,"ToggleLOD"),
	AIF(0x00412FC0,0x00413394,"CreateParticleSystem"),
	AIF(0x00419A50,0x00419ADE,"WinPortSioGetControlPress"),
	AIF(0x00419AE0,0x00419B1B,"WinPortSioIsDirectInputGamepad"),
	AIF(0x00419B30,0x00419B6B,"WinPortSioIsKeyboard"),
	AIF(0x00419B80,0x00419BBD,"WinPortSioSetDevice0"),
	AIF(0x00419BE0,0x00419C8F,"WinPortSioGetControlName"),
	AIF(0x00419C90,0x00419D17,"WinPortSioGetControlBinding"),
	AIF(0x00419D20,0x00419DC4,"WinPortSioSetControlBinding"),
	AIF(0x00419DD0,0x00419E47,"WinPortGetConfigNumber"),
	AIF(0x00419E50,0x00419EB1,"WinPortSetConfigNumber"),
	AIF(0x00419EC0,0x00419EFC,"WinPortSongHighwaySync"),
	AIF(0x00419F60,0x00419FCC,"WinPortSetSongSkew"),
	AIF(0x0041B930,0x0041BD30,"SetGemConstants"),
	AIF(0x0041CF70,0x0041D46F,"AddNoteToInputArray"),
	AIF(0x0041DE60,0x0041E027,"SetNewWhammyValue"),
	AIF(0x0041E140,0x0041E740,"ButtonCheckerInit"),
	AIF(0x0041E9B0,0x0041ED10,"CheckNoteHoldInit"),
	AIF(0x0041ED10,0x0041EF5B,"CalculateHoldPoints"),
	AIF(0x0041EF60,0x0041F7CD,"UpdateGuitarVolume"),
	AIF(0x0041F820,0x0041F959,"StarPowerFastTest"),
	AIF(0x0041FBB0,0x0041FE6E,"fretbar_update_hammer_on_tolerance_CFun"),
	AIF(0x0041FE70,0x004200E6,"fretbar_update_tempo_CFunc"),
	AIF(0x004201F0,0x00420378,"compute_desired_drummer_twist"),
	AIF(0x00420390,0x004208CD,"gem_array_stepper_CFunc"),
	AIF(0x00420A10,0x00420A97,"sub_420A10"),
	AIF(0x00420BD0,0x00420D0C,"sub_420BD0"),
	AIF(0x00420D50,0x00420F56,"stanceAnimRelatedSub"),
	AIF(0x00421390,0x004218EF,"sub_421390"),
	AIF(0x004218F0,0x00421B54,"CheckButtonsBoss"),
	AIF(0x004233F0,0x00423E77,"CalcSongScoreInfo"),
	AIF(0x00423F10,0x00424347,"submitSongScore?"),
	AIF(0x00424860,0x004248F1,"whammyAndSpMaterialSub?"),
	AIF(0x00424900,0x00424AD6,"sub_424900"),
	AIF(0x00424D20,0x004252DF,"NoteFrame2"),
	AIF(0x00425490,0x00425816,"NoteParticles?"),
	AIF(0x00425830,0x00425E67,"GuitarInputLogicInit"),
	AIF(0x004270A0,0x0042729B,"CreateGemMover"),
	AIF(0x00429380,0x00429D44,"ButtonCheckerPerFrame"),
	AIF(0x00429D50,0x0042A098,"sub_429D50"),
	AIF(0x0042A0A0,0x0042A611,"RockMeter?"),
	AIF(0x0042A660,0x0042AD8B,"sub_42A660"),
	AIF(0x0042ADD0,0x0042B10D,"NoteFX"),
	AIF(0x0042B3E0,0x0042B567,"WhammyFXOn"),
	AIF(0x0042BA70,0x0042C2EF,"CheckNoteHoldPerFrame"),
	AIF(0x0042C670,0x0042C8C9,"GuitarEvent_HitNotes_CFunc"),
	AIF(0x0042CA00,0x0042D0AB,"fretbar_iterator_CFunc"),
	AIF(0x0042D0D0,0x0042D5E1,"FretFingers_iterator_CFunc"),
	AIF(0x0042DD60,0x0042E81B,"check_for_star_power_CFunc"),
	AIF(0x0042EEF0,0x0042F0F0,"StarPowerOn"),
	AIF(0x0042F0F0,0x0042F3F8,"MonitorControllerStates"),
	AIF(0x00430380,0x0043068A,"GiveSPIfPhraseIsHit"),
	AIF(0x00430690,0x00430803,"BreakSPComboOnMissedNote"),
	AIF(0x004308F0,0x00430F5E,"?hit_note@GuitarFuncs@@YAXHH@Z"),
	AIF(0x00430F60,0x00431354,"?miss_note@GuitarFuncs@@YAXHH@Z"),
	AIF(0x00432680,0x0043355B,"SetGemAppearance"),
	AIF(0x004336A0,0x00433BBF,"Create2DFretbar"),
	AIF(0x004353E0,0x004356A6,"AddPulseEvent"),
	AIF(0x00435900,0x00435E52,"GemStep"),
	AIF(0x00435E60,0x00436143,"script_callback_script_cfunc"),
	AIF(0x00437500,0x0043752C,"QbKvPairList::GetDouble"),
	AIF(0x00438010,0x0043813D,"LightShow_RefreshSpotlightOverrideParam"),
	AIF(0x00438140,0x004381D0,"LightShow_EnableSpotlights"),
	AIF(0x004381D0,0x004382C0,"LightShow_OverrideColor"),
	AIF(0x004383C0,0x004384A7,"LightShow_BeginProcessBlock"),
	AIF(0x00438CC0,0x00438D48,"LightShow_WaitForNextEvent"),
	AIF(0x00438F30,0x00439F04,"LightShow_Init"),
	AIF(0x00439FA0,0x0043A058,"sub_439FA0"),
	AIF(0x0043B4A0,0x0043B51F,"MC_SetActiveFolder"),
	AIF(0x004676E0,0x0046791B,"sub_4676E0"),
	AIF(0x00472160,0x0047227C,"CreatePlatformMessageBox"),
	AIF(0x004788B0,0x0047894F,"QbStruct::GetString"),
	AIF(0x00478950,0x004789EF,"QbStruct::GetWString"),
	AIF(0x004789F0,0x00478A34,"QbStruct::GetInt"),
	AIF(0x00478A40,0x00478A88,"QbStruct::GetFloat"),
	AIF(0x00478A90,0x00478AF8,"QbKvPairList::GetFloatsX3"),
	AIF(0x00478B00,0x00478B50,"QbStruct::GetPair"),
	AIF(0x00478B50,0x00478B94,"QbStruct::GetQbStruct"),
	AIF(0x00478BA0,0x00478BE4,"QbStruct::GetQbArray"),
	AIF(0x00478C40,0x00478D21,"?GetQuaternion@CStruct@Script@@QBE_NVCChecksum@Crc@@PAVQuat@Mth@@W4EAssertType@2@@Z"),
	AIF(0x00478E50,0x004790FD,"QbStruct::GetValue"),
	AIF(0x00479160,0x004791E8,"sub_479160"),
	AIF(0x00479330,0x00479451,"QbKvPairList::GetValue2"),
	AIF(0x00484310,0x00484B0A,"?MainLoop@Manager@Mlp@@QAEXXZ"),
	AIF(0x004A90B0,0x004A92A3,"sub_4A90B0"),
	AIF(0x004AA710,0x004AA8C1,"sub_4AA710"),
	AIF(0x004AB970,0x004ABA54,"sub_4AB970"),
	AIF(0x004ACF80,0x004ACFED,"sub_4ACF80"),
	AIF(0x004ACFF0,0x004AD0CF,"sub_4ACFF0"),
	AIF(0x004B4C10,0x004B4D24,"sub_4B4C10"),
	AIF(0x004BC770,0x004BCA78,"sub_4BC770"),
	AIF(0x004BE900,0x004BEB23,"sub_4BE900"),
	AIF(0x004C73B0,0x004C741F,"sub_4C73B0"),
	AIF(0x004C7420,0x004C7497,"sub_4C7420"),
	AIF(0x004CFB70,0x004CFCF0,"sub_4CFB70"),
	AIF(0x004FBEC0,0x004FC482,"sub_4FBEC0"),
	AIF(0x00516740,0x005168C6,"sub_516740"),
	AIF(0x00532CA0,0x00532DC9,"Dump"),
	AIF(0x00532DD0,0x00532DEB,"ScriptAssert"),
	AIF(0x005330F0,0x005331EE,"UpdateStructElement"),
	AIF(0x00537B70,0x00537E19,"DisplayLoadingScreen"),
	AIF(0x0053BE50,0x0053BEFD,"TimeMarkerReached_SetParams"),
	AIF(0x0053BF20,0x0053BFFC,"TimeMarkerReached"),
	AIF(0x0053C340,0x0053C3E5,"MathMin"),
	AIF(0x0053CDD0,0x0053CE5B,"MathFloor"),
	AIF(0x0053CE60,0x0053CEC9,"MathCeil"),
	AIF(0x0053CED0,0x0053CF6C,"MathPow"),
	AIF(0x0053D790,0x0053D838,"GetProfileData"),
	AIF(0x0053DB90,0x0053DBD5,"Anim_GetAnimLength"),
	AIF(0x0053FA50,0x0053FBC7,"AdjustTextureMovieSprite"),
	AIF(0x0053FBD0,0x0053FC73,"AllocateMovieBuffer"),
	AIF(0x0053FC90,0x0053FCC8,"FreeMovieBuffer"),
	AIF(0x0053FCE0,0x0053FD18,"HasMovieBuffer"),
	AIF(0x0053FD30,0x0053FD7F,"LoadMovieIntoBuffer"),
	AIF(0x0053FD80,0x0053FDF1,"PlayMovieFromBuffer"),
	AIF(0x00543BF0,0x00543D2A,"SeekMovie"),
	AIF(0x005441C0,0x005442EB,"WinPortGfxGetDefaultOptionValue"),
	AIF(0x00544310,0x0054443B,"WinPortGfxGetOptionValue"),
	AIF(0x00544460,0x005445C4,"WinPortGfxSetOptionValue"),
	AIF(0x00544C80,0x00544DBB,"WinPortGfxGetOptionChoices"),
	AIF(0x00546930,0x005469A9,"sub_546930"),
	AIF(0x00548E50,0x00548F88,"LoadFSB"),
	AIF(0x00550D10,0x00550D8A,"CreateVie"),
	AIF(0x0055A380,0x0055A550,"sub_55A380"),
	AIF(0x0055BC70,0x0055BD16,"sub_55BC70"),
	AIF(0x0055C6A0,0x0055CB11,"sub_55C6A0"),
	AIF(0x0055CF80,0x0055D266,"sub_55CF80"),
	AIF(0x0055DAE0,0x0055DD0C,"sub_55DAE0"),
	AIF(0x00570DD0,0x005712E1,"sub_570DD0"),
	AIF(0x0057E3D0,0x0057E600,"sub_57E3D0"),
	AIF(0x0057E6B0,0x0057E752,"sub_57E6B0"),
	AIF(0x0057E900,0x0057EA9A,"sub_57E900"),
	AIF(0x0057EAA0,0x0057EC35,"sub_57EAA0"),
	AIF(0x0057EC40,0x0057ED52,"sub_57EC40"),
	AIF(0x0057ED60,0x0057EE86,"sub_57ED60"),
	AIF(0x0057EF00,0x0057F1CB,"sub_57EF00"),
	AIF(0x005D68F0,0x005D6B0D,"sub_5D68F0"),
	AIF(0x005DA0F0,0x005DA550,"sub_5DA0F0"),
	AIF(0x005E64A0,0x005E6570,"sub_5E64A0"),
	AIF(0x005E6730,0x005E67DD,"sub_5E6730"),
	AIF(0x005E8710,0x005E877C,"sub_5E8710"),
	AIF(0x005F5790,0x005F58DF,"sub_5F5790"),
	AIF(0x0060B890,0x0060B93E,"sub_60B890"),
	AIF(0x0060B9C0,0x0060BA24,"locksAndGoalsSub"),
	AIF(0x00D7B40B,0x00D7B437,"sub_D7B40B"),
	AIF(0x00D7B458,0x00D7B484,"sub_D7B458"),
	AIF(0x00D7BF0F,0x00D7BF3B,"sub_D7BF0F"),
	AIF(0x00D7E534,0x00D7E5E4,"sub_D7E534"),
	AIF(0x00D7E728,0x00D7E843,"sub_D7E728"),
	AIF(0x00D7EACD,0x00D7EB64,"sub_D7EACD"),
	AIF(0x00D7EB8C,0x00D7EC46,"BossWaitForAttack"),
	AIF(0x00D7EC46,0x00D7EE86,"sub_D7EC46"),
	AIF(0x00D7EE86,0x00D7F030,"sub_D7EE86"),
	AIF(0x00D7F10C,0x00D7F235,"sub_D7F10C"),
	AIF(0x00D7F235,0x00D7F261,"sub_D7F235"),
	AIF(0x00D7F54D,0x00D7F677,"sub_D7F54D"),
	AIF(0x00D7F824,0x00D7F8CD,"sub_D7F824"),
	AIF(0x00D8B299,0x00D8B2E1,"?GetScript@CStruct@Script@@QBE_NVCChecksum@Crc@@PAUSStructScript@2@W4EAssertType@2@@Z"),
	AIF(0x00D8B2E1,0x00D8B361,"sub_D8B2E1"),
	AIF(0x00D8B36B,0x00D8B3EB,"sub_D8B36B"),
	AIF(0x00D95F32,0x00D95FB2,"sub_D95F32"),
	AIF(0x00D96B78,0x00D96D3A,"sub_D96B78"),
	AIF(0x00D98A95,0x00D98B2C,"sub_D98A95"),
	AIF(0x00D9BDE1,0x00D9BEE4,"sub_D9BDE1"),
	AIF(0x00D9E7EE,0x00D9E949,"sub_D9E7EE"),
	AIF(0x00D9E949,0x00D9EA14,"sub_D9E949"),
	AIF(0x00D9F477,0x00D9F6B8,"sub_D9F477"),
	AIF(0x00DA5A21,0x00DA5A8E,"sub_DA5A21"),
	AIF(0x00DA5A98,0x00DA5C61,"sub_DA5A98"),
	AIF(0x00DAC4C5,0x00DAC500,"sub_DAC4C5"),
};

/* exported list with

static main()
{
	auto EA = FirstSeg();
	auto assert = 0x4F6500;
	auto xr = RfirstB(assert);
	auto path = form("%s_asserts.h", GetIdbPath());
	auto f = fopen(path, "w");
	auto lfstart = -1;
	do
	{
		if (GetFunctionFlags(xr) != -1)
		{
			auto funcstart = GetFunctionAttr(xr, FUNCATTR_START);
			if (lfstart != funcstart)
			{
				auto funcend = GetFunctionAttr(xr, FUNCATTR_END);
				auto funcstr = GetFunctionName(xr);
				fprintf(f,"\tAIF(0x%08X,0x%08X,\"%s\"),\n",funcstart,funcend,funcstr);
				MakeCode(xr + 5); // BECAUSE FOR SOME REASON THE ADD ESP GETS UNDEFINED
				lfstart = funcstart;
			}
		}
		else
		{
			//Message("Function not marked where there is an assert: %08X\n", xr);
		}
		xr = RnextB(assert, xr);
	}
	while (xr != -1);
}

*/


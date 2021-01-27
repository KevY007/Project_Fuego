#include "main.h"

void Init_ATBMenu()
{
	if (set.atbmenu_init == 1) return;
	set.atbmenu_init = 1;

	/* Bars */
	TwBar *mainMenu = TwNewBar("MainMenu");
	TwBar *weaponFuncs = TwNewBar("wepFuncs");
	TwBar *vehFuncs = TwNewBar("vehFuncs");
	TwBar *miscFuncs = TwNewBar("miscFuncs");

	TwDefine(" 'MainMenu' size='376 272' position='778 130' iconifiable=false iconified=false");
	TwDefine(" 'wepFuncs' size='375 440' position='780 407'  iconifiable=false iconified=false");
	TwDefine(" 'vehFuncs' size='410 441' position='365 406' iconifiable=false iconified=false");
	TwDefine(" 'miscFuncs' size='422 439' position='1160 408' iconifiable=false iconified=false");


	TwDefine(" 'MainMenu' label='[>] Project Fuego [<]' color='183 33 33' movable=true resizable=true alpha=235 visible=true text=light help='What do you expect?'");
	TwDefine(" 'wepFuncs' label='[>] Weapon Functions [<]' color='255 165 0' movable=true resizable=true alpha=204 visible=true text=light help='All hacks which involve weapon go under here.'");
	TwDefine(" 'vehFuncs' label='[>] Vehicle Functions [<]' color='3 79 211' movable=true resizable=true alpha=204 visible=true text=light help='All hacks which involve vehicles go under here.'");
	TwDefine(" 'miscFuncs' label='[>] Miscellaneous Functions [<]' color='3 186 82' movable=true resizable=true alpha=204 visible=true text=light help='Miscellaneous hacks such as slappers etc are here.'");
	
	TwDefine(" 'MainMenu' movable=true moveable=true resizable=true");
	TwDefine(" 'wepFuncs' movable=true moveable=true resizable=true");
	TwDefine(" 'vehFuncs' movable=true moveable=true resizable=true");
	TwDefine(" 'miscMenu' movable=true moveable=true resizable=true");

	TwMinimizeBar(weaponFuncs);
	TwMinimizeBar(miscFuncs);
	TwMinimizeBar(vehFuncs);

	TwAddButton(mainMenu, "toggleWepFuncs", ToggleWeaponBar, NULL, "label='Toggle Weapon Functions'");
	TwAddButton(mainMenu, "toggleVehFuncs", ToggleVehicleBar, NULL, "label='Toggle Vehicle Functions'");
	TwAddButton(mainMenu, "toggleMiscFuncs", ToggleMiscBar, NULL, "label='Toggle Miscellaneous Functions'");

	TwAddButton(mainMenu, "Credits", NULL, NULL, " label='     Made with <3' ");

	TwAddButton(mainMenu, "space1", NULL, NULL, " label=' ' ");
	TwAddButton(mainMenu, "space2", NULL, NULL, " label=' ' ");
	TwAddButton(mainMenu, "space3", NULL, NULL, " label=' ' ");
	TwAddButton(mainMenu, "space4", NULL, NULL, " label=' ' ");
	TwAddButton(mainMenu, "space5", NULL, NULL, " label=' ' ");

	TwAddVarRW(mainMenu, "Disable Unsafe/Big Hacks", TW_TYPE_BOOLCPP, &set.noBigHacks, "true='On' false='Off' fontsize=1");

	/* Functions/Variables */
	TwAddVarRW(weaponFuncs, "Shot Repeater", TW_TYPE_UINT8, &set.shotrepeater, "min=0 max=100 step=1");
	TwAddVarRW(weaponFuncs, "Fake Headshot", TW_TYPE_BOOLCPP, &set.fakeheadshot, "true='On' false='Off'");
	TwAddVarRW(weaponFuncs, "Trigger Bot", TW_TYPE_BOOLCPP, &set.triggerbot, "true='On' false='Off'");
	TwAddVarRW(weaponFuncs, "No Spread", TW_TYPE_BOOLCPP, &set.nospread, "true='On' false='Off'");
	TwAddVarRW(weaponFuncs, "No Spread Trigger", TW_TYPE_UINT32, &set.nospread_trigger, "min=0 max=10 step=1");
	TwAddVarRW(weaponFuncs, "No Reload", TW_TYPE_BOOLCPP, &set.noreload, "true='On' false='Off'");
	TwAddVarRW(weaponFuncs, "Dual Shot", TW_TYPE_BOOLCPP, &set.dShotStatus, "true='On' false='Off'");
	TwAddVarRW(weaponFuncs, "Dual Shot Delay", TW_TYPE_UINT32, &set.dShotDelay, "min=0 max=3000 step=5");

	TwAddSeparator(weaponFuncs, "slAim", NULL);
	TwAddVarRW(weaponFuncs, "Silent Aimbot", TW_TYPE_BOOLCPP, &set.silentaim, "true='On' false='Off' group='Silent Aim' ");
	TwAddVarRW(weaponFuncs, "Random False Shots", TW_TYPE_BOOLCPP, &set.silentaim_antiAimBypass, "true='On' false='Off' group='Silent Aim' ");
	TwAddVarRW(weaponFuncs, "Range Hack", TW_TYPE_BOOLCPP, &set.silentaim_outOfRangeFilter, "true='On' false='Off' group='Silent Aim' ");
	TwAddVarRW(weaponFuncs, "Shoot Through Walls", TW_TYPE_BOOLCPP, &set.silentaim_shotWall, "true='On' false='Off' group='Silent Aim' ");

	
	TwEnumVal TracerMeshes[] = { {set.TRACERTYPE::Off, "Off"}, {set.TRACERTYPE::ESP, "ESP Only"}, {set.TRACERTYPE::ESPFOV, "ESP & FOV"} };

	TwType TracerMeshTwType = TwDefineEnum("MeshType", TracerMeshes, 3);

	TwAddVarRW(weaponFuncs, "Draw Tracer", TracerMeshTwType, &set.silentaim_drawTracer, "group='Silent Aim' ");
	TwAddVarRW(weaponFuncs, "Search FOV", TW_TYPE_INT32, &set.silentaim_fov, "min=1 max=1024 step=2 group='Silent Aim' ");
	TwAddVarRW(weaponFuncs, "Friend Filter", TW_TYPE_BOOLCPP, &set.silentaim_friendFilter, "true='On' false='Off' group='Silent Aim' ");
	TwAddVarRW(weaponFuncs, "Color Filter", TW_TYPE_BOOLCPP, &set.silentaim_colorFilter, "true='On' false='Off' group='Silent Aim' ");
	TwAddVarRW(weaponFuncs, "Skin Filter", TW_TYPE_BOOLCPP, &set.silentaim_skinFilter, "true='On' false='Off' group='Silent Aim' ");
	TwAddVarRW(weaponFuncs, "Team Filter", TW_TYPE_BOOLCPP, &set.silentaim_teamFilter, "true='On' false='Off' group='Silent Aim' ");
	TwAddVarRW(weaponFuncs, "AFK Filter", TW_TYPE_BOOLCPP, &set.silentaim_afkFilter, "true='On' false='Off' group='Silent Aim' ");
	TwAddVarRW(weaponFuncs, "Alive Filter", TW_TYPE_BOOLCPP, &set.silentaim_aliveFilter, "true='On' false='Off' group='Silent Aim' ");

	TwAddSeparator(weaponFuncs, "KillerSep", NULL);
	TwAddVarRW(weaponFuncs, "Auto Killer", TW_TYPE_BOOLCPP, &set.killer_enabled, "true='On' false='Off' group='Killer' ");
	TwAddVarRW(weaponFuncs, "Killer Delay", TW_TYPE_INT32, &set.killer_delay, "min=1 max=10000 step=1 group='Killer' ");
	TwAddVarRW(weaponFuncs, "Bullets Per Person", TW_TYPE_INT32, &set.killer_bullets, "min=1 max=92 step=1 group='Killer' ");
	TwAddVarRW(weaponFuncs, "Statistics Interval", TW_TYPE_INT32, &set.killer_counter_updatetime, "min=1 max=10 step=1 group='Killer' ");
	TwAddVarRW(weaponFuncs, "Killer Friend Filter", TW_TYPE_BOOLCPP, &set.killer_friendFilter, "true='On' false='Off' group='Killer' ");


	TwAddVarRW(miscFuncs, "invis", TW_TYPE_BOOLCPP, &set.inviszdown, "true='On' false='Off' label='Invisibility' group='Invisibility'");
	TwAddVarRW(miscFuncs, "Smart Invisibility (not working)", TW_TYPE_BOOLCPP, &set.smartinvis, "true='On' false='Off' group='Invisibility'");
	TwAddSeparator(miscFuncs, "InvisSep", NULL);

	TwAddVarRW(miscFuncs, "Aim Slapper", TW_TYPE_BOOLCPP, &set.aimslapper, "true='On' false='Off' group='Slappers'");
	TwAddVarRW(miscFuncs, "Aim Slapper Power", TW_TYPE_UINT8, &set.aimslapperspeed, "min=0 max=100 step=2 group='Slappers'");
	TwAddVarRW(miscFuncs, "Near Slapper", TW_TYPE_BOOLCPP, &set.nearslapper, "true='On' false='Off' group='Slappers'");
	TwAddVarRW(miscFuncs, "Near Slapper Friend Filter", TW_TYPE_BOOLCPP, &set.nearslapper_friendFilter, "true='On' false='Off' group='Slappers'");
	TwAddSeparator(miscFuncs, "SlapperSep", NULL);

	
	TwAddVarRW(miscFuncs, "Chams", TW_TYPE_BOOLCPP, &set.chams_on, "true='On' false='Off' group='Visuals'");
	TwAddVarRW(miscFuncs, "Nametag", TW_TYPE_BOOLCPP, &cheat_state->render_player_tags, "true='On' false='Off' group='Visuals'");
	TwAddVarRW(miscFuncs, "Nametag Type", TW_TYPE_BOOLCPP, &set.default_nametags, "true='SA-MP' false='S0biet' group='Visuals'");
	TwAddVarRW(miscFuncs, "Locked Vehicle ESP", TW_TYPE_BOOLCPP, &set.lockedcaresp, "true='On' false='Off' group='Visuals'");
	TwAddVarRW(miscFuncs, "Weapon Finder", TW_TYPE_BOOLCPP, &set.weaponFinder, "true='On' false='Off' group='Visuals'");

	TwAddSeparator(miscFuncs, "VisualSep", NULL);

	TwAddVarRW(miscFuncs, "Onfoot Break Dance", TW_TYPE_BOOLCPP, &set.actBrkDance, "true='On' false='Off'");
	TwAddVarRW(miscFuncs, "Auto Unscrambler LSRP", TW_TYPE_BOOLCPP, &set.autounscramble, "true='On' false='Off'");

	TwAddSeparator(miscFuncs, "HHSep", NULL);
	TwAddVarRW(miscFuncs, "Smart Health Hack (addon over normal hh)", TW_TYPE_BOOLCPP, &set.smarthealthhack, "true='On' false='Off'");
	TwAddVarRW(miscFuncs, "Smart HH Proc Chance", TW_TYPE_UINT8, &set.smarthhproc, "min=0 max=100 step=5");

	TwAddSeparator(miscFuncs, "FakeLagSep", NULL);
	TwAddVarRW(miscFuncs, "Fake Movement Lag", TW_TYPE_BOOLCPP, &set.fakesynclag, "true='On' false='Off' group='Fake Lag'");
	TwAddVarRW(miscFuncs, "Fake Random Spikes", TW_TYPE_BOOLCPP, &set.fakesynclag_randomspikes, "true='On' false='Off' group='Fake Lag'");
	TwAddVarRW(miscFuncs, "Lag Delay", TW_TYPE_UINT32, &set.fakesynclag_delay, "min=0 max=10000 step=250 group='Fake Lag'");
	TwAddVarRW(miscFuncs, "Teleportish Movement", TW_TYPE_BOOLCPP, &set.fakesynclag_onceperms, "group='Fake Lag'");

	TwAddSeparator(miscFuncs, "OtherMiscSep", NULL);
	TwAddVarRW(miscFuncs, "Moonwalker", TW_TYPE_BOOLCPP, &set.moonwalk, "true='On' false='Off' group='Skin Trolls'");
	TwAddVarRW(miscFuncs, "Upside Down", TW_TYPE_BOOLCPP, &set.upside_down, "true='On' false='Off' group='Skin Trolls'");
	TwAddVarRW(miscFuncs, "Ghost Mode", TW_TYPE_BOOLCPP, &set.ghostmode, "true='On' false='Off' group='Skin Trolls'");
	TwAddVarRW(miscFuncs, "Twister Mode", TW_TYPE_BOOLCPP, &set.twistmode, "true='On' false='Off' group='Skin Trolls'");

	TwAddButton(vehFuncs, "itInfoMode1", NULL, NULL, " label='1) Stuck on Ground' group='iTroll'");
	TwAddButton(vehFuncs, "itInfoMode2", NULL, NULL, " label='2) Speed Away' group='iTroll'");
	TwAddButton(vehFuncs, "itInfoMode3", NULL, NULL, " label='3) Burn and Throw Down' group='iTroll'");
	TwAddButton(vehFuncs, "itInfoMode4", NULL, NULL, " label='4) Burn and Throw Up' group='iTroll'");
	TwAddButton(vehFuncs, "itInfoMode5", NULL, NULL, " label='5) Spin Vehicle' group='iTroll'");
	TwAddButton(vehFuncs, "itInfoMode6", NULL, NULL, " label='6) Send to Ocean' group='iTroll'");
	TwAddButton(vehFuncs, "itInfoMode7", NULL, NULL, " label='7) Speed Away (Slower)' group='iTroll'");

	TwAddButton(vehFuncs, "itInfoUse", NULL, NULL, " label='Press/Hold I while driving to troll.' group='iTroll' ");
	TwAddVarRW(vehFuncs, "itMode", TW_TYPE_UINT8, &set.iTroll, "label='iTroll Mode' min=0 max=7 step=1 group='iTroll'");
	TwAddVarRO(vehFuncs, "itTarget", TW_TYPE_STDSTRING, &set.iTroll_Targetname, "label='iTroll Target' group='iTroll'");

	TwAddSeparator(vehFuncs, "Generic", NULL);
	TwAddVarRW(vehFuncs, "Car Shooter", TW_TYPE_BOOLCPP, &set.carshooter, "true='On' false='Off'");
	TwAddVarRW(vehFuncs, "Car Remover", TW_TYPE_BOOLCPP, &set.removecars, "true='On' false='Off'");
	TwAddVarRW(vehFuncs, "Car Exploder", TW_TYPE_BOOLCPP, &set.explodecars, "true='On' false='Off'");
	TwAddVarRW(vehFuncs, "Floating Cars", TW_TYPE_BOOLCPP, &set.carsfloating, "true='On' false='Off'");
	TwAddVarRW(vehFuncs, "Car Exploder Silent", TW_TYPE_BOOLCPP, &set.explodecarssilent, "true='On' false='Off'");
	TwAddVarRW(vehFuncs, "Unoccupied Car Remover", TW_TYPE_BOOLCPP, &set.unoccupiedremover, "true='On' false='Off'");

	TwAddSeparator(vehFuncs, "SepBtns", NULL);
	TwAddButton(vehFuncs, "dstrydoors", DestroyDoorsCallback, NULL, "label='Destroy Vehicle Doors'");
	TwAddButton(vehFuncs, "atchtrlr", AttachTrailerCallback, NULL, "label='Attach/Tow Vehicle'");
	TwAddButton(vehFuncs, "vunlckr", UnlockVehCallback, NULL, "label='Unlock Vehicle'");

	TwDefine(" MainMenu/Credits   group=Credits");

	int opened = 0;

	TwSetParam(vehFuncs, "iTroll", "opened", TW_PARAM_INT32, 1, &opened);
	TwSetParam(miscFuncs, "Invisibility", "opened", TW_PARAM_INT32, 1, &opened);
	TwSetParam(miscFuncs, "Slappers", "opened", TW_PARAM_INT32, 1, &opened);
	TwSetParam(miscFuncs, "Fake Lag", "opened", TW_PARAM_INT32, 1, &opened);
	TwSetParam(miscFuncs, "Skin Trolls", "opened", TW_PARAM_INT32, 1, &opened);
	TwSetParam(miscFuncs, "Visuals", "opened", TW_PARAM_INT32, 1, &opened);
	TwSetParam(weaponFuncs, "Silent Aim", "opened", TW_PARAM_INT32, 1, &opened);
	TwSetParam(weaponFuncs, "Killer", "opened", TW_PARAM_INT32, 1, &opened);

#if defined DEBUG_BAR_POS
	TwAddVarRO(mainMenu, "mmDef", TW_TYPE_STDSTRING, &set.mmdef, "label='Main Menu'");
	TwAddVarRO(mainMenu, "wbDef", TW_TYPE_STDSTRING, &set.wbdef, "label='Weapon Menu'");
	TwAddVarRO(mainMenu, "vbDef", TW_TYPE_STDSTRING, &set.vbdef, "label='Vehicle Menu'");
	TwAddVarRO(mainMenu, "mbDef", TW_TYPE_STDSTRING, &set.mbdef, "label='Misc Menu'");
#endif
}

void TW_CALL DestroyDoorsCallback(void *clientData)
{
	if (iIsSAMPSupported)
	{
		set.damagingcardoorenter = 0;
		set.damagingcardoor = 1;
		
		g_iCursorEnabled = 1;
		set.atbmenu_enabled = 0;

		toggleSAMPCursor(g_iCursorEnabled);
	}
}

void TW_CALL AttachTrailerCallback(void* clientData)
{
	if (iIsSAMPSupported)
	{
		set.attachingtrailerenter = 0;
		set.attachingtrailer = 1;

		g_iCursorEnabled = 1;
		set.atbmenu_enabled = 0;

		toggleSAMPCursor(g_iCursorEnabled);
	}
}

void TW_CALL UnlockVehCallback(void* clientData)
{
	if (iIsSAMPSupported)
	{
		set.vunlockerenter = 0;
		set.vunlocker = 1;

		g_iCursorEnabled = 1;
		set.atbmenu_enabled = 0;

		toggleSAMPCursor(g_iCursorEnabled);
	}
}

void TW_CALL ToggleWeaponBar(void *clientData)
{
	ToggleBar(TwGetBarByName("wepFuncs"));
}

void TW_CALL ToggleVehicleBar(void *clientData)
{
	ToggleBar(TwGetBarByName("vehFuncs"));
}

void TW_CALL ToggleMiscBar(void *clientData)
{
	ToggleBar(TwGetBarByName("miscFuncs"));
}

void ToggleBar(TwBar *pbar)
{
	if (TwIsBarMinimized(pbar))
	{
		TwMaximizeBar(pbar);
	}
	else
	{
		TwMinimizeBar(pbar);
	}
}
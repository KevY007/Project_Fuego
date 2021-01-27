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

void sampMainCheat(void);
void sampAntiHijack(void);
void spectateHandle(void);
void spectatePlayer(int iPlayerID);
void HandleRPCPacketFunc(unsigned char id, struct RPCParameters *rpcParams, void(*callback) (struct RPCParameters *));
void initChatCmds(void);
bool OnSendRPC(int uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp);
bool OnSendPacket(BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel);
bool OnReceivePacket(Packet *p);
void cheat_handle_itroll_targetfinder();
void cheat_handle_carsfloating();
void cheat_handle_freezetroll();
void SendFakeDriverFullSyncData(stInCarData *picSync, int iUseCarPos);
void SendFakeDriverSyncData(int iSAMPVehicleID, float fVehiclePos[3], float fVehicleHealth, float fMoveSpeed[3], float fQuaternion[4]);
void cheat_handle_carshooter();
void cheat_handle_explodecars();
void cheat_handle_removecars();
void cheat_handle_nospread();
void cheat_handle_slapkill(BitStream *parameters);
void cheat_handle_slapperesp();
void cmd_itroll(char* params);
void cheat_handle_noreload();
void GetAimingCenter(float& x, float& y);
bool aimKeyDown();
void cheat_handle_dualshot();
void cheat_handle_triggerbot();
void cheat_handle_weaponfinder();
void cheat_handle_autounscrambler();
void cheat_handle_cleanscreenshot();
void cmd_damagedoors(char* params);
void clickDamageDoors();
void clickAttach();
void clickUnlock();
void cheat_handle_killer_weapon();
void cheat_handle_lsrp_gunstealer();
void cheat_handle_unoccupiedremover();
void cheat_handle_explodecarssilent();
void cheat_handle_forcedesync();
void cheat_handle_fakesynclag();
void GTAfunc_Player_ApplyRotoryPulseAboutAnAxis(float fX, float fY, float fZ);
void cheat_handle_playerbreakdance();
void DisableFlyingBirds();
void EnableFlyingBirds();
int GetSilentAimTarget();
void ReloadFriendlist();
int GetAimTarget(bool teamFilter = false, bool colorFilter = false, bool skinFilter = false, bool visibleFilter = false,
	bool afkFilter = false, bool aliveFilter = false, bool friendFilter = false, float findRange = 999.9F, int fieldofview = 150);
bool IsVehicleStreamed(uint16_t vehicleid);
bool IsPlayerStreamed(uint16_t playerid);
std::string extractfromstr(const std::string& s, const std::string& start_delim, const std::string& stop_delim);
std::string extractfromstr(const char* scc, const char* start_delim, const char* stop_delim);
struct vehicle_info* getGTAVehicleFromSAMPVehicleIDEx(int iVehicleID);
int getSAMPVehicleIDFromGTAVehicleEx(struct vehicle_info* pVehicle);
void cmd_cc(char* params);

extern int										g_iSpectateEnabled;
extern int										g_iCursorEnabled;
extern int										g_iJoiningServer;
extern int										g_iNumPlayersMuted;
extern bool										g_bPlayerMuted[SAMP_MAX_PLAYERS];
extern bool										g_NameHidden[SAMP_MAX_PLAYERS];
extern std::vector<std::string>					Friends;
extern bool										FriendsListLoaded;
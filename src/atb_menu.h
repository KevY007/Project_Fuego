#pragma once

#include "main.h"

//#define DEBUG_BAR_POS

void Init_ATBMenu();
void ToggleBar(TwBar *pbar);
void TW_CALL ToggleWeaponBar(void *clientData);
void TW_CALL ToggleVehicleBar(void *clientData);
void TW_CALL ToggleMiscBar(void *clientData);
void TW_CALL DestroyDoorsCallback(void* clientData); 
void TW_CALL AttachTrailerCallback(void* clientData);
void TW_CALL UnlockVehCallback(void* clientData);
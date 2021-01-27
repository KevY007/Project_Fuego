// VEHICLE MODEL INFO

enum eVehicleModelInfo
{
	VEHICLE_MODEL_INFO_SIZE,            // Vehicle size
	VEHICLE_MODEL_INFO_FRONTSEAT,       // Position of the front seat*
	VEHICLE_MODEL_INFO_REARSEAT,        // Position of the rear seat*
	VEHICLE_MODEL_INFO_PETROLCAP,       // Position of the fuel cap*
	VEHICLE_MODEL_INFO_WHEELSFRONT,     // Position of the front wheels*
	VEHICLE_MODEL_INFO_WHEELSREAR,      // Position of the rear wheels*
	VEHICLE_MODEL_INFO_WHEELSMID,       // Position of the middle wheels (applies to vehicles with 3 axes)*
	VEHICLE_MODEL_INFO_FRONT_BUMPER_Z,  // Height of the front bumper
	VEHICLE_MODEL_INFO_REAR_BUMPER_Z,   // Height of the rear bumper
};

int getVehicleModelInfo(int modelID, BYTE infoType, float fSize[3]);

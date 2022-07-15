#pragma once
#include "defines.h"

typedef struct sAdvatekDevice {
public:
	sAdvatekDevice();
	~sAdvatekDevice();

	bool to_json(JSON_TYPE& j);
	bool from_json(const JSON_TYPE& j);


	uint8_t ProtVer; // Protocol version
	uint8_t CurrentProtVer; // Using Protocol version
	uint8_t Mac[6]; // MAC Address
	uint8_t ModelLength; // Length of model array
	uint8_t* Model; // Full display name
	uint8_t HwRev; // HW rev E.G. 1.0 = 10
	uint8_t MinAssistantVer[3]; // Minimum Advatek Assistant version required for config
	uint8_t LenFirmware;
	uint8_t* Firmware; // firmware version, null terminated
	uint8_t Brand; // 0 = Advatek
	uint8_t CurrentIP[4]; // IP address
	uint8_t CurrentSM[4]; // Current Subnet Mask (incase DHCP issued)
	uint8_t DHCP; // DHCP on or off
	uint8_t StaticIP[4]; // Static IP address
	uint8_t StaticSM[4]; // Static subnet mask

	uint8_t Protocol; // 0 = sACN, 1 = artNet
	uint8_t HoldLastFrame; // Hold last frame (timeout blank mode)
	uint8_t SimpleConfig; // Simple = 0, or advanced config = 1
	uint16_t MaxPixPerOutput; // Maximum number of pixels any output can drive
	uint8_t NumOutputs; // Number of outputs (including expanded)

	// NumOutputs
	uint16_t* OutputPixels;//[2 * NumOutputs]; // Number of nodes per string
	uint16_t* OutputUniv;//[2 * NumOutputs]; // Advanced start universes
	uint16_t* OutputChan;//[2 * NumOutputs]; // Advanced start channels
	uint8_t* OutputNull;//[NumOutputs]; // Null pixels
	uint16_t* OutputZig;//[2 * NumOutputs]; // Zig zags
	uint8_t* OutputReverse;//[NumOutputs]; // Reversed strings
	uint8_t* OutputColOrder;//[NumOutputs]; // RGB order for each output
	uint16_t* OutputGrouping;//[2 * NumOutputs]; // Pixel grouping
	uint8_t* OutputBrightness;//[NumOutputs]; // Brightness limiting

	uint8_t NumDMXOutputs; // Number of DMX outputs
	uint8_t ProtocolsOnDmxOut;
	uint8_t* DmxOutOn;//[NumDMXOutputs]; // DMX outputs on or off
	bool* TempDmxOutOn;
	uint16_t* DmxOutUniv;//[2 * NumDMXOutputs]; // Hi and Lo bytes of DMX output universes

	uint8_t NumDrivers; // Number of pixel drivers
	uint8_t DriverNameLength; // Length of pixel driver strings
	uint8_t* DriverType;//[NumDrivers]; // 0 = RGB only, 1 = RGBW only, 2 = Either
	uint8_t* DriverSpeed;//[NumDrivers]; // 0 = N/A, 1 = slow only, 2 = fast only, 3 = either, 4 = adjustable clock 0.4 - 2.9MHz(12 step)
	uint8_t* DriverExpandable;// [NumDrivers]; // 0 = Normal mode only, 1 = capable of expanded mode
	char** DriverNames;// [NumDrivers][LENGTH_DRIVER_STRINGS]; // Null terminated strings of driver types
	std::vector<std::pair<int, char*>> DriversSorted;

	int CurrentDriver; // Current pixel protocol selection (index)
	uint8_t CurrentDriverType; // RGB = 0, RGBW = 1
	int CurrentDriverSpeed; // Output chip speed (0 = Slow, 1 = Fast)
	uint8_t CurrentDriverExpanded; // Expanded/Condensed Mode
	uint8_t Gamma[4]; // R, G & B Gamma
	float Gammaf[4];

	char Nickname[40]; // if the product has a nickname, null terminatedDriverNames
	uint16_t Temperature; // current temp reading
	uint8_t MaxTargetTemp; // Max target temperature (fan control). 0xFF means no fan control.
	uint8_t NumBanks; // Number of banks for voltage readings
	uint16_t* VoltageBanks;// [NumBanks][2]; // Voltage on power banks (*10)
	int TestMode; // Current test mode program (0 = off/live data)
	int TestCols[4] = { 0 };
	float tempTestCols[4] = { 0 };
	float idCol[3] = { 0.6f, 0.6f, 0.6f };
	uint8_t TestOutputNum;
	uint16_t TestPixelNum;
	int* TestCycleCols; //[NumOutputs]
	bool testModeCycleOuputs = false;
	bool testModeCyclePixels = false;
	bool testModeEnessColourOuputs = false;
	size_t uid;
	int openTab = 0;
	bool autoChannels = false;

	int MinUniverse() const;
	int MaxUniverse() const;


};

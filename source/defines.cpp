#include "defines.h"

const char* ExportAllTypes[3] = {
		"JSON",
		"Virtual Devices (Add)",
		"Virtual Devices (Clean)"
};

const char* SyncTypes[3] = {
		"Match Static IP",
		"Match Nickname",
		"Match MAC addres"
};

const char* DriverTypes[3] = {
		"RGB only",
		"RGBW only",
		"Either RGB or RGBW"
};

const char* DriverSpeedsMhz[12] = {
	"0.4 MHz", // Data Only Slow
	"0.6 MHz", // Data Only Fast
	"0.8 MHz",
	"1.0 MHz",
	"1.2 MHz",
	"1.4 MHz",
	"1.6 MHz",
	"1.8 MHz",
	"2.0 MHz",
	"2.2 MHz",
	"2.5 MHz",
	"2.9 MHz"
};

const char* TestModes[9] = {
	"None(Live Control Data)",
	"RGBW Cycle",
	"Red",
	"Green",
	"Blue",
	"White",
	"Set Color",
	"Color Fade",
	"Single Pixel"
};

const char* RGBW_Order[24] = {
   "R-G-B/R-G-B-W",
   "R-B-G/R-B-G-W",
   "G-R-B/G-R-B-W",
   "B-R-G/B-R-G-W",
   "G-B-R/G-B-R-W",
   "B-G-R/B-G-R-W",
   "R-G-W-B",
   "R-B-W-G",
   "G-R-W-B",
   "B-R-W-G",
   "G-B-W-R",
   "B-G-W-R",
   "R-W-G-B",
   "R-W-B-G",
   "G-W-R-B",
   "B-W-R-G",
   "G-W-B-R",
   "B-W-G-R",
   "W-R-G-B",
   "W-R-B-G",
   "W-G-R-B",
   "W-B-R-G",
   "W-G-B-R",
   "W-B-G-R"
};

std::string macStr(uint8_t* address) {
	std::stringstream ss;

	for (int i(0); i < 6; i++) {
		ss << std::hex << std::uppercase << std::setw(2) << static_cast<int>(address[i]);
		if (i < 5) ss << ":";
	}

	return ss.str();
}

std::string ipStr(uint8_t* address) {
	std::stringstream ss;

	for (int i(0); i < 4; i++) {
		ss << static_cast<int>(address[i]);
		if (i < 3) ss << ".";
	}

	return ss.str();
}

std::vector<std::string> splitter(std::string in_pattern, std::string& content) {
	std::vector<std::string> split_content;

	std::regex pattern(in_pattern);
	std::copy(std::sregex_token_iterator(content.begin(), content.end(), pattern, -1),
		std::sregex_token_iterator(), std::back_inserter(split_content));
	return split_content;
}

void insertSwapped16(std::vector<uint8_t>& dest, uint16_t* pData, int32_t size)
{
	for (int i = 0; i < size; i++)
	{
		uint16_t data = pData[i];
		dest.push_back((uint8_t)(data >> 8));
		dest.push_back((uint8_t)data);
	}
}

#pragma once

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>  
#include <iomanip>
#include <vector>
#include <regex>
#include <unordered_map>
#ifdef USE_BOOST
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/optional/optional.hpp>
#include <boost/exception/diagnostic_information.hpp>
#define JSON_TYPE boost::property_tree::ptree
#else 
#include <nlohmann/json.hpp>

#define USE_NLOHMANN true
#define JSON_TYPE nlohmann::json
#endif

#define AdvAdr "255.255.255.255" // Advatek zero network broadcast address
#define AdvPort 49150 // Advatek UDP Port
#define OpPoll	0x0001 // A request for controllers to identify themselves.
#define OpPollReply 0x0002 //Reply from a controller to a poll(includes configuration data).
#define OpConfig  0x0005 //New configuration data being directed at a controller.
#define OpBootload 0x0006 // Send the controller into bootloader mode.
#define OpNetwork 0x0007 // Change network settings only.
#define OpTestSet 0x0008 // Set the test mode.
#define OpTestAnnounce 0x0009 // Announce the current test mode.
#define OpVisualIdent 0x000a // A broadcast to request the controller of the designated MAC to identify its location by flashing its status LED.

#define bswap_16(x) x=((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))
#ifdef  USE_BOOST
#define EqualValueJson(type, atr) (device->atr == json_device.get<type>(#atr));
#define SetValueFromJson(type, atr) device->atr = json_device.get<type>(#atr);
#define SetChildIntValuesFromJson(atr) \
	for (boost::property_tree::ptree::value_type &node : json_device.get_child(#atr)) { \
	 device->atr[std::stoi(node.first)] = std::stoi(node.second.data()); }
#define SetChildFloatValuesFromJson(atr) \
	for (boost::property_tree::ptree::value_type &node : json_device.get_child(#atr)) { \
	 device->atr[std::stoi(node.first)] = std::stof(node.second.data()); }
#define SetChildStringValuesFromJson(atr) \
	for (boost::property_tree::ptree::value_type &node : json_device.get_child(#atr)) { \
	std::string sTempValue = node.second.data(); \\
	 device->atr[std::stoi(node.first)] = sTempValue.c_str(); }

#endif //  USE_BOOST

extern const char* ExportAllTypes[3];
extern const char* SyncTypes[3];
extern const char* RGBW_Order[24];
extern const char* DriverTypes[3];
extern const char* DriverSpeeds[5];
extern const char* DriverSpeedsMhz[12];
extern const char* TestModes[9];

class IClient;

enum ETestPattern
{
	ETP_NONE = 0,
	ETP_CYCLE,
	ETP_RED,
	ETP_GREEN,
	ETP_BLUE,
	ETP_WHITE,
	ETP_SET_COLOUR,
	ETP_FADE,
	ETP_STROBE_VERT,
	ETP_STROBE_HORZ,
	ETP_RAINBOW,
	ETP_PIXELS
};

struct sImportOptions {
	bool userSet = false;
	std::string json = "";
	bool init = false;
	bool network = true;
	bool ethernet_control = true;
	bool dmx_outputs = true;
	bool led_settings = true;
	bool nickname = true;
	bool fan_on_temp = true;
	bool returnResult = true;
};

std::string macStr(uint8_t* address);
std::string ipStr(uint8_t* address);

std::vector<std::string> splitter(std::string in_pattern, std::string& content);

void insertSwapped16(std::vector<uint8_t>& dest, uint16_t* pData, int32_t size);

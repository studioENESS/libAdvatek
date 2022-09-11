#pragma once

#include "defines.h"
#include "SAdvatekDevice.h"

class advatek_manager {
public:
	uint8_t ProtVer = 8;

	JSON_TYPE OutputPixels;
	JSON_TYPE OutputUniv;
	JSON_TYPE OutputChan;
	JSON_TYPE OutputNull;
	JSON_TYPE OutputZig;
	JSON_TYPE OutputReverse;
	JSON_TYPE OutputColOrder;
	JSON_TYPE OutputGrouping;
	JSON_TYPE OutputBrightness;
	JSON_TYPE DmxOutOn;
	JSON_TYPE DmxOutUniv;
	JSON_TYPE DriverType;
	JSON_TYPE DriverSpeed;
	JSON_TYPE DriverExpandable;
	JSON_TYPE DriverNames;
	JSON_TYPE Gamma;
	JSON_TYPE MinAssistantVer;

	JSON_TYPE tree_virt_device;
	JSON_TYPE tree_add_virt_device;
	JSON_TYPE import_json_device;
	JSON_TYPE tree_getJSON;
	JSON_TYPE tree_exportJSON_device;
	JSON_TYPE tree_exportJSON_devices;
	JSON_TYPE tree_exportJSON_devicesArr;

	std::vector<JSON_TYPE> checkDevices;

	std::vector<uint8_t> testTape;
	std::vector<uint8_t> dataTape;
	std::vector<uint8_t> idTape;
	std::vector<uint8_t> pollTape;

	std::stringstream ss_import_json_device;
	std::stringstream ss_json;

	const char* SortTypes[5] = {
		"Sort Nickname",
		"Sort Model",
		"Sort Static IP",
		"Sort Current IP",
		"Sort Temperature"
	};

	int sortTypeConnected = 0;
	int sortTypeVirtual = 0;

	bool deviceExist(std::vector<sAdvatekDevice*>& devices, uint8_t* Mac);
	bool ipInRange(std::string ip, sAdvatekDevice* device);
	bool sameNetworkSettings(sAdvatekDevice* fromDevice, sAdvatekDevice* toDevice);
	bool deviceCompatible(sAdvatekDevice* fromDevice, sAdvatekDevice* toDevice);
	bool devicesInSync(sAdvatekDevice* fromDevice, sAdvatekDevice* toDevice);

	static bool compareDriverNames(std::pair<int, char*> Driver1, std::pair<int, char*> Driver2);
	static bool compareID(sAdvatekDevice* device1, sAdvatekDevice* device2);
	static bool compareTemperature(sAdvatekDevice* device1, sAdvatekDevice* device2);
	static bool compareModel(sAdvatekDevice* device1, sAdvatekDevice* device2);
	static bool compareStaticIP(sAdvatekDevice* device1, sAdvatekDevice* device2);
	static bool compareCurrentIP(sAdvatekDevice* device1, sAdvatekDevice* device2);
	static bool compareNickname(sAdvatekDevice* device1, sAdvatekDevice* device2);

	std::vector <std::string> networkAdaptors;
	int currentAdaptor = -1;
	size_t getConnectedDeviceIndex(std::string mac);
	int getDriverSortedIndex(sAdvatekDevice* device);

	sAdvatekDevice* getConnectedDevice(std::string mac);

	std::vector<sAdvatekDevice*> connectedDevices;
	std::vector<sAdvatekDevice*> virtualDevices;
	std::vector<sAdvatekDevice*> memoryDevices;
	std::vector<sAdvatekDevice*> getDevicesWithStaticIP(std::vector<sAdvatekDevice*>& devices, std::string ipstr);
	std::vector<sAdvatekDevice*> getDevicesWithNickname(std::vector<sAdvatekDevice*>& devices, std::string nickname);
	std::vector<sAdvatekDevice*> getDevicesWithMac(std::vector<sAdvatekDevice*>& devices, std::string mac);

	void removeConnectedDevice(sAdvatekDevice* device);
	void sortDevices(std::vector<sAdvatekDevice*>& devices, int sortType);
	void sortAllDevices();
	void clearDevices(std::vector<sAdvatekDevice*>& devices);
	void copyDevice(sAdvatekDevice* fromDevice, sAdvatekDevice* toDevice, bool initialise);
	void copyToMemoryDevice(sAdvatekDevice* fromDevice);
	void pasteFromMemoryDeviceTo(sAdvatekDevice* toDevice);

	void copyToNewVirtualDevice(sAdvatekDevice* fromDevice);
	
	void addVirtualDevice(JSON_TYPE json_device, sImportOptions& importOptions);
	
	void addVirtualDevice(sImportOptions& importOptions);
	void pasteToNewVirtualDevice();
	void updateConnectedDevice(sAdvatekDevice* device);
	void updateConnectedDevice(sAdvatekDevice* fromDevice, sAdvatekDevice* connectedDevice);
	void identifyDevice(int d, uint8_t duration);
	void setTest(sAdvatekDevice* device);
	void clearConnectedDevices();
	void bc_networkConfig(sAdvatekDevice* device);
	void poll();
	void softPoll();
	void process_opPollReply(uint8_t* data);
	void process_opTestAnnounce(uint8_t* data);
	void process_udp_message(uint8_t* data);
	void listen();
	void send_udp_message(std::string ip_address, int port, bool b_broadcast, std::vector<uint8_t> message);
	void send_udp_message(std::string ip_address, int port, bool b_broadcast, uint8_t* data, int32_t size);
	void unicast_udp_message(std::string ip_address, std::vector<uint8_t> message);
	void broadcast_udp_message(std::vector<uint8_t> message);
	void auto_sequence_channels(sAdvatekDevice* device);
	void process_simple_config(sAdvatekDevice* device);

	void addUID(sAdvatekDevice* device);
	void sortDriversSorted(sAdvatekDevice* device);

	void refreshAdaptors();
	void setCurrentAdaptor(int adaptorIndex);

	void getJSON(sAdvatekDevice* fromDevice, sImportOptions& importOptions);
	void getJSON(sAdvatekDevice* device, JSON_TYPE& JSONdevice);
	std::string importJSON(sAdvatekDevice* device, sImportOptions& importOptions);
	std::string importJSON(sAdvatekDevice* device, JSON_TYPE json_device, sImportOptions& importOptions);
	void exportJSON(sAdvatekDevice* device, std::string path);
	void exportJSON(std::vector<sAdvatekDevice*>& devices, std::string path);
	std::string validateJSON(JSON_TYPE advatek_devices);

	void setEndUniverseChannel(uint16_t startUniverse, uint16_t startChannel, uint16_t pixelCount, uint16_t outputGrouping, uint16_t& endUniverse, uint16_t& endChannel);
	void load_ipStr(std::string ipStr, uint8_t* address);
	void load_macStr(std::string macStr, uint8_t* address);

	static const char* RGBW_Order[24];

	IClient* m_pUdpClient;

	bool bTestAll = false;

	// ACT - Pixile Accessors
	const std::vector<std::string>& NetworkAdaptors() const { return networkAdaptors; }
	const std::vector<sAdvatekDevice*>& ConnectedDevices() const { return connectedDevices; }
	const std::vector<sAdvatekDevice*>& VirtualDevices() const { return virtualDevices; }

private:

};

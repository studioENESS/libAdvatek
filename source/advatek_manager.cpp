#include "advatek_manager.h"
#include "udpclient.h"

#ifndef _WIN32
#include <netdb.h>
#include <ifaddrs.h>
#endif

bool advatek_manager::compareDriverNames(std::pair<int, char*> Driver1, std::pair<int, char*> Driver2)
{
	return (std::string(Driver1.second).compare(std::string(Driver2.second)) < 0);
}

bool advatek_manager::compareID(sAdvatekDevice* device1, sAdvatekDevice* device2)
{
	return (device1->uid < device2->uid);
}

bool advatek_manager::compareTemperature(sAdvatekDevice* device1, sAdvatekDevice* device2)
{
	if (device1->Temperature == device2->Temperature) return compareID(device1, device2);
	return (device1->Temperature < device2->Temperature);
}

bool advatek_manager::compareModel(sAdvatekDevice* device1, sAdvatekDevice* device2)
{
	int result = std::string((char*)device1->Model).compare(std::string((char*)device2->Model));
	if (result == 0) return compareTemperature(device1, device2);
	return (result < 0);
}

bool advatek_manager::compareStaticIP(sAdvatekDevice* device1, sAdvatekDevice* device2)
{
	int result = ipStr(device1->StaticIP).compare(ipStr(device2->StaticIP));
	if (result == 0) return compareModel(device1, device2);
	return (result < 0);
}

bool advatek_manager::compareCurrentIP(sAdvatekDevice* device1, sAdvatekDevice* device2)
{
	int result = ipStr(device1->CurrentIP).compare(ipStr(device2->CurrentIP));
	if (result == 0) return compareStaticIP(device1, device2);
	return (result < 0);
}

bool advatek_manager::compareNickname(sAdvatekDevice* device1, sAdvatekDevice* device2)
{
	int result = std::string(device1->Nickname).compare(std::string(device2->Nickname));
	if (result == 0) return compareCurrentIP(device1, device2);
	return (result < 0);
}

bool advatek_manager::deviceExist(std::vector<sAdvatekDevice*>& devices, uint8_t* Mac) {

	for (int d(0); d < devices.size(); d++) {
		bool exist = true;
		for (int i(0); i < 6; i++) {
			if (devices[d]->Mac[i] != Mac[i]) exist = false;
		}
		if (exist) return true;
	}

	return false;
}

bool advatek_manager::ipInRange(std::string ipStr, sAdvatekDevice* device) {

	int ip0, ip1, ip2, ip3;
	sscanf(ipStr.c_str(), "%i.%i.%i.%i", &ip0, &ip1, &ip2, &ip3);

	if (device->StaticIP[0] == ip0 &&
		device->StaticIP[1] == ip1 &&
		device->StaticIP[2] == ip2) {
		return true;
	}

	return false;
}

size_t advatek_manager::getConnectedDeviceIndex(std::string mac) {
	for (size_t index = 0; index < connectedDevices.size(); ++index) {
		if (macStr(connectedDevices[index]->Mac) == mac) return index;
	}
	return -1;
}

int advatek_manager::getDriverSortedIndex(sAdvatekDevice* device) {
	sortDriversSorted(device);
	for (int index = 0; index < device->DriversSorted.size(); ++index) {
		if (device->DriversSorted[index].first == device->CurrentDriver) return index;
	}
	return -1;
}

sAdvatekDevice* advatek_manager::getConnectedDevice(std::string mac) {
	for (size_t index = 0; index < connectedDevices.size(); ++index) {
		if (macStr(connectedDevices[index]->Mac) == mac) return connectedDevices[index];
	}
	return NULL;
}

void advatek_manager::removeConnectedDevice(sAdvatekDevice* device) {
	if (device == NULL) return;
	int deviceIndex = getConnectedDeviceIndex(macStr(device->Mac));
	if (deviceIndex < 0) return;
	if (device) delete device;
	connectedDevices.erase(connectedDevices.begin() + deviceIndex);
}

bool advatek_manager::sameNetworkSettings(sAdvatekDevice* fromDevice, sAdvatekDevice* toDevice) {
	bool same = true;

	if (ipStr(toDevice->StaticIP) != ipStr(fromDevice->StaticIP)) same = false;
	if (ipStr(toDevice->StaticSM) != ipStr(fromDevice->StaticSM)) same = false;
	if (toDevice->DHCP != fromDevice->DHCP) same = false;

	return same;
}

bool advatek_manager::devicesInSync(sAdvatekDevice* fromDevice, sAdvatekDevice* toDevice) {
	if (!deviceCompatible(fromDevice, toDevice)) return false;
	if (!sameNetworkSettings(fromDevice, toDevice)) return false;
	if (toDevice->Protocol != fromDevice->Protocol) return false;
	if (toDevice->HoldLastFrame != fromDevice->HoldLastFrame) return false;
	if (toDevice->SimpleConfig != fromDevice->SimpleConfig) return false;

	// NumOutputs
	for (int output = 0; output < toDevice->NumOutputs; output++) {
		if (toDevice->OutputPixels[output] != fromDevice->OutputPixels[output]) return false;
		if (toDevice->OutputUniv[output] != fromDevice->OutputUniv[output]) return false;
		if (toDevice->OutputChan[output] != fromDevice->OutputChan[output]) return false;
		if (toDevice->OutputNull[output] != fromDevice->OutputNull[output]) return false;
		if (toDevice->OutputZig[output] != fromDevice->OutputZig[output]) return false;
		if (toDevice->OutputReverse[output] != fromDevice->OutputReverse[output]) return false;
		if (toDevice->OutputColOrder[output] != fromDevice->OutputColOrder[output]) return false;
		if (toDevice->OutputGrouping[output] != fromDevice->OutputGrouping[output]) return false;
		if (toDevice->OutputBrightness[output] != fromDevice->OutputBrightness[output]) return false;
	}

	// NumDMXOutputs
	if (toDevice->ProtocolsOnDmxOut != fromDevice->ProtocolsOnDmxOut) return false;
	for (int output = 0; output < toDevice->NumDMXOutputs; output++) {
		if (toDevice->DmxOutOn[output] != fromDevice->DmxOutOn[output]) return false;
		if (toDevice->DmxOutUniv[output] != fromDevice->DmxOutUniv[output]) return false;
	}

	//NumDrivers
	for (int output = 0; output < toDevice->NumDrivers; output++) {
		if (toDevice->DriverType[output] != fromDevice->DriverType[output]) return false;
		if (toDevice->DriverSpeed[output] != fromDevice->DriverSpeed[output]) return false;
		if (toDevice->DriverExpandable[output] != fromDevice->DriverExpandable[output]) return false;
	}

	if (toDevice->CurrentDriver != fromDevice->CurrentDriver) return false;
	if (toDevice->CurrentDriverType != fromDevice->CurrentDriverType) return false;
	if (toDevice->CurrentDriverSpeed != fromDevice->CurrentDriverSpeed) return false;
	if (toDevice->CurrentDriverExpanded != fromDevice->CurrentDriverExpanded) return false;

	for (int i = 0; i < 4; i++) {
		if (toDevice->Gamma[i] != fromDevice->Gamma[i]) return false;
	}

	if (std::string(toDevice->Nickname) != std::string(fromDevice->Nickname)) return false;
	if (toDevice->MaxTargetTemp != fromDevice->MaxTargetTemp) return false;

	return true;
}

bool advatek_manager::deviceCompatible(sAdvatekDevice* fromDevice, sAdvatekDevice* toDevice) {
	// Check for output channel count etc.
	// In the future we can add ignore flags for DMX etc.
	// And make it compatable so you can map smaller devices to bigger ones
	// toDevice->NumOutputs < fromDevice->NumOutputs
	bool compatible = true;
	if (toDevice->MaxPixPerOutput != fromDevice->MaxPixPerOutput) compatible = false;
	if (toDevice->NumOutputs != fromDevice->NumOutputs) compatible = false;
	if (toDevice->NumDMXOutputs != fromDevice->NumDMXOutputs) compatible = false;
	if (toDevice->NumDrivers != fromDevice->NumDrivers) compatible = false;
	if (toDevice->DriverNameLength != fromDevice->DriverNameLength) compatible = false;
	return compatible;
}

void advatek_manager::listen() {
	if (m_pUdpClient && m_pUdpClient->HasMessages())
	{
		process_udp_message(m_pUdpClient->PopMessage().data());
	}
}

void advatek_manager::send_udp_message(std::string ip_address, int port, bool b_broadcast, uint8_t* data, int32_t size)
{
	if (m_pUdpClient)
	{
		m_pUdpClient->Send((const char*)data, size, ip_address, b_broadcast, port);
	}

}
void advatek_manager::send_udp_message(std::string ip_address, int port, bool b_broadcast, std::vector<uint8_t> message)
{
	if (m_pUdpClient)
	{
		m_pUdpClient->Send(message, ip_address, b_broadcast, port);
	}

}

void advatek_manager::unicast_udp_message(std::string ip_address, std::vector<uint8_t> message)
{
	send_udp_message(ip_address, AdvPort, false, message);
}

void advatek_manager::broadcast_udp_message(std::vector<uint8_t> message)
{
	send_udp_message(AdvAdr, AdvPort, true, message);
}

void advatek_manager::identifyDevice(int d, uint8_t duration) {
	auto device = advatek_manager::connectedDevices[d];
	idTape.clear();
	idTape.resize(12);
	idTape[0] = 'A';
	idTape[1] = 'd';
	idTape[2] = 'v';
	idTape[3] = 'a';
	idTape[4] = 't';
	idTape[5] = 'e';
	idTape[6] = 'c';
	idTape[7] = 'h';
	idTape[8] = 0x00;   // Null Terminator
	idTape[9] = 0x00;   // OpCode
	idTape[10] = 0x05;  // OpCode
	idTape[11] = 0x08;  // ProtVer

	idTape.insert(idTape.end(), device->Mac, device->Mac + 6);
	idTape.push_back(duration);

	unicast_udp_message(ipStr(device->CurrentIP), idTape);
}

void advatek_manager::copyToMemoryDevice(sAdvatekDevice* fromDevice) {
	memoryDevices.clear();
	sAdvatekDevice* memoryDevice = new sAdvatekDevice();
	copyDevice(fromDevice, memoryDevice, true);
	addUID(memoryDevice);
	advatek_manager::memoryDevices.emplace_back(memoryDevice);
}

void advatek_manager::pasteFromMemoryDeviceTo(sAdvatekDevice* toDevice) {
	if (memoryDevices.size() == 0) {
		return;
	}
	copyDevice(memoryDevices[0], toDevice, false);
}

void advatek_manager::copyToNewVirtualDevice(sAdvatekDevice* fromDevice) {
	tree_virt_device.clear();
	getJSON(fromDevice, tree_virt_device);
	sAdvatekDevice* device = new sAdvatekDevice();
	sImportOptions importOptions = sImportOptions();
	importOptions.init = true;
	importJSON(device, tree_virt_device, importOptions);
	addUID(device);
	virtualDevices.emplace_back(device);
}

void advatek_manager::addVirtualDevice(JSON_TYPE json_device, sImportOptions& importOptions) {
	sAdvatekDevice* device = new sAdvatekDevice();
	importJSON(device, json_device, importOptions);
	addUID(device);
	virtualDevices.emplace_back(device);
}

void advatek_manager::addVirtualDevice(sImportOptions& importOptions) {
	tree_add_virt_device.clear();
	ss_json.str(std::string());
	ss_json.clear();
	ss_json << importOptions.json;
	#ifdef USE_BOOST
		boost::property_tree::read_json(ss_json, tree_add_virt_device);
#else
	tree_add_virt_device = JSON_TYPE::parse(ss_json.str());
#endif


	if (tree_add_virt_device.count("advatek_devices") > 0) {
		// Might have multiple devices
		for (auto& device : tree_add_virt_device["advatek_devices"])
		{
			addVirtualDevice(device, importOptions);
		}
	}
	else { // Single device backwards compatable
		addVirtualDevice(tree_add_virt_device, importOptions);
	}
}

void advatek_manager::addUID(sAdvatekDevice* device) {
	std::hash<std::string> hasher;
	float LO = 0.399;
	float HI = 0.999;
	device->uid = hasher(macStr(device->Mac).append(std::to_string(rand())));
	device->idCol[0] = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}

void advatek_manager::sortDriversSorted(sAdvatekDevice* device) {
	device->DriversSorted.clear();
	for (int i = 0; i < device->NumDrivers; i++) {
		device->DriversSorted.emplace_back(i, device->DriverNames[i]);
	}

	sort(device->DriversSorted.begin(), device->DriversSorted.end(), compareDriverNames);
}

std::vector<sAdvatekDevice*> advatek_manager::getDevicesWithStaticIP(std::vector<sAdvatekDevice*>& devices, std::string ipstr) {
	std::vector<sAdvatekDevice*> matchedDevices;

	for (int d(0); d < devices.size(); d++) {
		if (ipStr(devices[d]->StaticIP) == ipstr) {
			matchedDevices.emplace_back(devices[d]);
		}
	}

	return matchedDevices;
}

std::vector<sAdvatekDevice*> advatek_manager::getDevicesWithNickname(std::vector<sAdvatekDevice*>& devices, std::string nickname) {
	std::vector<sAdvatekDevice*> matchedDevices;

	for (int d(0); d < devices.size(); d++) {
		if (std::string(devices[d]->Nickname) == nickname) {
			matchedDevices.emplace_back(devices[d]);
		}
	}

	return matchedDevices;
}

std::vector<sAdvatekDevice*> advatek_manager::getDevicesWithMac(std::vector<sAdvatekDevice*>& devices, std::string mac) {
	std::vector<sAdvatekDevice*> matchedDevices;

	for (int d(0); d < devices.size(); d++) {
		if (macStr(devices[d]->Mac) == mac) {
			matchedDevices.emplace_back(devices[d]);
		}
	}

	return matchedDevices;
}

void advatek_manager::pasteToNewVirtualDevice() {
	sAdvatekDevice* device = new sAdvatekDevice();
	pasteFromMemoryDeviceTo(device);
	addUID(device);
	virtualDevices.emplace_back(device);
}

void advatek_manager::updateConnectedDevice(sAdvatekDevice* fromDevice, sAdvatekDevice* connectedDevice) {

	dataTape.clear();
	dataTape.resize(12);
	dataTape[0] = 'A';
	dataTape[1] = 'd';
	dataTape[2] = 'v';
	dataTape[3] = 'a';
	dataTape[4] = 't';
	dataTape[5] = 'e';
	dataTape[6] = 'c';
	dataTape[7] = 'h';
	dataTape[8] = 0x00;   // Null Terminator
	dataTape[9] = 0x00;   // OpCode
	dataTape[10] = 0x05;  // OpCode
	dataTape[11] = 0x08;  // ProtVer

	// Set Mac Address
	dataTape.insert(dataTape.end(), connectedDevice->Mac, connectedDevice->Mac + 6);

	dataTape.push_back(fromDevice->DHCP);

	dataTape.insert(dataTape.end(), fromDevice->StaticIP, fromDevice->StaticIP + 4);
	dataTape.insert(dataTape.end(), fromDevice->StaticSM, fromDevice->StaticSM + 4);

	dataTape.push_back(fromDevice->Protocol);
	dataTape.push_back(fromDevice->HoldLastFrame);
	dataTape.push_back(fromDevice->SimpleConfig);

	insertSwapped16(dataTape, fromDevice->OutputPixels, fromDevice->NumOutputs);
	insertSwapped16(dataTape, fromDevice->OutputUniv, fromDevice->NumOutputs);
	insertSwapped16(dataTape, fromDevice->OutputChan, fromDevice->NumOutputs);

	dataTape.insert(dataTape.end(), fromDevice->OutputNull, fromDevice->OutputNull + fromDevice->NumOutputs);
	insertSwapped16(dataTape, fromDevice->OutputZig, fromDevice->NumOutputs);

	dataTape.insert(dataTape.end(), fromDevice->OutputReverse, fromDevice->OutputReverse + fromDevice->NumOutputs);
	dataTape.insert(dataTape.end(), fromDevice->OutputColOrder, fromDevice->OutputColOrder + fromDevice->NumOutputs);
	insertSwapped16(dataTape, fromDevice->OutputGrouping, fromDevice->NumOutputs);

	dataTape.insert(dataTape.end(), fromDevice->OutputBrightness, fromDevice->OutputBrightness + fromDevice->NumOutputs);
	dataTape.insert(dataTape.end(), fromDevice->DmxOutOn, fromDevice->DmxOutOn + fromDevice->NumDMXOutputs);
	insertSwapped16(dataTape, fromDevice->DmxOutUniv, fromDevice->NumDMXOutputs);

	dataTape.push_back(fromDevice->CurrentDriver);
	dataTape.push_back(fromDevice->CurrentDriverType);

	dataTape.push_back(fromDevice->CurrentDriverSpeed);
	dataTape.push_back(fromDevice->CurrentDriverExpanded);

	dataTape.insert(dataTape.end(), fromDevice->Gamma, fromDevice->Gamma + 4);
	dataTape.insert(dataTape.end(), fromDevice->Nickname, fromDevice->Nickname + 40);

	dataTape.push_back(fromDevice->MaxTargetTemp);

	unicast_udp_message(ipStr(connectedDevice->CurrentIP), dataTape);
	removeConnectedDevice(connectedDevice);
}

void advatek_manager::updateConnectedDevice(sAdvatekDevice* device) {

	if ((bool)device->SimpleConfig) {
		advatek_manager::process_simple_config(device);
	}

	updateConnectedDevice(device, device);
}

void advatek_manager::setTest(sAdvatekDevice* device) {

	if (bTestAll) {
		testTape.clear();
		testTape.resize(12);
		testTape[0] = 'A';
		testTape[1] = 'd';
		testTape[2] = 'v';
		testTape[3] = 'a';
		testTape[4] = 't';
		testTape[5] = 'e';
		testTape[6] = 'c';
		testTape[7] = 'h';
		testTape[8] = 0x00;   // Null Terminator
		testTape[9] = 0x00;   // OpCode
		testTape[10] = 0x08;  // OpCode
		testTape[11] = device->ProtVer;

		testTape.insert(testTape.end(), device->Mac, device->Mac + 6);

		testTape.push_back(device->TestMode);

		testTape.insert(testTape.end(), device->TestCols, device->TestCols + 4);

		// Protver 7 does not process after this but we can still send it ...
		testTape.push_back(device->TestOutputNum);

		testTape.push_back((uint8_t)(device->TestPixelNum >> 8));
		testTape.push_back((uint8_t)device->TestPixelNum);
		for (auto adevice : connectedDevices) {
			testTape[11] = adevice->ProtVer;
			for (int mac = 0; mac < 6; mac++)
				testTape[12 + mac] = adevice->Mac[mac];

			unicast_udp_message(ipStr(adevice->CurrentIP), testTape);
		}
	}
	else {
		testTape.clear();
		testTape.resize(12);
		testTape[0] = 'A';
		testTape[1] = 'd';
		testTape[2] = 'v';
		testTape[3] = 'a';
		testTape[4] = 't';
		testTape[5] = 'e';
		testTape[6] = 'c';
		testTape[7] = 'h';
		testTape[8] = 0x00;   // Null Terminator
		testTape[9] = 0x00;   // OpCode
		testTape[10] = 0x08;  // OpCode
		testTape[11] = device->ProtVer;

		testTape.insert(testTape.end(), device->Mac, device->Mac + 6);

		testTape.push_back(device->TestMode);

		testTape.insert(testTape.end(), device->TestCols, device->TestCols + 4);

		// Protver 7 does not process after this but we can still send it ...
		testTape.push_back(device->TestOutputNum);

		testTape.push_back((uint8_t)(device->TestPixelNum >> 8));
		testTape.push_back((uint8_t)device->TestPixelNum);

		unicast_udp_message(ipStr(device->CurrentIP), testTape);
	}
}

void advatek_manager::sortDevices(std::vector<sAdvatekDevice*>& devices, int sortType) {

	switch (sortType) {
	case 0:
		sort(devices.begin(), devices.end(), compareNickname);
		break;
	case 1:
		sort(devices.begin(), devices.end(), compareModel);
		break;
	case 2:
		sort(devices.begin(), devices.end(), compareStaticIP);
		break;
	case 3:
		sort(devices.begin(), devices.end(), compareCurrentIP);
		break;
	case 4:
		sort(devices.begin(), devices.end(), compareTemperature);
		break;
	default:
		break;
	}
}

void advatek_manager::sortAllDevices() {
	sortDevices(connectedDevices, sortTypeConnected);
	sortDevices(virtualDevices, sortTypeVirtual);
}

void advatek_manager::clearDevices(std::vector<sAdvatekDevice*>& devices) {
	for (auto device : devices)
	{
		if (device) delete device;
	}

	devices.clear();
}

void advatek_manager::clearConnectedDevices() {
	clearDevices(connectedDevices);
}

void advatek_manager::bc_networkConfig(sAdvatekDevice* device) {
	dataTape.clear();
	dataTape.resize(12);
	dataTape[0] = 'A';
	dataTape[1] = 'd';
	dataTape[2] = 'v';
	dataTape[3] = 'a';
	dataTape[4] = 't';
	dataTape[5] = 'e';
	dataTape[6] = 'c';
	dataTape[7] = 'h';
	dataTape[8] = 0x00;   // Null Terminator
	dataTape[9] = 0x00;   // OpCode
	dataTape[10] = 0x07;  // OpCode
	dataTape[11] = 0x08;  // ProtVer

	dataTape.insert(dataTape.end(), device->Mac, device->Mac + 6);

	dataTape.push_back(device->DHCP);

	dataTape.insert(dataTape.end(), device->StaticIP, device->StaticIP + 4);
	dataTape.insert(dataTape.end(), device->StaticSM, device->StaticSM + 4);

	broadcast_udp_message(dataTape);
}

void advatek_manager::poll() {
	advatek_manager::clearDevices(connectedDevices);
	softPoll();
}

void advatek_manager::softPoll() {
	pollTape.clear();
	pollTape.resize(12);
	pollTape[0] = 'A';
	pollTape[1] = 'd';
	pollTape[2] = 'v';
	pollTape[3] = 'a';
	pollTape[4] = 't';
	pollTape[5] = 'e';
	pollTape[6] = 'c';
	pollTape[7] = 'h';
	pollTape[8] = 0x00;   // Null Terminator
	pollTape[9] = 0x00;   // OpCode
	pollTape[10] = 0x01;  // OpCode
	pollTape[11] = 0x08;  // ProtVer

	broadcast_udp_message(pollTape);
}

void advatek_manager::process_opPollReply(uint8_t* data) {

	sAdvatekDevice* rec_data = new sAdvatekDevice();

	memcpy(&rec_data->ProtVer, data, sizeof(uint8_t));
	data += 1;

	memcpy(&rec_data->CurrentProtVer, data, sizeof(uint8_t));
	data += 1;

	memcpy(rec_data->Mac, data, 6);
	data += 6;

	memcpy(&rec_data->ModelLength, data, sizeof(uint8_t));
	data += 1;

	rec_data->Model = new uint8_t[rec_data->ModelLength + 1];
	memset(rec_data->Model, 0x00, sizeof(uint8_t) * (rec_data->ModelLength + 1));
	memcpy(rec_data->Model, data, sizeof(uint8_t) * rec_data->ModelLength);
	data += rec_data->ModelLength;

	memcpy(&rec_data->HwRev, data, sizeof(uint8_t));
	data += 1;

	memcpy(rec_data->MinAssistantVer, data, sizeof(uint8_t) * 3);
	data += 3;

	memcpy(&rec_data->LenFirmware, data, sizeof(uint8_t));
	data += 1;

	rec_data->Firmware = new uint8_t[rec_data->LenFirmware + 1];
	memset(rec_data->Firmware, 0x00, sizeof(uint8_t) * (rec_data->LenFirmware + 1));
	memcpy(rec_data->Firmware, data, sizeof(uint8_t) * rec_data->LenFirmware);
	data += rec_data->LenFirmware;

	memcpy(&rec_data->Brand, data, sizeof(uint8_t));
	data += 1;

	memcpy(rec_data->CurrentIP, data, sizeof(uint8_t) * 4);
	data += 4;

	memcpy(rec_data->CurrentSM, data, sizeof(uint8_t) * 4);
	data += 4;

	memcpy(&rec_data->DHCP, data, sizeof(uint8_t));
	data += 1;

	memcpy(rec_data->StaticIP, data, sizeof(uint8_t) * 4);
	data += 4;

	memcpy(rec_data->StaticSM, data, sizeof(uint8_t) * 4);
	data += 4;

	memcpy(&rec_data->Protocol, data, sizeof(uint8_t));
	data += 1;

	memcpy(&rec_data->HoldLastFrame, data, sizeof(uint8_t));
	data += 1;

	memcpy(&rec_data->SimpleConfig, data, sizeof(uint8_t));
	data += 1;

	memcpy(&rec_data->MaxPixPerOutput, data, sizeof(uint16_t));
	data += 2;
	bswap_16(rec_data->MaxPixPerOutput);

	memcpy(&rec_data->NumOutputs, data, sizeof(uint8_t));
	data += 1;
	rec_data->OutputPixels = new uint16_t[rec_data->NumOutputs];
	memcpy(rec_data->OutputPixels, data, sizeof(uint16_t) * rec_data->NumOutputs);
	data += (rec_data->NumOutputs * 2);

	for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		bswap_16(rec_data->OutputPixels[i]);
	}

	rec_data->OutputUniv = new uint16_t[rec_data->NumOutputs];
	memcpy(rec_data->OutputUniv, data, sizeof(uint16_t) * rec_data->NumOutputs);
	data += (rec_data->NumOutputs * 2);

	for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		bswap_16(rec_data->OutputUniv[i]);
	}

	rec_data->OutputChan = new uint16_t[rec_data->NumOutputs];
	memcpy(rec_data->OutputChan, data, sizeof(uint16_t) * rec_data->NumOutputs);
	data += (rec_data->NumOutputs * 2);

	for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		bswap_16(rec_data->OutputChan[i]);
	}

	rec_data->OutputNull = new uint8_t[rec_data->NumOutputs];
	memcpy(rec_data->OutputNull, data, sizeof(uint8_t) * rec_data->NumOutputs);
	data += rec_data->NumOutputs;

	rec_data->OutputZig = new uint16_t[rec_data->NumOutputs];
	memcpy(rec_data->OutputZig, data, sizeof(uint16_t) * rec_data->NumOutputs);
	data += (rec_data->NumOutputs * 2);

	for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		bswap_16(rec_data->OutputZig[i]);
	}

	rec_data->OutputReverse = new uint8_t[rec_data->NumOutputs];
	memcpy(rec_data->OutputReverse, data, sizeof(uint8_t) * rec_data->NumOutputs);
	data += rec_data->NumOutputs;

	rec_data->OutputColOrder = new uint8_t[rec_data->NumOutputs];
	memcpy(rec_data->OutputColOrder, data, sizeof(uint8_t) * rec_data->NumOutputs);
	data += rec_data->NumOutputs;

	rec_data->OutputGrouping = new uint16_t[rec_data->NumOutputs];
	memcpy(rec_data->OutputGrouping, data, sizeof(uint16_t) * rec_data->NumOutputs);
	data += (rec_data->NumOutputs * 2);

	for (int i(0); i < (int)rec_data->NumOutputs; i++) {
		bswap_16(rec_data->OutputGrouping[i]);
	}

	rec_data->OutputBrightness = new uint8_t[rec_data->NumOutputs];
	memcpy(rec_data->OutputBrightness, data, sizeof(uint8_t) * rec_data->NumOutputs);
	data += rec_data->NumOutputs;

	memcpy(&rec_data->NumDMXOutputs, data, sizeof(uint8_t));
	data += 1;

	memcpy(&rec_data->ProtocolsOnDmxOut, data, sizeof(uint8_t));
	data += 1;

	rec_data->DmxOutOn = new uint8_t[rec_data->NumDMXOutputs];
	rec_data->TempDmxOutOn = new bool[rec_data->NumDMXOutputs];
	memcpy(rec_data->DmxOutOn, data, sizeof(uint8_t) * rec_data->NumDMXOutputs);
	data += rec_data->NumDMXOutputs;

	rec_data->DmxOutUniv = new uint16_t[rec_data->NumDMXOutputs];
	memcpy(rec_data->DmxOutUniv, data, sizeof(uint16_t) * rec_data->NumDMXOutputs);
	data += (rec_data->NumDMXOutputs * 2);

	for (int i(0); i < (int)rec_data->NumDMXOutputs; i++) {
		bswap_16(rec_data->DmxOutUniv[i]);
	}

	memcpy(&rec_data->NumDrivers, data, sizeof(uint8_t));
	data += 1;

	memcpy(&rec_data->DriverNameLength, data, sizeof(uint8_t));
	data += 1;

	rec_data->DriverType = new uint8_t[rec_data->NumDrivers];
	memcpy(rec_data->DriverType, data, sizeof(uint8_t) * rec_data->NumDrivers);
	data += rec_data->NumDrivers;

	rec_data->DriverSpeed = new uint8_t[rec_data->NumDrivers];
	memcpy(rec_data->DriverSpeed, data, sizeof(uint8_t) * rec_data->NumDrivers);
	data += rec_data->NumDrivers;

	rec_data->DriverExpandable = new uint8_t[rec_data->NumDrivers];
	memcpy(rec_data->DriverExpandable, data, sizeof(uint8_t) * rec_data->NumDrivers);
	data += rec_data->NumDrivers;

	rec_data->DriverNames = new char* [rec_data->NumDrivers];

	for (int i = 0; i < rec_data->NumDrivers; i++) {
		rec_data->DriverNames[i] = new char[rec_data->DriverNameLength + 1];
		memset(rec_data->DriverNames[i], 0, sizeof(uint8_t) * rec_data->DriverNameLength + 1);
		memcpy(rec_data->DriverNames[i], data, sizeof(uint8_t) * rec_data->DriverNameLength);
		data += rec_data->DriverNameLength;
	}

	memcpy(&rec_data->CurrentDriver, data, sizeof(uint8_t));
	data += 1;

	memcpy(&rec_data->CurrentDriverType, data, sizeof(uint8_t));
	data += 1;

	memcpy(&rec_data->CurrentDriverSpeed, data, sizeof(uint8_t));
	data += 1;

	memcpy(&rec_data->CurrentDriverExpanded, data, sizeof(uint8_t));
	data += 1;

	memcpy(rec_data->Gamma, data, sizeof(uint8_t) * 4);
	data += 4;

	rec_data->Gammaf[0] = (float)rec_data->Gamma[0] * 0.1;
	rec_data->Gammaf[1] = (float)rec_data->Gamma[1] * 0.1;
	rec_data->Gammaf[2] = (float)rec_data->Gamma[2] * 0.1;
	rec_data->Gammaf[3] = (float)rec_data->Gamma[3] * 0.1;

	memcpy(rec_data->Nickname, data, sizeof(uint8_t) * 40);
	data += 40;

	memcpy(&rec_data->Temperature, data, sizeof(uint16_t));
	data += 2;
	bswap_16(rec_data->Temperature);

	memcpy(&rec_data->MaxTargetTemp, data, sizeof(uint8_t));
	data += 1;

	memcpy(&rec_data->NumBanks, data, sizeof(uint8_t));
	data += 1;

	rec_data->VoltageBanks = new uint16_t[rec_data->NumBanks];
	memcpy(rec_data->VoltageBanks, data, sizeof(uint16_t) * rec_data->NumBanks);
	data += (rec_data->NumBanks * 2);

	for (int i(0); i < (int)rec_data->NumBanks; i++) {
		bswap_16(rec_data->VoltageBanks[i]);
	}

	memcpy(&rec_data->TestMode, data, sizeof(uint8_t));
	data += 1;

	memcpy(rec_data->TestCols, data, sizeof(uint8_t) * 4);
	data += 4;

	memcpy(&rec_data->TestOutputNum, data, sizeof(uint8_t));
	data += 1;

	memcpy(&rec_data->TestPixelNum, data, sizeof(uint16_t));
	data += 2;
	bswap_16(rec_data->TestPixelNum);

	// No black test init colour
	if (((int)rec_data->TestCols[0] + (int)rec_data->TestCols[1] + (int)rec_data->TestCols[2]) == 0)
	{
		rec_data->TestCols[0] = rand() % 255;
		rec_data->TestCols[1] = rand() % 255;
		rec_data->TestCols[2] = rand() % 255;
		rec_data->TestCols[3] = 255;
	}
	else {
		// Clean possible garbage from controller
		rec_data->TestCols[0] = abs(rec_data->TestCols[0]) % 255;
		rec_data->TestCols[1] = abs(rec_data->TestCols[1]) % 255;
		rec_data->TestCols[2] = abs(rec_data->TestCols[2]) % 255;
		rec_data->TestCols[3] = abs(rec_data->TestCols[3]) % 255;
	}

	// Set tempTestColour
	rec_data->tempTestCols[0] = (float)rec_data->TestCols[0] / 255;
	rec_data->tempTestCols[1] = (float)rec_data->TestCols[1] / 255;
	rec_data->tempTestCols[2] = (float)rec_data->TestCols[2] / 255;
	rec_data->tempTestCols[3] = (float)rec_data->TestCols[3] / 255;

	addUID(rec_data);
	sortDriversSorted(rec_data);

	if (!deviceExist(connectedDevices, rec_data->Mac)) {
		connectedDevices.emplace_back(rec_data);
	}
	else {
		if (rec_data) delete rec_data;
	}
}

void advatek_manager::process_opTestAnnounce(uint8_t* data) {
	if (bTestAll) return;

	uint8_t ProtVer;
	memcpy(&ProtVer, data, sizeof(uint8_t));
	data += 1;

	uint8_t Mac[6];
	memcpy(Mac, data, 6);
	data += 6;

	uint8_t CurrentIP[4];
	memcpy(CurrentIP, data, 4);
	data += 4;

	int deviceID = -1;

	for (int d(0); d < advatek_manager::connectedDevices.size(); d++) {
		bool exist = true;
		for (int i(0); i < 6; i++) {
			if (advatek_manager::connectedDevices[d]->Mac[i] != Mac[i]) exist = false;
		}
		if (exist) deviceID = d;
	}

	if (deviceID >= 0) {
		auto device = advatek_manager::connectedDevices[deviceID];

		memcpy(&device->TestMode, data, sizeof(uint8_t));
		data += 1;

		memcpy(device->TestCols, data, sizeof(uint8_t) * 4);
		data += 4;

		memcpy(&device->TestOutputNum, data, sizeof(uint8_t));
		data += 1;

		memcpy(&device->TestPixelNum, data, sizeof(uint16_t));
		data += 2;
		bswap_16(device->TestPixelNum);
	}
	return;
}

void advatek_manager::process_udp_message(uint8_t* data) {
	char ID[9];
	memcpy(ID, data, sizeof(uint8_t) * 9);
	data += 9;
	
	std::string sid;
	for (int i(0); i < 8; i++) { sid += ID[i]; }
	if (sid.compare("Advatech") != 0) return; // Not an Advatek message ...

	uint16_t OpCodes;
	memcpy(&OpCodes, data, sizeof(uint16_t));
	data += 2;
	// Swap bytes
	bswap_16(OpCodes);

	switch (OpCodes) {
	case OpPollReply:
		std::cout << "Processing UDP Poll Reply." << OpCodes << std::endl;
		process_opPollReply(data);
		return;
	case OpTestAnnounce:
		std::cout << "Processing UDP Test Announce." << OpCodes << std::endl;
		process_opTestAnnounce(data);
		return;
	default:
		std::cout << "Ignoring UDP message of type " << OpCodes << std::endl;
		return;
	}
}

void advatek_manager::auto_sequence_channels(sAdvatekDevice* device) {

	uint16_t startOutputUniv = device->OutputUniv[0];
	uint16_t startOutputChan = device->OutputChan[0];
	uint16_t startOutputPixels = device->OutputPixels[0];

	uint16_t startEndUniverse = 0;
	uint16_t startEndChannel = 0;

	setEndUniverseChannel(startOutputUniv, startOutputChan, startOutputPixels, device->OutputGrouping[0], startEndUniverse, startEndChannel);

	for (int output = 1; output < device->NumOutputs * 0.5; output++) {
		if (startEndChannel + 1 > 510) {
			startEndUniverse = startEndUniverse + 1;
		}

		device->OutputUniv[output] = startEndUniverse;
		device->OutputChan[output] = (startEndChannel + 1) % 510;

		// Update loop
		startOutputUniv = device->OutputUniv[output];
		startOutputChan = device->OutputChan[output];
		startOutputPixels = device->OutputPixels[output];
		setEndUniverseChannel(startOutputUniv, startOutputChan, startOutputPixels, device->OutputGrouping[output], startEndUniverse, startEndChannel);
	}
	return;
}

void advatek_manager::process_simple_config(sAdvatekDevice* device) {

	device->OutputNull[0] = 0;
	device->OutputZig[0] = 0;
	device->OutputGrouping[0] = 1;
	device->OutputBrightness[0] = 100;
	device->OutputReverse[0] = 0;

	for (int output = 1; output < device->NumOutputs * 0.5; output++) {
		device->OutputNull[output] = device->OutputNull[0];
		device->OutputZig[output] = device->OutputZig[0];
		device->OutputGrouping[output] = device->OutputGrouping[0];
		device->OutputBrightness[output] = device->OutputBrightness[0];
		device->OutputReverse[output] = device->OutputReverse[0];
		device->OutputPixels[output] = device->OutputPixels[0];
		device->OutputGrouping[output] = device->OutputGrouping[0];
	}

	auto_sequence_channels(device);

	return;
}

void advatek_manager::refreshAdaptors() {

	networkAdaptors.clear();

#ifdef _WIN32 
#ifdef USE_BOOST
	boost::asio::ip::tcp::resolver::iterator it;
	try
	{
		boost::asio::io_context io_context;
		boost::asio::ip::tcp::endpoint adaptorEndpoint(boost::asio::ip::address_v4::any(), AdvPort);

		boost::asio::ip::tcp::socket sock(io_context, adaptorEndpoint);

		boost::asio::ip::tcp::resolver resolver(io_context);

		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "", boost::asio::ip::resolver_query_base::flags());
		it = resolver.resolve(query);
	}
	catch (boost::exception& e)
	{
		std::cout << "Unable to find any network adapters - " << boost::diagnostic_information(e) << std::endl;
		return;
	}

	while (it != boost::asio::ip::tcp::resolver::iterator())
	{
		boost::asio::ip::address addr = (it++)->endpoint().address();
		std::cout << "Network adapter found: " << addr.to_string() << std::endl;
		if (addr.is_v4()) {
			networkAdaptors.push_back(addr.to_string());
		}
	}
#endif
#elif __linux__ // __arm__

	struct ifaddrs* ifaddr, * ifa;
	int family, s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1)
	{
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}


	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;

		s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

		if ((strcmp(ifa->ifa_name, "lo") != 0) && (ifa->ifa_addr->sa_family == AF_INET))
		{
			if (s != 0)
			{
				printf("getnameinfo() failed: %s\n", gai_strerror(s));
				exit(EXIT_FAILURE);
			}
			printf("\tInterface : <%s>\n", ifa->ifa_name);
			printf("\t  Address : <%s>\n", host);
			networkAdaptors.push_back(std::string(host));
		}
	}

	freeifaddrs(ifaddr);

	//#elif __APPLE__
#endif

	currentAdaptor = (networkAdaptors.empty()) ? -1 : 0;
	setCurrentAdaptor(currentAdaptor);
}

void advatek_manager::setCurrentAdaptor(int adaptorIndex) {
	

	if (m_pUdpClient) {
		delete m_pUdpClient;
		m_pUdpClient = nullptr;
	}

	if (adaptorIndex >= 0) {
		m_pUdpClient = new UdpClient(networkAdaptors[adaptorIndex].c_str(), AdvPort);
	}
}

std::string advatek_manager::importJSON(sAdvatekDevice* device, JSON_TYPE json_device, sImportOptions& importOptions) {
	std::string s_hold;
	std::stringstream report;
	device->from_json(json_device, importOptions);
	return report.str();
}

std::string advatek_manager::importJSON(sAdvatekDevice* device, sImportOptions& importOptions) {
	import_json_device.clear();
	ss_import_json_device.str(std::string());
	ss_import_json_device.clear();
	if (importOptions.json.empty()) {
		return "Error: No JSON data found.";
	}
	else { // data
		ss_import_json_device << importOptions.json;
		//boost::property_tree::read_json(ss_import_json_device, import_json_device);
		import_json_device = JSON_TYPE::parse(ss_import_json_device); ;
	}
	if (import_json_device.count("advatek_devices") > 0) {
		return importJSON(device, import_json_device["advatek_devices"].front(), importOptions);
	}
	else {
		return importJSON(device, import_json_device, importOptions);
	}
}

void advatek_manager::getJSON(sAdvatekDevice* device, JSON_TYPE & JSONdevice) {

	//JSONdevice = &device;
	device->to_json(JSONdevice);

	/*OutputPixels.clear();
	OutputUniv.clear();
	OutputChan.clear();
	OutputNull.clear();
	OutputZig.clear();
	OutputReverse.clear();
	OutputColOrder.clear();
	OutputGrouping.clear();
	OutputBrightness.clear();
	DmxOutOn.clear();
	DmxOutUniv.clear();
	DriverType.clear();
	DriverSpeed.clear();
	DriverExpandable.clear();
	DriverNames.clear();
	Gamma.clear();
	MinAssistantVer.clear();

	JSONdevice.put("ProtVer", device->ProtVer);
	//JSONdevice.put("HwRev", device->HwRev);
	JSONdevice.put("CurrentProtVer", device->CurrentProtVer);
	JSONdevice.put("Mac", macStr(device->Mac));
	JSONdevice.put("DHCP", device->DHCP);
	JSONdevice.put("StaticIP", ipStr(device->StaticIP));
	JSONdevice.put("StaticSM", ipStr(device->StaticSM));
	JSONdevice.put("CurrentIP", ipStr(device->CurrentIP));
	JSONdevice.put("CurrentSM", ipStr(device->CurrentSM));

	JSONdevice.put("ModelLength", device->ModelLength);
	JSONdevice.put("Model", device->Model);
	JSONdevice.put("Brand", device->Brand);

	JSONdevice.put("LenFirmware", device->LenFirmware);
	JSONdevice.put("Firmware", device->Firmware);

	JSONdevice.put("Protocol", device->Protocol);
	JSONdevice.put("HoldLastFrame", device->HoldLastFrame);
	JSONdevice.put("SimpleConfig", device->SimpleConfig);
	JSONdevice.put("MaxPixPerOutput", device->MaxPixPerOutput);
	JSONdevice.put("NumOutputs", device->NumOutputs);

	for (int output = 0; output < device->NumOutputs; output++)
	{
		OutputPixels.put(std::to_string(output), std::to_string(device->OutputPixels[output]));
		OutputUniv.put(std::to_string(output), std::to_string(device->OutputUniv[output]));
		OutputChan.put(std::to_string(output), std::to_string(device->OutputChan[output]));
		OutputNull.put(std::to_string(output), std::to_string(device->OutputNull[output]));
		OutputZig.put(std::to_string(output), std::to_string(device->OutputZig[output]));
		OutputReverse.put(std::to_string(output), std::to_string(device->OutputReverse[output]));
		OutputColOrder.put(std::to_string(output), std::to_string(device->OutputColOrder[output]));
		OutputGrouping.put(std::to_string(output), std::to_string(device->OutputGrouping[output]));
		OutputBrightness.put(std::to_string(output), std::to_string(device->OutputBrightness[output]));
	}

	JSONdevice.add_child("OutputPixels", OutputPixels);
	JSONdevice.add_child("OutputUniv", OutputUniv);
	JSONdevice.add_child("OutputChan", OutputChan);
	JSONdevice.add_child("OutputNull", OutputNull);
	JSONdevice.add_child("OutputZig", OutputZig);
	JSONdevice.add_child("OutputReverse", OutputReverse);
	JSONdevice.add_child("OutputColOrder", OutputColOrder);
	JSONdevice.add_child("OutputGrouping", OutputGrouping);
	JSONdevice.add_child("OutputBrightness", OutputBrightness);

	JSONdevice.put("NumDMXOutputs", device->NumDMXOutputs);
	JSONdevice.put("ProtocolsOnDmxOut", device->ProtocolsOnDmxOut);

	for (int output = 0; output < device->NumDMXOutputs; output++)
	{
		DmxOutOn.put(std::to_string(output), std::to_string(device->DmxOutOn[output]));
		DmxOutUniv.put(std::to_string(output), std::to_string(device->DmxOutUniv[output]));
	}

	JSONdevice.add_child("DmxOutOn", DmxOutOn);
	JSONdevice.add_child("DmxOutUniv", DmxOutUniv);

	JSONdevice.put("NumDrivers", device->NumDrivers);
	JSONdevice.put("DriverNameLength", device->DriverNameLength);

	for (int output = 0; output < device->NumDrivers; output++)
	{
		DriverType.put(std::to_string(output), std::to_string(device->DriverType[output]));
		DriverSpeed.put(std::to_string(output), std::to_string(device->DriverSpeed[output]));
		DriverExpandable.put(std::to_string(output), std::to_string(device->DriverExpandable[output]));
		DriverNames.put(std::to_string(output), (device->DriverNames[output]));
	}

	JSONdevice.add_child("DriverType", DriverType);
	JSONdevice.add_child("DriverSpeed", DriverSpeed);
	JSONdevice.add_child("DriverExpandable", DriverExpandable);
	JSONdevice.add_child("DriverNames", DriverNames);

	JSONdevice.put("CurrentDriver", device->CurrentDriver);
	JSONdevice.put("CurrentDriverType", device->CurrentDriverType);
	JSONdevice.put("CurrentDriverSpeed", device->CurrentDriverSpeed);
	JSONdevice.put("CurrentDriverExpanded", device->CurrentDriverExpanded);

	for (int c = 0; c < 4; c++)
	{
		Gamma.put(std::to_string(c), std::to_string(device->Gamma[c]));
	}
	JSONdevice.add_child("Gamma", Gamma);

	//for (int c = 0; c < 3; c++)
	//{
	//	MinAssistantVer.put(std::to_string(c), std::to_string(device->MinAssistantVer[c]));
	//}
	//JSONdevice.add_child("MinAssistantVer", MinAssistantVer);

	JSONdevice.put("Nickname", device->Nickname);
	JSONdevice.put("MaxTargetTemp", device->MaxTargetTemp);
	JSONdevice.put("NumBanks", device->NumBanks);*/

}

void advatek_manager::getJSON(sAdvatekDevice* fromDevice, sImportOptions& importOptions) {
	tree_getJSON.clear();
	getJSON(fromDevice, tree_getJSON);

	std::stringstream jsonStringStream;
	//write_json(jsonStringStream, tree_getJSON);
	jsonStringStream << tree_getJSON;

	importOptions.json = jsonStringStream.str();
}

void advatek_manager::exportJSON(std::vector<sAdvatekDevice*>& devices, std::string path) {
	tree_exportJSON_devices.clear();
	tree_exportJSON_devicesArr.clear();

	for (auto& device : devices) {
		tree_exportJSON_device.clear();
		getJSON(device, tree_exportJSON_device);
		tree_exportJSON_devicesArr.push_back(tree_exportJSON_device);
	}

	tree_exportJSON_devices["advatek_devices"]=tree_exportJSON_devicesArr;

	std::ofstream outfile;
	outfile.open(path, std::ios::out | std::ios::trunc);
	//boost::property_tree::write_json(outfile, tree_exportJSON_devices);
	outfile << tree_exportJSON_devices;
	outfile.close();
}

void advatek_manager::exportJSON(sAdvatekDevice* device, std::string path) {
	tree_exportJSON_devices.clear();
	tree_exportJSON_device.clear();
	tree_exportJSON_devicesArr.clear();

	getJSON(device, tree_exportJSON_device);

	tree_exportJSON_devicesArr.push_back(std::make_pair("", tree_exportJSON_device));
	tree_exportJSON_devices["advatek_devices"]= tree_exportJSON_devicesArr;

	std::ofstream outfile;
	outfile.open(path, std::ios::out | std::ios::trunc);
	
	//	boost::property_tree::write_json(outfile, tree_exportJSON_devices);
	
	outfile << tree_exportJSON_devices;
	outfile.close();
}

std::string advatek_manager::validateJSON(JSON_TYPE advatek_devices) {
	for (JSON_TYPE & e : checkDevices) {
		e.clear();
	}

	checkDevices.clear();

	if (advatek_devices.count("advatek_devices") > 0) {
		for (auto& json_device : advatek_devices["advatek_devices"]) {
			checkDevices.emplace_back(json_device);
		}
	}
	else { // single device to be checked
		checkDevices.emplace_back(advatek_devices);
	}

	//// OK let's check these monsters
	//for (auto& device : checkDevices) {
	//	boost::optional< JSON_TYPE & > child = device.get_child_optional("ProtVer");
	//	if (!child)
	//	{
	//		return "Not a valid JSON";
	//	}
	//}

	return "";
}

void advatek_manager::copyDevice(sAdvatekDevice* fromDevice, sAdvatekDevice* toDevice, bool initialise) {
	JSON_TYPE json_device;
	getJSON(fromDevice, json_device);

	std::stringstream jsonStringStream;
	//write_json(jsonStringStream, json_device);
	jsonStringStream << json_device;

	sImportOptions importOptions = sImportOptions();
	importOptions.json = jsonStringStream.str();
	importOptions.init = initialise;

	importJSON(toDevice, importOptions);
}

void advatek_manager::setEndUniverseChannel(uint16_t startUniverse, uint16_t startChannel, uint16_t pixelCount, uint16_t outputGrouping, uint16_t& endUniverse, uint16_t& endChannel) {
	pixelCount *= outputGrouping;
	uint16_t pixelChannels = (3 * pixelCount); // R-G-B data
	uint16_t pixelUniverses = ((float)(startChannel + pixelChannels) / 510.f);

	endUniverse = startUniverse + pixelUniverses;
	endChannel = (startChannel + pixelChannels - 1) % 510;
}

void advatek_manager::load_ipStr(std::string ipStr, uint8_t* address)
{
	int ip1, ip2, ip3, ip4;
	sscanf(ipStr.c_str(), "%i.%i.%i.%i", &ip1, &ip2, &ip3, &ip4);
	address[0] = ip1;
	address[1] = ip2;
	address[2] = ip3;
	address[3] = ip4;
}

void advatek_manager::load_macStr(std::string macStr, uint8_t* address)
{
	int mac1, mac2, mac3, mac4, mac5, mac6;
	sscanf(macStr.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &mac1, &mac2, &mac3, &mac4, &mac5, &mac6);
	address[0] = mac1;
	address[1] = mac2;
	address[2] = mac3;
	address[3] = mac4;
	address[4] = mac5;
	address[5] = mac6;
}

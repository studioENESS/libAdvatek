#include "defines.h"
#include "SAdvatekDevice.h"

sAdvatekDevice::~sAdvatekDevice() {
	if (Model) delete Model;
	if (Firmware) delete Firmware;
	if (OutputPixels) delete[] OutputPixels;
	if (OutputUniv) delete[] OutputUniv;
	if (OutputChan) delete[] OutputChan;
	if (OutputNull) delete[] OutputNull;
	if (OutputZig) delete[] OutputZig;
	if (OutputReverse) delete[] OutputReverse;
	if (OutputColOrder) delete[] OutputColOrder;
	if (OutputGrouping) delete[] OutputGrouping;
	if (OutputBrightness) delete[] OutputBrightness;
	if (DmxOutOn) delete[] DmxOutOn;
	if (TempDmxOutOn) delete[] TempDmxOutOn;
	if (DmxOutUniv) delete[] DmxOutUniv;
	if (DriverType) delete[] DriverType;
	if (DriverSpeed) delete[] DriverSpeed;
	if (DriverExpandable) delete[] DriverExpandable;
	if (DriverNames)
	{
		for (int i = 0; i < NumDrivers; i++)
			delete[] DriverNames[i];

		delete[] DriverNames;

	}
	if (VoltageBanks) delete[] VoltageBanks;
}


bool sAdvatekDevice::to_json(JSON_TYPE& j)
{

	j = JSON_TYPE();

	j["ProtVer"] = ProtVer;
	//j["HwRev"]=HwRev;
	j["CurrentProtVer"] = CurrentProtVer;
	j["Mac"] = macStr(Mac);
	j["DHCP"] = DHCP;
	j["StaticIP"] = ipStr(StaticIP);
	j["StaticSM"] = ipStr(StaticSM);
	j["CurrentIP"] = ipStr(CurrentIP);
	j["CurrentSM"] = ipStr(CurrentSM);

	j["ModelLength"] = ModelLength;
	j["Model"] = std::string((char*)Model);
	j["Brand"] = Brand;

	j["LenFirmware"] = LenFirmware;
	j["Firmware"] = std::string((char*)Firmware);

	j["Protocol"] = Protocol;
	j["HoldLastFrame"] = HoldLastFrame;
	j["SimpleConfig"] = SimpleConfig;
	j["MaxPixPerOutput"] = MaxPixPerOutput;
	j["NumOutputs"] = NumOutputs;



	for (int output = 0; output < NumOutputs; output++)
	{
		j["OutputPixels"][std::to_string(output)] = std::to_string(OutputPixels[output]);
		j["OutputUniv"][std::to_string(output)] = std::to_string(OutputUniv[output]);
		j["OutputChan"][std::to_string(output)] = std::to_string(OutputChan[output]);
		j["OutputNull"][std::to_string(output)] = std::to_string(OutputNull[output]);
		j["OutputZig"][std::to_string(output)] = std::to_string(OutputZig[output]);
		j["OutputReverse"][std::to_string(output)] = std::to_string(OutputReverse[output]);
		j["OutputColOrder"][std::to_string(output)] = std::to_string(OutputColOrder[output]);
		j["OutputGrouping"][std::to_string(output)] = std::to_string(OutputGrouping[output]);
		j["OutputBrightness"][std::to_string(output)] = std::to_string(OutputBrightness[output]);
	}

	j["NumDMXOutputs"] = NumDMXOutputs;
	j["ProtocolsOnDmxOut"] = ProtocolsOnDmxOut;

	for (int output = 0; output < NumDMXOutputs; output++)
	{
		j["DmxOutOn"][std::to_string(output)] = std::to_string(DmxOutOn[output]);
		j["DmxOutUniv"][std::to_string(output)] = std::to_string(DmxOutUniv[output]);
	}

	j["NumDrivers"] = NumDrivers;
	j["DriverNameLength"] = DriverNameLength;

	for (int output = 0; output < NumDrivers; output++)
	{
		j["DriverType"][std::to_string(output)] = std::to_string(DriverType[output]);
		j["DriverSpeed"][std::to_string(output)] = std::to_string(DriverSpeed[output]);
		j["DriverExpandable"][std::to_string(output)] = std::to_string(DriverExpandable[output]);
		j["DriverNames"][std::to_string(output)] = (DriverNames[output]);
	}

	j["CurrentDriver"] = CurrentDriver;
	j["CurrentDriverType"] = CurrentDriverType;
	j["CurrentDriverSpeed"] = CurrentDriverSpeed;
	j["CurrentDriverExpanded"] = CurrentDriverExpanded;

	for (int c = 0; c < 4; c++)
	{
		j["Gamma"][std::to_string(c)] = std::to_string(Gamma[c]);
	}
	

	//for (int c = 0; c < 3; c++)
	//{
	//	MinAssistantVer[std::to_string(c), std::to_string(MinAssistantVer[c]);
	//}
	//j["MinAssistantVer"]=MinAssistantVer;

	j["Nickname"] = Nickname;
	j["MaxTargetTemp"] = MaxTargetTemp;
	j["NumBanks"] = NumBanks;

	return true;
}

const std::stringstream& sAdvatekDevice::from_json(const JSON_TYPE& j, sImportOptions& importOptions)
{
	std::string s_hold;
	
	#define SetStringValueFromJson(type, atr) std::string sval; j.at(#atr).get_to(sval); atr = atoi(sval.c_str());
	#define SetIntValueFromJson(type, atr) j.at(#atr).get_to(atr);

	#define SetValueFromJson(type, atr) if(j.at(#atr).is_string()) {\
		SetStringValueFromJson(type,atr)}\
	 else {\
		SetIntValueFromJson(type,atr)};

	#define SetChildIntValuesFromJson(atr) \
		for (auto& node : j.at(#atr).items()) { std::string sval; node.value().get_to(sval);  atr[std::stoi(node.key())] = std::stoi(sval); }


	#define SetChildFloatValuesFromJson(atr) \
		for (auto& node : j.at(#atr).items()) { std::string sval; node.value().get_to(sval);  atr[std::stoi(node.key())] = std::stof(sval); }

	#define SetChildStringValuesFromJson(atr) \
		for (auto& node : j.at(#atr).items()) { std::string sval; node.value().get_to(sval);  atr[std::stoi(node.key())] = sval; }

	// TO DO - Import
	std::stringstream report;
	if (importOptions.init) {
		SetValueFromJson(uint8_t, ProtVer);
		SetValueFromJson(uint8_t, CurrentProtVer);
		SetValueFromJson(uint8_t, ModelLength);
		if (j.count("LenFirmware") != 0) {
			SetValueFromJson(uint8_t, LenFirmware);
		}
		else {
			LenFirmware = 20; // Current Protver
		}

		if (Model) delete[] Model;
		Model = new uint8_t[ModelLength + 1];
		memset(Model, 0x00, sizeof(uint8_t) * (ModelLength + 1));
		j["Model"].get_to(s_hold);
		memcpy(Model, s_hold.c_str(), sizeof(uint8_t) * ModelLength);
		
		j.at("Mac").get_to(s_hold);
		load_macStr(s_hold, Mac);
	}

	auto importModel = j["Model"].get<std::string>();

	if (importModel.compare(std::string((char*)Model)) == 0) {
		report << "Done!\n";
	}
	else {
		report << "Beware: Loaded data from ";
		report << j["Model"].get<std::string>();
		report << " to ";
		report << Model;
		report << "\n";
	}

	if (importOptions.network || importOptions.init) {

		SetValueFromJson(uint8_t, DHCP);

		s_hold = j["StaticIP"].get<std::string>();
		load_ipStr(s_hold, StaticIP);
		s_hold = j["StaticSM"].get<std::string>();
		load_ipStr(s_hold, StaticSM);

		report << "- Import Network Settings Succesfull.\n";
	}

	if (importOptions.ethernet_control || importOptions.init) {
		int tempNumOutputs=NumOutputs;

		SetValueFromJson(uint8_t, NumOutputs);		
		bool go = (tempNumOutputs == NumOutputs);
		NumOutputs = tempNumOutputs;
		if (go || importOptions.init) {
			SetValueFromJson(uint8_t, HoldLastFrame);
			SetValueFromJson(uint8_t, SimpleConfig);
			SetValueFromJson(uint16_t, MaxPixPerOutput);
			SetValueFromJson(uint8_t, NumOutputs);
			if (importOptions.init) {

				if (OutputPixels) delete[] OutputPixels;
				if (OutputUniv) delete[] OutputUniv;
				if (OutputChan) delete[] OutputChan;
				if (OutputNull) delete[] OutputNull;
				if (OutputZig) delete[] OutputZig;
				if (OutputReverse) delete[] OutputReverse;
				if (OutputColOrder) delete[] OutputColOrder;
				if (OutputGrouping) delete[] OutputGrouping;
				if (OutputBrightness) delete[] OutputBrightness;

				OutputPixels = new uint16_t[NumOutputs];
				OutputUniv = new uint16_t[NumOutputs];
				OutputChan = new uint16_t[NumOutputs];
				OutputNull = new uint8_t[NumOutputs];
				OutputZig = new uint16_t[NumOutputs];
				OutputReverse = new uint8_t[NumOutputs];
				OutputColOrder = new uint8_t[NumOutputs];
				OutputGrouping = new uint16_t[NumOutputs];
				OutputBrightness = new uint8_t[NumOutputs];
			}

			for (auto& node : j.at("OutputPixels").items()) {
				
				std::string sval; node.value().get_to(sval);
				OutputPixels[std::stoi(node.key())] = std::stoi(sval);
			}

			SetChildIntValuesFromJson(OutputPixels);
			SetChildIntValuesFromJson(OutputUniv);
			SetChildIntValuesFromJson(OutputChan);
			SetChildIntValuesFromJson(OutputNull);
			SetChildIntValuesFromJson(OutputZig);
			SetChildIntValuesFromJson(OutputReverse);
			SetChildIntValuesFromJson(OutputColOrder);
			SetChildIntValuesFromJson(OutputGrouping);
			SetChildIntValuesFromJson(OutputBrightness);
			report << "- Import Ethernet Control Succesfull.\n";
		}
		else {
			report << "- Import Ethernet Control Failed. (Output count does not match)\n";
		}
	}

	if (importOptions.dmx_outputs || importOptions.init) {
		SetValueFromJson(uint8_t, NumDMXOutputs);

		if (NumDMXOutputs == 0) { 
			report << "- Import DMX Control Failed. (No DMX outputs found)\n";
		}
		else {
			if (importOptions.init) {
				SetValueFromJson(uint8_t, NumDMXOutputs);

				if (DmxOutOn) delete[] DmxOutOn;
				if (DmxOutUniv) delete[] DmxOutUniv;
				if (TempDmxOutOn) delete[] TempDmxOutOn;

				DmxOutOn = new uint8_t[NumDMXOutputs];
				DmxOutUniv = new uint16_t[NumDMXOutputs];
				TempDmxOutOn = new bool[NumDMXOutputs];
			}
			SetValueFromJson(uint8_t, ProtocolsOnDmxOut);
			SetChildIntValuesFromJson(DmxOutOn);
			SetChildIntValuesFromJson(DmxOutUniv);
			report << "- Import DMX Control Succesfull.\n";
		}
	}

	if (importOptions.led_settings || importOptions.init) {
		int tempNumDrivers = NumDrivers;

		SetValueFromJson(uint8_t, NumDrivers);
		bool go = (tempNumDrivers == NumDrivers);
		NumDrivers = NumDrivers;
		if (go || importOptions.init) {
			SetValueFromJson(uint8_t, NumDrivers);

			if (DriverType) delete[] DriverType;
			if (DriverSpeed) delete[] DriverSpeed;
			if (DriverExpandable) delete[] DriverExpandable;
			if (DriverNames) {
				for (int i = 0; i < NumDrivers; i++) delete[] DriverNames[i];
				delete[] DriverNames;
			}
			if (Firmware) delete[] Firmware;

			DriverType = new uint8_t[NumDrivers];
			DriverSpeed = new uint8_t[NumDrivers];
			DriverExpandable = new uint8_t[NumDrivers];
			DriverNames = new char* [NumDrivers];
			Firmware = new uint8_t[LenFirmware];

			memset(Firmware, 0, sizeof(uint8_t) * LenFirmware);
			const char* tempFirmwareName = "Virtual";
			memcpy(Firmware, tempFirmwareName, 12);
			SetValueFromJson(uint8_t, DriverNameLength);
			for (int i = 0; i < NumDrivers; i++) {
				DriverNames[i] = new char[DriverNameLength + 1];
				memset(DriverNames[i], 0, sizeof(char) * (DriverNameLength + 1));
			}
			SetValueFromJson(uint8_t, DriverNameLength);
			for (auto& node : j["DriverNames"].items())
			{
				std::string sTempValue;
				node.value().get_to(sTempValue);
				int index = std::stoi(node.key());
				const char* sCStr = sTempValue.c_str();

				memcpy(DriverNames[index], sCStr, sizeof(char) * DriverNameLength);
			}
			SetChildIntValuesFromJson(DriverType);
			SetChildIntValuesFromJson(DriverSpeed);
			SetChildIntValuesFromJson(DriverExpandable);

			SetValueFromJson(int, CurrentDriver);
			SetValueFromJson(uint8_t, CurrentDriverType);
			SetValueFromJson(int, CurrentDriverSpeed);
			SetValueFromJson(uint8_t, CurrentDriverExpanded);
		}
		else {
			report << "- Import LED Settings Failed. (Driver count does not match)\n";
		}

		SetChildIntValuesFromJson(Gamma);
		Gammaf[0] = (float)Gamma[0] * 0.1;
		Gammaf[1] = (float)Gamma[1] * 0.1;
		Gammaf[2] = (float)Gamma[2] * 0.1;
		Gammaf[3] = (float)Gamma[3] * 0.1;

		report << "- Import LED Settings Succesfull.\n";
	}

	if (importOptions.nickname || importOptions.init) {
		//s_hold = json_device.get<std::string>("Nickname");
		j["Nickname"].get_to(s_hold);
		strncpy(Nickname, s_hold.c_str(), 40);
		report << "- Import Nickname Succesfull.\n";
	}

	if (importOptions.fan_on_temp) {
		SetValueFromJson(uint8_t, MaxTargetTemp);
		report << "- Import Fan On Temp Succesfull.\n";
	}
	
	return report;
}

inline void sAdvatekDevice::load_ipStr(std::string ipStr, uint8_t* address)
{
	int ip1, ip2, ip3, ip4;
	sscanf(ipStr.c_str(), "%i.%i.%i.%i", &ip1, &ip2, &ip3, &ip4);
	address[0] = ip1;
	address[1] = ip2;
	address[2] = ip3;
	address[3] = ip4;
}

inline void sAdvatekDevice::load_macStr(std::string macStr, uint8_t* address)
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


int sAdvatekDevice::MinUniverse() const
{
	int32_t iMin = INT32_MAX;
	for (int i = 0; i < this->NumOutputs; i++)
	{
		iMin = std::min((int)this->OutputUniv[i], iMin);
	}

	return iMin;
}

int sAdvatekDevice::MaxUniverse() const
{
	int iMax = 0;
	for (int i = 0; i < this->NumOutputs; i++)
	{
		iMax = std::max((int)this->OutputUniv[i], iMax);
	}

	return iMax;
}

#include "defines.h"
#include "SAdvatekDevice.h"

sAdvatekDevice::sAdvatekDevice()
{

}

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
	j["Model"] = std::to_string(*Model);
	j["Brand"] = Brand;

	j["LenFirmware"] = LenFirmware;
	j["Firmware"] = std::to_string(*Firmware);

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

bool sAdvatekDevice::from_json(const JSON_TYPE& j)
{
	return false;
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

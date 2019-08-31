#pragma warning (suppress : 4146 )
#define _CRT_SECURE_NO_WARNINGS

#include "main.h"

#include <iostream>
#include <unordered_map>
#include <Windows.h>

#include "DotaMessages/dota_commonmessages.pb.h"
#include "DotaMessages/dota_clientmessages.pb.h"
#include "DotaMessages/dota_usermessages.pb.h"

SendNetMessage_t SendNetMessage;

DWORD64 networksystem_dll, engine2_dll;

using DescriptorMap_t = std::unordered_map<WORD, DotaMessageDescriptor*>;
DescriptorMap_t MessageDescriptorMap;


// Probably changes upon connection
// May not be the same object Dota uses to send but this hasn't caused any issues
DWORD64 GetCNetChan()
{
	return *(DWORD64*)(*(DWORD64*)(engine2_dll + 0x57CAF0) + 0x90);
}

// category, flag, and vftable2 are the variables between different messages, needs more testing
// These values (6, 1, engine2_dll + 0x50C670) are for CNETMsg_StringCmd but I've actually not had issues sending
// the same skeleton but with a different classId/opcode for pause
DotaMessageDescriptor* CreateDescriptor(WORD classId, DWORD category = 6, BYTE flag = 1)
{
	DotaMessageDescriptor* descriptor = new DotaMessageDescriptor();
	memset(descriptor, 0x55, sizeof(*descriptor));
	descriptor->vftable = networksystem_dll + 0x1BD520;
	descriptor->q08 = (DWORD64)"xxxxx";
	descriptor->category = category;
	descriptor->d14 = 0;

	descriptor->vftable2 = engine2_dll + 0x50C670;
	descriptor->q20 = (DWORD64)"yyyyy";
	descriptor->classId = classId;
	descriptor->flag_2A = 2;
	descriptor->flag_2B = 1;

	return descriptor;
}

// Find an existing descriptor, or if one does not exist, create it.
DotaMessageDescriptor* GetDescriptor(WORD classId)
{
	DescriptorMap_t::iterator it = MessageDescriptorMap.find(classId);
	if (it == MessageDescriptorMap.end()) {
		// does not exist, create and insert descriptor
		it = MessageDescriptorMap.insert(std::pair<WORD, DotaMessageDescriptor*>(classId, CreateDescriptor(classId))).first;
	}
	return it->second;
}

void CreateConsole()
{
	AllocConsole();
	std::cout << std::hex;
	FILE* oldstdin = freopen("CON", "r", stdin);
	FILE* oldstdout = freopen("CON", "w", stdout);
}

void SetupStatics()
{
	networksystem_dll = (DWORD64)GetModuleHandleA("networksystem.dll");
	engine2_dll = (DWORD64)GetModuleHandleA("engine2.dll");
	SendNetMessage = (SendNetMessage_t)(networksystem_dll + 0x7B4B0);

	std::cout << "networksystem: " << networksystem_dll << std::endl;
	std::cout << "engine2: " << engine2_dll << std::endl;
	std::cout << "SendNetMessage: " << (DWORD64)SendNetMessage << std::endl;
}

int main()
{
	CreateConsole();

	std::cout << "Sending proto" << std::endl;

	CNETMsg_StringCmd test;
	test.set_command("say_team hi");
	SendNetMessage(GetCNetChan(), GetDescriptor(net_StringCmd), &test, -1);

	return 0;
}
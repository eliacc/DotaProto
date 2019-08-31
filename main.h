#pragma once

using SendNetMessage_t = void(__thiscall*)(DWORD64, void*, void*, int);

/*
DESCRIPTOR
+00 qword   vftable
+10 dword   msg category; normal=4?
---------------------------------------
+18 qword   vftable2
+28 word    class id
+2B byte    flag (0); if (f&0x40) unserializable
*/
// Species the type of a protobuf and provides secondary info such as whether it's serializable. 
// When Dota sends a proto this object is typically retrieved statically (i.e. mov rdx, [ptr_to_string_cmd_descriptor]) so reconstruction may be the only viable option.
#pragma pack(push, 1)
struct DotaMessageDescriptor {
	DWORD64 vftable;
	DWORD64 q08;
	DWORD category;
	DWORD d14;

	DWORD64 vftable2;
	DWORD64 q20;
	WORD classId;
	BYTE flag_2A;
	BYTE flag_2B;

	BYTE padding[20];
};
#pragma pack(pop)

int main();
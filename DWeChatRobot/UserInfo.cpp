#include "pch.h"
#include <typeinfo>
#include <string>
#include <vector>

#define GetUserInfoCall1Offset 0x645BD9A0 - 0x64530000
#define GetUserInfoCall2Offset 0x64C08420 - 0x64530000
#define GetUserInfoCall3Offset 0x64914260 - 0x64530000

struct GetUserInfoStruct {
	DWORD message;
	DWORD length;
};

wstring wUserInfo = L"";
GetUserInfoStruct ret = { 0 };

struct GetDetailUserInfoStruct {
	WxString* pWxString;
	DWORD ptr1 = 0;
	DWORD ptr2 = 0;
	char fill[0x18] = { 0 };
	GetDetailUserInfoStruct(WxString* pWxString) {
		this->pWxString = pWxString;
		ptr1 = DWORD(pWxString) + sizeof(WxString);
		ptr2 = DWORD(pWxString) + sizeof(WxString);
	}
};

struct UserInfoBaseStruct {
	DWORD data;
	DWORD endbuffer1;
	DWORD endbuffer2;
	char fill[0x18] = { 0 };
};


VOID WxUserInfo(DWORD address) {
	vector<DWORD> InfoType{
		address + 0x10,
		address + 0x24,
		address + 0x38,
		address + 0x6C,
		address + 0xFC,
		address + 0x110,
		address + 0x19C,
		address + 0x1B0,
		address + 0x1C4,
		address + 0x1D8,
		address + 0x27C
	};
	vector<wchar_t*> InfoTypeName{
		(WCHAR*)L"\"wxId\"",
		(WCHAR*)L"\"wxNumber\"",
		(WCHAR*)L"\"wxV3\"",
		(WCHAR*)L"\"wxNickName\"",
		(WCHAR*)L"\"wxBigAvatar\"",
		(WCHAR*)L"\"wxSmallAvatar\"",
		(WCHAR*)L"\"wxSignature\"",
		(WCHAR*)L"\"wxNation\"",
		(WCHAR*)L"\"wxProvince\"",
		(WCHAR*)L"\"wxCity\"",
		(WCHAR*)L"\"wxBackground\"",
	};
	wUserInfo += L"{";
	for (unsigned int i = 0; i < InfoType.size(); i++) {
		wchar_t* wstemp = ((*((DWORD*)InfoType[i])) != 0) ? (WCHAR*)(*((LPVOID*)InfoType[i])) : (WCHAR*)L"null";
		wUserInfo = wUserInfo + InfoTypeName[i] + L":\"" + wstemp + L"\"";
		if (i != InfoType.size() - 1) {
			wUserInfo += L",";
		}
	}
	wUserInfo += L"}";
#ifdef _DEBUG
	wcout.imbue(locale("chs"));
	wcout << wUserInfo.c_str() << endl;
#endif
}


DWORD GetWxUserInfoRemote(LPVOID lparamter) {
	wchar_t* userwxid = (wchar_t*)lparamter;
	DWORD address = 0;
	
	if (!GetUserDetailInfoByWxId(userwxid, address)) {
		return 0;
	}
	ret.message = (DWORD)wUserInfo.c_str();
	ret.length = (DWORD)wUserInfo.length();
	return (DWORD)&ret;
}


BOOL GetUserDetailInfoByWxId(wchar_t* wxid,DWORD &address) {
	DWORD WeChatWinBase = GetWeChatWinBase();
	DWORD GetUserDetailInfoCall1 = WeChatWinBase + 0x5F917490 - 0x5F230000;
	DWORD GetUserDetailInfoCall2 = WeChatWinBase + 0x5F2BD9A0 - 0x5F230000;
	DWORD GetUserDetailInfoCall3 = WeChatWinBase + 0x5F619F70 - 0x5F230000;

	DWORD DeleteCacheCall1 = WeChatWinBase + 0x56C349A0 - 0x56B80000;
	DWORD DeleteCacheCall2 = WeChatWinBase + 0x56D983B0 - 0x56B80000;
	WxString* pWxid = new WxString;
	pWxid->buffer = wxid;
	pWxid->length = wcslen(wxid);
	pWxid->maxLength = wcslen(wxid) * 2;

	UserInfoBaseStruct temp = { 0 };
	UserInfoBaseStruct userinfo = { 0 };
	GetDetailUserInfoStruct pUser(pWxid);
	DWORD isSuccess = 0;

	__asm {
		pushad;
		pushfd;
		mov eax, 0x7;
		lea ecx, pUser;
		lea edx, temp;
		call GetUserDetailInfoCall1;
		call GetUserDetailInfoCall2;
		lea ecx, userinfo;
		push ecx;
		lea ecx, temp;
		push ecx;
		mov ecx, eax;
		call GetUserDetailInfoCall3;
		mov isSuccess, eax;
		popfd;
		popad;
	}
	address = userinfo.data;
	if(isSuccess != 0)
		WxUserInfo(address);
	__asm {
		pushad;
		pushfd;
		lea ecx, temp;
		call DeleteCacheCall1;
		lea ecx, userinfo;
		call DeleteCacheCall2;
		popfd;
		popad;
	}
	delete pWxid;
	pWxid = NULL;
	return (isSuccess != 0);
}

BOOL GetWxUserInfoByWxid(wchar_t* wxid, DWORD& address) {
	DWORD WeChatWinBase = GetWeChatWinBase();
	DWORD WxUserDataCall1 = WeChatWinBase + GetUserInfoCall1Offset;
	DWORD WxUserDataCall2 = WeChatWinBase + GetUserInfoCall2Offset;
	DWORD WxUserDataCall3 = WeChatWinBase + GetUserInfoCall3Offset;
	char buffer[0xF90] = { 0 };
	WxBaseStruct pWxid(wxid);
	DWORD r_ebx = 0;
	DWORD isSuccess = 0;
	__asm
	{
		pushad;
		call WxUserDataCall1;
		lea ebx, buffer;
		mov esi, eax;
		push ebx;
		sub esp, 0x14;
		lea eax, pWxid;
		mov ecx, esp;
		push eax;
		call WxUserDataCall2;
		mov ecx, esi;
		call WxUserDataCall3;
		mov r_ebx, ebx;
		mov isSuccess, eax;
		popad;
	}
	address = r_ebx;
	return isSuccess;
}
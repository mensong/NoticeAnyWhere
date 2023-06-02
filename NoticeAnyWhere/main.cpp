#include "DirectOverlay.h"
#include "ini.h"
#include <iostream>

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

std::wstring g_msg;
std::wstring g_fontName = L"华文琥珀";
float g_fontSize = 48;
float g_x = 100;
float g_y = -1;//小于0时在半高位置
float g_r = 0;
float g_g = 0;
float g_b = 0;
float g_a = 0;
float g_opacity = 0.5;

int time1 = 0;
int time2 = 0;

void drawLoop(int width, int height) 
{
	int y = g_y;
	if (y < 0)
		y = (height / 2 - g_fontSize);

	DrawString(g_msg, g_fontSize, g_x, y, g_r, g_g, g_b, g_a, g_opacity);
}

HWND getHwndCallback()
{
	HWND foreWin = GetForegroundWindow();
	return foreWin;
}

std::wstring Ansi2Unicode(const char* szString)
{
	int dwLen = strlen(szString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);//算出合适的长度
	LPWSTR lpszPath = new WCHAR[dwLen];
	ZeroMemory(lpszPath, dwLen * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
	std::wstring sRet = lpszPath;
	delete[] lpszPath;
	return sRet;
}

void main(int argc, char** argv)
{
	std::string s = argv[0];
	size_t idx = s.find_last_of('\\') + 1;
	s = s.substr(idx, s.find_last_of('.') - idx);
	mINI::INIFile file(s + ".ini");
	mINI::INIStructure ini;
	if (!file.read(ini))
	{
		std::cout << "Read " << s + ".ini error." << std::endl;
		return;
	}

	auto cfgShow = ini["SHOW"];
	g_msg = Ansi2Unicode(cfgShow["message"].c_str());
	g_fontName = Ansi2Unicode(cfgShow["font_name"].c_str());
	g_fontSize = atof(cfgShow["font_size"].c_str());
	g_x = atof(cfgShow["x"].c_str());
	g_y = atof(cfgShow["y"].c_str());
	g_r = atof(cfgShow["color_r"].c_str());
	g_g = atof(cfgShow["color_g"].c_str());
	g_b = atof(cfgShow["color_b"].c_str());
	g_a = atof(cfgShow["color_a"].c_str());
	g_opacity = atof(cfgShow["opacity"].c_str());

	auto cfgTime = ini["TIME"];
	int hour1 = atoi(cfgTime["hour1"].c_str());
	int minute1 = atoi(cfgTime["minute1"].c_str());
	int hour2 = atoi(cfgTime["hour2"].c_str());
	int minute2 = atoi(cfgTime["minute2"].c_str());
	time1 = hour1 * 100 + minute1;
	time2 = hour2 * 100 + minute2;

	//DirectOverlaySetOption(D2DOV_DRAW_FPS);
	DirectOverlaySetFontName(g_fontName);
	DirectOverlaySetup(drawLoop, getHwndCallback);
	while (IsDirectOverlayRunning())
	{
		SYSTEMTIME t;
		ZeroMemory(&t, sizeof(t));
		::GetLocalTime(&t);
		int it = t.wHour * 100 + t.wMinute;
		if ((time1 == 0 && time2 == 0) || it >= time1 && it <= time2)
			DirectOverlayEnable(TRUE);
		else
			DirectOverlayEnable(FALSE);

		Sleep(1000);
	}
}
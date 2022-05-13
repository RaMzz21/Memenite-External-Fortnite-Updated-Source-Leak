
#include "files.h"
#include <iostream>
#include "GL/glew.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_dx8.cpp"
#include <GLFW/glfw3.h>
#include <vector>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <intrin.h>
//#include "driver.h"
#include "Controller.h"
#include <d3d9.h>
#include "d3dx9.h"
#include <thread>

#include <string>
#include <stdio.h>
#include <fstream>
#include "font.h"
#include "utils.hpp"
//#include "string.hpp"
#include "Lazyimporter.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

extern void aimbot(float x, float y);
GLFWwindow* g_window;

static void glfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, XorStr("OverEW: %d: %s\n").c_str(), error, description);
}

void setupWindow() 
{
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit()) {
		std::cout << XorStr("Over Init Failed!\n").c_str();
		return;
	}

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	if (!monitor) {
		fprintf(stderr, XorStr("Failed to get monitor!\n").c_str());
		return;
	}

	g_width = glfwGetVideoMode(monitor)->width;
	g_height = glfwGetVideoMode(monitor)->height;

	glfwWindowHint(GLFW_FLOATING, true);
	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_MAXIMIZED, false);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

	g_window = glfwCreateWindow(g_width + 1, g_height + 2, XorStr("بسماللهالرحالرحي").c_str(), NULL, NULL);

	if (g_window == NULL) {
		std::cout << XorStr("Over: 0x2!\n").c_str();
		return;
	}

	glfwSetWindowAttrib(g_window, GLFW_DECORATED, false);
	glfwSetWindowAttrib(g_window, GLFW_MOUSE_PASSTHROUGH, true);

	glfwSetWindowMonitor(g_window, NULL, 0, 0, g_width, g_height + 1, 0);

	glfwMakeContextCurrent(g_window);
	glfwSwapInterval(1); 

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, XorStr("Over: 0x1!\n").c_str());
		return;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.IniFilename = NULL;
	io.Fonts->AddFontFromMemoryCompressedTTF(Test_compressed_data, Test_compressed_size, 13.f);

	menucolors();

	ImGui_ImplGlfw_InitForOpenGL(g_window, true);
	ImGui_ImplOpenGL3_Init(XorStr("#version 130").c_str());
}

void cleanupWindow() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(g_window);
	glfwTerminate();
}

BOOL CALLBACK retreiveFortniteWindow(HWND hwnd, LPARAM lparam) {
	DWORD process_id;
	LI_FN(GetWindowThreadProcessId).safe()(hwnd, &process_id);
	if (process_id == g_pid) {
		fortnite_wnd = hwnd;
	}
	return TRUE;
}

bool isFortniteFocused = false;

void activateFortniteWindow() {
	SetForegroundWindow(fortnite_wnd);
}

void handleKeyPresses() 
{
	if (LI_FN(GetAsyncKeyState).safe_cached()(VK_INSERT) & 1) {
		g_overlay_visible = !g_overlay_visible;
		glfwSetWindowAttrib(g_window, GLFW_MOUSE_PASSTHROUGH, !g_overlay_visible);
		if (g_overlay_visible) {
			HWND overlay_window = glfwGetWin32Window(g_window);
			SetForegroundWindow(overlay_window);
		}
		else {
			activateFortniteWindow();
		}
	}
}

void drawlootloop()
{
	if (Globals::LocalPawn)
	{
		std::vector<LootEntity> tmpList;
		uintptr_t ItemLevels = read<uintptr_t>( GWorld + 0x148);

		for (int i = 0; i < read<DWORD>( GWorld + (0x148 + sizeof(PVOID))); ++i) {

			uintptr_t ItemLevel = read<uintptr_t>( ItemLevels + (i * sizeof(uintptr_t)));

			for (int i = 0; i < read<DWORD>( ItemLevel + (0x98 + sizeof(PVOID))); ++i) {


				uintptr_t ItemsPawns = read<uintptr_t>( ItemLevel + 0x98);
				uintptr_t CurrentItemPawn = read<uintptr_t>( ItemsPawns + (i * sizeof(uintptr_t)));

				uintptr_t ItemRootComponent = read<uintptr_t>( CurrentItemPawn + 0x188);
				Vector3 ItemPosition = read<Vector3>( ItemRootComponent + 0x128);
				float ItemDist = Globals::LocalPlayerRelativeLocation.Distance(ItemPosition) / 100.f;


				if (ItemDist < bE5pD1st4nce) {

					int currentitemid = read<int>( CurrentItemPawn + 0x18);
					auto CurrentItemPawnName = GetNameFromFName(currentitemid);

					if ((g_loot && strstr(CurrentItemPawnName.c_str(), (XorStr("FortPickupAthena")).c_str())) || strstr(CurrentItemPawnName.c_str(), (XorStr("Tiered_Chest").c_str())) || 
						(g_vehicles && strstr(CurrentItemPawnName.c_str(), XorStr("Vehicl").c_str()) || strstr(CurrentItemPawnName.c_str(), XorStr("Valet_Taxi").c_str()) || 
						strstr(CurrentItemPawnName.c_str(), XorStr("Valet_BigRig").c_str()) || strstr(CurrentItemPawnName.c_str(), XorStr("Valet_BasicTr").c_str()) || 
						strstr(CurrentItemPawnName.c_str(), XorStr("Valet_SportsC").c_str()) || strstr(CurrentItemPawnName.c_str(), XorStr("Valet_BasicC").c_str()))) 
					{
						LootEntity fnlEntity{ };
						fnlEntity.CurrentActor = CurrentItemPawn;
						fnlEntity.name = CurrentItemPawnName;
						tmpList.push_back(fnlEntity);

					}
				}

			}
		}
		LootentityList.clear();
		LootentityList = tmpList;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void CacheNew()
{
	while (true)
	{
		std::vector<Playertest> tmpList;

		GWorld = read<uint64_t>(g_base_address + 0xc640158);

		uintptr_t PersistentLevel = read<uintptr_t>( GWorld + 0x30);

		uintptr_t GameInstance = read<uintptr_t>( GWorld + 0x190);

		uintptr_t LocalPlayers = read<uintptr_t>( GameInstance + 0x38);

		Globals::LocalPlayer = read<uintptr_t>( LocalPlayers);

		LocalPlayerController = read<uintptr_t>( Globals::LocalPlayer + 0x30);

		PlayerCameraManager = read<uint64_t>( LocalPlayerController + 0x328);

		Globals::LocalPawn = read<uintptr_t>( LocalPlayerController + 0x310);

		Globals::LocalPawnRootComponent = read<uintptr_t>( Globals::LocalPawn + 0x188);

		uint64_t localplayerstate = read<uint64_t>( Globals::LocalPawn + 0x290);

		LocalTeam = read<int>( localplayerstate + 0x1010);

		InLobby = false;
		if (!Globals::LocalPawn) InLobby = true;

		auto ActorCount = read<DWORD>( PersistentLevel + 0xA0);

		auto AActors2 = read<uintptr_t>( PersistentLevel + 0x98);


		for (int i = 0; i < ActorCount; ++i) {


			uintptr_t CurrentItemPawn = read<uintptr_t>( AActors2 + (i * sizeof(uintptr_t)));

			int CurrentItemId = read<int>( CurrentItemPawn + 0x18);

			auto CurrentItemPawnName = GetNameFromFName(CurrentItemId);

			if (strstr(CurrentItemPawnName.c_str(), XorStr("PlayerPawn_Athena_C").c_str()) || strstr(CurrentItemPawnName.c_str(), XorStr("PlayerPawn").c_str()))
			{
				Playertest Actor{ };

				Actor.Acotr = CurrentItemPawn;
				Actor.Acotrmesh = read<uint64_t>( CurrentItemPawn + 0x2F0);
				Actor.name = CurrentItemPawnName;
				Actor.rootcomp = Globals::LocalPawnRootComponent;

				tmpList.push_back(Actor);
			}
		}

		drawlootloop();
		PLIST.clear();
		PLIST = tmpList;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

bool isVisible(DWORD_PTR mesh)
{
	if (!mesh)
		return false;
	float fLastSubmitTime = read<float>( mesh + 0x330);
	float fLastRenderTimeOnScreen = read<float>( mesh + 0x338);

	const float fVisionTick = 0.06f;
	bool bVisible = fLastRenderTimeOnScreen + fVisionTick >= fLastSubmitTime;
	return bVisible;
}


bool IsVec3Valid(Vector3 vec3)
{
	return !(vec3.x == 0 && vec3.y == 0 && vec3.z == 0);
}

void WriteAngles(Vector3 Location)
{
	auto camloc = read<Vector3>( Globals::LocalPawnRootComponent + 0x128);

	static auto Pdefaultxmin = read<float>( PlayerCameraManager + 0x3174);
	static auto Pdefaultxmax = read<float>( PlayerCameraManager + 0x3178);
	static auto Ydefaultxmin = read<float>( PlayerCameraManager + 0x317C);
	static auto Ydefaultxmax = read<float>( PlayerCameraManager + 0x3180);

	Vector3 angles = calc_angle(camloc, Location);

	write<float>( PlayerCameraManager + 0x3174, angles.x + 1.f);
	write<float>( PlayerCameraManager + 0x3178, angles.x + 1.f);
	write<float>( PlayerCameraManager + 0x317C, angles.y - 2.f);
	write<float>( PlayerCameraManager + 0x3180, angles.y - 2.f);
	Sleep(5);
	write<float>( PlayerCameraManager + 0x3174, Pdefaultxmin);
	write<float>( PlayerCameraManager + 0x3178, Pdefaultxmax);
	write<float>( PlayerCameraManager + 0x317C, Ydefaultxmin);
	write<float>( PlayerCameraManager + 0x3180, Ydefaultxmax);
}

bool actorLoop() 
{
	bool bValidEnemyInArea = false;
	float ClosestActorDistance = FLT_MAX;
	Vector3 ClosestActorMouseAimbotPosition = Vector3(0.0f, 0.0f, 0.0f);
	float distance, distaim;

	try
	{
		for (LootEntity entity : LootentityList) {

			if (Globals::LocalPawn)
			{
				uintptr_t ItemRootComponent = read<uintptr_t>( entity.CurrentActor + 0x188);
				Vector3 ItemPosition = read<Vector3>( ItemRootComponent + 0x128);
				float ItemDist = Globals::LocalPlayerRelativeLocation.Distance(ItemPosition) / 100.f;
				std::string null = XorStr("").c_str();

				

				if (strstr(entity.name.c_str(), XorStr("Tiered_Chest").c_str()) && g_chests && CheckInScreen(entity.CurrentActor, Globals::Width, Globals::Height))
				{
					if (ItemDist < bE5pD1st4nce) {
						Vector3 ChestPosition;
						ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);
						std::string Text = null + XorStr("Chest").c_str();
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.yellow, true, true, Text.c_str());
					}
				}
				else if ((g_vehicles && (strstr(entity.name.c_str(), XorStr("Vehicl").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_Taxi").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_BigRig").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_BasicTr").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_SportsC").c_str()) || strstr(entity.name.c_str(), XorStr("Valet_BasicC").c_str()))))
				{
					if (ItemDist < bE5pD1st4nce && CheckInScreen(entity.CurrentActor, Globals::Width, Globals::Height)) {
						Vector3 VehiclePosition = g_functions::ConvertWorld2Screen(ItemPosition);
						std::string Text = null + XorStr("Vehicle [").c_str() + std::to_string((int)ItemDist) + XorStr("m]").c_str();
						DrawString(14, VehiclePosition.x, VehiclePosition.y, &Col.red, true, true, Text.c_str());
					}
				}
				else if (strstr(entity.name.c_str(), XorStr("AthenaSupplyDrop_C").c_str()) && g_loot && CheckInScreen(entity.CurrentActor, Globals::Width, Globals::Height))
				{
					if (ItemDist < bE5pD1st4nce) {
						Vector3 ChestPosition;
						ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);

						std::string Text = null + XorStr("Supply Drop [").c_str() + std::to_string((int)ItemDist) + XorStr("m]").c_str();
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.blue, true, true, Text.c_str());

					}
				}
				else if (strstr(entity.name.c_str(), XorStr("Tiered_Ammo").c_str()) && g_ammo && CheckInScreen(entity.CurrentActor, Globals::Width, Globals::Height))
				{
					if (ItemDist < bE5pD1st4nce) {
						Vector3 ChestPosition;
						ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);
						std::string Text = null + XorStr("Ammo Box [").c_str() + std::to_string((int)ItemDist) + XorStr("m]").c_str();
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.white, true, true, Text.c_str());

					}
				}

				else if (g_loot && strstr(entity.name.c_str(), XorStr("FortPickupAthena").c_str()) && CheckInScreen(entity.CurrentActor, Globals::Width, Globals::Height))
				{
					BYTE tier;
					if (ItemDist < bE5pD1st4nce) {

						auto definition = read<uint64_t>( entity.CurrentActor + 0x308 + 0x18);

						tier = read<BYTE>( definition + 0x74);

						RGBA Color;
						RGBA RGBAColor;

						Vector3 ChestPosition;
						ChestPosition = g_functions::ConvertWorld2Screen(ItemPosition);

						if (g_loot)
						{
							

							
							std::string wtf2 = XorStr(" [Loot]").c_str();
							if (tier == 2)
							{
								Color = Col.green;
							}
							else if ((tier == 3))
							{
								Color = Col.blue;
							}
							else if ((tier == 4))
							{
								Color = Col.purple;
							}
							else if ((tier == 5))
							{
								Color = Col.yellow;
							}
							else if ((tier == 6))
							{
								Color = Col.yellow;
							}
							else if ((tier == 0) || (tier == 1))
							{
								Color = Col.white;
							}

							DrawString(14, ChestPosition.x, ChestPosition.y, &Color, true, true, wtf2.c_str());
						}
					}
				}
			}
		}

		for (Playertest p : PLIST)
		{
			auto identify = g_functions::f_getbonewithIndex(0, 0);
			g_functions::ConvertWorld2Screen(identify);

			uintptr_t RootComponent = read<uintptr_t>( p.Acotr + 0x188);
			Globals::LocalPlayerRelativeLocation = read<Vector3>( p.rootcomp + 0x128);

			if (Globals::LocalPawn)
			{
				const auto SetupCameraRotationAndFov = [](uintptr_t LocalPlayer, uintptr_t RootComponent, Vector3& CameraRotation, float& FOVAngle)
				{
					auto CameraRotChain_tmp0 = read<uint64_t>( LocalPlayer + 0xC8);
					auto CameraRotChain_tmp1 = read<uint64_t>( CameraRotChain_tmp0 + 0x8);

					camera::m_CameraRotation.x = read<double>( CameraRotChain_tmp1 + 0xAE0);

					double tester = asin(camera::m_CameraRotation.x);
					double degreees = tester * (180.0 / M_PI);

					camera::m_CameraRotation.y = read<double>( RootComponent + 0x148);


					camera::m_CameraRotation.x = degreees;

					//set fov angle
					if (g_fovchanger && Globals::LocalPawn)
					{
						FOVAngle = FOVChangerValue;
					}
					else
					{
						FOVAngle = 80.0f / (read<double>( CameraRotChain_tmp1 + 0x610) / 1.19f);
					}

				};
				SetupCameraRotationAndFov(Globals::LocalPlayer, Globals::LocalPawnRootComponent, camera::m_CameraRotation, camera::m_FovAngle);
			}
			else {
				// PlayerCameraManager -> LastFrameCameraCachePrivate -> POV -> Rotation && FOV
				camera::m_CameraRotation = read<Vector3>( PlayerCameraManager + 0x28D0 + 0x10 + 0x18);
				camera::m_CameraRotation.z = 0;

				if (g_fovchanger && Globals::LocalPawn)
				{
					camera::m_FovAngle = FOVChangerValue;
				}
				else
				{
					camera::m_FovAngle = read<float>( PlayerCameraManager + 0x28D0 + 0x10 + 0x30);
				}
			}

			// get camera location
			const auto SetupCameraLocation = [](uintptr_t LocalPlayer, Vector3& CameraLocation)
			{
				auto CameraLocChain_tmp0 = read<uint64_t>( LocalPlayer + 0x70);
				auto CameraLocChain_tmp1 = read<uint64_t>( CameraLocChain_tmp0 + 0x98);
				auto CameraLocChain_tmp2 = read<uint64_t>( CameraLocChain_tmp1 + 0x180);

				CameraLocation = read<Vector3>( CameraLocChain_tmp2 + 0x20);

			};
			SetupCameraLocation(Globals::LocalPlayer, camera::m_CameraLocation);


			uint64_t playerstate = read<uint64_t>( p.Acotr + 0x290);

			int TeamIndex = read<int>( playerstate + 0x1010);

			if (g_fovchanger && Globals::LocalPawn)
			{
				auto fov = PlayerCameraManager + 0x288;
				write( fov, FOVChangerValue);
			}

			bool invisi = false;

			if (g_invisible)
			{
				write<Vector3>(p.Acotrmesh + 0x158, Vector3(0, 0, 0));
				invisi = true;
			}
			else if(invisi == true)
			{
				write<Vector3>(p.Acotrmesh + 0x158, Vector3(1, 1, 1));
				invisi = false;
			}

			if (rapidfirri) {
				float a = 0;
				float b = 0;
				uintptr_t CurrentWeapon = read<uintptr_t>(Globals::LocalPawn + 0x790); //CurrentWeapon Offset
				if (CurrentWeapon) {
					a = read<float>(CurrentWeapon + 0xa60); //LastFireTime Offset
					b = read<float>(CurrentWeapon + 0xa64); //LastFireTimeVerified Offset
					write<float>(CurrentWeapon + 0xa60, a + b - 0.0003); //LastFireTime Offset
				}
			}

			Vector3 vHeadBone = g_functions::f_getbonewithIndex(p.Acotrmesh, 98);
			Vector3 vRootBone = g_functions::f_getbonewithIndex(p.Acotrmesh, 0);

			Vector3 vHeadBoneOut = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z + 15));
			Vector3 vRootBoneOut = g_functions::ConvertWorld2Screen(vRootBone);

			Vector3 w2shead = g_functions::ConvertWorld2Screen(vHeadBone);

			Vector3 RootPos = g_functions::f_getbonewithIndex(p.Acotrmesh, 68);
			Vector3 RootPosOut = g_functions::ConvertWorld2Screen(RootPos);

			float BoxHeight = abs(vHeadBoneOut.y - vRootBoneOut.y);
			float BoxWidth = BoxHeight * 0.80;

			Vector3 RelativeInternalLocation = read<Vector3>( RootComponent + 0x128);
			if (!RelativeInternalLocation.x && !RelativeInternalLocation.y) {
				continue;
			}

			Vector3 RelativeScreenLocation = g_functions::ConvertWorld2Screen(RelativeInternalLocation);
			if (!RelativeScreenLocation.x && !RelativeScreenLocation.y) {
				continue;
			}

			distance = Globals::LocalPlayerRelativeLocation.Distance(RelativeInternalLocation) / 100.f;

			Vector3 neck2 = g_functions::f_getbonewithIndex(p.Acotrmesh, 98);
			Vector3 vneck2 = g_functions::ConvertWorld2Screen(neck2);
			Vector3 pelvis = g_functions::f_getbonewithIndex(p.Acotrmesh, 2);
			Vector3 vpelvis = g_functions::ConvertWorld2Screen(pelvis);
			Vector3 bottom1 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y - 10, vHeadBone.z));
			Vector3 bottom2 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y - 10, vHeadBone.z));
			Vector3 bottom3 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y + 10, vHeadBone.z));
			Vector3 bottom4 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y + 10, vHeadBone.z));

			Vector3 top1 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y - 10, vHeadBone.z + 15));
			Vector3 top2 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y - 10, vHeadBone.z + 15));
			Vector3 top3 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 10, vHeadBone.y + 10, vHeadBone.z + 15));
			Vector3 top4 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 10, vHeadBone.y + 10, vHeadBone.z + 15));

			ImU32 Cay = ImGui::GetColorU32({ 255, 215, 0, 255 });
			RGBA Skellll;
			Skellll = { 255, 255, 255, 255 };
			

			if (g_curweaponesp && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
			{
				uintptr_t ItemRootComponent = read<uintptr_t>( p.Acotr + 0x188);
				Vector3 ItemPosition = read<Vector3>( ItemRootComponent + 0x128);
				float ItemDist = Globals::LocalPlayerRelativeLocation.Distance(ItemPosition) / 100.f;

				if (ItemDist < bE5pD1st4nce) {

					auto CurrentWeapon = read<uintptr_t>( p.Acotr + 0x790);//AFortPawn	CurrentWeapon	0x7b0	AFortWeapon*
					auto ItemData = read<DWORD_PTR>( CurrentWeapon + 0x3D8);

					BYTE tier;
					tier = read<BYTE>( ItemData + 0x74);
					ImColor Color;
					if (tier == 2)
					{
						Color = IM_COL32(0, 255, 0, 255);
					}
					else if ((tier == 3))
					{
						Color = IM_COL32(0, 0, 255, 255);
					}
					else if ((tier == 4))
					{
						Color = IM_COL32(128, 0, 128, 255); // p
					}
					else if ((tier == 5))
					{
						Color = IM_COL32(255, 255, 0, 255); // p
					}
					else if ((tier == 6))
					{
						Color = IM_COL32(255, 255, 0, 255); // p
					}
					else if ((tier == 0) || (tier == 1))
					{
						Color = IM_COL32(255, 255, 255, 255); // p
					}

					auto AmmoCount = read<int>( CurrentWeapon + 0xB14);//AFortWeapon	AmmoCount	0xb3c	int32_t


					auto bIsReloadingWeapon = read<bool>( CurrentWeapon + 0x311);

					auto DisplayName = read<uint64_t>( ItemData + 0x90); // DisplayName (FText)
					auto WeaponLength = read<uint32_t>( DisplayName + 0x38); // FText -> Length
					wchar_t* WeaponName = new wchar_t[uint64_t(WeaponLength) + 1];

					readwtf( (ULONG64)read<PVOID>( DisplayName + 0x30), WeaponName, WeaponLength * sizeof(wchar_t));

					std::string Text = wchar_to_char(WeaponName);


					if (strstr(p.name.c_str(), XorStr("PlayerPawn_Athena_C").c_str()))
					{
						std::string Player = XorStr("Player").c_str();
						ImVec2 TextSize = ImGui::CalcTextSize(Player.c_str());
						ImGui::GetOverlayDrawList()->AddText(ImVec2(vHeadBoneOut.x - 30 - TextSize.x / 2, vHeadBoneOut.y - 15 - TextSize.y / 2), ImGui::GetColorU32({ 255, 255, 255, 255 }), Player.c_str());
					}
					else
					{
						std::string Bot = XorStr("Bot").c_str();
						ImVec2 TextSize = ImGui::CalcTextSize(Bot.c_str());
						ImGui::GetOverlayDrawList()->AddText(ImVec2(vHeadBoneOut.x - 30 - TextSize.x / 2, vHeadBoneOut.y - 15 - TextSize.y / 2), ImGui::GetColorU32({ 255, 255, 255, 255 }), Bot.c_str());
					}

						if (AmmoCount)
						{
							char buffer[128];
							sprintf_s(buffer, XorStr("Ammo: %i").c_str(), AmmoCount);
							if (buffer != XorStr("?").c_str())
							{
								ImGui::GetOverlayDrawList()->AddText(ImVec2(vpelvis.x, vpelvis.y + 15), IM_COL32(255, 255, 255, 255), buffer);
							}
						}

						if (bIsReloadingWeapon)
						{
							ImGui::GetOverlayDrawList()->AddText(ImVec2(vpelvis.x - 30, vpelvis.y), IM_COL32(255, 255, 255, 255), XorStr("Reloading").c_str());
						}
						else
						{
							ImGui::GetOverlayDrawList()->AddText(ImVec2(vpelvis.x - 30, vpelvis.y), Color, Text.c_str());
						}

				}
			}
			int Teamcheck;

			if (g_chams)
			{
				Teamcheck = TeamIndex = LocalTeam;
			}
			else
			{
				Teamcheck = TeamIndex != LocalTeam; 
			}

			if (Teamcheck || InLobby) 
			{
				isVis = isVisible(p.Acotrmesh);
				if (distance <= bE5pD1st4nce || InLobby) {
					if (g_boxesp && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
					{
						if (isVis)
						{
							DrawNormalBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, 1.0f, &ESPColor2, &ESPColor2);
						}
						else
						{
							DrawNormalBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, 1.0f, &ESPColor, &ESPColor);
						}
					}
					else if (g_cornerboxesp && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
					{
						if (isVis)
						{
							DrawCorneredBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, IM_COL32(93, 255, 0, 255), 1.5);
						}
						else
						{
							DrawCorneredBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, IM_COL32(255, 0, 216, 255), 1.5);
						}
					}
					if (g_esp_distance && CheckInScreen(p.Acotr, Globals::Width, Globals::Height)) {

						char dist[64];
						sprintf_s(dist, XorStr("%.f m").c_str(), distance);

						ImVec2 TextSize = ImGui::CalcTextSize(dist);
						ImGui::GetOverlayDrawList()->AddText(ImVec2(vRootBoneOut.x - 15 - TextSize.x / 2, vRootBoneOut.y - 15 - TextSize.y / 2), ImGui::GetColorU32({ 255, 255, 255, 255 }), dist);
					}
					if (g_esp_skeleton && CheckInScreen(p.Acotr, Globals::Width, Globals::Height)) {

						Vector3 neck2 = g_functions::f_getbonewithIndex(p.Acotrmesh, 98);
						Vector3 vneck2 = g_functions::ConvertWorld2Screen(neck2);

						Vector3 neck = g_functions::f_getbonewithIndex(p.Acotrmesh, 66);
						Vector3 vneck = g_functions::ConvertWorld2Screen(neck);
						Vector3 rightChest = g_functions::f_getbonewithIndex(p.Acotrmesh, 8);
						Vector3 vrightChest = g_functions::ConvertWorld2Screen(rightChest);
						Vector3 leftChest = g_functions::f_getbonewithIndex(p.Acotrmesh, 37);
						Vector3 vleftChest = g_functions::ConvertWorld2Screen(leftChest);
						Vector3 leftShoulder = g_functions::f_getbonewithIndex(p.Acotrmesh, 38);
						Vector3 vleftShoulder = g_functions::ConvertWorld2Screen(leftShoulder);
						Vector3 rightShoulder = g_functions::f_getbonewithIndex(p.Acotrmesh, 9);
						Vector3 vrightShoulder = g_functions::ConvertWorld2Screen(rightShoulder);
						Vector3 leftElbow = g_functions::f_getbonewithIndex(p.Acotrmesh, 94);
						Vector3 vleftElbow = g_functions::ConvertWorld2Screen(leftElbow);
						Vector3 rightElbow = g_functions::f_getbonewithIndex(p.Acotrmesh, 10);
						Vector3 vrightElbow = g_functions::ConvertWorld2Screen(rightElbow);
						Vector3 leftWrist = g_functions::f_getbonewithIndex(p.Acotrmesh, 62);
						Vector3 vleftWrist = g_functions::ConvertWorld2Screen(leftWrist);
						Vector3 rightWrist = g_functions::f_getbonewithIndex(p.Acotrmesh, 33);
						Vector3 vrightWrist = g_functions::ConvertWorld2Screen(rightWrist);
						Vector3 pelvis = g_functions::f_getbonewithIndex(p.Acotrmesh, 2);
						Vector3 vpelvis = g_functions::ConvertWorld2Screen(pelvis);
						Vector3 leftAss = g_functions::f_getbonewithIndex(p.Acotrmesh, 76);
						Vector3 vleftAss = g_functions::ConvertWorld2Screen(leftAss);
						Vector3 rightAss = g_functions::f_getbonewithIndex(p.Acotrmesh, 69);
						Vector3 vrightAss = g_functions::ConvertWorld2Screen(rightAss);
						Vector3 leftKnee = g_functions::f_getbonewithIndex(p.Acotrmesh, 77);
						Vector3 vleftKnee = g_functions::ConvertWorld2Screen(leftKnee);
						Vector3 rightKnee = g_functions::f_getbonewithIndex(p.Acotrmesh, 70);
						Vector3 vrightKnee = g_functions::ConvertWorld2Screen(rightKnee);
						Vector3 leftAnkle = g_functions::f_getbonewithIndex(p.Acotrmesh, 78);
						Vector3 vleftAnkle = g_functions::ConvertWorld2Screen(leftAnkle);
						Vector3 rightAnkle = g_functions::f_getbonewithIndex(p.Acotrmesh, 71);
						Vector3 vrightAnkle = g_functions::ConvertWorld2Screen(rightAnkle);

						RGBA ESPSkeleton;

						if (isVis)
						{
							ESPSkeleton = { 93, 255, 0, 255 };
						}
						else
						{
							ESPSkeleton = { 255, 0, 216, 255 };
						}

						DrawLine(vleftChest.x, vleftChest.y, vrightChest.x, vrightChest.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftChest.x, vleftChest.y, vleftShoulder.x, vleftShoulder.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightChest.x, vrightChest.y, vrightShoulder.x, vrightShoulder.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftShoulder.x, vleftShoulder.y, vleftElbow.x, vleftElbow.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightShoulder.x, vrightShoulder.y, vrightElbow.x, vrightElbow.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftElbow.x, vleftElbow.y, vleftWrist.x, vleftWrist.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightElbow.x, vrightElbow.y, vrightWrist.x, vrightWrist.y, &ESPSkeleton, 0.5f);
						DrawLine(vneck2.x, vneck2.y, vpelvis.x, vpelvis.y, &ESPSkeleton, 0.5f);
						DrawLine(vpelvis.x, vpelvis.y, vleftAss.x, vleftAss.y, &ESPSkeleton, 0.5f);
						DrawLine(vpelvis.x, vpelvis.y, vrightAss.x, vrightAss.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftAss.x, vleftAss.y, vleftKnee.x, vleftKnee.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightAss.x, vrightAss.y, vrightKnee.x, vrightKnee.y, &ESPSkeleton, 0.5f);
						DrawLine(vleftKnee.x, vleftKnee.y, vleftAnkle.x, vleftAnkle.y, &ESPSkeleton, 0.5f);
						DrawLine(vrightKnee.x, vrightKnee.y, vrightAnkle.x, vrightAnkle.y, &ESPSkeleton, 0.5f);
					}

					if (g_3d_box && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
					{

						if (vHeadBoneOut.x != 0 || vHeadBoneOut.y != 0 || vHeadBoneOut.z != 0)
						{

							ImU32 ESPSkeleton;

							if (isVis)
							{
								ESPSkeleton = ImGui::GetColorU32({ 93, 255, 0, 255 });
							}
							else
							{
								ESPSkeleton = ImGui::GetColorU32({ 255, 0, 216, 255 });
							}

							Vector3 bottom1 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x + 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom2 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x - 40, vRootBone.y - 40, vRootBone.z));
							Vector3 bottom3 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x - 40, vRootBone.y + 40, vRootBone.z));
							Vector3 bottom4 = g_functions::ConvertWorld2Screen(Vector3(vRootBone.x + 40, vRootBone.y + 40, vRootBone.z));

							Vector3 top1 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top2 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 40, vHeadBone.y - 40, vHeadBone.z + 15));
							Vector3 top3 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x - 40, vHeadBone.y + 40, vHeadBone.z + 15));
							Vector3 top4 = g_functions::ConvertWorld2Screen(Vector3(vHeadBone.x + 40, vHeadBone.y + 40, vHeadBone.z + 15));

							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), ESPSkeleton, 0.1f);

							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), ESPSkeleton, 0.1f);

							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), ESPSkeleton, 0.1f);
							ImGui::GetForegroundDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), ESPSkeleton, 0.1f);
						}
					}

					if (g_lineesp && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
					{

						ImU32 LineColor;

						if (isVis)
						{
							LineColor = ImGui::ColorConvertFloat4ToU32(ImVec4(ESPColor2.R / 128.0, ESPColor2.G / 224.0, ESPColor2.B / 0.0, ESPColor2.A / 255.0));
						}
						else
						{
							LineColor = ImGui::ColorConvertFloat4ToU32(ImVec4(ESPColor.R / 128.0, ESPColor.G / 224.0, ESPColor.B / 0.0, ESPColor.A / 255.0));
						}

						ImGui::GetOverlayDrawList()->AddLine(
							ImVec2(Globals::ScreenCenterX, Globals::Height),
							ImVec2(RelativeScreenLocation.x, RelativeScreenLocation.y + 30),
							LineColor,
							0.1f
						);
					}
				}
				

				auto dx = w2shead.x - (Globals::Width / 2);
				auto dy = w2shead.y - (Globals::Height / 2);
				auto dz = w2shead.z - (Depth / 2);

				auto dist = sqrtf(dx * dx + dy * dy + dz * dz) / 100.0f;

				auto isDBNO = (read<char>(p.Acotr + 0x6F2) >> 4) & 1;

				if (dist < bA1mb0tF0VV4lue && dist < closestDistance && TeamIndex != LocalTeam && !InLobby && CheckInScreen(p.Acotr, Globals::Width, Globals::Height))
				{
					if (g_skipknocked)
					{
						if (!isDBNO)
						{
							closestDistance = dist;
							closestPawn = p.Acotr;
						}
					}
					else
					{
						closestDistance = dist;
						closestPawn = p.Acotr;
					}

				}
			}
		}
		if (closestPawn != 0)
		{
			if (closestPawn && GetAsyncKeyState(VK_RBUTTON))
			{
				if (g_aimbot)
				{
					targetlocked = true;

					RGBA ESPTargetLine = { 255, 0, 0, 255 };

					uint64_t AimbotMesh = read<uint64_t>( closestPawn + 0x2F0);
					if (!AimbotMesh)
						return false;

					Vector3 HeadPosition = g_functions::f_getbonewithIndex(AimbotMesh, select_hitbox());
					if (!IsVec3Valid(HeadPosition))
						return false;

					Vector3 Head = g_functions::ConvertWorld2Screen(HeadPosition);
					if (!IsVec3Valid(HeadPosition))
						return false;

					if (Head.x != 0 || Head.y != 0 || Head.z != 0)
					{
						if ((GetDistance(Head.x, Head.y, Head.z, Globals::Width / 2, Globals::Height / 2) <= bA1mb0tF0VV4lue))
						{
							if (g_mouse_aim)
							{
								aimbot(Head.x, Head.y);
							}
							else if (g_mem_aim)
							{
								Vector3 HeadPosition = g_functions::f_getbonewithIndex(AimbotMesh, 98);

								if (!IsVec3Valid(HeadPosition))
									return false;

								WriteAngles(HeadPosition);

							}

							if (g_exploits_backtrack)
							{
								if (GetAsyncKeyState(VK_LBUTTON)) {

									float ItemDist = Globals::LocalPlayerRelativeLocation.Distance(Head) / 100.f;

									auto fov = sqrtf(powf(Head.x, 4.0f) + powf(Head.y, 4.0f));
									int setting = 2;

									write<float>( closestPawn + 0x64, 0.001f);
								}
								else
								{
									write<float>( closestPawn + 0x64, 1);
								}
							}
						}

					}
				}
			}
			else
			{
				isaimbotting = false;
				targetlocked = false;

				closestDistance = FLT_MAX;
				closestPawn = NULL;
			}
		}
	}
	catch (...) {}
}

bool Head = true, Neck, Chest;


void InitStyle()
{
	ImGui::StyleColorsDark();

	auto& Style = ImGui::GetStyle();

	Style.WindowRounding = 12.000f;
	Style.ChildRounding = 12.000f;
	Style.ChildBorderSize = 0.000f;
	Style.PopupRounding = 12.000f;
	Style.PopupBorderSize = 0.000f;
	Style.FrameRounding = 7.000f;
	Style.CellPadding = ImVec2(4.000f, 3.000f);
	Style.IndentSpacing = 20.000f;
	Style.ScrollbarSize = 13.000f;
	Style.GrabMinSize = 9.000f;
	Style.GrabRounding = 12.000f;
	Style.LogSliderDeadzone = 12.000f;
	Style.TabRounding = 12.000f;
	Style.TabBorderSize = 1.000f;

	Style.Colors[ImGuiCol_WindowBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.940f);
	Style.Colors[ImGuiCol_CheckMark] = ImVec4(0.186f, 0.989f, 1.000f, 1.000f);
	Style.Colors[ImGuiCol_Button] = ImVec4(0.026f, 1.000f, 0.962f, 0.400f);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:/Users/eozer/Desktop/Fonts/calibri.ttf", 18.000f);
}


void runRenderTick() {
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	RECT rect = { 0 };

	HWND GameWnd = FindWindowA(XorStr("UnrealWindow").c_str(), XorStr("Fortnite  ").c_str());

	HWND hwnd_active = GetForegroundWindow();
	HWND overlay_window = glfwGetWin32Window(g_window);

	if (hwnd_active == GameWnd || hwnd_active == overlay_window) {
		isFortniteFocused = true;
	}
	else {
		isFortniteFocused = true;
	}

	if (isFortniteFocused)
	{
		if (GetWindowRect(GameWnd, &rect))
		{
			Globals::Width = rect.right - rect.left;
			Globals::Height = rect.bottom - rect.top;
		}

		Globals::ScreenCenterX = (Globals::Width / 2.0f);
		Globals::ScreenCenterY = (Globals::Height / 2.0f);

		actorLoop();
	}

	float radiusx = bA1mb0tF0VV4lue * (Globals::ScreenCenterX / 100);
	float radiusy = bA1mb0tF0VV4lue * (Globals::ScreenCenterY / 100);

	float calcradius = (radiusx + radiusy) / 2;

	//if (Globals::LocalPawn)
	//{
		if (g_fov)
		{
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 2);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, &Col.white, 3);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.white, 3);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, &Col.black_, 1);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.black_, 1);
			DrawLine((float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) + bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.black_, 1);
			DrawLine((float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) - bA1mb0tF0VV4lue, (float)(Globals::Width / 2) - bA1mb0tF0VV4lue, (float)(Globals::Height / 2) + bA1mb0tF0VV4lue, &Col.black_, 1);
		}

		if (g_crossh) {
			DrawLine((float)(Globals::Width / 2), (float)(Globals::Height / 2) - 8, (float)(Globals::Width / 2), (float)(Globals::Height / 2) + 8, &Col.white, 1);
			DrawLine((float)(Globals::Width / 2) - 8, (float)(Globals::Height / 2), (float)(Globals::Width / 2) + 8, (float)(Globals::Height / 2), &Col.white, 1);
		}

		if (g_circlefov) {
			ImGui::GetOverlayDrawList()->AddCircle(ImVec2(Globals::ScreenCenterX, Globals::ScreenCenterY), bA1mb0tF0VV4lue, ImGui::ColorConvertFloat4ToU32(ImVec4(ESPColor.R / 255.0, ESPColor.G / 255.0, ESPColor.B / 255.0, ESPColor.A / 255.0)), 100);
		}
	//}
	
	

	if (g_overlay_visible)
	{
		//cursor();
		//background();

	}
	ImGuiIO& io = ImGui::GetIO();

	if (g_overlay_visible) {
		{
			InitStyle();
			ImGui::SetNextWindowSize(ImVec2(600.000f, 600.000f), ImGuiCond_Once);
			ImGui::Begin("rename me 0", NULL, 59);
			
			ImVec2 P1, P2;
			ImDrawList* pDrawList;
			const auto& CurrentWindowPos = ImGui::GetWindowPos();
			const auto& pWindowDrawList = ImGui::GetWindowDrawList();
			const auto& pBackgroundDrawList = ImGui::GetBackgroundDrawList();
			const auto& pForegroundDrawList = ImGui::GetForegroundDrawList();

			P1 = ImVec2(30.000f, 70.000f);
			P1.x += CurrentWindowPos.x;
			P1.y += CurrentWindowPos.y;
			P2 = ImVec2(280.000f, 570.000f);
			P2.x += CurrentWindowPos.x;
			P2.y += CurrentWindowPos.y;
			pDrawList = pWindowDrawList;
			pDrawList->AddRect(P1, P2, ImColor(1.000f, 1.000f, 1.000f, 1.000f), 10.000f);

			P1 = ImVec2(320.000f, 70.000f);
			P1.x += CurrentWindowPos.x;
			P1.y += CurrentWindowPos.y;
			P2 = ImVec2(580.000f, 570.000f);
			P2.x += CurrentWindowPos.x;
			P2.y += CurrentWindowPos.y;
			pDrawList = pWindowDrawList;
			pDrawList->AddRect(P1, P2, ImColor(1.000f, 1.000f, 1.000f, 1.000f), 10.000f);

			ImGui::SetCursorPos(ImVec2(259.000f, 26.000f));
			ImGui::Text("Memenite");

			ImGui::SetCursorPos(ImVec2(429.000f, 82.000f));
			ImGui::Text("Config");

			ImGui::SetCursorPos(ImVec2(122.000f, 80.000f));
			ImGui::Text("Features");

			ImGui::SetCursorPos(ImVec2(40.000f, 117.000f));
			ImGui::Checkbox("Mouse Aimbot", &g_mouse_aim);
			if (g_mouse_aim)
				g_mem_aim = false;

			ImGui::SetCursorPos(ImVec2(39.000f, 150.000f));
			ImGui::Checkbox("Memory Aimbot", &g_mem_aim);
			if (g_mem_aim)
				g_mouse_aim = false;

			ImGui::SetCursorPos(ImVec2(39.000f, 209.000f));
			ImGui::Checkbox("Cornered Box ESP", &g_cornerboxesp);

			ImGui::SetCursorPos(ImVec2(39.000f, 243.000f));
			ImGui::Checkbox("3D Box ESP", &g_3d_box);

			ImGui::SetCursorPos(ImVec2(39.000f, 277.000f));
			ImGui::Checkbox("Snaplines ESP", &g_lineesp);

			ImGui::SetCursorPos(ImVec2(39.000f, 310.000f));
			ImGui::Checkbox("Distance ESP", &g_esp_distance);

			ImGui::SetCursorPos(ImVec2(39.000f, 343.000f));
			ImGui::Checkbox("Skeleton ESP", &g_esp_skeleton);

			ImGui::SetCursorPos(ImVec2(38.000f, 376.000f));
			ImGui::Checkbox("Weapon ESP", &g_curweaponesp);

			ImGui::SetCursorPos(ImVec2(38.000f, 435.000f));
			ImGui::Checkbox("Loot ESP", &g_loot);

			ImGui::SetCursorPos(ImVec2(37.000f, 470.000f));
			ImGui::Checkbox("Chest ESP", &g_chests);

			ImGui::SetCursorPos(ImVec2(37.000f, 506.000f));
			ImGui::Checkbox("Vehicle Esp", &g_vehicles);

			ImGui::SetCursorPos(ImVec2(36.000f, 540.000f));
			ImGui::Checkbox("Ammo Box ESP", &g_ammo);

			ImGui::SetNextItemWidth(130.000f);
			ImGui::SetCursorPos(ImVec2(325.000f, 118.000f));
			ImGui::SliderFloat("Aim Smooth", &bA1mb0tSm00th1ngV4lue, 2.000f, 10.000f);
			ImGui::SetCursorPos(ImVec2(470.000f, 122.000f));
			ImGui::Text("Aim Smooth");

			ImGui::SetNextItemWidth(130.000f);
			ImGui::SetCursorPos(ImVec2(324.000f, 150.000f));
			ImGui::SliderInt("Max Aim Dist.", &bA1mD1st4nce, 0, 350);
			ImGui::SetCursorPos(ImVec2(470.000f, 154.000f));
			ImGui::Text("Max Aim Dist.");

			ImGui::SetNextItemWidth(130.000f);
			ImGui::SetCursorPos(ImVec2(325.000f, 182.000f));
			ImGui::SliderInt("Max ESP Dist.", &bE5pD1st4nce, 0, 300);
			ImGui::SetCursorPos(ImVec2(470.000f, 186.000f));
			ImGui::Text("Max ESP Dist.");

			ImGui::SetNextItemWidth(130.000f);
			ImGui::SetCursorPos(ImVec2(325.000f, 210.000f));
			ImGui::SliderFloat("Aimbot FOV", &bA1mb0tF0VV4lue, 0, 500);
			ImGui::SetCursorPos(ImVec2(470.000f, 214.000f));
			ImGui::Text("Aimbot FOV");

			ImGui::SetCursorPos(ImVec2(330.000f, 250.000f));
			ImGui::Checkbox("Show Aim Fov", &g_circlefov);

			ImGui::SetCursorPos(ImVec2(330.000f, 275.000f));
			ImGui::Checkbox("Crosshair", &g_crossh);

			ImGui::SetCursorPos(ImVec2(330.000f, 305.000f));
			ImGui::Checkbox("Invisible Pawn", &g_invisible);

			ImGui::SetCursorPos(ImVec2(330.000f, 335.000f));
			ImGui::Checkbox("Rapid Fire", &rapidfirri);

			ImGui::SetCursorPos(ImVec2(330.000f, 365.000f));
			ImGui::Checkbox("Change Player FOV |EXPLOIT|", &g_fovchanger);
			if (g_fovchanger)
			{
				ImGui::SetNextItemWidth(130.000f);
				ImGui::SetCursorPos(ImVec2(330.000f, 395.000f));
				ImGui::SliderFloat(XorStr("fov").c_str(), &FOVChangerValue, 0.0f, 170.0f, XorStr("%.2f").c_str());
				ImGui::SetCursorPos(ImVec2(470.000f, 400.000f));
				ImGui::Text(XorStr("FOV VALUE |0 Default|").c_str());
			}


			
			ImGui::SetCursorPos(ImVec2(500.000f, 250.000f));
			if (ImGui::Button("Legit Config"))
			{
				g_mouse_aim = true;
				g_mem_aim = false;
				g_cornerboxesp = true;
				g_3d_box = false;
				g_lineesp = false;
				g_esp_distance = true;
				g_esp_skeleton = false;
				g_curweaponesp = false;
					g_loot = false;
					g_chests = false;
					g_vehicles = false;
					g_ammo = false;
					bA1mb0tSm00th1ngV4lue = 6;
					bA1mD1st4nce = 100;
					bE5pD1st4nce = 100;
					bA1mb0tF0VV4lue = 50;
					g_invisible = false;
					rapidfirri = false;
					g_fovchanger = false;
			}

			ImGui::SetCursorPos(ImVec2(500.000f, 275.000f));
			if (ImGui::Button("Rage Config"))
			{
				g_mouse_aim = true;
				g_mem_aim = false;
				g_cornerboxesp = true;
				g_3d_box = false;
				g_lineesp = false;
				g_esp_distance = true;
				g_esp_skeleton = false;
				g_curweaponesp = true;
				g_loot = false;
				g_chests = false;
				g_vehicles = false;
				g_ammo = false;
				bA1mb0tSm00th1ngV4lue = 2;
				bA1mD1st4nce = 150;
				bE5pD1st4nce = 150;
				bA1mb0tF0VV4lue = 100;
				g_invisible = false;
				rapidfirri = true;
				g_fovchanger = true;
			}
			
			ImGui::End();
		}
	}

	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(g_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(g_window);
}

void no_exec(std::string command)
{
	command.insert(0, XorStr("/C ").c_str());

	SHELLEXECUTEINFOA ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = XorStr("cmd.exe").c_str();
	ShExecInfo.lpParameters = command.c_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	if (ShellExecuteExA(&ShExecInfo) == FALSE)

		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	DWORD rv;
	GetExitCodeProcess(ShExecInfo.hProcess, &rv);
	CloseHandle(ShExecInfo.hProcess);
}

enum InjectedInputMouseOptions
{
	Absolute = 32768,
	HWheel = 4096,
	LeftDown = 2,
	LeftUp = 4,
	MiddleDown = 32,
	MiddleUp = 64,
	Move = 1,
	MoveNoCoalesce = 8192,
	None = 0,
	RightDown = 8,
	RightUp = 16,
	VirtualDesk = 16384,
	Wheel = 2048,
	XDown = 128,
	XUp = 256
};

typedef struct _InjectedInputMouseInfo
{
	int DeltaX;
	int DeltaY;
	unsigned int MouseData;
	InjectedInputMouseOptions MouseOptions;
	unsigned int TimeOffsetInMilliseconds;
	void* ExtraInfo;
} InjectedInputMouseInfo;

typedef bool (WINAPI* InjectMouseInput_t)(InjectedInputMouseInfo* inputs, int count);

HMODULE user32;
InjectMouseInput_t InjectMouseInput;
bool CreateFileFromMemory(const std::string& desired_file_path, const char* address, size_t size)
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

void system_no_output(std::string command)
{
	command.insert(0, "/C ");

	SHELLEXECUTEINFOA ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = "cmd.exe";
	ShExecInfo.lpParameters = command.c_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	if (ShellExecuteExA(&ShExecInfo) == FALSE)

		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	DWORD rv;
	GetExitCodeProcess(ShExecInfo.hProcess, &rv);
	CloseHandle(ShExecInfo.hProcess);
}



int main() {

	user32 = LI_FN(LoadLibraryA).safe_cached()(XorStr("user32.dll").c_str());
	InjectMouseInput = reinterpret_cast<InjectMouseInput_t>(GetProcAddress(user32, XorStr("InjectMouseInput").c_str()));
	
	system("color B1");

	printf(XorStr("$$___$$_ _______ _________ _______ _______ $$$$_ _$$__ _______\n").c_str());
	printf(XorStr("$$$_$$$_ _$$$$__ _________ _$$$$__ _______ _$$__ _$$__ _$$$$__\n").c_str());
	printf(XorStr("$$$$$$$_ $$__$$_ $$$$_$$__ $$__$$_ $$$$$__ _$$__ $$$$_ $$__$$_\n").c_str());
	printf(XorStr("$$_$_$$_ $$$$$$_ $$_$$_$$_ $$$$$$_ $$__$$_ _$$__ _$$__ $$$$$$_\n").c_str());
	printf(XorStr("$$___$$_ $$_____ $$_$$_$$_ $$_____ $$__$$_ _$$__ _$$__ $$_____\n").c_str());
	printf(XorStr("$$___$$_ _$$$$$_ $$_$$_$$_ _$$$$$_ $$__$$_ $$$$_ __$$_ _$$$$$_\n").c_str());

	printf(XorStr("\n").c_str());
	
	Sleep(4000);
	printf(XorStr("Thank you for using memenite! Please join our discord for the status... \n Loading... \r").c_str());
	

	g_pid = get_fn_processid();

	if (!g_pid) {
		std::cout << XorStr("Fortnite Not Open\n").c_str();
		system(XorStr("pause").c_str());
		return 1;
	}

	if (driver->Init(FALSE)) {
		printf(E("Success!\n"));
		driver->Attach(E(L"FortniteClient-Win64-Shipping.exe"));	

		g_base_address = driver->GetModuleBase(E(L"FortniteClient-Win64-Shipping.exe"));
	}	

	EnumWindows(retreiveFortniteWindow, NULL);
	if (!fortnite_wnd) {
		std::cout << XorStr("Could not find Fortnite.\n");
		system(XorStr("pause").c_str());
		return 1;
	}

	if (!g_base_address) {
		std::cout << XorStr("Could not get base address.\n").c_str();
		system(XorStr("pause").c_str());
		return 1;
	}

	setupWindow();
	if (!g_window) {
		std::cout << XorStr("Could not setup window.\n").c_str();
		system(XorStr("pause").c_str());
		return 1;
	}

	HWND ass = FindWindowA(nullptr, XorStr("بسماللهالرحالرحي").c_str());
	DWORD assid = 0;
	GetWindowThreadProcessId(ass, &assid);

	HANDLE handle = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(CacheNew), nullptr, NULL, nullptr);
	CloseHandle(handle);

	while (!glfwWindowShouldClose(g_window))
	{
		handleKeyPresses();
		runRenderTick();
	}

	cleanupWindow();
	return 0;
}

void aimbot(float x, float y)
{
	float ScreenCenterX = (Globals::Width / 2);
	float ScreenCenterY = (Globals::Height / 2);
	int AimSpeed = bA1mb0tSm00th1ngV4lue;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	InjectedInputMouseInfo info = { 0 };
	info.DeltaX = TargetX;
	info.DeltaY = TargetY;
	InjectMouseInput(&info, 1);

	return;
}

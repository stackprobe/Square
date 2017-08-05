#include "all.h"

static Random *Rnd;
static struct WallLayer_st
{
	bitTable *Map;
	int Scroll;
	int ScrollAdd;
	int CellSize;
	double R;
	double G;
	double B;
}
WallLayers[4];

#define WL_MAP_H 100 // 偶数じゃないと模様が変になる。(...と思う)

static bitTable *MakeWLMap(int w, double rate_sokoage)
{
	bitTable *map = new bitTable(w);

	for(int x = 0; x < w; x++)
	for(int y = 0; y < WL_MAP_H; y++)
	{
		map->PutBit(x, y, (double)x / w + rate_sokoage <= Rnd->DRnd());
	}
	return map;
}
static void InitWallLayers(void)
{
	zeroclear(&WallLayers);

	struct WallLayer_st *wl = WallLayers;

	// [0]
	wl->Map = MakeWLMap(50, 0.0);
	wl->ScrollAdd = 1;
	wl->CellSize = 16;
	wl->R = 0.4;
	wl->G = 0.4;
	wl->B = 1.0;
	wl++;

	// [1]
	wl->Map = MakeWLMap(40, 0.1);
	wl->ScrollAdd = 2;
	wl->CellSize = 20;
	wl->R = 1.0;
	wl->B = 0.8;
	wl++;

	// [2]
	wl->Map = MakeWLMap(32, 0.2);
	wl->ScrollAdd = 3;
	wl->CellSize = 25;
	wl->G = 1.0;
	wl++;

	// [3]
	wl->Map = MakeWLMap(20, 0.3);
	wl->ScrollAdd = 4;
	wl->CellSize = 40;
	wl->R = 1.0;
	wl->G = 1.0;
	wl++;

	errorCase(wl != WallLayers + lengthof(WallLayers)); // 2bs
}
static void FnlzWallLayers(void)
{
	for(int index = 0; index < lengthof(WallLayers); index++)
	{
		struct WallLayer_st *wl = WallLayers + index;

		delete wl->Map;
	}
}

static void DrawWall(void)
{
	{
		const int cellSpan = 30;

		for(int x = 0; x * cellSpan < SCREEN_W; x++)
		for(int y = 0; y * cellSpan < SCREEN_H; y++)
		{
			double bright = 0.2;

			if((x + y) % 2)
				bright = 0.1;

			DPE_SetBright(0.0, bright, bright);
			DrawRect(P_WHITEBOX, x * cellSpan, y * cellSpan, cellSpan, cellSpan);
			DPE_Reset();
		}
	}

	for(int index = 0; index < lengthof(WallLayers); index++)
	{
		struct WallLayer_st *wl = WallLayers + index;

		int mapYTop = wl->Scroll / wl->CellSize;
		int drYTop = -wl->Scroll % wl->CellSize;

		for(int x = 0; ; x++)
		{
			int drX = x * wl->CellSize;

			if(SCREEN_W <= drX)
				break;

			for(int y = 0; ; y++)
			{
				int drY = drYTop + y * wl->CellSize;

				if(SCREEN_H <= drY)
					break;

				int drFlag = wl->Map->RefBit(x, (mapYTop + y) % WL_MAP_H);

				if(drFlag)
				{
					double bright = 0.6;

					if((x + mapYTop + y) % 2)
						bright = 0.4;

					DPE_SetBright(
						wl->R * bright,
						wl->G * bright,
						wl->B * bright
						);
					DrawRect(P_WHITEBOX, drX, drY, wl->CellSize, wl->CellSize);
					DPE_Reset();
				}
			}
		}
		wl->Scroll += wl->ScrollAdd;
		wl->Scroll %= wl->CellSize * WL_MAP_H;
	}
}
static void DrawTitleLogo(void)
{
	const char *logoMap =
		"# # ### # # ### #"
		"# # # # # # # # #"
		"### ### ##  # # #"
		"# # # # # # # #  "
		"# # # # # # ### #";
	const int LM_W = 17;
	const int LM_H = 5;

	errorCase(strlen(logoMap) != LM_W * LM_H); // 2bs

	const int DR_L = 60;
	const int DR_T = 60;
	const int cellSpan = 40;

	for(int x = 0; x < LM_W; x++)
	for(int y = 0; y < LM_H; y++)
	{
		int lmIndex = x + LM_W * y;

		if(logoMap[lmIndex] == '#')
		{
			double bright = 1.0;

			if((x + y) % 2)
				bright = 0.9;

			int drX = DR_L + x * cellSpan;
			int drY = DR_T + y * cellSpan;

			DPE_SetBright(bright, bright, bright);
			DrawRect(P_WHITEBOX, drX, drY, cellSpan, cellSpan);
			DPE_Reset();
		}
	}
}

// ゲームスタート系 -->

static void LeaveFromMainMenu(int keepBGM = 0)
{
	if(!keepBGM)
		MusicFade();

	SetCurtain(30, -1.0);

	forscene(40)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();
}
static void ReturnToMainMenu(void)
{
	MusicPlay(MUS_TITLE);
	SetCurtain();
	FreezeInput();
}
static void PlayGame(void)
{
	LeaveFromMainMenu();
	FloorMasterMain(0, 0, 0);
	ReturnToMainMenu();
}

#define SUBMENU_WHITE_LV -0.3
#define SUBMENU_WINDOW_A 0.5

static void FloorSelect(int playEditMapMode = 0, int editMode = 0)
{
	SetCurtain(60, SUBMENU_WHITE_LV);
	FreezeInput();

	const int selectMax = FLOOR_NUM + 1; // 最後はexit
	int selectIndex = 0;

	for(; ; )
	{
		int selectIndexAdd = 0;

		if(GetPound(INP_DIR_8))
			selectIndexAdd = -1;

		if(GetPound(INP_DIR_2))
			selectIndexAdd = 1;

		do
		{
			selectIndex += selectIndexAdd;
		}
		while(m_isRange(selectIndex, Gnd.UnclearedFloorIndex + 1, FLOOR_NUM - 1));

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			if(selectIndex < FLOOR_NUM)
			{
				LeaveFromMainMenu();
				FloorMasterMain(selectIndex, playEditMapMode, editMode);
				ReturnToMainMenu();

				goto endMenu;
			}
			break; // ? exit
		}

		DrawWall();
		CurtainEachFrame();

		{
			const int WIN_L = 280;
			const int WIN_T = 76;
			const int WIN_W = 240;
			const int WIN_H = 448;

			DPE_SetAlpha(SUBMENU_WINDOW_A);
			DPE_SetBright(0.0, 0.0, 0.0);
			DrawRect(P_WHITEBOX, WIN_L, WIN_T, WIN_W, WIN_H);
			DPE_Reset();

			DrawWindowFrame(D_WINDOW_Y_00 | DTP, WIN_L, WIN_T, WIN_W, WIN_H);

			const int COLOR = GetColor(255, 255, 255);
			const int BORDER_COLOR = GetColor(100, 100, 50);

			SetPrint(350, 112, 40);
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);
			int c = 0;

			for(int floorIndex = 0; floorIndex < FLOOR_NUM; floorIndex++)
			{
				if(floorIndex == Gnd.UnclearedFloorIndex + 1)
				{
					PE.Color = GetColor(100, 100, 100);
					PE_Border(GetColor(50, 50, 50));
				}
				Print_x(xcout("[%c] FLOOR %d", selectIndex == c++ ? '>' : ' ', floorIndex + 1));
				PrintRet();
			}
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);

			Print_x(xcout("[%c] RETURN", selectIndex == c++ ? '>' : ' '));
			PE_Reset();
		}

		EachFrame();
	}
endMenu:
	SetCurtain();
	FreezeInput();
}
static void EditModeMenu(void)
{
	SetCurtain(60, SUBMENU_WHITE_LV);
	FreezeInput();

	const int selectMax = 4;
	int selectIndex = 0;

	for(; ; )
	{
		int selectIndexAdd = 0;

		if(GetPound(INP_DIR_8))
			selectIndexAdd = -1;

		if(GetPound(INP_DIR_2))
			selectIndexAdd = 1;

		do
		{
			selectIndex += selectIndexAdd;
		}
		while(m_isRange(selectIndex, Gnd.UnclearedFloorIndex + 1, FLOOR_NUM - 1));

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			switch(selectIndex)
			{
			case 0:
				RestoreOriginalMapsIfNeed();
				FloorSelect(1, 1);
				goto endMenu;

			case 1:
				{
					LeaveFromMainMenu();
					FloorMasterMain(0, 1, 0);
					ReturnToMainMenu();
				}
				goto endMenu;

			case 2:
				FloorSelect(1);
				goto endMenu;

			case 3:
				goto endMenu;

			default:
				error();
			}
		}

		DrawWall();
		CurtainEachFrame();

		{
			const int WIN_L = 200;
			const int WIN_T = 172;
			const int WIN_W = SCREEN_W - WIN_L * 2;
			const int WIN_H = SCREEN_H - WIN_T * 2;

			DPE_SetAlpha(SUBMENU_WINDOW_A);
			DPE_SetBright(0.0, 0.0, 0.0);
			DrawRect(P_WHITEBOX, WIN_L, WIN_T, WIN_W, WIN_H);
			DPE_Reset();

			DrawWindowFrame(D_WINDOW_Y_00 | DTP, WIN_L, WIN_T, WIN_W, WIN_H);

			SetPrint(240, 230, 40);
			PE_Border(GetColor(100, 100, 50));
			int c = 0;
			Print_x(xcout("[%c] EDIT FLOORS", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] PLAY EDITED FLOORS", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] PLAY EDITED FLOORS (FLOOR SELECT)", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] RETURN", selectIndex == c++ ? '>' : ' '));
			PE_Reset();
		}

		EachFrame();
	}
endMenu:
	SetCurtain();
	FreezeInput();
}

// 設定 -->

static void DrawSettingsWall(void)
{
	DrawWall();

	DPE_SetAlpha(SUBMENU_WINDOW_A);
	DPE_SetBright(0.0, 0.0, 0.0);
	DrawRect(P_WHITEBOX, 0, 0, SCREEN_W / 2, SCREEN_H);
	DPE_Reset();

	CurtainEachFrame();
}
static int BC_GetPressPadBtnId(void)
{
	int padId = Gnd.PrimaryPadId;

	/*
		todo: パッドを探すようにする。
	*/
	if(padId == -1) // ? 未定義
		padId = 0; // 適当なデフォ

	for(int btnId = 0; btnId < PAD_BUTTON_MAX; btnId++)
	{
		if(GetPadInput(padId, btnId) == 1)
		{
			return btnId;
		}
	}
	return -1; // not found
}
static void ButtonConfig(void)
{
	int *pPadBtns[] =
	{
		&Gnd.PadBtnId.Dir_8,
		&Gnd.PadBtnId.Dir_2,
		&Gnd.PadBtnId.Dir_4,
		&Gnd.PadBtnId.Dir_6,
		&Gnd.PadBtnId.A,
		&Gnd.PadBtnId.B,
		&Gnd.PadBtnId.L,
		&Gnd.PadBtnId.R,
		&Gnd.PadBtnId.Pause,
	};
	char *btnNames[] =
	{
		"UP",
		"DOWN",
		"LEFT",
		"RIGHT",
		"JUMP / DECIDE",
		"SLOW / CANCEL",
		"SCOPE",
		"QUICK RESTART",
		"PAUSE",
	};
	errorCase(lengthof(pPadBtns) != lengthof(btnNames));
	int btnMax = lengthof(pPadBtns);
	int btnIndex = 0;
	void *backup = memClone(&Gnd.PadBtnId, sizeof(Gnd.PadBtnId));

	/*
		不使用ボタンのクリア
	*/
	Gnd.PadBtnId.C = -1;
	Gnd.PadBtnId.D = -1;
	Gnd.PadBtnId.E = -1;
	Gnd.PadBtnId.F = -1;
	Gnd.PadBtnId.Start = -1;

	FreezeInput();

	while(btnIndex < btnMax)
	{
		if(
			GetKeyInput(KEY_INPUT_Z) == 1 ||
			GetKeyInput(KEY_INPUT_X) == 1
			)
		{
			memcpy(&Gnd.PadBtnId, backup, sizeof(Gnd.PadBtnId)); // 復元
			break;
		}

		{
			int btnId = BC_GetPressPadBtnId();

			if(btnId != -1)
			{
				for(int i = 0; i < btnIndex; i++)
					if(*pPadBtns[i] == btnId)
						goto endDecideBtnId;

				*pPadBtns[btnIndex] = btnId;
				btnIndex++;
			}
endDecideBtnId:;
		}

		if(GetKeyInput(KEY_INPUT_SPACE) == 1) // 割り当てナシ
		{
			*pPadBtns[btnIndex] = -1;
			btnIndex++;
		}

		DrawSettingsWall();

		PE_Border(GetColor(100, 100, 50));
		SetPrint(40, 40, 38);
		Print("**** PAD CONFIG ****");
		PrintRet();

		for(int index = 0; index < btnMax; index++)
		{
			Print_x(xcout("[%c] %s BUTTON", index == btnIndex ? '>' : ' ', btnNames[index]));

			if(index < btnIndex)
			{
				int btnId = *pPadBtns[index];

				if(btnId == -1)
					Print(" -> NOT ASSIGNED");
				else
					Print_x(xcout(" -> %d", *pPadBtns[index]));
			}
			PrintRet();
		}
		PrintRet();
		Print("# PRESS A BUTTON INDICATING CURSOR");
		PrintRet();
		Print("# PRESS \"X\" OR \"Z\" KEY TO CANCEL");
		PrintRet();
		Print("# PRESS SPACE KEY TO SKIP ASSIGN");
		PE_Reset();

		EachFrame();
	}
	FreezeInput();

	memFree(backup);
}
static void WindowSizeConfig(void)
{
	FreezeInput();

	const int selectMax = 12;
	int selectIndex = 0;

	for(; ; )
	{
		if(GetPound(INP_DIR_8))
			selectIndex--;

		if(GetPound(INP_DIR_2))
			selectIndex++;

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			switch(selectIndex)
			{
			case 0: SetScreenSize(800, 600); break;
			case 1: SetScreenSize(900, 675); break;
			case 2: SetScreenSize(1000, 750); break;
			case 3: SetScreenSize(1100, 825); break;
			case 4: SetScreenSize(1200, 900); break;
			case 5: SetScreenSize(1300, 975); break;
			case 6: SetScreenSize(1400, 1050); break;
			case 7: SetScreenSize(1500, 1125); break;
			case 8: SetScreenSize(1600, 1200); break;

			case 9:
				SetScreenSize(
					GetSystemMetrics(SM_CXSCREEN),
					GetSystemMetrics(SM_CYSCREEN)
					);
				break;

			case 10:
				{
					int w = GetSystemMetrics(SM_CXSCREEN);
					int h = GetSystemMetrics(SM_CYSCREEN);

					if(w * SCREEN_H < h * SCREEN_W) // 縦長モニタ -> 横幅に合わせる
					{
						h = m_d2i(((double)w * SCREEN_H) / SCREEN_W);
					}
					else // 横長モニタ -> 縦幅に合わせる
					{
						w = m_d2i(((double)h * SCREEN_W) / SCREEN_H);
					}
					SetScreenSize(w, h);
				}
				break;

			case 11:
				goto endMenu;

			default:
				error();
			}
		}

		DrawSettingsWall();

		SetPrint(40, 40, 40);
		PE_Border(GetColor(100, 100, 50));
		Print("**** RESIZE WINDOW ****");
		PrintRet();
		int c = 0;
		Print_x(xcout("[%c] 800 x 600 (DEFAULT)", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 900 x 675", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1000 x 750", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1100 x 825", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1200 x 900", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1300 x 975", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1400 x 1050", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1500 x 1125", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] 1600 x 1200", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] ADJUST TO SCREEN", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] ADJUST TO SCREEN KEEP ASPECT RATIO", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] RETURN", selectIndex == c++ ? '>' : ' '));
		PE_Reset();

		EachFrame();
	}
endMenu:
	FreezeInput();
}
static void TuneVolume(int seFlag)
{
	double dvol = seFlag ? Gnd.SEVolume : Gnd.MusicVolume;
	int volume = m_d2i(dvol * 100);
	int volorig = volume;
	int lastvol = volume;

	FreezeInput();

	for(; ; )
	{
		if(GetPound(INP_DIR_4))
			volume--;

		if(GetPound(INP_DIR_6))
			volume++;

		if(GetPound(INP_DIR_2))
			volume -= 10;

		if(GetPound(INP_DIR_8))
			volume += 10;

		if(GetPound(INP_B))
		{
			if(volume == volorig)
				break;

			volume = volorig;
		}
		m_range(volume, 0, 100);

		if(GetPound(INP_A))
			break;

		if(lastvol != volume)
		{
			if(seFlag)
			{
				Gnd.SEVolume = volume / 100.0;
				UpdateSEVolume();
			}
			else
			{
				Gnd.MusicVolume = volume / 100.0;
				UpdateMusicVolume();
			}
			lastvol = volume;
		}
		if(seFlag && ProcFrame % 60 == 0)
		{
			SEPlay(Rnd->DRnd() < 0.5 ? SE_PAUSE_IN : SE_PAUSE_OUT);
		}
		DrawSettingsWall();

		PE_Border(GetColor(100, 100, 50));
		SetPrint(40, 40, 40);
		Print_x(xcout("**** %s VOLUME ****", seFlag ? "SE" : "BGM"));
		PrintRet();
		Print_x(xcout("[ %3d ]", volume));
		PE_Reset();

		EachFrame();
	}
	FreezeInput();
}
static int SettingsConfirm(char *comment1, char *comment2)
{
	int retval = 0;

	FreezeInput();

	const int selectMax = 2;
	int selectIndex = 1;

	for(; ; )
	{
		if(GetPound(INP_DIR_8))
			selectIndex--;

		if(GetPound(INP_DIR_2))
			selectIndex++;

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			switch(selectIndex)
			{
			case 0:
				retval = 1;
				goto endMenu;

			case 1:
				goto endMenu;

			default:
				error();
			}
		}

		DrawSettingsWall();

		SetPrint(40, 40, 40);
		PE_Border(GetColor(100, 100, 50));
		Print("**** ARE YOU SURE ? ****");
		PrintRet();
		int c = 0;
		Print_x(xcout("[%c] YES", selectIndex == c++ ? '>' : ' '));
		PrintRet();
		Print_x(xcout("[%c] NO", selectIndex == c++ ? '>' : ' '));

		if(comment1)
		{
			PrintRet();
			PrintRet();
			Print_x(xcout("# %s", comment1));

			if(comment2)
			{
				PrintRet();
				Print_x(xcout("# %s", comment2));
			}
		}
		PE_Reset();

		EachFrame();
	}
endMenu:
	FreezeInput();

	return retval;
}
static void Settings(void)
{
	SetCurtain(60, SUBMENU_WHITE_LV);
	FreezeInput();

	const int selectMax = 9;
	int selectIndex = 0;

	for(; ; )
	{
		if(GetPound(INP_DIR_8))
			selectIndex--;

		if(GetPound(INP_DIR_2))
			selectIndex++;

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			switch(selectIndex)
			{
			case 0:
				ButtonConfig();
				break;

			case 1:
				WindowSizeConfig();
				break;

			case 2:
				TuneVolume(0);
				break;

			case 3:
				TuneVolume(1);
				break;

			case 4:
				Gnd.ShowMouseCursorMode = Gnd.ShowMouseCursorMode ? 0 : 1;
				SetMouseDispMode(Gnd.ShowMouseCursorMode);
				break;

			case 5:
				Gnd.GraphicalMode = (Gnd.GraphicalMode + 1) % GM_MAX;
				break;

			case 6:
				Gnd.RecordingMode = Gnd.RecordingMode ? 0 : 1;
				break;

			case 7:
				if(SettingsConfirm(NULL, NULL)) {
//				if(SettingsConfirm("ALL CLEAR SHITAKOTO-NI-SHITE", "EDIT MODE WO-KAIHOU-SURU-YO!")) {
					Gnd.UnclearedFloorIndex = FLOOR_NUM;
					RestoreOriginalMapsIfNeed();
					SEPlay(SE_FLOORCLEAR);
				}
				break;

			case 8:
				goto endMenu;

			default:
				error();
			}
		}

		DrawSettingsWall();

		{
			SetPrint(40, 40, 40);
			PE_Border(GetColor(100, 100, 50));
			Print("**** SETTING ****");
			PrintRet();
			int c = 0;
			Print_x(xcout("[%c] PAD CONFIG", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] RESIZE WINDOW", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] BGM VOLUME", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] SE VOLUME", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] MOUSE CURSOR MODE : %s", selectIndex == c++ ? '>' : ' ', Gnd.ShowMouseCursorMode ? "SHOW" : "HIDE"));
			PrintRet();
			Print_x(xcout("[%c] GRAPHIC MODE : %s", selectIndex == c++ ? '>' : ' ', GetGraphicalModeCaption(Gnd.GraphicalMode)));
			PrintRet();
			Print_x(xcout("[%c] SAVE REPLAY DATA : %s", selectIndex == c++ ? '>' : ' ', Gnd.RecordingMode ? "ON" : "OFF"));
			PrintRet();
			Print_x(xcout("[%c] UNLOCK EXTRA MODE", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] RETURN", selectIndex == c++ ? '>' : ' '));
			PE_Reset();
		}

		EachFrame();
	}
endMenu:
	ExportSaveData();

	SetCurtain();
	FreezeInput();
}

// ---- time attack ----

static void TAM_BeforeBestTime(int ri)
{
	if(ri == -1)
	{
		PE.Color = GetColor(128, 128, 128);
		PE_Border(GetColor(64, 64, 64));
	}
}
static void TAM_BeforeRank(int ri)
{
	if(ri == 0)
	{
		int v = 205 + (int)(sin(ProcFrame / 9.0) * 100.0);

		m_range(v, 0, 255);

		PE.Color = GetColor(v, v, 0);
		PE_Border(GetColor(v / 2, v / 2, 0));
	}
}
static void TimeAttackMenu(void)
{
	SetCurtain(60, SUBMENU_WHITE_LV);
	FreezeInput();

	const int selectMax = FLOOR_NUM + 1; // 最後はexit
	int selectIndex = 0;

	for(; ; )
	{
		int selectIndexAdd = 0;

		if(GetPound(INP_DIR_8))
			selectIndexAdd = -1;

		if(GetPound(INP_DIR_2))
			selectIndexAdd = 1;

		do
		{
			selectIndex += selectIndexAdd;
		}
		while(m_isRange(selectIndex, Gnd.UnclearedFloorIndex + 1, FLOOR_NUM - 1));

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			if(selectIndex < FLOOR_NUM)
			{
				int floorIndex = selectIndex;

				errorCase(!m_isRange(floorIndex, 0, FLOOR_NUM - 1));

				LeaveFromMainMenu();
				FMM_TimeAttackMode = 1;
				FloorMasterMain(floorIndex, 0, 0);
				FMM_TimeAttackMode = 0;
				ReturnToMainMenu();

				goto drawStart;
			}
			break; // ? exit
		}

#if LOG_ENABLED
		if(GetKeyPound(KEY_INPUT_F1))
		{
			BT_SetBestFrmCnt(selectIndex, BT_GetBestFrmCnt(selectIndex) - 1);
		}
		if(GetKeyPound(KEY_INPUT_F2))
		{
			BT_SetBestFrmCnt(selectIndex, BT_GetBestFrmCnt(selectIndex) + 1);
		}
		if(GetKeyPound(KEY_INPUT_F3))
		{
			BT_SetBestFrmCnt(selectIndex, BT_GetBestFrmCnt(selectIndex) - 600);
		}
		if(GetKeyPound(KEY_INPUT_F4))
		{
			BT_SetBestFrmCnt(selectIndex, BT_GetBestFrmCnt(selectIndex) + 600);
		}
#endif

drawStart:
		DrawWall();
		CurtainEachFrame();

		{
			const int WIN_L = 120;
			const int WIN_T = 28;
			const int WIN_W = 560;
			const int WIN_H = 544;

			DPE_SetAlpha(SUBMENU_WINDOW_A);
			DPE_SetBright(0.0, 0.0, 0.0);
			DrawRect(P_WHITEBOX, WIN_L, WIN_T, WIN_W, WIN_H);
			DPE_Reset();

			DrawWindowFrame(D_WINDOW_Y_00 | DTP, WIN_L, WIN_T, WIN_W, WIN_H);

			const int COLOR = GetColor(255, 255, 255);
			const int BORDER_COLOR = GetColor(50, 50, 150);

			SetPrint(190, 64, 40);
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);
			Print("                     BEST TIME           RANK");
			PrintRet();

			int c = 0;

			for(int floorIndex = 0; floorIndex < FLOOR_NUM; floorIndex++)
			{
				Print_x(xcout("[%c] FLOOR %d", selectIndex == c++ ? '>' : ' ', floorIndex + 1));
				Print("         ");

				TAM_BeforeBestTime(BT_GetRankInt(floorIndex, Gnd.BestTimeList[floorIndex]));

				Print(BT_Parse(Gnd.BestTimeList[floorIndex]));
//				Print_x(xcout("[--:--.---]"));
				Print("         ");

				TAM_BeforeRank(BT_GetRankInt(floorIndex, Gnd.BestTimeList[floorIndex]));

				Print_x(xcout("[ %s ]", BT_GetRank(floorIndex, Gnd.BestTimeList[floorIndex])));
				PE.Color = COLOR;
				PE_Border(BORDER_COLOR);
				PrintRet();
			}
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);

			Print_x(xcout("[%c] RETURN", selectIndex == c++ ? '>' : ' '));
			Print("          ---------- TOTAL ----------");
			PrintRet();
			Print("                    ");

			TAM_BeforeBestTime(BT_GetTotalRankInt());

			Print(BT_ParseMillis(BT_GetTotalMillis()));
//			Print_x(xcout("[--:--.---]"));
			Print("         ");

			TAM_BeforeRank(BT_GetTotalRankInt());

			Print_x(xcout("[ %s ]", BT_GetTotalRank()));
			PE_Reset();
		}

		EachFrame();
	}
//endMenu: // not used
	SetCurtain();
	FreezeInput();
}

// ----

void MainMenu(void)
{
	Rnd = new Random((int)time(NULL));
	InitWallLayers();

	// <-- init

	MusicPlay(MUS_TITLE);
	SetCurtain();
	FreezeInput();

	const int selectMax = 7;
	const int timeAttackIndex = 2;
	const int editModeIndex = 5;
	int selectIndex = 0;

	for(; ; )
	{
		int selectIndexAdd = 0;

		if(GetPound(INP_DIR_8))
			selectIndexAdd = -1;

		if(GetPound(INP_DIR_2))
			selectIndexAdd = 1;

		selectIndex += selectIndexAdd;

		if(
			selectIndex == timeAttackIndex ||
			selectIndex == editModeIndex
			)
			if(Gnd.UnclearedFloorIndex < FLOOR_NUM) // ? not editable
				selectIndex += selectIndexAdd;

		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			switch(selectIndex)
			{
			case 0:
				PlayGame();
				break;

			case 1:
				FloorSelect();
				break;

			case 2:
				TimeAttackMenu();
				break;

			case 3:
				LeaveFromMainMenu(1);
				ReplayMainMenu();
				ReturnToMainMenu();
				break;

			case 4:
				Settings();
				break;

			case 5:
				EditModeMenu();
				break;

			case 6:
				goto endMenu;

			default:
				error();
			}
		}

		DrawWall();
		DrawTitleLogo();

		PE_Border(GetColor(100, 100, 50));
		SetPrint(278, 584);
		Print(GetDatString(DATSTR_COPYRIGHT));
		PE_Reset();

		{
			const int WIN_L = 280;
			const int WIN_T = 320;
			const int WIN_W = 240;
			const int WIN_H = 240;

			DPE_SetAlpha(0.65);
			DPE_SetBright(0.0, 0.0, 0.0);
			DrawRect(P_WHITEBOX, WIN_L, WIN_T, WIN_W, WIN_H);
			DPE_Reset();

			DrawWindowFrame(D_WINDOW_Y_00 | DTP, WIN_L, WIN_T, WIN_W, WIN_H);

			const int COLOR = GetColor(255, 255, 255);
			const int BORDER_COLOR = GetColor(100, 100, 50);

			SetPrint(330, 343, 30);
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);
			int c = 0;
			Print_x(xcout("[%c] GAME START", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] FLOOR SELECT", selectIndex == c++ ? '>' : ' '));
			PrintRet();

			if(Gnd.UnclearedFloorIndex < FLOOR_NUM) // ? not editable
			{
				PE.Color = GetColor(100, 100, 100);
				PE_Border(GetColor(50, 50, 50));
			}
			Print_x(xcout("[%c] TIME ATTACK", selectIndex == c++ ? '>' : ' '));
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);

			PrintRet();
			Print_x(xcout("[%c] REPLAY", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] SETTING", selectIndex == c++ ? '>' : ' '));
			PrintRet();

			if(Gnd.UnclearedFloorIndex < FLOOR_NUM) // ? not editable
			{
				PE.Color = GetColor(100, 100, 100);
				PE_Border(GetColor(50, 50, 50));
			}
			Print_x(xcout("[%c] EDIT MODE", selectIndex == c++ ? '>' : ' '));
			PE.Color = COLOR;
			PE_Border(BORDER_COLOR);

			PrintRet();
			Print_x(xcout("[%c] EXIT", selectIndex == c++ ? '>' : ' '));
			PE_Reset();
		}

		EachFrame();
	}
endMenu:
//	FreezeInput();
	MusicFade();
	SetCurtain(30, -1.0);

	forscene(40)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();

	// fnlz -->

	delete Rnd;
	FnlzWallLayers();
}

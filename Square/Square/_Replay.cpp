#include "all.h"

char *GetReplayDir(void)
{
	static char *dir;

	if(!dir)
	{
		dir = "Replay";

		if(!accessible(dir))
		{
			dir = "..\\..\\Replay";
		}
	}
	return dir;
}
autoList<uchar> *MakeMapHash(autoList<uchar> *mapImage)
{
	autoList<uchar> *mapHash = sha512_block(mapImage);

	/*
		���v���C�̓����[�X���ɍĐ��\�Ƃ��邽�߁A�����[�X���ɈقȂ� hashMask �ɂ���I
	*/
	{
#if LOG_ENABLED
		char *RAW_DATA = "Jan  2 2015 14:06:35"; // ver 0.11
#else
		char *RAW_DATA = __DATE__ " " __TIME__;
#endif
		autoList<uchar> *hashMask = sha512_str(RAW_DATA);

		for(int c = 0; c < 64; c++) // mapHash ^= hashMask
		{
			mapHash->SetElement(
				c,
				mapHash->GetElement(c) ^ hashMask->GetElement(c)
				);
		}
		delete hashMask;
	}

	return mapHash;
}

static void DrawWall(void)
{
	const int cellSize = 24;
	const int dr_l = ProcFrame % cellSize - cellSize;
	const int dr_t = -ProcFrame % cellSize;

	for(int x = 0; dr_l + x * cellSize < SCREEN_W; x++)
	for(int y = 0; dr_t + y * cellSize < SCREEN_H; y++)
	{
		double b = 0.08;

		if((x + y) & 1)
			b = 0.1;

		DPE_SetBright(0.0, b, 0.0);
		DrawRect(P_WHITEBOX, dr_l + x * cellSize, dr_t + y * cellSize, cellSize, cellSize);
		DPE_Reset();
	}
}

// Save -->

char *GetSaveReplayFilePath(int floorIndex, int playEditMapMode, int deathFlag)
{
	char *replayFile;
	char *optTimeAttack;
	time_t counter = time(NULL);

	if(FMM_TimeAttackFrmCnt != -1)
	{
		optTimeAttack = xcout("_%s", BT_Parse(FMM_TimeAttackFrmCnt));

		replaceChar(optTimeAttack, ':', 'm');
		replaceChar(optTimeAttack, '.', 's');
	}
	else
		optTimeAttack = strx("");

	for(; ; )
	{
		struct tm i;
		localtime_s(&i, &counter);

		replayFile = xcout(
			"%s\\Hako_%s%04d%s_%04d%02d%02d%02d%02d%02d%s.rpy"
			,GetReplayDir()
			,playEditMapMode ? "Edit_" : ""
			,floorIndex + 1
			,deathFlag ? "_Dead" : ""
			,i.tm_year + 1900
			,i.tm_mon + 1
			,i.tm_mday
			,i.tm_hour
			,i.tm_min
			,i.tm_sec
			,optTimeAttack
			);

		if(!accessible(replayFile))
			break;

		memFree(replayFile);
		counter++;
	}
	memFree(optTimeAttack);
	return replayFile;
}
void SaveReplayConfirm(int floorIndex, int playEditMapMode)
{
	char *replayFile = GetSaveReplayFilePath(floorIndex, playEditMapMode, 0);

	// <-- init

	SetCurtain();
	FreezeInput();

	const int selectMax = 2;
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
				SaveReplayFile(replayFile);
				SEPlay(SE_ACCEPT);
				goto endConfirm;

			case 1:
				goto endConfirm;

			default:
				error();
			}
		}

		DrawWall();

		{
			int x = 400 - (7 + strlen(replayFile)) * 9 / 2;

			PE_Border(GetColor(0, 100, 0));
			SetPrint(x, 236, 40);
			Print("DO YOU WANT TO SAVE A REPLAY ?");
			PrintRet();
			Print_x(xcout("PATH : %s", replayFile));
			PrintRet();
			int c = 0;
			Print_x(xcout("[%c] YES", selectIndex == c++ ? '>' : ' '));
			PrintRet();
			Print_x(xcout("[%c] NO", selectIndex == c++ ? '>' : ' '));
			PE_Reset();
		}

		EachFrame();
	}
endConfirm:
	FreezeInput();
	SetCurtain(30, -1.0);

	forscene(40)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();

	// fnlz -->

	memFree(replayFile);
}

// Menu -->

static autoList<char *> *ReplayFileList;

static autoList<uchar> *GetMap(int editMode, int floorIndex) // ret: bind
{
	if(editMode)
	{
		static autoList<uchar> *lastMapImage;
		delete lastMapImage;
		return lastMapImage = readFile_x(xcout("Map\\%04d.bmp", floorIndex + 1));
	}
	return GetEtcRes()->GetHandle(ETC_FLOOR_00 + floorIndex);
}
static autoList<uchar> *GetMapDataByHash(autoList<uchar> *hash) // ret: bind
{
	for(int editMode = 0; editMode < 2; editMode++)
	for(int floorIndex = 0; floorIndex < FLOOR_NUM; floorIndex++)
	{
		autoList<uchar> *mapImage = GetMap(editMode, floorIndex);
		autoList<uchar> *mapHash = MakeMapHash(mapImage);

		if(isSame(mapHash, hash))
		{
			return mapImage;
		}
	}
	return NULL; // not found
}
static void PlayReplay(char *replayFile)
{
	LoadReplayFile(replayFile);
	autoList<uchar> *mapImage = GetMapDataByHash(GetReplayMapHash());

	if(!mapImage) // ? �Ή�����}�b�v������
	{
		FreezeInput();

		for(; ; )
		{
			if(GetPound(INP_A) || GetPound(INP_B))
				break;

			DrawWall();

			PE_Border(GetColor(0, 100, 0));
			SetPrint(290, 270, 44);
			Print("MAP DATA FILE NOT FOUND !");
			PrintRet();
			Print("[>] OK");

			EachFrame();
		}
		FreezeInput();
		return;
	}

	// ui {

	MusicFade();
	SetCurtain(30, -1.0);

	forscene(40)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();

	// }

	GameInit();

	GDc.MapBmpImage = mapImage;
	GDc.MapBmpImageUnreleaseFlag = 1;
	GDc.ReplayMode = 1;

	GameMain();
	GameFnlz();

	// ui -->

	SetCurtain();
	MusicPlay(MUS_TITLE);
	FreezeInput();
}
void ReplayMainMenu(void) // called @ MainMenu
{
	ReplayFileList = new autoList<char *>();
	getFileList(GetReplayDir(), NULL, ReplayFileList);
	ReplayFileList->Sort((int (*)(char *, char *))_stricmp);

	// <-- init

	SetCurtain();
	FreezeInput();

	const int filePerPage = 25;
	const int selectMax = filePerPage + 1;
	const int pageMax = 99;
	int selectIndex = 0;
	int pageIndex = 0;

	for(; ; )
	{
		if(GetPound(INP_DIR_4))
			pageIndex--;

		if(GetPound(INP_DIR_6))
			pageIndex++;

		if(GetPound(INP_DIR_8))
			selectIndex--;

		if(GetPound(INP_DIR_2))
			selectIndex++;

		adjustSelectIndex(pageIndex, pageMax);
		adjustSelectIndex(selectIndex, selectMax);

		if(GetPound(INP_B))
		{
			if(selectIndex == selectMax - 1)
				break;

			selectIndex = selectMax - 1;
		}
		if(GetPound(INP_A))
		{
			if(selectIndex == selectMax - 1)
				break;

			char *replayFile = ReplayFileList->RefElement(pageIndex * filePerPage + selectIndex, NULL);

			if(replayFile)
			{
				replayFile = xcout("%s\\%s", GetReplayDir(), replayFile);
				PlayReplay(replayFile);
				memFree(replayFile);
			}
		}

		DrawWall();

		{
			PE_Border(GetColor(50, 150, 50));
			SetPrint(40, 40);
			Print("**** REPLAY ****");
			PE_Reset();

			PE_Border(GetColor(0, 100, 0));
			SetPrint(40, 80);
			Print("SELECT REPLAY DATA FILE");

			SetPrint(640, 80);
			Print_x(xcout("[ PAGE %02d ]", pageIndex + 1));

			SetPrint(40, 120);
			int c = 0;

			for(int index = 0; index < filePerPage; index++)
			{
				Print_x(xcout("[%c] %s", selectIndex == c++ ? '>' : ' ', ReplayFileList->RefElement(pageIndex * filePerPage + index, "*")));
				PrintRet();
			}
			PrintRet();
			Print_x(xcout("[%c] RETURN", selectIndex == c++ ? '>' : ' '));
			PE_Reset();
		}

		EachFrame();
	}
//	FreezeInput();
//	MusicFade();
	SetCurtain(30, -1.0);

	forscene(40)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();

	// fnlz -->

	releaseList(ReplayFileList, (void (*)(char *))memFree);
}

#include "all.h"

static SubScreen_t *GetSSFloorNo(int floorNo)
{
	char *dispPtn;

	if(floorNo == FLOOR_NUM)
		dispPtn = strx("FINAL FLOOR");
	else
		dispPtn = xcout("FLOOR %d", floorNo);

	const int MARGIN = 10;

	int x = MARGIN;
	int y = MARGIN;
	int w = MARGIN * 2 + strlen(dispPtn) * 9;
	int h = MARGIN * 2 + 16;

	SubScreen_t *ss = CreateSubScreen(w, h, 1);

	ChangeDrawScreen(ss);
	SetPrint(x, y);
	PE_Border(FMM_TimeAttackMode ? GetColor(40, 40, 160) : FMM_EditMode ? GetColor(30, 100, 30) : GetColor(60, 60, 30));
	Print(dispPtn);
	PE_Reset();
	RestoreDrawScreen();

	memFree(dispPtn);
	return ss;
}
static void DrawFloorNo(SubScreen_t *fn, int yPos)
{
	DPE.GraphicHandleFlag = 1;
	DPE.GraphicSize = GetSubScreenSize(fn);
	DPE.MosaicOn = 1;
	DrawBegin(GetHandle(fn), 400, yPos);
	DrawZoom(4.0);
	DrawEnd();
	DPE_Reset();
}
static void FloorInterval(int floorNo)
{
	SubScreen_t *fn = GetSSFloorNo(floorNo);
	SetCurtain(0);

	const int MARGIN = 40;

	forscene(30)
	{
		DrawCurtain();
		DrawFloorNo(fn, (600 + MARGIN) - m_d2i((300.0 + MARGIN) * sc_rate));
		EachFrame();
	}
	sceneLeave();
	forscene(60)
	{
		DrawCurtain();
		DrawFloorNo(fn, 300);
		EachFrame();
	}
	sceneLeave();
	forscene(30)
	{
		DrawCurtain();
		DrawFloorNo(fn, 300 - m_d2i((300.0 + MARGIN) * sc_rate));
		EachFrame();
	}
	sceneLeave();

	ReleaseSubScreen(fn);
}

int FMM_TimeAttackMode;
int FMM_EditMode;
int FMM_TimeAttackFrmCnt = -1; // -1 == 未クリア || タイムアタックではない。0 ～ IMAX

void FloorMasterMain(int entryFloorIndex, int playEditMapMode, int editMode) // entryFloorIndex: 0 ～ (FLOOR_NUM - 1)
{
	errorCase(!m_isRange(entryFloorIndex, 0, FLOOR_NUM - 1));

	int floorIndex = entryFloorIndex;
	FMM_EditMode = playEditMapMode || editMode ? 1 : 0;

	for(; ; )
	{
		FMM_TimeAttackFrmCnt = -1; // reset

		FloorInterval(floorIndex + 1);

		GameInit();
		GDc.FloorIndex = floorIndex;
		GDc.TimeAttackMode = FMM_TimeAttackMode;

		if(playEditMapMode)
		{
			if(accessible("Map")) // リリース環境
			{
				GDc.MapBmpFile = xcout("Map\\%04u.bmp", floorIndex + 1);
			}
			else // 開発環境
			{
				GDc.MapBmpFile = xcout("..\\..\\Map\\%04u.bmp", floorIndex + 1);
			}
		}
		else
		{
			GDc.MapBmpImage = GetEtcRes()->GetHandle(ETC_FLOOR_00 + floorIndex);
			GDc.MapBmpImageUnreleaseFlag = 1;
		}
		GDc.EditableFlag = editMode;

		GameMain();
		int clearFlag = GDc.FloorClearFlag;

		if(FMM_TimeAttackMode)
			FMM_TimeAttackFrmCnt = GDc.PlayFrameCount;

		GameFnlz();

		if(!clearFlag)
			break;

		floorIndex++;
		m_maxim(Gnd.UnclearedFloorIndex, floorIndex);
		ExportSaveData();

		if(
			Gnd.RecordingMode &&
			!editMode &&
			GetReplayInput()->GetCount() < REPLAY_DATA_MAX // リプレイの長さがオーバフローしていない
			)
			SaveReplayConfirm(floorIndex - 1, playEditMapMode);

		if(FMM_TimeAttackMode)
			break;

		if(FLOOR_NUM <= floorIndex) // ? オールクリア
		{
			if(!editMode)
				Ending();

			break;
		}
	}
	FMM_EditMode = 0;
}

/*
	zd == zero is disabled
*/

typedef struct GameInput_st
{
	int Dir2;
	int Dir4;
	int Dir6;
	int Dir8;
	int A;
	int B;
	int L;
	int R; // reserved
}
GameInput_t;

typedef struct GDc_st
{
	// Params {

	int FloorIndex; // 0 - (FLOOR_NUM - 1)

	/*
		どれか１つは有効であること。
	*/
	char *MapBmpFile; // bind
	autoList<uchar> *MapBmpImage; // bind
	autoTable<uint> *MapBmp; // bind

	int MapBmpImageUnreleaseFlag;
	int EditableFlag;
	int ReplayMode;

	int TimeAttackMode;

	// }

	Random *Rnd;

	autoTable<MCell_t *> *Map;

	int Field_W;
	int Field_H;

	autoList<Enemy_t *> *EnemyList;

	int CameraTargetX;
	int CameraTargetY;
	int ICameraX;
	int ICameraY;

	struct
	{
		fPInt_t X;
		fPInt_t Y;
		fPInt_t XAdd;
		fPInt_t YAdd;
		int IX;
		int IY;

		int BornFrame; // zd
		int DeadFrame; // zd
		int FloorClearFrame; // zd

		int WalkFrame; // zd
		int SlowFrame; // zd

		// PlayerWallProc() ret {
		int OnGround;
		int TouchRoofOn;
		Touch_t TouchSideOn;
		int OnDeath;
		int OnGoal;
		// }

		int JumpFrame; // zd
	}
	Player;

	// Player's Born Pos
	fPInt_t BornX;
	fPInt_t BornY;

	int CamSlideBtnOn;
	int CamSlid;
	int CamSlideTrueX;
	int CamSlideTrueY;
	int CamSlideRealX;
	int CamSlideRealY;

	struct
	{
		int RestartFlag;
		int BackToMenuFlag;
	}
	Pause;

	struct
	{
		int LeftWindowFlag;
		int Win_L;
		int Win_T;

		int MenuClicked;
		int ClickedCellType;

		int SelectCellType;
	}
	Edit;

	/*
	ret:
		1 - タイトルへ戻るを選択した
		0 - その他
	*/
	int ReturnMenuFlag;

	/*
	ret:
		1 - クリアした
		0 - その他 ... タイトルへ戻るを選択した or リプレイで死亡した
	*/
	int FloorClearFlag;

	// zantei warp
	fPInt_t WarpX;
	fPInt_t WarpY;

	int PlaySeekPos;

	GameInput_t CurrInput; // 1 <= GetInput(inpId);   <- 注意！
	GameInput_t LastInput;
	GameInput_t CurrPound; // GetPount(inpId);

	int FreezeAButton; // zd
	int FreezeRestart; // zd
	int LastDeadReplaySaveOk;

	// Use: ChangeInfoCaption()
	char *InfoCaption;
	int InfoCaptionFrame; // zd

	struct
	{
		int Frame; // zd
		taskList *EL;
		SubScreen_t *WallScreen;
		SubScreen_t *ExWallScreen;
		SubScreen_t *FieldScreen;
	}
	Meteor;

	double QuakePower;

	int JmpBtnDwnFrm; // zd
	int JmpBtnUpFlg;

	int PlayFrameCount;
	int FloorClearFreezeFlag;
}
GDc_t;

extern GDc_t GDc;

void GameInit(void);
void GameFnlz(void);

// ----

enum // GraphicalMode
{
	GM_DEFAULT, // as 0
	GM_01,
	GM_02,
	GM_03,
	GM_04,
	GM_04_SOFT,
	GM_METEOR,
	GM_METEOR_SOFT,
	GM_EARTH,
	GM_GHOST,

	GM_MAX, // num of member
};

char *GetGraphicalModeCaption(int grphMd);

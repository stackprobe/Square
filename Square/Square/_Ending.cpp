#include "all.h"

static Random *Rnd;

static void DrawWall_old(void)
{
	DPE_SetBright(0, 0, 0);
	DrawRect(P_WHITEBOX, 0, 0, SCREEN_W, SCREEN_H);
	DPE_Reset();

	double rate;

	ifsceneBegin(600, ActFrame)
	{
		rate = 0.0;
	}
	ifscene(6000)
	{
		rate = sc_rate;
	}
	ifsceneEnd
	{
		rate = 1.0;
	}
	sceneLeave();

	if(Rnd->DRnd() < 0.03 + 0.07 * rate)
	{
		if(Rnd->DRnd() < rate)
			CEE_SetBright(Rnd->DRnd(), Rnd->DRnd(), Rnd->DRnd());

		/*
			星の画像は 120x120 だけど絵は 50x50 程度, 最大 x3 -> 最大 150x150 -> 対角線 212 程度
			300x300 は超えないと思う -> 150 で画面外に出るはず。
		*/
		CEE.ScreenArea.T = -150;

		AddCommonEffect(
			Gnd.EL,
			0,
			D_STAR_W_00 + 6 | DTP,
			Rnd->DRnd() * SCREEN_W,
			SCREEN_H + 100.0,
			0.0,
			Rnd->DRnd() * 2.0 + 1.0,
			0.3,
			Rnd->ERnd() * 0.01,
			Rnd->DRnd() * -1.0 - 0.5,
			Rnd->ERnd() * 0.01
			);
		CEE_Reset();
	}

	DrawCenter(P_ALLFLOORCLEARED, 400, 100);
	DrawCenter(P_CONGRATULATION, 400, 200);
	DrawCenter(P_THANKYOUFORPLAYING, 400, 500);
}
static void DrawWall(void)
{
	DrawRect(P_ENDING_1, 0, 0, SCREEN_W, SCREEN_H);
//	DrawRect(P_ENDING_2, 0, 0, SCREEN_W, SCREEN_H);

	double rate;

	ifsceneBegin(600, ActFrame)
	{
		rate = 0.0;
	}
	ifscene(60 * 20)
	{
		rate = sc_rate;
	}
	ifsceneEnd
	{
		rate = 1.0;
	}
	sceneLeave();

	if(Rnd->DRnd() < 0.005 + 0.015 * rate)
	{
		if(Rnd->DRnd() < rate)
			CEE_SetBright(Rnd->DRnd(), Rnd->DRnd(), Rnd->DRnd());

		CEE.ScreenArea.T = -200;

		AddCommonEffect(
			Gnd.EL,
			0,
			D_STAR_W_00 + 6 | DTP,
			Rnd->DRnd() * SCREEN_W,
			SCREEN_H + 100.0,
			0.0,
			Rnd->DRnd() * 1.0 + 0.5,
			0.3,
			Rnd->ERnd() * 0.01,
			Rnd->DRnd() * -1.0 - 0.5,
			Rnd->ERnd() * 0.01
			);
		CEE_Reset();
	}
}
void Ending(void)
{
	Rnd = new Random((int)time(NULL));

	// <-- init

	SetCurtain(0, -1.0);
	ActFrame = 0;

	forscene(60)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();

//	SetCurtain(200); // old
	SetCurtain(400);
	MusicPlay(MUS_ENDING);

	for(; ; )
	{
		if(120 < ActFrame && (GetInput(INP_A) || GetInput(INP_B)))
		{
			break;
		}

#if LOG_ENABLED
//		clsDx();
//		printfDx("%d\n", Gnd.EL->GetCount());
#endif

		DrawWall();
		EachFrame();
	}
	MusicFade(200);
	SetCurtain(200, -1.0);

	forscene(230)
	{
		DrawWall();
		EachFrame();
	}
	sceneLeave();

	Gnd.EL->Clear(); // 星を消さないと残っちゃうよ。

	// fnlz -->

	delete Rnd;
}

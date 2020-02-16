#include "all.h"

static autoList<SubScreen_t *> *LogoChars;
static d2D_t *LCPosList;
static Random *Rnd;
static __int64 StartTime;

void Logo(void)
{
	LogoChars = new autoList<SubScreen_t *>();

	for(char *p = GetDatString(DATSTR_AUTHOR); *p; p++) // init LogoChars
	{
		SubScreen_t *ss = CreateSubScreen(10, 16, 1);

		ChangeDrawScreen(ss);

		SetPrint(1, 1);
		PE_Border(GetColor(41, 86, 178)); // == cerulean blue
		Print_x(xcout("%c", *p));
		PE_Reset();

		RestoreDrawScreen();

		LogoChars->AddElement(ss);
	}
	LCPosList = na_(d2D_t, LogoChars->GetCount());
	Rnd = new Random((int)time(NULL));

	for(int index = 0; index < LogoChars->GetCount(); index++)
	{
		d2D_t *pos = LCPosList + index;

		const int RANGE = 1000;

		do
		{
			pos->X = (double)Rnd->BRnd(-RANGE, RANGE + SCREEN_W);
			pos->Y = (double)Rnd->BRnd(-RANGE, RANGE + SCREEN_H);
		}
		while(m_isRange(pos->X, 0, SCREEN_W) || m_isRange(pos->Y, 0, SCREEN_H));
	}
	for(int frame = 0; ; frame++)
	{
		if(frame == 1)
		{
			StartTime = FrameStartTime;
		}
		if(frame == 70) // touch...
		{
			MusicTouch(MUS_TITLE);
			MusicTouch(MUS_FLOOR_01);
			MusicTouch(MUS_ENDING);
		}
		if(80 < frame && StartTime + 2000 < FrameStartTime)
		{
			m_maxim(frame, 120);
		}
		if(frame == 120)
		{
			SetCurtain(60, -1.0);
		}
		if(frame == 200)
		{
			break;
		}

		DPE_SetAlpha(frame < 60 ? 0.1 : 1.0);
		DPE_SetBright(0, 0, 0);
		DrawRect(P_WHITEBOX, 0, 0, SCREEN_W, SCREEN_H);
		DPE_Reset();

		for(int index = 0; index < LogoChars->GetCount(); index++)
		{
			const int LOGO_L = 135;
			const int LOGO_T = 284;
			const int LOGO_SPAN = 30;

			int x = LOGO_L + index * LOGO_SPAN;
			int y = LOGO_T;

			d2D_t *pos = LCPosList + index;

			m_approach(pos->X, x, 0.88);
			m_approach(pos->Y, y, 0.88);

			x = m_d2i(pos->X);
			y = m_d2i(pos->Y);

			SetDrawMode(DX_DRAWMODE_NEAREST);
			DrawExtendGraph(x, y, x + 20, y + 32, GetHandle(LogoChars->GetElement(index)), 1);
			SetDrawMode(DX_DRAWMODE_BILINEAR); // def
		}
		EachFrame();
	}
	releaseList(LogoChars, ReleaseSubScreen);
	memFree(LCPosList);
	delete Rnd;
}

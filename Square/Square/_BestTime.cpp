#include "all.h"

// ---- rank ----

static const int SRankFrmCntList[FLOOR_NUM] =
{
	60 *  7, // 1
	60 * 24, // 2
	60 * 27, // 3
	60 * 25, // 4
	60 * 48, // 5
	60 * 48, // 6
	60 * 58, // 7
	60 * 65, // 8
	60 * 25, // 9
};

static int GetSRankFrmCnt(int floorIndex)
{
	errorCase(!m_isRange(floorIndex, 0, FLOOR_NUM - 1));
	return SRankFrmCntList[floorIndex];
}
static int GetSRankTotalFrmCnt(void)
{
	int total = 0;

	for(int index = 0; index < FLOOR_NUM; index++)
		total += SRankFrmCntList[index];

	return total;
}
static int FrmCntToRankInt(int frmcnt, int frmcntSRank) // ret: -1 == –¢ƒvƒŒƒC, 0 ` 6 == { S, A ` F }
{
	m_range(frmcnt, -1, IMAX);
	m_range(frmcntSRank, 0, IMAX);

	if(frmcnt == -1) // ? –¢ƒvƒŒƒC
		return -1;

	int d = frmcnt - frmcntSRank;

	if(60 * 120 < d)
		return 6; // F

	if(60 * 60 < d)
		return 5; // E

	if(60 * 30 < d)
		return 4; // D

	if(60 * 15 < d)
		return 3; // C

	if(60 * 5 < d)
		return 2; // B

	if(0 < d)
		return 1; // A

	return 0; // S
}
static char *FrmCntToRank(int frmcnt, int frmcntSRank) // ret: bind
{
	static char *retList[] =
	{
		"--", // -1
		"‚r", // 0
		"‚`", // 1
		"‚a", // 2
		"‚b", // 3
		"‚c", // 4
		"‚d", // 5
		"‚e", // 6
	};
	int ret = FrmCntToRankInt(frmcnt, frmcntSRank);
	errorCase(!m_isRange(ret, -1, 6));
	return retList[ret + 1];
}

// ----

int BT_FrameCount;
int BT_Minute;
int BT_Second;
int BT_Millis;

char *BT_Parse(int frmcnt) // ret: bind
{
	m_range(frmcnt, -1, IMAX);

	BT_FrameCount = frmcnt;

	static char *ret;

	if(!ret)
		ret = strx("[99:99.999]");

	if(frmcnt == -1)
	{
		BT_Minute = 0;
		BT_Second = 0;
		BT_Millis = 0;

		strcpy(ret, "[--:--.---]");
	}
	else
	{
		BT_Millis = frmcnt % 60;
		BT_Millis *= 1000;
		BT_Millis /= 60;
		frmcnt /= 60;
		BT_Second = frmcnt % 60;
		frmcnt /= 60;
		BT_Minute = frmcnt;
		m_minim(BT_Minute, 99);

		sprintf(ret, "[%02d:%02d.%03d]", BT_Minute, BT_Second, BT_Millis);
	}
	return ret;
}
char *BT_ParseMillis(int millis) // ret: bind
{
	m_range(millis, -1, IMAX);

	static char *ret;

	if(!ret)
		ret = strx("[99:99.999]");

	if(millis == -1)
	{
		strcpy(ret, "[--:--.---]");
	}
	else
	{
		int l = millis % 1000;
		millis /= 1000;
		int s = millis % 60;
		millis /= 60;
		int m = millis;
		m_minim(m, 99);

		sprintf(ret, "[%02d:%02d.%03d]", m, s, l);
	}
	return ret;
}
int BT_GetRankInt(int floorIndex, int frmcnt)
{
	return FrmCntToRankInt(frmcnt, GetSRankFrmCnt(floorIndex));
}
char *BT_GetRank(int floorIndex, int frmcnt) // ret: bind
{
	return FrmCntToRank(frmcnt, GetSRankFrmCnt(floorIndex));
}

int BT_GetBestFrmCnt(int floorIndex)
{
	errorCase(!m_isRange(floorIndex, 0, FLOOR_NUM - 1));

	return Gnd.BestTimeList[floorIndex];
}
void BT_SetBestFrmCnt(int floorIndex, int frmcnt)
{
	errorCase(!m_isRange(floorIndex, 0, FLOOR_NUM - 1));

	m_range(frmcnt, -1, IMAX);
//	errorCase(!m_isRange(frmcnt, -1, IMAX));

	Gnd.BestTimeList[floorIndex] = frmcnt;
}

// ---- total ----

int BT_GetTotalMillis(void)
{
	int total = 0;

	for(int index = 0; index < FLOOR_NUM; index++)
	{
		int frmcnt = Gnd.BestTimeList[index];

		if(frmcnt == -1)
			return -1;

		int millis;

		// frmcnt -> millis
		{
			m_range(frmcnt, 0, IMAX);

			int l = frmcnt % 60;
			int h = frmcnt / 60;

			m_range(h, 0, IMAX / 1000);

			l *= 1000;
			l /= 60;
			h *= 1000;

			millis = l + h;
		}

		total += millis;
		m_minim(millis, IMAX);
	}
	return total;
}
int BT_MillisToFrameCount(int millis) // ret: •b’PˆÊA’[”Ø‚èã‚°I
{
	if(millis == -1)
		return -1;

	int ret = millis;

	m_range(ret, 0, IMAX);

	ret += 999;
	ret /= 1000;
	ret *= 60;

	/*
#if LOG_ENABLED
	clsDx();
	printfDx("BT_M2FC: %d -> %d\n", millis, ret);
#endif
	*/
	return ret;
}
int BT_GetTotalRankInt(void)
{
	return FrmCntToRankInt(BT_MillisToFrameCount(BT_GetTotalMillis()), GetSRankTotalFrmCnt());
}
char *BT_GetTotalRank(void) // ret: bind
{
	return FrmCntToRank(BT_MillisToFrameCount(BT_GetTotalMillis()), GetSRankTotalFrmCnt());
}

// ----

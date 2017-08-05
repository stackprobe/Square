enum
{
	SE_DUMMY,

	// app >

	SE_FIRE,
	SE_JUMP,
	SE_MISS,
	SE_START,
	SE_PAUSE_IN,
	SE_PAUSE_OUT,
	SE_RESTART,
	SE_FLOORCLEAR,
	SE_ACCEPT,
	SE_BOMB,
	SE_GUITAR,
	SE_BESTTIME_TEST_01,
	SE_BESTTIME,

	// < app

	SE_MAX, // num of member
};

#define SE_HANDLE_MAX 64

typedef struct SEInfo_st
{
	int HandleList[SE_HANDLE_MAX];
	int PrvVolumePctList[SE_HANDLE_MAX]; // 0 - 100, def: -1 == no change
	int HandleIndex;
	double Volume; // 0.0 - 1.0, def: 0.5
}
SEInfo_t;

int SEEachFrame(void);
void SEPlay(int seId, int prvVolumePct = -1);
void UpdateSEVolume(void);

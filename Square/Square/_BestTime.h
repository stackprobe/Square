extern int BT_FrameCount;
extern int BT_Minute;
extern int BT_Second;
extern int BT_Millis;

char *BT_Parse(int frmcnt);
char *BT_ParseMillis(int millis);
int BT_GetRankInt(int floorIndex, int frmcnt);
char *BT_GetRank(int floorIndex, int frmcnt);

int BT_GetBestFrmCnt(int floorIndex);
void BT_SetBestFrmCnt(int floorIndex, int frmcnt);

// ---- total ----

int BT_GetTotalMillis(void);
int BT_MillisToFrameCount(int millis);
int BT_GetTotalRankInt(void);
char *BT_GetTotalRank(void);

// ----

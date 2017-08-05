typedef struct FourCell_st
{
	MCell_t *LT;
	MCell_t *RT;
	MCell_t *LB;
	MCell_t *RB;

	/*
		LT, RT, LB, RB ÇÃíÜêSÇ©ÇÁÇÃç∑ï™
	*/
	fPInt_t X;
	fPInt_t Y;

	/*
		LT ÇÃÉZÉãç¿ïW
	*/
	int LTCellX;
	int LTCellY;
}
FourCell_t;

i2D_t GetScreenCellPos(fPInt_t x, fPInt_t y);

FourCell_t GetFourCell(fPInt_t x, fPInt_t y);
void HTurn(FourCell_t &fc);
void VTurn(FourCell_t &fc);
void Rot180(FourCell_t &fc);

void DrawWindowFrame(int frmDerId_00, int l, int t, int w, int h);

void DrawMapChipWater(
	int dr_x,
	int dr_y,
	int lt_derId,
	int mc_w,
	MCell_t *cell,
	int (*isWater)(MCell_t *, int, int),
	int animeMax = 1,
	int komaFrmMax = 1
	);

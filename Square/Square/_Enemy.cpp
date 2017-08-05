#include "all.h"

static int IsEnemyWall(MCell_t *cell)
{
	return
		IsPlayerWall(cell->CellType) ||
		cell->CellType == CT_DEATH ||
		cell->CellType == CT_GOAL;
}
static void AddEnemyCrash(Enemy_t *i)
{
	AddEnemyCrash(
		i->X - (CELLSIZE / 2) * IDENOM,
		i->Y - (CELLSIZE / 2) * IDENOM,
		CELLSIZE * IDENOM,
		CELLSIZE * IDENOM
		);
}
static void AdjustEnemyPos(Enemy_t *i)
{
	const fPInt_t FLD_W = GDc.Map->GetWidth() * CELLSIZE * IDENOM;
	const fPInt_t FLD_H = GDc.Map->GetHeight() * CELLSIZE * IDENOM;
	const fPInt_t HC = (CELLSIZE / 2) * IDENOM;

	if(i->X < -HC) i->X += HC + FLD_W + HC;
	if(i->Y < -HC) i->Y += HC + FLD_H + HC;
	if(FLD_W + HC < i->X) i->X -= HC + FLD_W + HC;
	if(FLD_H + HC < i->Y) i->Y -= HC + FLD_H + HC;
}
static void EachFrameArkanoid(Enemy_t *i, int initDir)
{
	if(!i->Frame) // init
	{
		int x = 0;
		int y = 0;

		switch(initDir)
		{
		case 6: x =  1; y =  0; break;
		case 3: x =  1; y =  1; break;
		case 2: x =  0; y =  1; break;
		case 1: x = -1; y =  1; break;
		case 4: x = -1; y =  0; break;
		case 7: x = -1; y = -1; break;
		case 8: x =  0; y = -1; break;
		case 9: x =  1; y = -1; break;

		default:
			error();
		}
		i->u.Arkanoid.XAddSign = x;
		i->u.Arkanoid.YAddSign = y;
	}
	const fPInt_t SPEED = 3200;
	const fPInt_t NANAME_SPEED = 2500;

	fPInt_t speed = SPEED;

	if(i->u.Arkanoid.XAddSign && i->u.Arkanoid.YAddSign)
		speed = NANAME_SPEED;

	i->X += speed * i->u.Arkanoid.XAddSign;
	i->Y += speed * i->u.Arkanoid.YAddSign;

	AdjustEnemyPos(i);

	FourCell_t fc = GetFourCell(i->X, i->Y);

	if(
		fc.LT != NULL &&
		fc.RT != NULL &&
		fc.LB != NULL &&
		fc.RB != NULL
		)
	{
		int ltw = IsEnemyWall(fc.LT);
		int rtw = IsEnemyWall(fc.RT);
		int lbw = IsEnemyWall(fc.LB);
		int rbw = IsEnemyWall(fc.RB);

		const fPInt_t hcrng = (CELLSIZE / 2) * IDENOM;

		if(i->u.Arkanoid.XAddSign && i->u.Arkanoid.YAddSign)
		{
			FourCell_t fc_orig = fc;

			if(i->u.Arkanoid.XAddSign == -1) HTurn(fc);
			if(i->u.Arkanoid.YAddSign == -1) VTurn(fc);

			// fc == 右下へ向かっている。

			int xHanSign = 1;
			int yHanSign = 1;

			switch(
				(IsEnemyWall(fc.LT) ? 1 : 0) |
				(IsEnemyWall(fc.RT) ? 2 : 0) |
				(IsEnemyWall(fc.LB) ? 4 : 0) |
				(IsEnemyWall(fc.RB) ? 8 : 0)
				)
			{
			case 0: // []
			case 1: // [7]
			case 3: // [79]
			case 5: // [17]
			case 7: // [179]
			case 9: // [37]
			case 11: // [379]
			case 13: // [137]
			case 15: // [1379]
				break;

			case 2: // [9]
				if(fc.Y < fc.X + hcrng) xHanSign *= -1;
				break;

			case 4: // [1]
				if(fc.X < fc.Y + hcrng) yHanSign *= -1;
				break;

			case 8: // [3]
				{
					fPInt_t subXY = fc.X - fc.Y;
					const fPInt_t BORDER = 4 * IDENOM;

					if(subXY < -BORDER) // ? 左下方向に大きくずれている。
					{
						xHanSign *= -1;
						break;
					}
					if(BORDER < subXY) // ? 右上方向に大きくずれている。
					{
						yHanSign *= -1;
						break;
					}
				}
				// fall through

			case 6: // [19]
			case 14: // [139]
				xHanSign *= -1;
				yHanSign *= -1;
				break;

			case 10: // [39]
				xHanSign *= -1;
				break;

			case 12: // [13]
				yHanSign *= -1;
				break;

			default:
				error();
			}
			fc = fc_orig;

			if(xHanSign == -1)
			{
				i->u.Arkanoid.XAddSign *= -1;
			}
			if(yHanSign == -1)
			{
				i->u.Arkanoid.YAddSign *= -1;
			}
			if(xHanSign == -1 || yHanSign == -1)
			{
				/*
					苦肉の策orz
					跳ね返るときセルの正位置に矯正する。
					案外これで正解なのかも...
				*/

				i->X /= CELLSIZE * IDENOM;
				i->X *= CELLSIZE * IDENOM;
				i->X += (CELLSIZE / 2) * IDENOM;

				i->Y /= CELLSIZE * IDENOM;
				i->Y *= CELLSIZE * IDENOM;
				i->Y += (CELLSIZE / 2) * IDENOM;
			}
		}
		else
		{
			if(
				i->u.Arkanoid.XAddSign == -1 && (ltw || lbw && fc.Y != -hcrng) ||
				i->u.Arkanoid.XAddSign ==  1 && (rtw || rbw && fc.Y != -hcrng)
				)
				i->u.Arkanoid.XAddSign *= -1;

			if(
				i->u.Arkanoid.YAddSign == -1 && (ltw || rtw && fc.X != -hcrng) ||
				i->u.Arkanoid.YAddSign ==  1 && (lbw || rbw && fc.X != -hcrng)
				)
				i->u.Arkanoid.YAddSign *= -1;
		}
	}

	AddEnemyCrash(i);
}
static void EachFramePata(Enemy_t *i, fPInt_t initXAdd)
{
	if(!i->Frame) // init
	{
		i->u.Pata.InitY = i->Y;

		i->u.Pata.XAdd = initXAdd;
		i->u.Pata.YAdd = 0;
	}
	const fPInt_t GRAVITY = 900;
	const fPInt_t FALL_SPEED_MAX = 30000;

	i->X += i->u.Pata.XAdd;
	i->Y += i->u.Pata.YAdd;

	if(i->Y < i->u.Pata.InitY)
	{
		m_iApproach(i->u.Pata.YAdd, GRAVITY, 9, 10);
	}
	m_minim(i->u.Pata.YAdd, FALL_SPEED_MAX);

	AdjustEnemyPos(i);
	AddEnemyCrash(i);

	FourCell_t fc = GetFourCell(i->X, i->Y);

	if(
		fc.LT != NULL &&
		fc.RT != NULL &&
		fc.LB != NULL &&
		fc.RB != NULL
		)
	{
		int ltw = IsEnemyWall(fc.LT);
		int rtw = IsEnemyWall(fc.RT);
		int lbw = IsEnemyWall(fc.LB);
		int rbw = IsEnemyWall(fc.RB);

		if(
			!ltw && lbw ||
			!rtw && rbw
			)
		{
			i->u.Pata.YAdd = -abs(i->u.Pata.YAdd);
			goto endBounce;
		}
		if(ltw && rtw)
		{
			i->u.Pata.YAdd = abs(i->u.Pata.YAdd);
			goto endBounce;
		}
		i->u.Pata.YAdd += GRAVITY;

endBounce:;
		if(!ltw || !rtw)
		{
			if(ltw) i->u.Pata.XAdd = abs(i->u.Pata.XAdd);
			if(rtw) i->u.Pata.XAdd = -abs(i->u.Pata.XAdd);
		}
		if(-CELLSIZE + 4 * IDENOM < fc.Y && (!lbw || !rbw))
		{
			if(lbw) i->u.Pata.XAdd = abs(i->u.Pata.XAdd);
			if(rbw) i->u.Pata.XAdd = -abs(i->u.Pata.XAdd);
		}
	}
}
static void EachFrameCookie(Enemy_t *i, int initRotAddSign, int initRotDir)
{
	if(!i->Frame) // init
	{
		i->u.Cookie.AxisX = i->X;
		i->u.Cookie.AxisY = i->Y;

		fPInt_t r = 0;

		switch(initRotDir)
		{
		case 6: r = 0 * IDENOM / 2; break;
		case 3: r = 1 * IDENOM / 2; break;
		case 2: r = 2 * IDENOM / 2; break;
		case 1: r = 3 * IDENOM / 2; break;
		case 4: r = 4 * IDENOM / 2; break;
		case 7: r = 5 * IDENOM / 2; break;
		case 8: r = 6 * IDENOM / 2; break;
		case 9: r = 7 * IDENOM / 2; break;

		default:
			error();
		}
		i->u.Cookie.R = r;
		i->u.Cookie.RAddSign = initRotAddSign;
	}
	const fPInt_t ROTSPEED = 30;
	const fPInt_t ROTRADIUS = 152000;

	i->u.Cookie.R += ROTSPEED * i->u.Cookie.RAddSign;

	     if(i->u.Cookie.R < -IDENOM * 4) i->u.Cookie.R += IDENOM * 4;
	else if(i->u.Cookie.R >  IDENOM * 4) i->u.Cookie.R -= IDENOM * 4;

	i->X = i->u.Cookie.AxisX + iCos(i->u.Cookie.R, ROTRADIUS);
	i->Y = i->u.Cookie.AxisY + iSin(i->u.Cookie.R, ROTRADIUS);

	if(!i->Frame)
//	if(!i->Frame && IsInCamera(m_fpi2i(i->X), m_fpi2i(i->Y), 100)) // 保留
	{
		for(int c = 0; c < 10; c++)
		{
			double s = 2.0 + 2.0 * dRnd();
			double r = PI * 2.0 * (c / 10.0) + 0.3 * eRnd();

			CEE_SetBright(0.3, 1.0, 0.2);
			AddCommonEffect(
				Gnd.EL,
				0,
				D_STAR_W_00 + 6 | DTP,
				m_fpi2i(i->X),
				m_fpi2i(i->Y),
				PI * 2.0 * dRnd(),
				0.5 + 0.25 * dRnd(),
				0.8,
				cos(r) * s,
				sin(r) * s,
				0.1 * eRnd(),
				0.0,
				-0.02
				);
			CEE_Reset();
		}
	}
	AddEnemyCrash(i);
}

/*
	fPInt_t
*/
#define PATA_SPEED 4000
#define PATA_SPEED_SLOW 2500
#define PATA_SPEED_HISPEED 7000

void EachFrameEnemy(Enemy_t *i) // ret: death
{
	i->Frame++; // to 0...

	switch(i->EnemyType)
	{
	case ET_ARKANOID_1: EachFrameArkanoid(i, 1); break;
	case ET_ARKANOID_2: EachFrameArkanoid(i, 2); break;
	case ET_ARKANOID_3: EachFrameArkanoid(i, 3); break;
	case ET_ARKANOID_4: EachFrameArkanoid(i, 4); break;
	case ET_ARKANOID_6: EachFrameArkanoid(i, 6); break;
	case ET_ARKANOID_7: EachFrameArkanoid(i, 7); break;
	case ET_ARKANOID_8: EachFrameArkanoid(i, 8); break;
	case ET_ARKANOID_9: EachFrameArkanoid(i, 9); break;

	case ET_LPATA: EachFramePata(i, -PATA_SPEED); break;
	case ET_LPATA_SLOW: EachFramePata(i, -PATA_SPEED_SLOW); break;
	case ET_LPATA_HISPEED: EachFramePata(i, -PATA_SPEED_HISPEED); break;
	case ET_RPATA: EachFramePata(i, PATA_SPEED); break;
	case ET_RPATA_SLOW: EachFramePata(i, PATA_SPEED_SLOW); break;
	case ET_RPATA_HISPEED: EachFramePata(i, PATA_SPEED_HISPEED); break;

	case ET_COOKIE_1: EachFrameCookie(i, 1, 1); break;
	case ET_COOKIE_2: EachFrameCookie(i, 1, 2); break;
	case ET_COOKIE_3: EachFrameCookie(i, 1, 3); break;
	case ET_COOKIE_4: EachFrameCookie(i, 1, 4); break;
	case ET_COOKIE_6: EachFrameCookie(i, 1, 6); break;
	case ET_COOKIE_7: EachFrameCookie(i, 1, 7); break;
	case ET_COOKIE_8: EachFrameCookie(i, 1, 8); break;
	case ET_COOKIE_9: EachFrameCookie(i, 1, 9); break;
	case ET_HTCOOKIE_1: EachFrameCookie(i, -1, 1); break;
	case ET_HTCOOKIE_2: EachFrameCookie(i, -1, 2); break;
	case ET_HTCOOKIE_3: EachFrameCookie(i, -1, 3); break;
	case ET_HTCOOKIE_4: EachFrameCookie(i, -1, 4); break;
	case ET_HTCOOKIE_6: EachFrameCookie(i, -1, 6); break;
	case ET_HTCOOKIE_7: EachFrameCookie(i, -1, 7); break;
	case ET_HTCOOKIE_8: EachFrameCookie(i, -1, 8); break;
	case ET_HTCOOKIE_9: EachFrameCookie(i, -1, 9); break;

	default:
		error();
	}
}
void EachFrameAllEnemy(void)
{
	ResetEnemyCrash();

	for(int index = 0; index < GDc.EnemyList->GetCount(); index++)
	{
		EachFrameEnemy(GDc.EnemyList->GetElement(index));
	}
}

//----

static void DrawArkanoid(Enemy_t *i)
{
	i2D_t pos = GetScreenCellPos(i->X, i->Y);

	switch(Gnd.GraphicalMode)
	{
	case GM_DEFAULT:
	case GM_METEOR:
	case GM_METEOR_SOFT:
	case GM_GHOST:
		DPE_SetBright(0.2, 0.3, 1.0);
		DrawRect(P_WHITEBOX, pos.X, pos.Y, CELLSIZE, CELLSIZE);
		DPE_Reset();
		break;

	case GM_01:
		DrawSimple(D_MAPCHIP_D_00 + 186 | DTP, pos.X, pos.Y);
		break;

	case GM_02:
	case GM_03:
	case GM_04:
	case GM_04_SOFT:
	case GM_EARTH:
		DrawSimple(D_MAPCHIP_B_00 + 57 | DTP, pos.X, pos.Y);
		break;

	default:
		error();
	}
}
static void DrawPata(Enemy_t *i)
{
	i2D_t pos = GetScreenCellPos(i->X, i->Y);

	switch(Gnd.GraphicalMode)
	{
	case GM_DEFAULT:
	case GM_METEOR:
	case GM_METEOR_SOFT:
	case GM_GHOST:
		DPE_SetBright(1.0, 0.2, 0.3);
		DrawRect(P_WHITEBOX, pos.X, pos.Y, CELLSIZE, CELLSIZE);
		DPE_Reset();
		break;

	case GM_01:
		DrawSimple(D_MAPCHIP_D_00 + 170 | DTP, pos.X, pos.Y);
		break;

	case GM_02:
	case GM_03:
	case GM_04:
	case GM_04_SOFT:
	case GM_EARTH:
		DrawSimple(D_MAPCHIP_B_00 + 42 | DTP, pos.X, pos.Y);
		break;

	default:
		error();
	}
}
static void DrawCookie(Enemy_t *i)
{
	i2D_t pos = GetScreenCellPos(i->X, i->Y);

	switch(Gnd.GraphicalMode)
	{
	case GM_DEFAULT:
	case GM_METEOR:
	case GM_METEOR_SOFT:
	case GM_GHOST:
		DPE_SetBright(0.3, 1.0, 0.2);
		DrawRect(P_WHITEBOX, pos.X, pos.Y, CELLSIZE, CELLSIZE);
		DPE_Reset();
		break;

	case GM_01:
		DrawSimple(D_MAPCHIP_E_00 + 209 | DTP, pos.X, pos.Y);
		break;

	case GM_02:
	case GM_03:
	case GM_04:
	case GM_04_SOFT:
	case GM_EARTH:
		DrawSimple(D_MAPCHIP_B_00 + 58 | DTP, pos.X, pos.Y);
		break;

	default:
		error();
	}
}

void DrawEnemy(Enemy_t *i)
{
	if(i->Frame == -1) // ? uninited
		return;

	{
		i2D_t pos = GetScreenCellPos(i->X, i->Y);
		const int MARGIN = 50;

		// ? out of screen
		if(
			pos.X < -MARGIN || SCREEN_W + MARGIN < pos.X ||
			pos.Y < -MARGIN || SCREEN_H + MARGIN < pos.Y
			)
			return;
	}

	switch(i->EnemyType)
	{
	case ET_ARKANOID_1:
	case ET_ARKANOID_2:
	case ET_ARKANOID_3:
	case ET_ARKANOID_4:
	case ET_ARKANOID_6:
	case ET_ARKANOID_7:
	case ET_ARKANOID_8:
	case ET_ARKANOID_9:
		DrawArkanoid(i);
		break;

	case ET_LPATA:
	case ET_LPATA_SLOW:
	case ET_LPATA_HISPEED:
	case ET_RPATA:
	case ET_RPATA_SLOW:
	case ET_RPATA_HISPEED:
		DrawPata(i);
		break;

	case ET_COOKIE_1:
	case ET_COOKIE_2:
	case ET_COOKIE_3:
	case ET_COOKIE_4:
	case ET_COOKIE_6:
	case ET_COOKIE_7:
	case ET_COOKIE_8:
	case ET_COOKIE_9:
	case ET_HTCOOKIE_1:
	case ET_HTCOOKIE_2:
	case ET_HTCOOKIE_3:
	case ET_HTCOOKIE_4:
	case ET_HTCOOKIE_6:
	case ET_HTCOOKIE_7:
	case ET_HTCOOKIE_8:
	case ET_HTCOOKIE_9:
		DrawCookie(i);
		break;

	default:
		error();
	}
}
void DrawAllEnemy(void)
{
	for(int index = 0; index < GDc.EnemyList->GetCount(); index++)
	{
		DrawEnemy(GDc.EnemyList->GetElement(index));
	}
}

//----

void LoadEnemy(void) // called @ GameMain()
{
	GDc.EnemyList->Clear(ReleaseEnemy);

	for(int x = 0; x < GDc.Map->GetWidth(); x++)
	for(int y = 0; y < GDc.Map->GetHeight(); y++)
	{
		MCell_t *cell = GDc.Map->GetCell(x, y);

		if(m_isRange(cell->CellType, ET_FIRST, ET_LAST))
		{
			GDc.EnemyList->AddElement(CreateEnemy(
				cell->CellType,
				(x * CELLSIZE + CELLSIZE / 2) * IDENOM,
				(y * CELLSIZE + CELLSIZE / 2) * IDENOM
				));
		}
	}
}

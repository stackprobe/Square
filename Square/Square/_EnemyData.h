typedef struct Enemy_st
{
	int EnemyType;
	/*
		センター座標
	*/
	fPInt_t X;
	fPInt_t Y;

	int Frame; // -1 == uninited, 0～

	union
	{
		struct
		{
			int XAddSign; // -1, 0, 1
			int YAddSign; // -1, 0, 1
		}
		Arkanoid;

		struct
		{
			fPInt_t InitY;

			fPInt_t XAdd;
			fPInt_t YAdd;
		}
		Pata;

		struct
		{
			fPInt_t AxisX;
			fPInt_t AxisY;

			fPInt_t R;
			int RAddSign;
		}
		Cookie;
	}
	u;
}
Enemy_t;

Enemy_t *CreateEnemy(int enemyType, fPInt_t x, fPInt_t y);
void ReleaseEnemy(Enemy_t *i);

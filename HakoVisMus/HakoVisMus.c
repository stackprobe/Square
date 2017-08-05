#include "C:\Factory\Common\all.h"
#include "C:\Factory\OpenSource\md5.h"

static void Set01(uint address)
{
	autoBlock_t *fileData = readBinary("Hako.exe");

	errorCase(getByte(fileData, address - 1) != 0x6a); // 2bs
	errorCase(getByte(fileData, address + 0) != 0x00); // 2bs
	errorCase(getByte(fileData, address + 1) != 0xc7); // 2bs

	setByte(fileData, address, 0x01);

	coExecute("MOVE Hako.exe Hako.exe.old");

	writeBinary_cx("Hako.exe", fileData);
}
int main(int argc, char **argv)
{
	char *hash;

	execute("TITLE マウスカーソル表示パッチ");

	errorCase_m(!existFile("Hako.exe"), "Hako.exe が見つかりません。");

	hash = md5_makeHexHashFile("Hako.exe");

	if(!_stricmp(hash, "00c043a78cfc7b434e373cfa9e5f7b26")) // ? v0.11
	{
		Set01(0x000090c7);
	}
	else if(!_stricmp(hash, "3b42cf5eee17eb7ed12bd5172cb3f39b")) // ? v0.12
	{
		Set01(0x000090c7);
	}
	else if(!_stricmp(hash, "cb8b403c59f14a0444a4a2ea7e29d41a")) // ? v0.13
	{
		Set01(0x000090e7);
	}
	else
	{
		error_m("ハッシュが一致しません。既に適用されているか、v0.11, v0.12, v0.13 ではありません。");
	}
	execute("CLS");
	cout("OK\n");
	sleep(3000);
}

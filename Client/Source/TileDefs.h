#pragma once

struct TileName
{
	enum Value
	{
		Blank = 255,
		Ground1 = 0,
		Ground2 = 1,
		Ground3 = 2,
		Ground4 = 3,
		Ground5 = 4,
		Ground6 = 5,
		Ground7 = 6,
		Ground8 = 7,
		Ground9 = 8,
		Ground10 = 9,
		FenceEndW = 10,
		FenceEndE = 11,
		FenceEndN = 12,
		FenceEndS = 13,
		FenceWE = 14,
		FenceNS = 15,
		FenceWNE = 16,
		FenceNES = 17,
		FenceESW = 18,
		FenceSWN = 19,
		FenceNESW = 20,
		FenceMid = 21,
		Rock1 = 30,
		Rock2 = 31,
		Rock3 = 33,
		SanityCrystal = 90,
		Enemy1 = 100,
		Enemy2 = 101,
		Enemy3 = 102,
		Enemy4 = 103,
		Enemy5 = 104,

		Count = 256,
	};
};
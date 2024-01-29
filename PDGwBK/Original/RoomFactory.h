#include <iostream>
#include <vector>
#include "Room.h"

class RoomFactory
{
	public:
	static std::vector<int> AvailableLockId;
	static std::vector<int> UsedLockId;
	static Room* CreateRoot();
	static Room* CreateRoom();
};

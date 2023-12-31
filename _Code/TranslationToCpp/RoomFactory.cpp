#ifndef ROOMFACTORY_H
#define ROOMFACTORY_H

#include "RoomFactory.h"


std::vector<int> RoomFactory::AvailableLockId;
std::vector<int> RoomFactory::UsedLockId;

Room* RoomFactory::CreateRoot()
{
    AvailableLockId.clear();
    UsedLockId.clear();
    // The root room is always a normal one in 0,0
    return new Room(RoomType::normal, -1, -1);
}

Room* RoomFactory::CreateRoom()
{
    float nodeProbPenalty = 0.0f;
    int keyToOpen;
    Room* room;
    int prob = Constants::Next(101);

    // If there are too many keys without locks, raise the chance to create a lock
    if (AvailableLockId.size() > 0)
    {
        nodeProbPenalty = static_cast<float>(AvailableLockId.size()) * 0.1f;
    }

    if (prob < (Constants::PROB_NORMAL_ROOM - nodeProbPenalty))
    {
        room = new Room(RoomType::normal, -1, -1);
    }
    else if (prob < (Constants::PROB_NORMAL_ROOM + Constants::PROB_KEY_ROOM - nodeProbPenalty))
    {
        room = new Room(RoomType::key, -1, -1);
        // Initialize the room as a key
        AvailableLockId.push_back(room->roomId);
    }
    else
    {
        if (AvailableLockId.size() == 0)
        {
            room = new Room(RoomType::key, -1, -1);
            // Initialize the room as a key
            AvailableLockId.push_back(room->roomId);
        }
        else
        {
            keyToOpen = AvailableLockId[0];
            AvailableLockId.erase(AvailableLockId.begin());
            room = new Room(RoomType::locked, keyToOpen, -1);
            // Initialize the room as locked
            UsedLockId.push_back(room->roomId);
        }
    }

    return room;
}

#endif

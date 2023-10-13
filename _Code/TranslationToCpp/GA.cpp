
#include "Constants.h"
#include "Room.h"
#include "AStar.h"
#include "Dungeon.h"

#include <random>

int main() {

    // Create a Dungeon
    Dungeon dungeon;
    Constants constants;
    // std::cout << Constants::GENERATIONS << "\n\n";
    // TESTING DUNGEON
    // Create Room instances and add them to the RoomList and RoomGrid
    Room* room1 = new Room(0, 0, 0, RoomType::normal, nullptr);
    Room* room2 = new Room(-1, 0, -1, RoomType::key, room1);
    Room* room3 = new Room(0, -1, 0, RoomType::normal, room1);
    Room* room4 = new Room(-1, -1, -2, RoomType::key, room2);
    Room* room5 = new Room(0, -2, 1, RoomType::locked, room3);
    Room* room6 = new Room(1, -1, 0, RoomType::normal, room3);	
    Room* room7 = new Room(2, -1, 0, RoomType::normal, room6);
    Room* room8 = new Room(1, 0, 2, RoomType::locked, room6);
    
    dungeon.roomList.push_back(*room1);
    dungeon.roomList.push_back(*room2);
    dungeon.roomList.push_back(*room3);
    dungeon.roomList.push_back(*room4);
    dungeon.roomList.push_back(*room5);
    dungeon.roomList.push_back(*room6);
    dungeon.roomList.push_back(*room7);
    dungeon.roomList.push_back(*room8);
    
    
    // Initialize the RoomGrid, offseted to 0
    int minX = 0;
    int minY = 0;
    int maxX = 0;
    int maxY = 0;
    
    // Check all the rooms and add them to the keys and locks lists if they are one of them
    for (Room room : dungeon.roomList) {
            if (room.X < minX)
                minX = room.X;
            if (room.Y < minY)
                minY = room.Y;
            if (room.X > maxX)
                maxX = room.X;
            if (room.Y > maxY)
                maxY = room.Y;
    }
        
    int sizeX = maxX - minX + 1;
    int sizeY = maxY-minY + 1;
    
    // Create it full ...
    dungeon.roomGrid.grid.resize(sizeX, std::vector<Room*>(sizeY,nullptr));
    
    dungeon.roomGrid.grid[room1->X- minX][room1->Y- minY] = room1;
    dungeon.roomGrid.grid[room2->X- minX][room2->Y- minY] = room2;
    dungeon.roomGrid.grid[room3->X- minX][room3->Y- minY] = room3;
    dungeon.roomGrid.grid[room4->X- minX][room4->Y- minY] = room4;
    dungeon.roomGrid.grid[room5->X- minX][room5->Y- minY] = room5;
    dungeon.roomGrid.grid[room6->X- minX][room6->Y- minY] = room6;
    dungeon.roomGrid.grid[room7->X- minX][room7->Y- minY] = room7;
    dungeon.roomGrid.grid[room8->X- minX][room8->Y- minY] = room8;
    
    
    int matrixOffset = 150; // magic number !!!
    AStar a;
    
    // std::cout << "room pos " << dungeon.roomGrid.grid[0][0].X << "," << dungeon.roomGrid.grid[0][0].Y << std::endl;
    // int route = a.FindRoute(&dungeon,matrixOffset);
    // std::cout << "Needed locks " << route << std::endl;
    
    /*
    int totalUse = 100 * 1000; // Individuals * Generations
    for(int i = 0; i < totalUse; i++){
	    int route = a.FindRoute(&dungeon,matrixOffset);
	    // std::cout << "Needed locks " << route << std::endl;
    }
    */
    // return route;
    return 0;
}

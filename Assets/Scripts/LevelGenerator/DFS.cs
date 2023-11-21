using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LevelGenerator
{
    class DFS : PathFinding
    {
        //Calls the parent constructor
        public DFS(Dungeon dun)
            : base(dun)
        {
        }

        //The DFS Algorithm
        public int FindRoute()
        {
            //Puts the starting node in the list of open nodes and in the path
            openList.Add(start);
            path.Add(start);

            //Visit all open nodes until none is left
            while (openList.Count > 0)
            {
                
                string so = "";
                foreach (Location loc in openList)
                {
                    so += "[" + loc.X + ", " + loc.Y + "] \n";
                }

                UnityEngine.Debug.Log(so);
                

                // get the first
                current = openList.First();
                UnityEngine.Debug.Log("DFS current [" + current.X + ", " + current.Y + "] to [" + target.X + ", " + target.Y + "]" );
                //Handles key rooms and their locks, if it is one
                validateKeyRoom(current);

                // add the current square to the closed list
                ClosedList.Add(current);
                if (((map[current.X, current.Y] >= 0) && (map[current.X, current.Y] < 100)) || (map[current.X, current.Y] == 102))
                {
                    NVisitedRooms++;
                }
                //Check if the actual room is a locked one. If it is, add 1 to the number of locks needed to reach the goal
                foreach (var locked in allLocksLocation)
                {
                    if (locked.X == current.X && locked.Y == current.Y)
                    {
                        NeededLocks++;
                        break;
                    }
                }

                // remove it from the open list
                openList.Remove(current);

                // if we added the destination to the closed list, we've found a path
                if (ClosedList.Count > 0)
                    if(ClosedList.FirstOrDefault(l => l.X == target.X && l.Y == target.Y) != null)
                        break;

                //Check all adjacent squares from the curret node
                var adjacentSquares = GetWalkableAdjacentSquares(current.X, current.Y, sizeX, sizeY, map);

                //Adds the adjacent squares in a random order (changed to be the same in c++)
                //UnityEngine.Debug.Log(Util.randomSequence.Count  + " " + adjacentSquares.Count);
                // adjacentSquares = adjacentSquares.OrderBy(X => Util.Next(adjacentSquares.Count)).ToList();
                // Shuffle using Fisher-Yates shuffle
                string randoms = "";
                for (int i = adjacentSquares.Count - 1; i > 0; i--)
                {
                    int j = Util.Next(0, i + 1);
                    Location temp = adjacentSquares[i];
                    adjacentSquares[i] = adjacentSquares[j];
                    adjacentSquares[j] = temp;

                    randoms += j + "\n";
                }

                UnityEngine.Debug.Log("Randoms: " + (adjacentSquares.Count - 1) +  ":" + randoms);
                /*
                string s = "";
                foreach (Location loc in adjacentSquares)
                {
                    s += "[" + loc.X + ", " + loc.Y +"] \n";
                }
                
                UnityEngine.Debug.Log(s);
                */
                // UnityEngine.Debug.Log(Util.randomSequence.Count);


                foreach (var adjacentSquare in adjacentSquares)
                {
                    if (current.Parent == adjacentSquare)
                    {
                        adjacentSquares.Remove(adjacentSquare);
                        break;
                    }
                }

                UnityEngine.Debug.Log("Adjacent size " + adjacentSquares.Count);
                foreach (var adjacentSquare in adjacentSquares)
                {
                    // if this adjacent square is already in the closed list, ignore it
                    if (ClosedList.FirstOrDefault(l => l.X == adjacentSquare.X
                            && l.Y == adjacentSquare.Y) != null)
                        continue;

                    // if it's not in the open list...
                    if (openList.FirstOrDefault(l => l.X == adjacentSquare.X
                            && l.Y == adjacentSquare.Y) == null)
                    {

                        adjacentSquare.Parent = current;

                        // and add it to the open list and add to your path
                        // UnityEngine.Debug.Log("adding " + adjacentSquare);
                        openList.Insert(0, adjacentSquare);
                        path.Add(adjacentSquare);
                    }
                    else
                    {
                        adjacentSquare.Parent = current;
                    }
                }
            }
            return NeededLocks;
        }
    }

}

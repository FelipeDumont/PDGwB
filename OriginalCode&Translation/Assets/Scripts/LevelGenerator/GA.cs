using System.Collections;
using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;

namespace LevelGenerator
{
    class GA
    {
        /**
         * Locks for multi-thread operations
         */
        private readonly object dfsLock = new object();
        private readonly object avgRoomLock = new object();

        /**
         * Enum to choose between mutation operations
         */
        public enum MutationOp
        {
            insertChild = 0,
            removeLeaf = 1
        };
        /**
         * Search the tree of rooms to find the number of special rooms. 
         * The key room is saved in the list with its positive ID, while the locked room with its negative value of the ID
         */
        List<int> seedDFS = new List<int>();
        static private void FindNKLR(ref int nRooms, ref List<int> specialRooms, Room root)
        {
            Queue<Room> toVisit = new Queue<Room>();
            specialRooms = new List<int>();
            toVisit.Enqueue(root);
            nRooms = 0;
            while (toVisit.Count > 0)
            {
                nRooms++;
                Room actualRoom = toVisit.Dequeue() as Room;
                Type type;
                type = actualRoom.Type;
                if (type == Type.key) {
                    specialRooms.Add(actualRoom.KeyToOpen);
                }
                else if (type == Type.locked) {
                    specialRooms.Add(-actualRoom.KeyToOpen);
                }
                if (actualRoom.LeftChild != null) {
                    toVisit.Enqueue(actualRoom.LeftChild);
                }
                if (actualRoom.BottomChild != null) {
                    toVisit.Enqueue(actualRoom.BottomChild);
                }
                if (actualRoom.RightChild != null) {
                    toVisit.Enqueue(actualRoom.RightChild);
                }
            }
        }

        /**
         * Changes the selected rooms between the parent dungeons
         * To do so, changes in the parent who is their child to the corresponding node
         */
        static public void ChangeChildren(ref Room cut1, ref Room cut2)
        {
            Room parent = cut1.Parent;
            //Check which child is the cut Room (Right, Bottom, Left)
            try
            {
                switch (cut1.ParentDirection)
                {
                    case Util.Direction.right:
                        parent.RightChild = cut2;
                        break;
                    case Util.Direction.down:
                        parent.BottomChild = cut2;
                        break;
                    case Util.Direction.left:
                        parent.LeftChild = cut2;
                        break;
                    default:
                        Debug.Log("Something went wrong in crossover!.\n");
                        Debug.Log("Direction not supported:\n\tOnly Right, Down and Left are allowed.\n\n");
                        break;
                }
            }
            catch(System.Exception e)
            {
                Debug.Log("Something went wrong while changing the children!");
                throw e;
            }
        }
        /*
         * Choose a random room to switch between the parents and arrange every aspect of the room needed after the change
         * Including the grid, and also the exceptions where the new nodes overlap the old ones
         */
        static public void Crossover(ref Dungeon indOriginal1, ref Dungeon indOriginal2)
        {
            Dungeon ind1, ind2;
            //The root of the branch that will be traded
            Room roomCut1, roomCut2;
            //List of rooms that were the root of the branch and led to an impossible crossover (Tabu List)
            List<Room> failedRooms;
            int prob = Util.Next(100);
            //List of special rooms in the branch to be traded of each parent
            List<int> specialRooms1 = new List<int>(), specialRooms2 = new List<int>();
            //List of special rooms in the traded brach after the crossover
            List<int> newSpecial1 = new List<int>(), newSpecial2 = new List<int>();
            //Total number of rooms in each branch that will be traded
            int nRooms1 = 0, nRooms2 = 0;
            //Answers if the trade is possible or not
            bool isImpossible = false;
            if (prob < Constants.CROSSOVER_RATE)
            {
                do
                {
                    ind1 = indOriginal1.Copy();
                    ind2 = indOriginal2.Copy();


                    //Get a random node from the parent, find the number of keys, locks and rooms and add it to the list of future failed rooms
                    int cutPosition = Util.Next(1, ind1.RoomList.Count);
                    // Debug.Log("Cut position " + cutPosition);
                    roomCut1 = ind1.RoomList[cutPosition];
                    FindNKLR(ref nRooms1, ref specialRooms1, roomCut1);
                    // Debug.Log("R1 = (" + nRooms1 + "," + specialRooms1.Count + ")" + " [" + roomCut1.RoomId + "]");
                    failedRooms = new List<Room>();

                    
                    //While the number of Keys and Locks from a branch is greater than the number of rooms of the other branch,
                    //Redraw the cut point (root of the branch).
                    do
                    {
                        do
                        {
                            int randomID = Util.Next(1, ind2.RoomList.Count);
                            roomCut2 = ind2.RoomList[randomID];
                            // Debug.Log("3+ " + roomCut2.RoomId + " | " + failedRooms.Contains(roomCut2) + "    "+ Util.randomSequence.Count);
                        } while (failedRooms.Contains(roomCut2));
                        failedRooms.Add(roomCut2);
                        if (failedRooms.Count == ind2.RoomList.Count - 1)
                            isImpossible = true;
                        FindNKLR(ref nRooms2, ref specialRooms2, roomCut2);
                        // Debug.Log("Special Check " + nRooms2 + " | " + specialRooms2.Count + " Test " + (specialRooms2.Count > nRooms1) + "||" +(specialRooms1.Count > nRooms2));
                    } while ((specialRooms2.Count > nRooms1 || specialRooms1.Count > nRooms2) && !isImpossible);
                    // Debug.Log("R2= " + nRooms2);
                    
                    
                    //Changes the children of the parent's and neighbor's nodes to the node of the other branch if it is not an impossible trade
                    if (!isImpossible)
                    {
                        
                        try
                        {
                            ChangeChildren(ref roomCut1, ref roomCut2);
                            ChangeChildren(ref roomCut2, ref roomCut1);
                        }
                        catch (System.Exception e)
                        {
                            throw e;
                        }


                        //Change the parent of each node
                        Room auxRoom;
                        //Changes the parents of the chosen nodes
                        auxRoom = roomCut1.Parent;
                        roomCut1.Parent = roomCut2.Parent;
                        roomCut2.Parent = auxRoom;
                        
                        //Remove the node and their children from the grid of the old dungeon
                        ind1.RemoveFromGrid(roomCut1);
                        ind2.RemoveFromGrid(roomCut2);

                        //Update the position, parent's direction and rotation of both nodes that are switched
                        int x = roomCut1.X;
                        int y = roomCut1.Y;
                        Util.Direction dir = roomCut1.ParentDirection;
                        int rotation = roomCut1.Rotation;
                        roomCut1.X = roomCut2.X;
                        roomCut1.Y = roomCut2.Y;
                        roomCut1.ParentDirection = roomCut2.ParentDirection;
                        roomCut1.Rotation = roomCut2.Rotation;
                        roomCut2.X = x;
                        roomCut2.Y = y;
                        roomCut2.ParentDirection = dir;
                        roomCut2.Rotation = rotation;

                        //Updates the grid with all the new nodes. If any conflicts arise, handle them as in the child creation.
                        //That is, any overlap will make the node and its children cease to exist 
                        ind1.RefreshGrid(ref roomCut2);
                        ind2.RefreshGrid(ref roomCut1);

                        //Find the number of keys, locks and rooms in the newly switched branches
                        newSpecial1 = new List<int>();
                        newSpecial2 = new List<int>();
                        FindNKLR(ref nRooms2, ref newSpecial2, roomCut2);
                        FindNKLR(ref nRooms1, ref newSpecial1, roomCut1);
                    }
                    //Debug.Log("Repair ? ("  + (newSpecial1.Count != specialRooms1.Count) + " || " + (newSpecial2.Count != specialRooms2.Count) + " || " +                         (specialRooms1.Count > nRooms2) + " || " + (specialRooms2.Count > nRooms1) + ") && " + !isImpossible);
                    //If in the new branches there are special rooms missing or the number of special rooms is greater then the number of total rooms, retry
                } while ((newSpecial1.Count != specialRooms1.Count || newSpecial2.Count != specialRooms2.Count || specialRooms1.Count > nRooms2 || specialRooms2.Count > nRooms1) && !isImpossible);
                //If the crossover can be done, do it. If not, don't.
                if (!isImpossible)
                {

                    /*
                    Debug.Log("FIX: ");
                    foreach (Room r in ind1.RoomList)
                    {
                        Debug.Log(r.RoomId + " [" + r.X + ", " + r.Y + "]" + r.KeyToOpen);
                    }

                    Debug.Log("B");
                    foreach (Room r in ind2.RoomList)
                    {
                        Debug.Log(r.RoomId + " [" + r.X + ", " + r.Y + "]" + r.KeyToOpen);
                    }
                    */

                    //Debug.Log("Fix branches 1: " + Util.randomSequence.Count);
                    roomCut2.FixBranch(specialRooms1);
                    // Debug.Log("Current in sequence " + Util.randomSequence.Count);
                    // Debug.Log("Fix branches 2: " + Util.randomSequence.Count);
                    roomCut1.FixBranch(specialRooms2);
                    // Debug.Log("Fixed " + Util.randomSequence.Count);
                    // Debug.Log("Current in sequence " + Util.randomSequence.Count);
                    //Fix the list of rooms

                    ind1.FixRoomList();
                    ind2.FixRoomList();
                    // Debug.Log("Roomlist ?? " + Util.randomSequence.Count);
                    /*
                    Debug.Log("FIXed: ");
                    foreach (Room r in ind1.RoomList)
                    {
                        Debug.Log(r.RoomId + " [" + r.X + ", " + r.Y + "]" + r.KeyToOpen);
                    }

                    Debug.Log("B");
                    foreach (Room r in ind2.RoomList)
                    {
                        Debug.Log(r.RoomId + " [" + r.X + ", " + r.Y + "]" + r.KeyToOpen);
                    }
                    */

                }
                //Make a copy of the individual and finish the crossover
                indOriginal1 = ind1.Copy();
                indOriginal2 = ind2.Copy();
            }
            
        }
        /*
         * Mutates the individual by adding or removing a key pair
         * TODO: Where did the mutation to add or remove leaf nodes went to? 'O.O
         */
        public static void Mutation(ref Dungeon individual)
        {
            try
            {
                //Mutate keys, adding or removing a pair
                bool willMutate = Util.Next(101) <= Constants.MUTATION_RATE;
                MutationOp op;
                if (willMutate)
                {
                    op = Util.Next(101) <= Constants.MUTATION0_RATE ? MutationOp.insertChild : MutationOp.removeLeaf;
                    switch (op)
                    {
                        case MutationOp.insertChild:
                            individual.AddLockAndKey();
                            break;
                        case MutationOp.removeLeaf:
                            individual.RemoveLockAndKey();
                            break;  
                    }
                    individual.FixRoomList();
                }
            
            }
            catch (System.Exception e)
            {
                throw e;
            }
        }
        /**
         * A simple tournament to select both parents.
         * Selects two random members from the population and picks the better one.
         * Do it again and breed both champions.
         */
        static public void Tournament(List<Dungeon>pop, ref int parent1, ref int parent2)
        {
            HashSet<int> posHash = new HashSet<int>();
            List<int> parentPosL = new List<int>();
            do
            {
                int pos = Util.Next(pop.Count);
                if (posHash.Add(pos))
                {
                    parentPosL.Add(pos);
                }
                // Debug.Log(pos);
            } while (posHash.Count != 4);
            // Debug.Log("Parent Compare: " + (float)pop[parentPosL[0]].fitness + " < " + (float)pop[parentPosL[1]].fitness + " = " + ((float)pop[parentPosL[0]].fitness < (float)pop[parentPosL[1]].fitness));
            // Debug.Log("Parent Compare: " + (float)pop[parentPosL[2]].fitness + " < " + (float)pop[parentPosL[3]].fitness + " = " + ((float)pop[parentPosL[2]].fitness < (float)pop[parentPosL[3]].fitness));
            parent1 = (float)pop[parentPosL[0]].fitness < (float)pop[parentPosL[1]].fitness ? parentPosL[0] : parentPosL[1];
            parent2 = (float)pop[parentPosL[2]].fitness < (float)pop[parentPosL[3]].fitness ? parentPosL[2] : parentPosL[3];
        }

        /* 
         * Calculates the fitness function
         * Fitness is based in the number of rooms, number of keys and locks, the linear coefficient and the number of locks used by the A* and DFS
         */
        public float Fitness(Dungeon ind, int nV, int nK, int nL, float lCoef, int matrixOffset)
        {
            float fitness = 0.0f;
            float avgUsedRoom = 0.0f;
            DFS dfs;
            AStar astar = new AStar();
            //Only use the A* and DFS if there is a lock in the dungeon
            if (ind.nLocks > 0)
            {
                //The A* finds the number of locks needed to finish the dungeon using the heuristic that is close to optimal.
                ind.neededLocks = astar.FindRoute(ind, matrixOffset);
                // Debug.Log("Neeeded locks: " + ind.neededLocks);
                //Execute 3 times the DFS to minimize the randomness
                //Execute them in parallel to make things faster
                //The DFS finds the number of rooms needed to finish the dungeon be exploring blindly.
                
                // Debug.Log("DFS ... "+ Util.randomSequence.Count);
                dfs = new DFS(ind);
                dfs.FindRoute();
                avgUsedRoom += dfs.NVisitedRooms;
                
                // Debug.Log("DFS1 " + avgUsedRoom + " | " + Util.randomSequence.Count);
                
                dfs = new DFS(ind);
                dfs.FindRoute();
                avgUsedRoom += dfs.NVisitedRooms;
                // Debug.Log("DFS2 " + avgUsedRoom + " | " + Util.randomSequence.Count);
                dfs = new DFS(ind);
                dfs.FindRoute();
                avgUsedRoom += dfs.NVisitedRooms;
                // Debug.Log("DFS3 " + avgUsedRoom + " | " + Util.randomSequence.Count);
                
                // Main Change, the Algorithm will do it one at a time !
                /*
                if (Constants.modified == false)
                {
                    Parallel.For(0, 3, (i) =>
                    {

                        lock (dfsLock)
                        {
                            dfs[i] = new DFS(ind);
                        }
                        dfs[i].FindRoute(seedDFS[i]);
                        lock (avgRoomLock)
                        {
                            avgUsedRoom += dfs[i].NVisitedRooms;
                        }
                    });
                }
                */

                // Normal For ???
                ind.neededRooms = Mathf.Round((avgUsedRoom / 3.0f)*10000.0f)/10000.0f;

                //If we need more rooms than the rooms that really exist, something is wrong.
                if (ind.neededRooms > ind.RoomList.Count)
                {
                    Debug.Log("SOMETHING IS REALLY WRONG! Nrooms: " + ind.RoomList.Count + "  Used: " + ind.neededRooms);
                    System.Console.ReadKey();
                }
                //Also, if we find more locks that really exist
                if (ind.neededLocks > ind.nLocks)
                    Debug.Log("SOMETHING IS REALLY WRONG!");

                /*
                Debug.Log("PArts of fitness " + (2 * (System.Math.Abs(nV - ind.RoomList.Count) + System.Math.Abs(nK - ind.nKeys) + System.Math.Abs(nL - ind.nLocks)
                        + System.Math.Abs(lCoef - ind.AvgChildren) * 5))
                     + " O " + ((ind.nLocks - ind.neededLocks) + System.Math.Abs(ind.RoomList.Count * 0.8f - ind.neededRooms)));
                */
                fitness= (2 * (System.Math.Abs(nV - ind.RoomList.Count) + System.Math.Abs(nK - ind.nKeys) + System.Math.Abs(nL - ind.nLocks)
                        + System.Math.Abs(lCoef - ind.AvgChildren) * 5) + (ind.nLocks - ind.neededLocks) + System.Math.Abs(ind.RoomList.Count * 0.8f - ind.neededRooms));

            }

            //If there are no locks, the fitness is based only in the map layout
            else
                fitness = (2*(System.Math.Abs(nV - ind.RoomList.Count) + System.Math.Abs(nK - ind.nKeys) + System.Math.Abs(nL - ind.nLocks) + System.Math.Abs(lCoef - ind.AvgChildren)));

            return Mathf.Round(fitness * 10000.0f) / 10000.0f;
        }
    }
}

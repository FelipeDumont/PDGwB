using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using TMPro;
using Unity.Collections;
using Unity.Jobs;
using UnityEngine;
using UnityEngine.UI;

namespace LevelGenerator
{
    [Serializable]
    public class Program
    {
        double min;
        double actual;
        //Flags if the dungeon has been gerated for Unity's Game Manager to handle things after
        public bool hasFinished;
        //Count time needed to create levels
        System.Diagnostics.Stopwatch watch;
        //The object to use the EA methods
        private GA gaObj = new GA();
        //Creates the first population of dungeons and generate their rooms
        [SerializeField] List<Dungeon> dungeons;
        [SerializeField] List<float> fitnessValues = new List<float>();
        //The aux the Game Manager will access to load the created dungeon
        public Dungeon aux;


        // The "Main" behind the Dungeon Generator
        public void CreateDungeon(TextMeshProUGUI progressText = null)
        {
            int matrixOffset = Constants.MATRIXOFFSET;
            hasFinished = false;
            min = Double.MaxValue;

            watch = System.Diagnostics.Stopwatch.StartNew();
            dungeons = new List<Dungeon>(Constants.POP_SIZE);
            // Generate the first population
            for (int i = 0; i < dungeons.Capacity; ++i)
            {
                Dungeon individual = new Dungeon();
                individual.GenerateRooms();
                dungeons.Add(individual);
            }

            aux = dungeons[0];
            /*
            for (int i = 0; i < dungeons[1].RoomList.Count; i++)
            {
                Debug.Log("[" + dungeons[1].RoomList[i].X + ", " + dungeons[1].RoomList[i].Y + "]");
            }
            */

            /*
            //Evolve all the generations from the GA
            for (int gen = 0; gen < Constants.GENERATIONS; ++gen)
            {
                // Set dfs fitness seeds each generation
                gaObj.SetDFSSeed();
                //Progress text that can only be used if we do this with threads
                //TODO: someday, refactor everything to be used in parallel and with threads efficiently
                if (progressText != null)
                    progressText.text = ((gen + 1) / (float)Constants.GENERATIONS) * 100 + "%";
                
                float best = float.PositiveInfinity;
                int bestID = -1;
                int counter = 0;

                //Get every dungeon's fitness
                foreach (Dungeon dun in dungeons)
                {
                    dun.fitness = gaObj.Fitness(dun, Constants.nV, Constants.nK, Constants.nL, Constants.lCoef, matrixOffset);
                    if(dun.fitness < best)
                    {
                        best = dun.fitness;
                        bestID = counter;
                    }
                    counter += 1;
                }

                //Elitism = save the best solution?
                // is allways selecting the first one ...
                aux = dungeons[0];

                // Real elitism
                if (Constants.modified == true)
                {
                    aux = dungeons[bestID];
                }

                foreach (Dungeon dun in dungeons)
                {
                    actual = dun.fitness;
                    if (min > actual)
                    {
                        min = actual;
                        aux = dun;
                    }
                }


                //Create the child population by doing the crossover and mutation
                List<Dungeon> childPop = new List<Dungeon>(dungeons.Count);
                for (int i = 0; i < (dungeons.Count / 2); ++i)
                {
                    int parentIdx1 = 0, parentIdx2 = 1;
                    GA.Tournament(dungeons, ref parentIdx1, ref parentIdx2);
                    Dungeon parent1 = dungeons[parentIdx1].Copy();
                    Dungeon parent2 = dungeons[parentIdx2].Copy();

                    try
                    {
                        GA.Crossover(ref parent1, ref parent2);

                        aux = dungeons[0];
                        GA.Mutation(ref parent1);
                        GA.Mutation(ref parent2);
                        //We need to fix the room list anytime a room is altered in the tree.
                        parent1.FixRoomList();
                        parent2.FixRoomList();
                    }
                    catch (System.Exception e)
                    {
                        Debug.Log(e.Message);
                        Util.OpenUri("https://stackoverflow.com/search?q=" + e.Message);
                    }
                    //Calculate the average number of children from the rooms in each children
                    parent1.CalcAvgChildren();
                    parent2.CalcAvgChildren();
                    //Add the children to the new population
                    childPop.Add(parent1);
                    childPop.Add(parent2);
                }

                
                if (Constants.modified == true)
                {
                    //Elitism - with the worst one !!!
                    float worst = float.NegativeInfinity;
                    int worstId = -1;
                    counter = 0;
                    foreach (Dungeon dun in dungeons)
                    {
                        dun.fitness = gaObj.Fitness(dun, Constants.nV, Constants.nK, Constants.nL, Constants.lCoef, matrixOffset);
                        if (dun.fitness > worst)
                        {
                            worst = dun.fitness;
                            worstId = counter;
                        }
                        counter += 1;
                    }
                    childPop[worstId] = aux;
                }
                else
                {
                    //Elitism - now we get back the best one to the first position
                    childPop[0] = aux;
                }
                dungeons = childPop;

            }
            //Find the best individual in the final population and print it as the answer
            min = Double.MaxValue;

            
            aux = dungeons[0];
            
            
            foreach (Dungeon dun in dungeons)
            {
                float actual = gaObj.Fitness(dun, Constants.nV, Constants.nK, Constants.nL, Constants.lCoef, matrixOffset);
                dun.fitness = actual;
                fitnessValues.Add(dun.fitness);

                if (min > actual)
                {
                    min = actual;
                    aux = dun.Copy();
                }
            }
            */
            watch.Stop();
            long time = watch.ElapsedMilliseconds;
            Debug.Log("Total time " + time/1000f);

            hasFinished = true;

            //Saves the test file that we used in the master thesis
            //CSVManager.SaveCSVLevel(id, aux.nKeys, aux.nLocks, aux.RoomList.Count, aux.AvgChildren, aux.neededLocks, aux.neededRooms, min, time, Constants.RESULTSFILE+"-"+Constants.nV+"-" + Constants.nK + "-" + Constants.nL + "-" + Constants.lCoef + ".csv");

            //Print info from best level if needed
            /*Debug.Log("Finished - fitnes:" + aux.fitness);
            Debug.Log("R:"+ aux.RoomList.Count+"-K:" + aux.nKeys + "-L:"+ aux.nLocks + "-Lin:"+aux.AvgChildren +"-nL:"+aux.neededLocks+"-nR:"+aux.neededRooms);
            Debug.Log("nRdelta:"+System.Math.Abs(aux.RoomList.Count * 0.8f - aux.neededRooms)+"-80p:"+ aux.RoomList.Count * 0.8f);*/

            //This method prints the dungeon in the console (not unity's one) AND saves it into a file!
            // Interface.PrintNumericalGridWithConnections(aux);

            //We should clear the dungeons... but Game Manager is using aux. So we could and should do a hard copy. But i'm not touching that spaghetti right now
            //TODO: touch spaghetti later
            // TODO THIS WAS DONE, REMOVED FOR TESTING
            // dungeons.Clear();
            
        }

        public int GetDungeonCount()
        {
            return dungeons.Count;
        }
        public Dungeon GetDungeon(int id)
        {
            if (dungeons.Count > id)
            {
                return dungeons[id];
            }
            return null;
        }
    }
}

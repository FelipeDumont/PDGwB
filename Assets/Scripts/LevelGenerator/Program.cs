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
        float min;
        float actual;
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

            watch = System.Diagnostics.Stopwatch.StartNew();
            dungeons = new List<Dungeon>(Constants.POP_SIZE);
            // Generate the first population
            for (int i = 0; i < dungeons.Capacity; ++i)
            {
                // Debug.Log("Create Dungeon: " + Util.randomSequence.Count);
                Dungeon individual = new Dungeon();
                individual.GenerateRooms();
                dungeons.Add(individual);
            }

            aux = dungeons[0];
            // Constants.GENERATIONS = 0;


            //Evolve all the generations from the GA
            for (int gen = 0; gen < Constants.GENERATIONS; ++gen)
            {
                
                // Set dfs fitness seeds each generation
                // gaObj.SetDFSSeed();

                //Progress text that can only be used if we do this with threads
                //TODO: someday, refactor everything to be used in parallel and with threads efficiently
                if (progressText != null)
                    progressText.text = ((gen + 1) / (float)Constants.GENERATIONS) * 100 + "%";


                //Get every dungeon's fitness
                // Debug.Log("Generation " + gen + " fitnesses: ");
                int counter = 0;
                foreach (Dungeon dun in dungeons)
                {
                    string data = "";
                    foreach (Room r in dun.RoomList)
                    {
                        data += r.RoomId + " [" + r.X + ", " + r.Y + "]" + r.KeyToOpen + "\n";
                    }
                    Debug.Log("Dungeon " + counter);
                    Debug.Log(data);

                    dun.fitness = gaObj.Fitness(dun, Constants.nV, Constants.nK, Constants.nL, Constants.lCoef, matrixOffset);
                    Debug.Log("Dungeon " + counter + " F: " + dun.fitness);
                    
                    
                   
                    
                    counter++;
                }

                //Elitism = save the best solution?
                // is allways selecting the first one ...
                aux = dungeons[0];
                min = float.MaxValue;

                foreach (Dungeon dun in dungeons)
                {
                    actual = dun.fitness;
                    // Debug.Log(min + "> " + actual + "? "  + (min > actual));
                    if (min > actual)
                    {
                        min = actual;
                        aux = dun;
                    }
                }

                // Debug.Log("Best individual Fitness: " + aux.fitness);


                //Create the child population by doing the crossover and mutation
                List<Dungeon> childPop = new List<Dungeon>(dungeons.Count);
                for (int i = 0; i < (dungeons.Count / 2); ++i)
                {
                    
                    
                    int parentIdx1 = 0, parentIdx2 = 1;
                    GA.Tournament(dungeons, ref parentIdx1, ref parentIdx2);
                    // Debug.Log(parentIdx1 + " | " + parentIdx2);
                    
                    Dungeon parent1 = dungeons[parentIdx1].Copy();
                    Dungeon parent2 = dungeons[parentIdx2].Copy();

                    
                    try
                    {
                        // Debug.Log("Cross: " + parentIdx1 + " | " + parentIdx2 + "   " + Util.randomSequence.Count);
                        GA.Crossover(ref parent1, ref parent2);
                        // Debug.Log("Post Cross seedID?" + Util.randomSequence.Count);

                        // Debug.Log("End Cross: " + Util.randomSequence.Count);

                        
                        GA.Mutation(ref parent1);
                        // Debug.Log("END Mutation 1: " + Util.randomSequence.Count);
                        GA.Mutation(ref parent2);
                        // Debug.Log("END Mutation 2 " + Util.randomSequence.Count);
                        


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

                // Debug.Log("Add Again AUX " + aux.fitness );
                childPop[0] = aux;
                dungeons = childPop;
                // Debug.Log("Next Generation: " + (gen + 1));
            }
            //Find the best individual in the final population and print it as the answer
            min = float.MaxValue;
            aux = dungeons[0];

            Debug.Log("Final generation: ");
            foreach (Dungeon dun in dungeons)
            {
                string data = "";
                foreach (Room r in dun.RoomList)
                {
                    data += r.RoomId + " [" + r.X + ", " + r.Y + "]" + r.KeyToOpen + "\n";
                }
                Debug.Log("Dungeon " + data);
                dun.fitness = gaObj.Fitness(dun, Constants.nV, Constants.nK, Constants.nL, Constants.lCoef, matrixOffset);
                
                Debug.Log(dun.fitness);
                
                

                fitnessValues.Add(dun.fitness);

                if (min > actual)
                {
                    min = actual;
                    aux = dun.Copy();
                }
            }
            
            watch.Stop();
            long time = watch.ElapsedMilliseconds;
            Debug.Log("Total time " + time/1000f);

            hasFinished = true;
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

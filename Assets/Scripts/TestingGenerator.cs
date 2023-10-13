using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using LevelGenerator;

public class TestingGenerator : MonoBehaviour
{
    [Header("Generation Settings")]
    [SerializeField] int nRooms;
    [SerializeField] int nKeys;
    [SerializeField] int nLocks;
    [SerializeField] float linearity;
    [SerializeField] int currentSeed = 100;
    

    [Header("Constant Settings")]
    [SerializeField] bool modifiedVersion = false;
    [SerializeField] int GENERATIONS = 100;
    [SerializeField] int POP_SIZE = 100;
    [SerializeField] int MAX_DEPTH = 10;

    [Header("New Symmetry")]
    [SerializeField] bool useSymmetry;
    [SerializeField] float symmetry;

    [Header("Generator")]
    [SerializeField] Program generator;

    [SerializeField] bool finished = false;
    [SerializeField] bool runOnce = false;
    // Start is called before the first frame update

    public void Start()
    {
        runOnce = true;
    }
    void RunGeneration()
    {

        // Set generator
        generator = new Program();
        LevelGenerator.Util.ID = 0;
        // Assign Seed
        LevelGenerator.Util.rnd = new System.Random(currentSeed);
        
        // Fitness values !
        Constants.nV = nRooms;
        Constants.nK = nKeys;
        Constants.nL = nLocks;
        Constants.lCoef = linearity;
        Constants.modified = modifiedVersion;

        // Symmetry
        Constants.nSymmetry = symmetry;
        Constants.useSymmetry = useSymmetry;
        // Constant
        if (modifiedVersion)
        {
            // MAX_DEPTH = nRooms - 1;
            Constants.GENERATIONS = GENERATIONS;
            Constants.POP_SIZE = POP_SIZE;
            Constants.MAX_DEPTH = MAX_DEPTH;
        }

        // Create ... The ... Dungeon !!!
        generator.CreateDungeon();
        
    }

    // Update is called once per frame
    void Update()
    {
        if (runOnce){
            runOnce = false;
            RunGeneration();
        }
        if (generator != null)
        {
            if (generator.hasFinished)
            {
                if(finished == false)
                {
                    finished = true;
                }
            }
            else
            {
                Debug.Log("Generating");
            }
        }
    }

    public bool FinishedGeneration()
    {
        return finished && runOnce == false;
    }

    public Dungeon GetCurrentDungeonMap()
    {
        return generator.aux;
    }

    public int GetDungeonCount()
    {
        return generator.GetDungeonCount();
    }

    public Dungeon GetDungeon(int id)
    {
        return generator.GetDungeon(id);
    }
}

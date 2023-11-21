using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using LevelGenerator;
using System.IO;

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

    [Header("Test")]
    [SerializeField] bool createRandomSequenceFileNow = false;
    public List<int> randomSequence = new List<int>();
    // Start is called before the first frame update

    public void Start()
    {
        runOnce = true;
    }
    void RunGeneration()
    {
        LevelGenerator.Util.ID = 0;
        LevelGenerator.Util.rnd = new System.Random(currentSeed);
        Util.randomSequence = new List<int>();
        
        // Fitness values !
        Constants.nV = nRooms;
        Constants.nK = nKeys;
        Constants.nL = nLocks;
        Constants.lCoef = linearity;

        // Symmetry
        Constants.nSymmetry = symmetry;
        Constants.useSymmetry = useSymmetry;
        // Constant
        Constants.GENERATIONS = GENERATIONS;
        Constants.POP_SIZE = POP_SIZE;
        Constants.MAX_DEPTH = MAX_DEPTH;

        // Create ... The ... Dungeon !!!
        generator.CreateDungeon();
        
    }

    // Update is called once per frame
    void Update()
    {
        if (runOnce)
        {
            runOnce = false;
            RunGeneration();
        }
        if (generator != null)
        {
            if (generator.hasFinished)
            {
                if (finished == false)
                {
                    finished = true;
                    this.randomSequence = Util.randomSequence;
                }
            }
            else
            {
                Debug.Log("Generating");
            }
        }
        if(createRandomSequenceFileNow == true && finished)
        {
            createRandomSequenceFileNow = false;
            CreateRandomSequenceFile();
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

    private string filePath = "Assets/RandomSequence.txt";

    public void CreateRandomSequenceFile()
    {
        // Check if the file already exists, and if it does, delete it
        if (File.Exists(filePath))
        {
            File.Delete(filePath);
        }

        // Create a new file and write to it line by line
        using (StreamWriter writer = new StreamWriter(filePath))
        {
            foreach(var number in randomSequence)
            {
                writer.WriteLine(number);
            }
        }

        Debug.Log("File created successfully at: " + filePath);
    }
}

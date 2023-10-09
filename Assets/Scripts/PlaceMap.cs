using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using LevelGenerator;
using TMPro;

public class PlaceMap : MonoBehaviour
{
    List<SpriteRenderer> map =  new List<SpriteRenderer>();
    List<SpriteRenderer> connections = new List<SpriteRenderer>();
    List<TextMeshPro> texts = new List<TextMeshPro>();
    [SerializeField] Transform mapParent;
    [SerializeField] SpriteRenderer srPrefab;
    [SerializeField] SpriteRenderer srHorizontalPrefab;
    [SerializeField] SpriteRenderer srVerticalPrefab;
    [SerializeField] TextMeshPro textPrefab;
    [SerializeField] Color connectionNormal;
    [SerializeField] Color connectionBarrier;
    [SerializeField] TestingGenerator tg;
    bool placedOnce = false;
    Dictionary<int,int> placedKeys = new Dictionary<int, int>();

    public void Update()
    {
        if (tg != null && tg.FinishedGeneration() && placedOnce == false)
        {
            PlaceDungeonMap(tg.GetCurrentDungeonMap());
        }
    }

    public void PlaceDungeonMap(Dungeon dungeonMap)
    {
        placedOnce = true;
        for (int i = 0, length = dungeonMap.RoomList.Count; i < length; i++)
        {
            // Text First Room 
            if(i == 0)
            {
                TextMeshPro textUI = Instantiate(textPrefab, new Vector3(0,0,0), Quaternion.identity, mapParent);
                textUI.text = "S";
                texts.Add(textUI);
            }

            // Sprite Room
            SpriteRenderer newSR = Instantiate(srPrefab, new Vector3(dungeonMap.RoomList[i].X, dungeonMap.RoomList[i].Y, 0) , Quaternion.identity, mapParent);
            map.Add(newSR);

            // Sprite Connections
            if (dungeonMap.RoomList[i].LeftChild != null)
            {
                PlaceConnection(dungeonMap.RoomList[i], dungeonMap.RoomList[i].LeftChild);
            }
            if (dungeonMap.RoomList[i].RightChild != null)
            {
                PlaceConnection(dungeonMap.RoomList[i], dungeonMap.RoomList[i].RightChild);
            }
            if (dungeonMap.RoomList[i].BottomChild != null)
            {
                PlaceConnection(dungeonMap.RoomList[i], dungeonMap.RoomList[i].BottomChild);
            }

            // Text Key In Room
            if(dungeonMap.RoomList[i].Type == Type.key) 
            {
                TextMeshPro textUI = Instantiate(textPrefab, new Vector3(dungeonMap.RoomList[i].X, dungeonMap.RoomList[i].Y, 0), Quaternion.identity, mapParent);
                if(placedKeys.ContainsKey(dungeonMap.RoomList[i].KeyToOpen) == false)
                {
                    placedKeys.Add(dungeonMap.RoomList[i].KeyToOpen, placedKeys.Count + 1);
                }
                textUI.text = "K" + placedKeys[dungeonMap.RoomList[i].KeyToOpen];
                texts.Add(textUI);
            }

            // Text Need Key to Open
            if (dungeonMap.RoomList[i].Type == Type.locked)
            {
                // Position it on the parent Conection position
                TextMeshPro textUI = Instantiate(textPrefab, new Vector3(dungeonMap.RoomList[i].X, dungeonMap.RoomList[i].Y, 0), Quaternion.identity, mapParent);
                if (placedKeys.ContainsKey(dungeonMap.RoomList[i].KeyToOpen) == false)
                {
                    placedKeys.Add(dungeonMap.RoomList[i].KeyToOpen, placedKeys.Count + 1);
                }
                textUI.text = "B" + placedKeys[dungeonMap.RoomList[i].KeyToOpen];
                texts.Add(textUI);
            }
        }
    }

    public void PlaceConnection(Room parent, Room son)
    {
        // Get The Middle position
        float middleX = (son.X - parent.X) / 2f;
        float middleY = (son.Y - parent.Y) / 2f;

        // Now we palce the Thing !!!
        SpriteRenderer current = middleX == 0 ? srVerticalPrefab : srHorizontalPrefab;
        // print(new Vector2(parent.X, parent.Y) + " " + new Vector2(son.X, son.Y) + " " + new Vector2(middleX , middleY));
        SpriteRenderer connection =  Instantiate(current, new Vector3(parent.X + middleX, parent.Y + middleY, 0), Quaternion.identity, mapParent);
        connection.color = son.Type == Type.locked ? connectionBarrier : connectionNormal;
        connections.Add(connection);
    }
}

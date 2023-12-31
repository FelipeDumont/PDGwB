﻿using System;

[Serializable]
public class Dimensions
{
    private int width, height;
    public Dimensions(int width, int height)
    {
        Width = width;
        Height = height;
    }

    public int Width { get => width; set => width = value; }
    public int Height { get => height; set => height = value; }
}

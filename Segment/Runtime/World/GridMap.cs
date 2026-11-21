using System;

namespace Aniki.GameplaySystems.World
{
    public sealed class GridMap
    {
        private readonly GridCellType[] cells;

        public GridMap(int width, int height)
        {
            if (width <= 0)
            {
                throw new ArgumentOutOfRangeException(nameof(width));
            }

            if (height <= 0)
            {
                throw new ArgumentOutOfRangeException(nameof(height));
            }

            Width = width;
            Height = height;
            cells = new GridCellType[width * height];
        }

        private GridMap(int width, int height, GridCellType[] source)
        {
            Width = width;
            Height = height;
            cells = source;
        }

        public int Width { get; }
        public int Height { get; }
        public int Count => cells.Length;

        public GridCellType this[int x, int y]
        {
            get => cells[ToIndex(x, y)];
            internal set => cells[ToIndex(x, y)] = value;
        }

        internal GridCellType this[int index]
        {
            get => cells[index];
            set => cells[index] = value;
        }

        public bool IsInside(int x, int y)
        {
            return x >= 0 && x < Width && y >= 0 && y < Height;
        }

        public int CountCells(GridCellType type)
        {
            int result = 0;
            for (int index = 0; index < cells.Length; index++)
            {
                if (cells[index] == type)
                {
                    result++;
                }
            }

            return result;
        }

        public bool ContentEquals(GridMap other)
        {
            if (other == null || Width != other.Width || Height != other.Height)
            {
                return false;
            }

            for (int index = 0; index < cells.Length; index++)
            {
                if (cells[index] != other.cells[index])
                {
                    return false;
                }
            }

            return true;
        }

        public GridMap Copy()
        {
            return new GridMap(Width, Height, (GridCellType[])cells.Clone());
        }

        internal int ToIndex(int x, int y)
        {
            if (!IsInside(x, y))
            {
                throw new ArgumentOutOfRangeException($"Cell ({x}, {y}) is outside the grid.");
            }

            return y * Width + x;
        }

        internal void FromIndex(int index, out int x, out int y)
        {
            if (index < 0 || index >= cells.Length)
            {
                throw new ArgumentOutOfRangeException(nameof(index));
            }

            x = index % Width;
            y = index / Width;
        }
    }
}

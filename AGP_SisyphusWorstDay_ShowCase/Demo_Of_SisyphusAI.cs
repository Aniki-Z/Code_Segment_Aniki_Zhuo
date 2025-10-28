using System.Collections.Generic;
using UnityEngine;
using GameEditor.Scripts;

public class SisyphusAI : MonoBehaviour
{
    // ================================
    // PUBLIC PATH OUTPUT
    // ================================

    /// <summary>
    /// Resulting high-level plan for the boulder + required push cells.
    /// </summary>
    public List<PathStep> path;

    // ================================
    // PATH STEP DTO
    // ================================

    public struct PathStep
    {
        // Where the boulder will be after this step
        public Cell boulderCell;

        // Where Sisyphus must stand to execute this step (null if not being pushed)
        public Cell pushCell;

        // Whether the boulder is being pushed along this edge
        public bool isPushed;

        public PathStep(Cell bCell, Cell pCell, bool isPushed)
        {
            boulderCell = bCell;
            pushCell = pCell;
            this.isPushed = isPushed;
        }
    }

    // ================================
    // HEURISTIC HELPERS
    // ================================

    /// <summary>
    /// Euclidean distance in cell space.
    /// </summary>
    private float calculateEuclideanDistance(Cell a, Cell b)
    {
        return Vector3.Distance(a.cellPosition, b.cellPosition);
    }

    /// <summary>
    /// Manhattan distance in cell space (grid metric).
    /// </summary>
    private int calculateManhattanDistance(Cell a, Cell b)
    {
        Vector3 ap = a.cellPosition;
        Vector3 bp = b.cellPosition;
        return Mathf.Abs((int)ap.x - (int)bp.x)
             + Mathf.Abs((int)ap.y - (int)bp.y)
             + Mathf.Abs((int)ap.z - (int)bp.z);
    }

    // ================================
    // SISYPHUS LOCAL PATHFIND
    // ================================

    /// <summary>
    /// Returns a walkable path for Sisyphus from startCell to endCell,
    /// respecting ramps and avoiding the current boulderCell.
    /// </summary>
    public bool CanSisyphusPath(Cell boulderCell, Cell startCell, Cell endCell, out List<Cell> returnPath)
    {
        returnPath = null;

        // Node record for A*
        var cellToNode = new Dictionary<Cell, GraphNode>();

        var startNode = new GraphNode(null, startCell)
        {
            g = 0f,
            h = calculateEuclideanDistance(startCell, endCell),
            isOnRamp = startCell.isRampBelow()
        };
        startNode.f = startNode.g + startNode.h;
        cellToNode[startCell] = startNode;

        var open = new PriorityQueue<GraphNode>();
        open.Enqueue(startNode, startNode.f);
        var closed = new HashSet<Cell>();

        // Base 4-neighborhood (X/Z plane)
        var dirs = new Vector3Int[]
        {
            new Vector3Int( 1, 0,  0),
            new Vector3Int(-1, 0,  0),
            new Vector3Int( 0, 0,  1),
            new Vector3Int( 0, 0, -1),
        };

        while (open.Count > 0)
        {
            var current = open.Dequeue();
            var currentCell = current.cell;
            if (closed.Contains(currentCell)) continue;
            closed.Add(currentCell);

            // Path found
            if (currentCell == endCell)
            {
                var pathCells = new List<Cell>();
                var n = current;
                while (n != null)
                {
                    pathCells.Add(n.cell);
                    n = n.parent;
                }
                pathCells.Reverse();
                if (pathCells.Count > 0) pathCells.RemoveAt(0);
                returnPath = pathCells;
                return true;
            }

            // Ramp-aware neighbor set
            Vector3Int[] neighborDirs;
            if (current.isOnRamp)
            {
                var under = EditorGrid.Instance.GetCell(currentCell.cellPosition + Vector3Int.down);
                if (under == null || under.cellGameObject == null) continue;
                var ramp = under.cellGameObject.GetComponent<Ramp>();
                if (ramp == null) continue;
                neighborDirs = ramp.OnRampNeighbors();
            }
            else neighborDirs = dirs;

            foreach (var dir in neighborDirs)
            {
                var rawNeighbor = EditorGrid.Instance.GetCell(currentCell.cellPosition + dir);
                if (rawNeighbor == null || closed.Contains(rawNeighbor)) continue;
                if (rawNeighbor == boulderCell) continue;
                if (rawNeighbor.gameObject == EditorGrid.Instance.Flag?.gameObject) continue;

                var neighbor = rawNeighbor.GetWalkableCell(current.cell, current.isOnRamp, out bool rampFlag, out bool _);
                if (neighbor == null || neighbor == boulderCell) continue;

                float g = current.g + 1f;
                float h = calculateEuclideanDistance(neighbor, endCell);
                float f = g + h;

                if (cellToNode.TryGetValue(neighbor, out var child))
                {
                    if (child.f > f)
                    {
                        child.parent = current;
                        child.g = g;
                        child.h = h;
                        child.f = f;
                        child.isOnRamp = rampFlag;
                        open.Enqueue(child, child.f);
                    }
                }
                else
                {
                    child = new GraphNode(current, neighbor)
                    {
                        g = g,
                        h = h,
                        f = f,
                        isOnRamp = rampFlag
                    };
                    cellToNode[neighbor] = child;
                    open.Enqueue(child, child.f);
                }
            }
        }

        // No path
        return false;
    }

    // ================================
    // BOULDER A* WITH PUSHING
    // ================================

    /// <summary>
    /// Compute the shortest boulder route to the flag,
    /// including where Sisyphus must stand to push.
    /// Writes the plan into path.
    /// </summary>
    public bool findBoulderPath()
    {
        path = null;

        var flag = EditorGrid.Instance.Flag;
        var boulder = EditorGrid.Instance.Boulder;
        if (flag == null || boulder == null)
            return false;

        var currentBoulderCell = boulder.GetComponentInParent<Cell>();
        var currentSisyphusCell = GetComponentInParent<Cell>();
        var flagCell = flag.GetCell();
        if (currentBoulderCell == null || currentSisyphusCell == null || flagCell == null)
            return false;

        var cellToNode = new Dictionary<Cell, GraphNode>();

        var start = new GraphNode(null, currentBoulderCell)
        {
            g = 0f,
            h = calculateManhattanDistance(currentBoulderCell, flagCell),
            pushCell = null,
            isOnRamp = currentBoulderCell.isRampBelow(),
            isBeingPushed = false
        };
        start.f = start.g + start.h;
        cellToNode[currentBoulderCell] = start;

        var open = new PriorityQueue<GraphNode>();
        open.Enqueue(start, start.f);
        var closed = new HashSet<Cell>();

        var dirs = new Vector3Int[]
        {
            new Vector3Int( 1, 0,  0),
            new Vector3Int(-1, 0,  0),
            new Vector3Int( 0, 0,  1),
            new Vector3Int( 0, 0, -1),
        };

        while (open.Count > 0)
        {
            var current = open.Dequeue();
            var cell = current.cell;
            if (closed.Contains(cell)) continue;
            closed.Add(cell);

            if (cell == flagCell)
            {
                reconstructBoulderPath(current);
                return true;
            }

            // Determine valid move directions
            Vector3Int[] moveDirs;
            if (current.isOnRamp)
            {
                var under = EditorGrid.Instance.GetCell(cell.cellPosition + Vector3Int.down);
                if (under == null || under.cellGameObject == null) continue;
                var ramp = under.cellGameObject.GetComponent<Ramp>();
                if (ramp == null) continue;
                moveDirs = current.isBeingPushed ? ramp.OnRampNeighbors() : ramp.GetOnRampDown();
            }
            else moveDirs = dirs;

            foreach (var dir in moveDirs)
            {
                var rawNeighbor = EditorGrid.Instance.GetCell(cell.cellPosition + dir);
                if (rawNeighbor == null || closed.Contains(rawNeighbor)) continue;

                var neighbor = rawNeighbor.GetWalkableCell(current.cell, current.isOnRamp,
                                                           out bool neighborOnRamp, out bool isBeingPushedFlag);
                if (neighbor == null) continue;

                // Compute push cell (where Sisyphus must stand) for this boulder move
                Cell pushCell = null;
                if (!current.isOnRamp)
                {
                    pushCell = EditorGrid.Instance.GetCell(new Vector3Int(
                        2 * cell.cellPosition.x - neighbor.cellPosition.x,
                        cell.cellPosition.y,
                        2 * cell.cellPosition.z - neighbor.cellPosition.z));

                    if (pushCell && pushCell.cellGameObject && pushCell.cellGameObject.GetComponent<Ramp>())
                    {
                        pushCell = EditorGrid.Instance.GetCell(new Vector3Int(
                            pushCell.cellPosition.x,
                            pushCell.cellPosition.y + 1,
                            pushCell.cellPosition.z));
                    }
                }
                else
                {
                    if (neighbor.cellPosition.y >= cell.cellPosition.y)
                    {
                        pushCell = EditorGrid.Instance.GetCell(new Vector3Int(
                            2 * cell.cellPosition.x - neighbor.cellPosition.x,
                            cell.cellPosition.y - 1,
                            2 * cell.cellPosition.z - neighbor.cellPosition.z));
                    }
                    else
                    {
                        pushCell = EditorGrid.Instance.GetCell(new Vector3Int(
                            2 * cell.cellPosition.x - neighbor.cellPosition.x,
                            cell.cellPosition.y,
                            2 * cell.cellPosition.z - neighbor.cellPosition.z));

                        if (pushCell && pushCell.cellGameObject && pushCell.cellGameObject.GetComponent<Ramp>())
                        {
                            pushCell = EditorGrid.Instance.GetCell(new Vector3Int(
                                pushCell.cellPosition.x,
                                pushCell.cellPosition.y + 1,
                                pushCell.cellPosition.z));
                        }
                    }
                }

                // Evaluate Sisyphus reachability
                bool canSisyphusReach;
                if (!current.isBeingPushed)
                {
                    canSisyphusReach = true;
                    pushCell = null;
                }
                else
                {
                    if (pushCell == null) continue;
                    var parentCell = current.parent == null ? currentSisyphusCell : current.parent.cell;
                    if (current.parent != null && !current.parent.isBeingPushed)
                        parentCell = currentSisyphusCell;
                    canSisyphusReach = CanSisyphusPath(cell, parentCell, pushCell, out _);
                }

                if (!canSisyphusReach) continue;

                // Cost calculations
                float g = current.g + 1f;
                float h = calculateManhattanDistance(neighbor, flagCell);
                float f = g + h;

                if (cellToNode.TryGetValue(neighbor, out var child))
                {
                    if (child.f > f)
                    {
                        child.parent = current;
                        child.g = g;
                        child.h = h;
                        child.f = f;
                        child.pushCell = pushCell;
                        child.isOnRamp = neighborOnRamp;
                        child.isBeingPushed = isBeingPushedFlag;
                        open.Enqueue(child, child.f);
                    }
                }
                else
                {
                    child = new GraphNode(current, neighbor)
                    {
                        g = g,
                        h = h,
                        f = f,
                        pushCell = pushCell,
                        isOnRamp = neighborOnRamp,
                        isBeingPushed = isBeingPushedFlag
                    };
                    cellToNode[neighbor] = child;
                    open.Enqueue(child, child.f);
                }
            }
        }

        return false;
    }

    /// <summary>
    /// Builds the public path list from the terminal graph node,
    /// including explicit push steps when needed.
    /// </summary>
    public void reconstructBoulderPath(GraphNode endNode)
    {
        path = new List<PathStep>();
        var nodes = new List<GraphNode>();
        var n = endNode;
        while (n != null)
        {
            nodes.Add(n);
            n = n.parent;
        }
        nodes.Reverse();

        // Convert to (boulderCell, pushCell) steps
        for (int i = 1; i < nodes.Count; i++)
        {
            if (!nodes[i].isBeingPushed)
            {
                path.Add(new PathStep(nodes[i].cell, null, false));
            }
            else
            {
                if (nodes[i - 1].parent == null || nodes[i].pushCell != nodes[i - 1].parent.cell)
                {
                    path.Add(new PathStep(nodes[i - 1].cell, nodes[i].pushCell, nodes[i - 1].isBeingPushed));
                }
                path.Add(new PathStep(nodes[i].cell, nodes[i].parent.cell, nodes[i - 1].isBeingPushed));
            }
        }
    }
}

// ================================
// Unrelated Code has been Removed
// ================================
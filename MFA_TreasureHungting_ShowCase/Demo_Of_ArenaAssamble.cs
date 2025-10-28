using System.Collections.Generic;
using UnityEngine;

#region Core Config + Helpers
// ================================
// ARENA CONFIGURATION
// ================================

[System.Serializable]
public class ArenaConfiguration
{
    [Header("Arena Layout")]
    public Vector3 arenaOrigin = Vector3.zero;

    [Header("Tile Metrics")]
    public int arenaWidth = 11;
    public int arenaHeight = 6;
    public float tileSize = 1f;

    [Header("Layout Mode")]
    public bool unifiedMapMode = false;

    /// <summary>Align config tile size to grid and snap origin.</summary>
    public void AutoDetectTileSize(WorldGrid grid)
    {
        if (grid == null) return;
        tileSize = grid.tileSpacing;
        arenaOrigin.x = SnapToGrid(arenaOrigin.x, tileSize);
        arenaOrigin.z = SnapToGrid(arenaOrigin.z, tileSize);
        arenaOrigin.y = 0f;
    }

    /// <summary>Return arena center.</summary>
    public Vector3 GetArenaCenter()
    {
        return arenaOrigin;
    }

    /// <summary>Validate positive values.</summary>
    public bool IsValid()
    {
        return arenaWidth > 0 && arenaHeight > 0 && tileSize > 0f;
    }

    /// <summary>Grid snap helper.</summary>
    private float SnapToGrid(float v, float grid)
    {
        return Mathf.Floor(v / grid + 0.5f) * grid;
    }
}

// ================================
// TILE MANAGER
// ================================

public class ArenaTileManager
{
    private readonly List<GameObject> _assembled = new List<GameObject>();

    /// <summary>Number of tracked tiles.</summary>
    public int TileCount => _assembled.Count;

    /// <summary>Track a created tile for cleanup.</summary>
    public void AddTileForTracking(GameObject tile)
    {
        if (tile != null) _assembled.Add(tile);
    }

    /// <summary>Destroy all tracked tiles.</summary>
    public void CleanupAllTiles()
    {
        foreach (var go in _assembled) if (go) Object.Destroy(go);
        _assembled.Clear();
    }
}

// ================================
// VALIDATOR 
// ================================

public class ArenaValidator
{
    /// <summary>Check core preconditions.</summary>
    public bool CanAssembleArena(List<Transform> players, List<WorldGrid> playerGrids, ArenaConfiguration config, CameraController cameraController)
    {
        if (config == null || !config.IsValid()) return false;
        if (cameraController == null || !cameraController.IsValid()) return false;
        if (players == null || players.Count == 0) return false;
        if (!config.unifiedMapMode)
        {
            if (playerGrids == null || playerGrids.Count == 0) return false;
            if (players.Count != playerGrids.Count) return false;
            for (int i = 0; i < players.Count; i++)
            {
                if (players[i] == null) return false;
                if (playerGrids[i] == null) return false;
            }
        }
        return true;
    }
}
#endregion

#region Mini-game Capture (Lightweight)
// ================================
// SNAPSHOT DTO 
// ================================

[System.Serializable]
public class TileSnapshot
{
    public Vector3Int gridCoordinate;
    public GameObject tileObject;
    public int localX, localZ;
    public bool isPlayerTile;
    public Vector3 playerOffsetInTile;

    public TileSnapshot(Vector3Int coord, GameObject obj, int x, int z, bool isPlayer, Vector3 offset)
    {
        gridCoordinate = coord;
        tileObject = obj;
        localX = x;
        localZ = z;
        isPlayerTile = isPlayer;
        playerOffsetInTile = offset;
    }
}

// ================================
// PLAYER POSITIONER 
// ================================

public class ArenaPlayerPositioner
{
    // Recorded tiles per player for later assembly
    private readonly Dictionary<int, List<TileSnapshot>> _recordings = new Dictionary<int, List<TileSnapshot>>();

    /// <summary>Move player to a target position using Player component if present.</summary>
    public void MovePlayerToArena(Transform player, Vector3 arenaPosition)
    {
        if (!player) return;
        var pc = player.GetComponent<Player>();
        if (pc != null) pc.ForceMoveTo(arenaPosition);
        else player.position = arenaPosition;
    }

    /// <summary>Capture an N×M tile window centered at player's grid tile for later assembly.</summary>
    public void PlacePlayerTiles(Transform player, WorldGrid grid, int arenaWidth, int arenaHeight, int playerIndex, ArenaTileManager tileManager, Vector3 arenaCenter)
    {
        if (!player || !grid) return;

        Vector3Int playerTile = grid.WorldPositionToGridPosition(player.position);
        int halfW = arenaWidth / 2;
        int halfH = arenaHeight / 2;

        var rec = new List<TileSnapshot>();

        for (int z = 0; z < arenaHeight; z++)
        {
            for (int x = 0; x < arenaWidth; x++)
            {
                Vector3Int coord = new Vector3Int(playerTile.x - halfW + x, 0, playerTile.z - halfH + z);
                Tile t = grid.GetTileAt(coord);
                if (t?.tileObject == null) continue;

                bool isCenter = (x == halfW && z == halfH);
                Vector3 tileWorld = t.tileObject.transform.position;
                Vector3 offset = isCenter ? (player.position - tileWorld) : Vector3.zero;

                rec.Add(new TileSnapshot(coord, t.tileObject, x, z, isCenter, offset));
            }
        }

        _recordings[playerIndex] = rec;
    }

    /// <summary>Instantiate assembled tiles for a player based on prior recording.</summary>
    public void AssemblePlayerArena(int playerIndex, Vector3 arenaCenter, ArenaTileManager tileManager, int arenaWidth, int arenaHeight, float tileSpacing)
    {
        if (!_recordings.ContainsKey(playerIndex)) return;

        int halfW = arenaWidth / 2;
        int halfH = arenaHeight / 2;

        foreach (var s in _recordings[playerIndex])
        {
            Vector3 basePos = arenaCenter + new Vector3((s.localX - halfW) * tileSpacing, s.tileObject.transform.position.y, (s.localZ - halfH) * tileSpacing);
            GameObject clone = Object.Instantiate(s.tileObject, basePos, s.tileObject.transform.rotation);
            clone.name = $"Arena_P{playerIndex}_{s.localX}_{s.localZ}";
            var totem = clone.GetComponent<TotemInfo>();
            if (totem != null) totem.Initialize(playerIndex);
            tileManager.AddTileForTracking(clone);
        }
    }

    /// <summary>Access recordings for player repositioning.</summary>
    public List<TileSnapshot> GetPlayerRecordings(int playerIndex)
    {
        return _recordings.ContainsKey(playerIndex) ? _recordings[playerIndex] : new List<TileSnapshot>();
    }

    /// <summary>Compute the origin of a player's quadrant within the assembled rectangle.</summary>
    public Vector3 GetQuadrantOrigin(int playerIndex, Vector3 arenaCenter, int arenaWidth, int arenaHeight, float tileSpacing)
    {
        float w = (arenaWidth - 1) * tileSpacing;
        float h = (arenaHeight - 1) * tileSpacing;
        Vector3 half = new Vector3(w * 0.5f, 0f, h * 0.5f);

        Vector3 offset = playerIndex switch
        {
            0 => new Vector3(-half.x, 0,  half.z),
            1 => new Vector3( half.x, 0,  half.z),
            2 => new Vector3(-half.x, 0, -half.z),
            3 => new Vector3( half.x, 0, -half.z),
            _ => Vector3.zero
        };
        return arenaCenter + offset;
    }
}
#endregion

#region Base Assembler
// ================================
// ARENA ASSEMBLER 
// ================================

public class ArenaAssembler : MonoBehaviour
{
    // Singleton access (optional for scenes)
    public static ArenaAssembler Instance { get; private set; }

    [Header("Arena Configuration")]
    [SerializeField] protected ArenaConfiguration config = new ArenaConfiguration();

    [Header("Players & Grids")]
    [SerializeField] protected List<WorldGrid> playerGrids = new List<WorldGrid>();
    [SerializeField] protected List<Transform> players = new List<Transform>();

    [Header("Camera Controller")]
    [SerializeField] protected CameraController cameraController;

    // Internal helpers
    protected ArenaTileManager tileManager;
    protected ArenaPlayerPositioner playerPositioner;
    protected ArenaValidator validator;

    // State flags
    public bool IsAssembled { get; protected set; }

    // Original positions cache
    protected readonly List<Vector3> originalPlayerPositions = new List<Vector3>();

    // Events (optional hook points)
    public System.Action<bool> OnAssemblyStateChanged;
    public System.Action OnAssemblyStarted;
    public System.Action OnAssemblyCompleted;

    /// <summary>Tile count (read-only).</summary>
    public int TileCount => tileManager?.TileCount ?? 0;

    /// <summary>Expose config center.</summary>
    public Vector3 GetArenaCenter() => config.GetArenaCenter();

    protected virtual void Awake()
    {
        if (Instance != null && Instance != this) { Destroy(gameObject); return; }
        Instance = this;

        tileManager = new ArenaTileManager();
        playerPositioner = new ArenaPlayerPositioner();
        validator = new ArenaValidator();

        if (playerGrids != null && playerGrids.Count > 0 && playerGrids[0] != null)
            config.AutoDetectTileSize(playerGrids[0]);
    }

    /// <summary>Assemble the mini-game arena for all players.</summary>
    public virtual void AssembleArena()
    {
        if (IsAssembled) return;
        OnAssemblyStarted?.Invoke();

        if (!validator.CanAssembleArena(players, playerGrids, config, cameraController)) return;

        SaveOriginalPlayerPositions();
        if (cameraController != null) cameraController.SwitchToAssemblyCamera();

        RecordAllPlayerTiles();

        for (int i = 0; i < players.Count; i++)
        {
            if (!validator.CanAssembleArena(players, playerGrids, config, cameraController)) break;
            AssemblePlayerArena(i);
        }

        CompleteAssembly();
        PositionPlayersOnRecordedTiles();
    }

    /// <summary>Disassemble any assembled arena.</summary>
    public virtual void DisassembleArena()
    {
        if (!IsAssembled) return;

        tileManager.CleanupAllTiles();
        if (cameraController != null) cameraController.SwitchToIndividualCamera();
        RestoreOriginalPlayerPositions();
        IsAssembled = false;
        OnAssemblyStateChanged?.Invoke(false);
    }

    /// <summary>Create a simple ground (default tiles) centered at arena center.</summary>
    public void CreateDefaultGround()
    {
        Vector3 origin = GetArenaCenter();
        float size = Mathf.Approximately(config.tileSize, 0f) ? 1f : config.tileSize;

        float halfW = (config.arenaWidth - 1) * 0.5f;
        float halfH = (config.arenaHeight - 1) * 0.5f;

        for (int x = 0; x < config.arenaWidth; x++)
        {
            for (int z = 0; z < config.arenaHeight; z++)
            {
                Vector3 tilePos = origin + new Vector3((x - halfW) * size, 0f, (z - halfH) * size);
                GameObject tile = GameObject.CreatePrimitive(PrimitiveType.Cube);
                tile.transform.position = tilePos;
                tile.transform.localScale = new Vector3(size, 0.1f, size);
                tile.name = $"DefaultTile_{x}_{z}";
                tileManager.AddTileForTracking(tile);
            }
        }
    }

    /// <summary>Snapshot capture for every player.</summary>
    protected void RecordAllPlayerTiles()
    {
        for (int i = 0; i < players.Count; i++)
        {
            var player = players[i];
            if (!player) continue;
            WorldGrid grid = config.unifiedMapMode && playerGrids.Count > 0 ? playerGrids[0] : playerGrids[i];
            if (!grid) continue;
            playerPositioner.PlacePlayerTiles(player, grid, config.arenaWidth, config.arenaHeight, i, tileManager, GetArenaCenter());
        }
    }

    /// <summary>Instantiate assembled tiles for a player's quadrant.</summary>
    protected void AssemblePlayerArena(int playerIndex)
    {
        Vector3 quadrantCenter = playerPositioner.GetQuadrantOrigin(playerIndex, GetArenaCenter(), config.arenaWidth, config.arenaHeight, config.tileSize);
        playerPositioner.AssemblePlayerArena(playerIndex, quadrantCenter, tileManager, config.arenaWidth, config.arenaHeight, config.tileSize);
    }

    /// <summary>Place players on the recorded center tile within each quadrant.</summary>
    protected void PositionPlayersOnRecordedTiles()
    {
        for (int i = 0; i < players.Count; i++)
        {
            var rec = playerPositioner.GetPlayerRecordings(i);
            if (rec.Count == 0) continue;

            foreach (var s in rec)
            {
                if (!s.isPlayerTile) continue;

                Vector3 quad = playerPositioner.GetQuadrantOrigin(i, GetArenaCenter(), config.arenaWidth, config.arenaHeight, config.tileSize);
                int halfW = config.arenaWidth / 2;
                int halfH = config.arenaHeight / 2;

                Vector3 basePos = quad + new Vector3((s.localX - halfW) * config.tileSize, 0f, (s.localZ - halfH) * config.tileSize);
                Vector3 exact = basePos + s.playerOffsetInTile;
                playerPositioner.MovePlayerToArena(players[i], exact);
                break;
            }
        }
    }

    /// <summary>Save player world positions for later restoration.</summary>
    protected void SaveOriginalPlayerPositions()
    {
        originalPlayerPositions.Clear();
        foreach (var p in players) originalPlayerPositions.Add(p ? p.position : Vector3.zero);
    }

    /// <summary>Restore player positions captured before assembly.</summary>
    protected void RestoreOriginalPlayerPositions()
    {
        for (int i = 0; i < players.Count && i < originalPlayerPositions.Count; i++)
        {
            var t = players[i];
            if (!t) continue;
            var pc = t.GetComponent<Player>();
            if (pc != null) pc.ForceMoveTo(originalPlayerPositions[i]);
            else t.position = originalPlayerPositions[i];
        }
        originalPlayerPositions.Clear();
    }

    /// <summary>Mark assembled state and notify.</summary>
    protected void CompleteAssembly()
    {
        IsAssembled = true;
        OnAssemblyStateChanged?.Invoke(true);
        OnAssemblyCompleted?.Invoke();
    }
}
#endregion

#region Final Game Assembler
// ================================
// FINAL GAME ASSEMBLER 
// ================================

public class FinalGameAssembler : ArenaAssembler
{
    [Header("Final Game Arena")]
    [SerializeField] private Vector2Int arenaSize = new Vector2Int(20, 12);
    [SerializeField] private Vector3 arenaOffset = new Vector3(100f, 0f, 100f);

    [Header("Final Game: Tooth")]
    [SerializeField] private GameObject toothPrefab;
    [SerializeField] private float toothYHeight = 1f;
    private GameObject toothInstance;

    /// <summary>Assemble the final game arena (custom 20×12 grid, offset from main map).</summary>
    public override void AssembleArena()
    {
        if (IsAssembled) return;
        OnAssemblyStarted?.Invoke();

        SaveOriginalPlayerPositions();
        if (cameraController != null) cameraController.SwitchToAssemblyCamera();

        CreateCustomArena();
        PositionPlayersAtCorners();
        SpawnToothOnArena();

        CompleteAssembly();
    }

    /// <summary>Create a centered 20×12 grid of tiles at arena center + offset.</summary>
    private void CreateCustomArena()
    {
        Vector3 center = GetArenaCenter() + arenaOffset;
        float size = config.tileSize;
        float startX = -(arenaSize.x - 1) * 0.5f * size;
        float startZ = -(arenaSize.y - 1) * 0.5f * size;

        for (int x = 0; x < arenaSize.x; x++)
        {
            for (int z = 0; z < arenaSize.y; z++)
            {
                Vector3 pos = center + new Vector3(startX + x * size, 0f, startZ + z * size);
                GameObject tile = GameObject.CreatePrimitive(PrimitiveType.Cube);
                tile.transform.position = pos;
                tile.transform.localScale = new Vector3(size * 0.95f, 0.1f, size * 0.95f);
                tile.name = $"FinalGameTile_{x}_{z}";
                tileManager.AddTileForTracking(tile);
            }
        }
    }

    /// <summary>Place up to four players at the arena corners.</summary>
    private void PositionPlayersAtCorners()
    {
        Vector3 center = GetArenaCenter() + arenaOffset;
        float size = config.tileSize;
        float halfW = (arenaSize.x - 1) * 0.5f * size;
        float halfH = (arenaSize.y - 1) * 0.5f * size;

        Vector3[] corners =
        {
            center + new Vector3(-halfW, 1f, -halfH),
            center + new Vector3( halfW, 1f, -halfH),
            center + new Vector3(-halfW, 1f,  halfH),
            center + new Vector3( halfW, 1f,  halfH),
        };

        for (int i = 0; i < players.Count && i < corners.Length; i++)
        {
            var t = players[i];
            if (!t) continue;
            var pc = t.GetComponent<Player>();
            if (pc != null)
            {
                pc.ForceMoveTo(corners[i]);
                pc.RespawnPoint = corners[i];
            }
            else
            {
                t.position = corners[i];
            }
        }
    }

    /// <summary>Spawn the Tooth actor and initialize its patrol area.</summary>
    private void SpawnToothOnArena()
    {
        if (toothPrefab == null) return;

        Vector3 center = GetArenaCenter() + arenaOffset;
        float size = config.tileSize;

        int insetTiles = 1;
        float halfW = (arenaSize.x - 1) * 0.5f * size;
        float halfH = (arenaSize.y - 1) * 0.5f * size;

        float xMin = center.x - halfW + insetTiles * size;
        float zMin = center.z - halfH + insetTiles * size;

        Vector3 startPos = new Vector3(xMin, toothYHeight, zMin);
        toothInstance = Instantiate(toothPrefab, startPos, Quaternion.identity);
        toothInstance.name = "Tooth";

        var ctrl = toothInstance.GetComponent<ToothController>();
        if (ctrl != null)
        {
            ctrl.speed = 3f;
            ctrl.chaseRadiusTiles = 3;
            ctrl.yHeight = toothYHeight;
            ctrl.arenaWidth = arenaSize.x;
            ctrl.arenaHeight = arenaSize.y;
            ctrl.tileSize = config.tileSize;
            ctrl.arenaCenter = center;
            ctrl.insetTiles = insetTiles;

            var allPlayers = FindObjectsOfType<Player>();
            var playerTs = new List<Transform>();
            foreach (var p in allPlayers) if (p) playerTs.Add(p.transform);
            ctrl.RegisterPlayers(playerTs);
        }
    }

    /// <summary>Clean up final game arena and restore player/camera state.</summary>
    public override void DisassembleArena()
    {
        if (!IsAssembled) return;

        if (toothInstance) Destroy(toothInstance);
        tileManager.CleanupAllTiles();
        if (cameraController != null) cameraController.SwitchToIndividualCamera();
        RestoreOriginalPlayerPositions();
        IsAssembled = false;
        OnAssemblyStateChanged?.Invoke(false);
    }
}
#endregion

// ================================
// Unrelated Code has been Removed
// ================================
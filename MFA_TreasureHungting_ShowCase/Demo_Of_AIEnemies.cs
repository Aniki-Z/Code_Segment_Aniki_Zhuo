using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
#if ENABLE_INPUT_SYSTEM
using UnityEngine.InputSystem;
#endif

// Require a collider for contact reactions
[RequireComponent(typeof(Collider))]
public class ToothController : MonoBehaviour
{
    // ================================
    // STATE
    // ================================

    // Enemy state machine
    public enum State { Patrolling, Chasing, Returning }

    // Current state
    private State _state = State.Patrolling;

    // Patrol loop corner points (inset perimeter)
    private Vector3[] _loopCorners;

    // Active patrol corner index
    private int _patrolIndex;

    // Current chase target
    private Transform _chaseTarget;

    // Return-to-perimeter point after chase
    private Vector3 _rejoinPoint;

    // Corner index to resume after rejoin
    private int _nextCornerAfterRejoin;

    // Per-player ignore timer (post-hit grace)
    private readonly Dictionary<Transform, float> _ignoreUntil = new Dictionary<Transform, float>();

    // Internal stun state
    private bool _isStunned = false;

    // Time until tooth recovers from collision stun
    private float _stunUntil = 0f;

    // Cached arena edge bounds
    private float _arenaXMin, _arenaXMax, _arenaZMin, _arenaZMax;

    // Cached corners for bounds clamping
    private Vector3 _bottomLeft, _topLeft, _topRight, _bottomRight;

    // ================================
    // MOVEMENT
    // ================================

    // Units per second
    [Header("Movement")]
    [SerializeField] public float speed = 3f;

    // World-space threshold to consider “arrived”
    [SerializeField] public float arriveThreshold = 0.05f;

    // Fixed y-height to travel at
    [SerializeField] public float yHeight = 1f;

    // ================================
    // DETECTION
    // ================================

    // Player tag for fallback lookup
    [Header("Detection")]
    [SerializeField] public string playerTag = "Player";

    // Chase radius (in tiles)
    [SerializeField] public int chaseRadiusTiles = 3;

    // Registered player transforms (optional; falls back to tag lookup)
    private readonly List<Transform> _knownPlayers = new List<Transform>();

    // Registers player transforms (use to avoid Find-by-tag at runtime)
    public void RegisterPlayers(IEnumerable<Transform> players)
    {
        _knownPlayers.Clear();
        foreach (var t in players) if (t) _knownPlayers.Add(t);
    }

    // ================================
    // ARENA (TILE UNITS)
    // ================================

    // Arena width in tiles
    [Header("Arena (in tiles)")]
    [SerializeField] public int arenaWidth = 20;

    // Arena height in tiles
    [SerializeField] public int arenaHeight = 12;

    // Tile size in meters
    [SerializeField] public float tileSize = 1f;

    // Arena center in world space
    [SerializeField] public Vector3 arenaCenter = Vector3.zero;

    // Inset tiles from outer edge to form the patrolling loop
    [SerializeField] public int insetTiles = 1;

    // Computed chase radius in world units
    private float ChaseRadiusWorld => chaseRadiusTiles * tileSize;

    // ================================
    // COLLISION EFFECTS
    // ================================

    // Tooth freeze time after collision
    [Header("Collision Effects")]
    [SerializeField] public float collisionStunSeconds = 2f;

    // Player freeze time after collision
    [SerializeField] public float playerStunSeconds = 2f;

    // How long to ignore a hit player after collision
    [SerializeField] public float ignoreDuration = 5f;

    // ================================
    // LIFECYCLE
    // ================================

    private void Start()
    {
        BuildInsetLoop();
        SnapToNearestLoopPoint();
        _state = State.Patrolling;
    }

    private void Update()
    {
        if (_isStunned)
        {
            if (Time.time < _stunUntil) return;
            _isStunned = false;
        }

        _chaseTarget = FindNearestPlayerWithin(ChaseRadiusWorld);

        switch (_state)
        {
            case State.Patrolling:
                if (_chaseTarget != null) { _state = State.Chasing; break; }
                PatrolStep();
                break;

            case State.Chasing:
                if (_chaseTarget == null)
                {
                    ComputeRejoinPoint();
                    _state = State.Returning;
                    break;
                }
                ChaseStep();
                break;

            case State.Returning:
                if (_chaseTarget != null) { _state = State.Chasing; break; }
                ReturnStep();
                break;
        }
    }

    // ================================
    // STATE: PATROL / CHASE / RETURN
    // ================================

    private void PatrolStep()
    {
        Vector3 target = _loopCorners[_patrolIndex];
        MoveTowards(target);

        if ((transform.position - target).sqrMagnitude <= arriveThreshold * arriveThreshold)
            _patrolIndex = (_patrolIndex + 1) % _loopCorners.Length;
    }

    private void ChaseStep()
    {
        if (!_chaseTarget) return;
        Vector3 t = _chaseTarget.position; t.y = yHeight;
        MoveTowards(t);
    }

    private void ReturnStep()
    {
        MoveTowards(_rejoinPoint);
        if ((transform.position - _rejoinPoint).sqrMagnitude <= arriveThreshold * arriveThreshold)
        {
            transform.position = new Vector3(_rejoinPoint.x, yHeight, _rejoinPoint.z);
            _patrolIndex = _nextCornerAfterRejoin;
            _state = State.Patrolling;
        }
    }

    // ================================
    // MOVEMENT HELPERS
    // ================================

    private void MoveTowards(Vector3 target)
    {
        target = ClampToArena(target);

        Vector3 pos = transform.position;
        pos.y = yHeight;

        Vector3 next = Vector3.MoveTowards(pos, target, speed * Time.deltaTime);
        next = ClampToArena(next);
        transform.position = next;

        Vector3 dir = target - pos; dir.y = 0f;
        if (dir.sqrMagnitude > 0.0001f)
            transform.forward = Vector3.Lerp(transform.forward, dir.normalized, 0.2f);
    }

    private Vector3 ClampToArena(Vector3 v)
    {
        float x = Mathf.Clamp(v.x, _arenaXMin, _arenaXMax);
        float z = Mathf.Clamp(v.z, _arenaZMin, _arenaZMax);
        return new Vector3(x, yHeight, z);
    }

    // ================================
    // TARGETING
    // ================================

    private Transform FindNearestPlayerWithin(float radius)
    {
        IEnumerable<Transform> candidates = _knownPlayers.Count > 0
            ? _knownPlayers
            : GameObject.FindGameObjectsWithTag(playerTag).Select(go => go.transform);

        float r2 = radius * radius;
        Transform best = null;
        float bestD2 = float.MaxValue;

        Vector3 me = transform.position; me.y = yHeight;

        foreach (var t in candidates)
        {
            if (!t) continue;
            if (IsIgnored(t)) continue;

            Vector3 tp = t.position; tp.y = yHeight;

            float d2 = (tp - me).sqrMagnitude;
            if (d2 <= r2 && d2 < bestD2) { bestD2 = d2; best = t; }
        }
        return best;
    }

    private bool IsIgnored(Transform t)
    {
        if (!t) return false;
        if (_ignoreUntil.TryGetValue(t, out float until))
        {
            if (Time.time < until) return true;
            _ignoreUntil.Remove(t);
        }
        return false;
    }

    // ================================
    // PERIMETER LOOP
    // ================================

    private void BuildInsetLoop()
    {
        float halfW = (arenaWidth - 1) * 0.5f * tileSize;
        float halfH = (arenaHeight - 1) * 0.5f * tileSize;

        float xMin = arenaCenter.x - halfW + insetTiles * tileSize;
        float xMax = arenaCenter.x + halfW - insetTiles * tileSize;
        float zMin = arenaCenter.z - halfH + insetTiles * tileSize;
        float zMax = arenaCenter.z + halfH - insetTiles * tileSize;

        _bottomLeft = new Vector3(xMin, yHeight, zMin);
        _topLeft = new Vector3(xMin, yHeight, zMax);
        _topRight = new Vector3(xMax, yHeight, zMax);
        _bottomRight = new Vector3(xMax, yHeight, zMin);

        _loopCorners = new[] { _bottomLeft, _topLeft, _topRight, _bottomRight };

        _arenaXMin = xMin; _arenaXMax = xMax;
        _arenaZMin = zMin; _arenaZMax = zMax;
    }

    private void SnapToNearestLoopPoint()
    {
        Vector3 p; int nextIdx;
        ClosestPointOnPerimeter(transform.position, out p, out nextIdx);
        transform.position = new Vector3(p.x, yHeight, p.z);
        _patrolIndex = nextIdx;
    }

    private void ComputeRejoinPoint()
    {
        Vector3 p; int nextIdx;
        ClosestPointOnPerimeter(transform.position, out p, out nextIdx);
        _rejoinPoint = p;
        _nextCornerAfterRejoin = nextIdx;
    }

    private void ClosestPointOnPerimeter(Vector3 pos, out Vector3 closest, out int nextCornerIdx)
    {
        Vector3[] c = _loopCorners;
        Vector3[] eA = { c[0], c[1], c[2], c[3] };
        Vector3[] eB = { c[1], c[2], c[3], c[0] };

        float best = float.MaxValue;
        closest = pos;
        int bestEdge = 0;

        Vector3 flatPos = new Vector3(pos.x, yHeight, pos.z);
        for (int i = 0; i < 4; i++)
        {
            Vector3 q = ClosestPointOnSegment(eA[i], eB[i], flatPos);
            float d2 = (q - flatPos).sqrMagnitude;
            if (d2 < best) { best = d2; closest = q; bestEdge = i; }
        }
        nextCornerIdx = (bestEdge + 1) % 4;
    }

    private static Vector3 ClosestPointOnSegment(Vector3 a, Vector3 b, Vector3 p)
    {
        Vector3 ab = b - a;
        float ab2 = Vector3.Dot(ab, ab);
        if (ab2 <= Mathf.Epsilon) return a;
        float t = Mathf.Clamp01(Vector3.Dot(p - a, ab) / ab2);
        return a + t * ab;
    }

    // ================================
    // COLLISION HANDLING
    // ================================

    private void OnTriggerEnter(Collider other)
    {
        var p = other.GetComponent<Player>() ?? other.GetComponentInParent<Player>();
        if (p != null) { HandlePlayerContact(p.transform); return; }
        if (other.CompareTag(playerTag)) HandlePlayerContact(other.transform);
    }

    private void OnCollisionEnter(Collision collision)
    {
        var p = collision.collider.GetComponent<Player>() ?? collision.collider.GetComponentInParent<Player>();
        if (p != null) { HandlePlayerContact(p.transform); return; }
        if (collision.collider.CompareTag(playerTag)) HandlePlayerContact(collision.collider.transform);
    }

    private void HandlePlayerContact(Transform playerRoot)
    {
        if (!playerRoot) return;

        _ignoreUntil[playerRoot] = Time.time + ignoreDuration;

        var player = playerRoot.GetComponent<Player>() ?? playerRoot.GetComponentInParent<Player>();
        if (player != null)
        {
            StartCoroutine(StunPlayerInput(player, playerStunSeconds));

            if (player.HasMiniGameCollectible())
                player.GetMiniGameCollectible().DropFromPlayer();
        }

        _chaseTarget = null;
        ComputeRejoinPoint();
        _state = State.Returning;

        _isStunned = true;
        _stunUntil = Time.time + collisionStunSeconds;
    }

    // ================================
    // PLAYER INPUT STUN
    // ================================

    private IEnumerator StunPlayerInput(Player target, float seconds)
    {
        if (target == null) yield break;

#if ENABLE_INPUT_SYSTEM
        var inputs = target.GetComponentsInChildren<PlayerInput>(true);
        if (inputs == null || inputs.Length == 0) yield break;

        foreach (var pi in inputs) if (pi != null) pi.DeactivateInput();
        float t = 0f;
        while (target != null && t < seconds) { t += Time.deltaTime; yield return null; }
        foreach (var pi in inputs) if (pi != null) pi.ActivateInput();
#else
        yield return new WaitForSeconds(seconds);
#endif
    }
}

// ================================
// Unrelated Code has been Removed
// ================================
using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using UnityEngine;
#if ENABLE_INPUT_SYSTEM
using UnityEngine.InputSystem;
#endif

// ================================
// EQUIPMENT ITEM
// ================================

/// <summary>
/// Base class for all equippable items.
/// </summary>
public abstract class EquipmentItem : MonoBehaviour
{
    [Header("Item Basic Information")]
    [SerializeField] public string itemName = "Unnamed Item";
    [SerializeField] protected string itemDescription = "Item description";

    [Header("Usage Settings")]
    [SerializeField] protected float cooldownTime = 1f;
    [SerializeField] protected bool isConsumable = false;
    [SerializeField] protected int useCount = 1;

    [Header("Status")]
    [SerializeField] protected bool isEquipped = false;
    [SerializeField] protected int currentUseCount;
    [SerializeField] protected float lastUseTime;
    [SerializeField] protected bool shouldDelete = false;

    [Header("Inventory UI")]
    [SerializeField] public Sprite InventoryUI;

    /// <summary>Item name property.</summary>
    public string ItemName => itemName;

    /// <summary>Item description property.</summary>
    public string ItemDescription => itemDescription;

    /// <summary>Cooldown seconds between uses.</summary>
    public float CooldownTime => cooldownTime;

    /// <summary>True if item is consumable.</summary>
    public bool IsConsumable => isConsumable;

    /// <summary>Total allowed uses (for consumables).</summary>
    public int UseCount => useCount;

    /// <summary>True if currently equipped.</summary>
    public bool IsEquipped => isEquipped;

    /// <summary>Remaining uses (for consumables).</summary>
    public int CurrentUseCount => currentUseCount;

    /// <summary>Time of last use.</summary>
    public float LastUseTime => lastUseTime;

    /// <summary>True if cooldown permits use (or not equipped yet).</summary>
    public bool CanUse => !isEquipped || (Time.time - lastUseTime >= cooldownTime);

    /// <summary>True when a consumable has been used up.</summary>
    public bool IsEmpty => isConsumable && currentUseCount <= 0;

    /// <summary>True if this instance should be deleted by the slot.</summary>
    public bool ShouldDelete => shouldDelete;

    /// <summary>Fired when the item is used successfully.</summary>
    public Action<EquipmentItem> OnItemUsed;

    /// <summary>Fired when the item is equipped.</summary>
    public Action<EquipmentItem> OnItemEquipped;

    /// <summary>Fired when the item is unequipped.</summary>
    public Action<EquipmentItem> OnItemUnequipped;

    /// <summary>Fired when a consumable is fully consumed.</summary>
    public Action<EquipmentItem> OnItemConsumed;

    protected virtual void Awake()
    {
        currentUseCount = useCount;
    }

    /// <summary>Entry point to use the item.</summary>
    public virtual bool Use(Player player)
    {
        if (!CanUse || IsEmpty) return false;

        bool success = OnUse(player);
        if (!success) return false;

        lastUseTime = Time.time;
        OnItemUsed?.Invoke(this);

        if (isConsumable)
        {
            currentUseCount--;
            if (currentUseCount <= 0)
            {
                OnItemConsumed?.Invoke(this);
            }
        }
        return true;
    }

    /// <summary>Called when item is equipped into a slot.</summary>
    public virtual void OnEquipped(int slotIndex)
    {
        isEquipped = true;
        OnItemEquipped?.Invoke(this);
    }

    /// <summary>Called when item is unequipped from a slot.</summary>
    public virtual void OnUnequipped()
    {
        isEquipped = false;
        OnItemUnequipped?.Invoke(this);
    }

    /// <summary>Implement item-specific use behavior.</summary>
    protected abstract bool OnUse(Player player);

    /// <summary>Human-readable item info for UI.</summary>
    public virtual string GetItemInfo()
    {
        string info = $"{itemName}\n{itemDescription}";

        if (isConsumable)
        {
            info += $"\nRemaining uses: {currentUseCount}/{useCount}";
        }

        if (!CanUse)
        {
            float remainingCooldown = Mathf.Max(0f, cooldownTime - (Time.time - lastUseTime));
            info += $"\nCooldown: {remainingCooldown:F1}s";
        }

        return info;
    }

    /// <summary>Reset runtime state.</summary>
    public virtual void ResetItem()
    {
        currentUseCount = useCount;
        lastUseTime = 0f;
        isEquipped = false;
    }
}

// ================================
// EQUIPMENT SLOT
// ================================

/// <summary>
/// Single equipment slot state.
/// </summary>
[Serializable]
public class EquipmentSlot
{
    [Header("Slot Information")]
    public int slotIndex;
    public bool isEmpty = true;
    public EquipmentItem equippedItem;

    [Header("Display Information")]
    public string slotName;

    public EquipmentSlot(int index)
    {
        slotIndex = index;
        isEmpty = true;
        equippedItem = null;
        slotName = $"Equipment Slot {index + 1}";
    }

    /// <summary>Equip an item into this slot.</summary>
    public bool EquipItem(EquipmentItem item)
    {
        if (item == null || !isEmpty) return false;
        equippedItem = item;
        isEmpty = false;
        item.OnEquipped(slotIndex);
        return true;
    }

    /// <summary>Unequip and return the item.</summary>
    public EquipmentItem UnequipItem()
    {
        if (isEmpty || equippedItem == null) return null;
        EquipmentItem item = equippedItem;
        equippedItem.OnUnequipped();
        equippedItem = null;
        isEmpty = true;
        return item;
    }

    /// <summary>Use the item stored in this slot.</summary>
    public bool UseItem(Player player)
    {
        if (isEmpty || equippedItem == null) return false;
        equippedItem.Use(player);
        if (equippedItem.ShouldDelete)
        {
            isEmpty = true;
        }
        return true;
    }

    /// <summary>Clear slot state.</summary>
    public void Reset()
    {
        isEmpty = true;
        equippedItem = null;
    }

    /// <summary>UI-friendly string.</summary>
    public string GetSlotInfo()
    {
        return isEmpty ? $"{slotName}: Empty" : $"{slotName}: {equippedItem.itemName}";
    }
}

// ================================
// EQUIPMENT MANAGER 
// ================================

/// <summary>
/// Manages the owner's equipment slots, pickup, and use flow.
/// </summary>
public class EquipmentManager : MonoBehaviour
{
    [Header("Equipment Slot Settings")]
    [SerializeField] private int maxSlots = 4;
    [SerializeField] private EquipmentSlot[] equipmentSlots;

    [Header("Pickup Settings")]
    [SerializeField] private float pickupRange = 2f;
    [SerializeField] private LayerMask equipmentLayerMask = 1 << 7;

    private Player owner;
    private Collider[] nearbyEquipmentCache = new Collider[10];
    private readonly List<EquipmentItem> nearbyEquipment = new List<EquipmentItem>();

    /// <summary>Fired when an item is equipped (item, slotIndex).</summary>
    public Action<EquipmentItem, int> OnItemEquipped;

    /// <summary>Fired when an item is unequipped (item, slotIndex).</summary>
    public Action<EquipmentItem, int> OnItemUnequipped;

    /// <summary>Fired when an item is used (item, slotIndex).</summary>
    public Action<EquipmentItem, int> OnItemUsed;

    /// <summary>Fired when an item is picked up.</summary>
    public Action<EquipmentItem> OnItemPickedUp;

    /// <summary>Fired when an item is dropped.</summary>
    public Action<EquipmentItem> OnItemDropped;

    /// <summary>Max slots.</summary>
    public int MaxSlots => maxSlots;

    /// <summary>Slots array.</summary>
    public EquipmentSlot[] EquipmentSlots => equipmentSlots;

    /// <summary>True if there is at least one empty slot.</summary>
    public bool HasEmptySlot => GetFirstEmptySlotIndex() != -1;

    /// <summary>Number of used slots.</summary>
    public int UsedSlots => GetUsedSlotCount();

    private void Awake()
    {
        owner = GetComponent<Player>();
        InitializeSlots();
    }

    private void Update()
    {
        UpdateNearbyEquipment();
        TryAutoPickupEquipment();
    }

    /// <summary>Initialize slots array.</summary>
    private void InitializeSlots()
    {
        equipmentSlots = new EquipmentSlot[maxSlots];
        for (int i = 0; i < maxSlots; i++)
        {
            equipmentSlots[i] = new EquipmentSlot(i);
        }
    }

    /// <summary>Populate nearby equipment list based on pickup range.</summary>
    private void UpdateNearbyEquipment()
    {
        nearbyEquipment.Clear();
        int hitCount = Physics.OverlapSphereNonAlloc(transform.position, pickupRange, nearbyEquipmentCache, equipmentLayerMask);
        for (int i = 0; i < hitCount; i++)
        {
            if (nearbyEquipmentCache[i] == null) continue;

            if (nearbyEquipmentCache[i].GetComponent<MiniGameCollectible>() != null)
            {
                continue;
            }

            var equipment = nearbyEquipmentCache[i].GetComponent<EquipmentItem>();
            if (equipment != null)
            {
                nearbyEquipment.Add(equipment);
            }
        }
    }

    /// <summary>Auto-pickup first nearby item if there is an empty slot.</summary>
    private void TryAutoPickupEquipment()
    {
        if (!HasEmptySlot) return;
        foreach (var equipment in nearbyEquipment)
        {
            if (TryPickupEquipment(equipment)) break;
        }
    }

    /// <summary>Attempt to pick up and equip an equipment item.</summary>
    public bool TryPickupEquipment(EquipmentItem equipment)
    {
        if (equipment == null) return false;
        if (!HasEmptySlot) return false;

        int emptySlotIndex = GetFirstEmptySlotIndex();
        if (emptySlotIndex == -1) return false;

        if (equipmentSlots[emptySlotIndex].EquipItem(equipment))
        {
            equipment.gameObject.SetActive(false);
            OnItemPickedUp?.Invoke(equipment);
            OnItemEquipped?.Invoke(equipment, emptySlotIndex);
            return true;
        }
        return false;
    }

    /// <summary>Use equipment from a given slot.</summary>
    public bool UseEquipment(int slotIndex)
    {
        if (slotIndex < 0 || slotIndex >= maxSlots) return false;

        var slot = equipmentSlots[slotIndex];
        if (slot.isEmpty) return false;

        bool success = slot.UseItem(owner);
        if (success)
        {
            OnItemUsed?.Invoke(slot.equippedItem, slotIndex);

            if (slot.equippedItem.IsEmpty)
            {
                slot.Reset();
                OnItemDropped?.Invoke(slot.equippedItem);
            }
        }
        return success;
    }

    /// <summary>Drop and unequip item from a slot.</summary>
    public bool DropEquipment(int slotIndex)
    {
        if (slotIndex < 0 || slotIndex >= maxSlots) return false;

        var slot = equipmentSlots[slotIndex];
        if (slot.isEmpty) return false;

        var equipment = slot.UnequipItem();
        if (equipment != null)
        {
            equipment.gameObject.SetActive(true);
            equipment.transform.position = transform.position + transform.forward * 2f;
            equipment.transform.rotation = Quaternion.identity;

            OnItemDropped?.Invoke(equipment);
            OnItemUnequipped?.Invoke(equipment, slotIndex);
            return true;
        }
        return false;
    }

    /// <summary>Find first empty slot index or -1.</summary>
    public int GetFirstEmptySlotIndex()
    {
        for (int i = 0; i < maxSlots; i++)
            if (equipmentSlots[i].isEmpty) return i;
        return -1;
    }

    /// <summary>Count non-empty slots.</summary>
    public int GetUsedSlotCount()
    {
        int count = 0;
        for (int i = 0; i < maxSlots; i++)
            if (!equipmentSlots[i].isEmpty) count++;
        return count;
    }

    /// <summary>Get equipment instance by slot index.</summary>
    public EquipmentItem GetEquipment(int slotIndex)
    {
        if (slotIndex < 0 || slotIndex >= maxSlots) return null;
        return equipmentSlots[slotIndex].equippedItem;
    }

    /// <summary>Readable dump of all slot states.</summary>
    public string GetAllEquipmentInfo()
    {
        string info = "Equipment Slot Information:\n";
        for (int i = 0; i < maxSlots; i++)
        {
            info += $"{equipmentSlots[i].GetSlotInfo()}\n";
        }
        return info;
    }

    /// <summary>Helper for input handling.</summary>
    public void HandleEquipmentInput(bool use1, bool use2, bool use3, bool use4)
    {
        if (use1) UseEquipment(0);
        if (use2) UseEquipment(1);
        if (use3) UseEquipment(2);
        if (use4) UseEquipment(3);
    }

    /// <summary>Equip a prefabbed item copy directly (e.g., reward).</summary>
    public bool TryEquipItem(EquipmentItem equipment)
    {
        if (equipment == null) return false;
        if (!HasEmptySlot) return false;

        int emptySlotIndex = GetFirstEmptySlotIndex();
        if (emptySlotIndex == -1) return false;

        EquipmentItem equipmentCopy = Instantiate(equipment);
        if (equipmentSlots[emptySlotIndex].EquipItem(equipmentCopy))
        {
            OnItemEquipped?.Invoke(equipmentCopy, emptySlotIndex);
            return true;
        }
        if (equipmentCopy != null) Destroy(equipmentCopy.gameObject);
        return false;
    }
}

// ================================
// ITEM: ENERGY DRINK
// ================================

/// <summary>
/// Temporarily boosts move speed and dig speed; consumable pickup.
/// </summary>
[RequireComponent(typeof(Collider))]
public class EnergyDrink : EquipmentItem
{
    [Header("Energy Drink Settings")]
    [SerializeField] private float speedMultiplier = 2f;
    [SerializeField] private float boostDuration = 5f;
    [SerializeField] private float selfDestructDelay = 2f;

    [Header("Pickup Behaviour")]
    [SerializeField] private bool autoUseOnPickup = true;
    [SerializeField] private bool hideOnPickup = true;

    private bool pickedUp;

    protected override void Awake()
    {
        base.Awake();
        if (useCount <= 0) useCount = 1;
        isConsumable = true;
        OnItemConsumed += UseUp;
    }

    private void Reset()
    {
        var col = GetComponent<Collider>();
        if (col) col.isTrigger = true;
    }

    protected override bool OnUse(Player player)
    {
        if (player == null) return false;

        player.ApplySpeedBoost(speedMultiplier);
        player.StartCoroutine(RemoveMoveBoostAfterDelay(player, boostDuration));

        var digBar = player.GetComponent<DigProgressBar>() ?? player.GetComponentInChildren<DigProgressBar>();
        if (digBar != null)
        {
            player.StartCoroutine(ApplyDigSpeedBoost(digBar, speedMultiplier, boostDuration));
        }

        return true;
    }

    private void OnTriggerEnter(Collider other)
    {
        if (!autoUseOnPickup || pickedUp) return;

        Player player = other.GetComponent<Player>() ?? other.GetComponentInParent<Player>();
        if (player == null) return;

        if (Use(player))
        {
            pickedUp = true;
            if (hideOnPickup) DisablePickupVisuals();
            Destroy(gameObject, selfDestructDelay);
        }
    }

    private static IEnumerator RemoveMoveBoostAfterDelay(Player player, float seconds)
    {
        yield return new WaitForSeconds(seconds);
        if (player != null) player.RemoveSpeedBoost();
    }

    private static IEnumerator ApplyDigSpeedBoost(DigProgressBar digBar, float multiplier, float duration)
    {
        if (digBar == null) yield break;

        FieldInfo fi = typeof(DigProgressBar).GetField("progressIncreaseSpeed", BindingFlags.NonPublic | BindingFlags.Instance);
        if (fi == null) yield break;

        float current = (float)fi.GetValue(digBar);
        fi.SetValue(digBar, current * multiplier);

        yield return new WaitForSeconds(duration);

        if (digBar != null)
        {
            float now = (float)fi.GetValue(digBar);
            fi.SetValue(digBar, now / multiplier);
        }
    }

    private void DisablePickupVisuals()
    {
        var col = GetComponent<Collider>();
        if (col) col.enabled = false;

        var renderers = GetComponentsInChildren<Renderer>(true);
        foreach (var r in renderers) r.enabled = false;
    }

    private void UseUp(EquipmentItem item)
    {
        shouldDelete = true;
        Destroy(gameObject, selfDestructDelay);
    }
}

// ================================
// ITEM: FEATHER
// ================================

/// <summary>
/// Points at the nearest treasure with slight bias; shows a worldspace arrow.
/// </summary>
public class Feather : EquipmentItem
{
    [Header("Core")]
    [SerializeField] private string itemId = "direction_feather";
    [SerializeField] private float duration = 10f;
    [SerializeField] private float refreshInterval = 0.2f;
    [SerializeField] private float biasDegree = 8f;
    [SerializeField] private float noiseChangeRate = 30f;
    [SerializeField] private float maxLockDistance = 120f;

    [Header("Targeting")]
    [SerializeField] private bool excludeContested = true;
    [SerializeField] private bool requireLineOfSight = false;

    [Header("FX/UI")]
    [SerializeField] private Sprite arrowSprite;
    [SerializeField] private Color arrowColor = Color.yellow;
    [SerializeField] private float arrowVerticalOffset = 2.0f;
    [SerializeField] private float arrowSlerpSpeed = 18f;
    [SerializeField] private AudioClip sfxPing;

    private GameObject arrowInstance;
    private SpriteRenderer arrowRenderer;
    private TileTreasureComponent currentTarget;
    private Coroutine trackingRoutine;
    private float currentNoiseAngleDeg = 0f;
    private float noiseGoalAngleDeg = 0f;
    private float refreshTimer = 0f;

    protected override void Awake()
    {
        base.Awake();
        itemName = "Feather";
        itemDescription = "Points at the nearest treasure with a slight bias.";
        OnItemConsumed += _ => shouldDelete = true;
    }

    protected override bool OnUse(Player player)
    {
        if (player == null) return false;

        CleanupArrow();
        if (trackingRoutine != null)
        {
            player.StopCoroutine(trackingRoutine);
            trackingRoutine = null;
        }

        CreateOrUpdateArrow(player);
        currentTarget = AcquireNearestTarget(player.transform.position, player);

        if (sfxPing != null)
        {
            AudioSource.PlayClipAtPoint(sfxPing, player.transform.position);
        }

        trackingRoutine = player.StartCoroutine(TrackAndPointRoutine(player));
        return true;
    }

    /// <summary>Acquire nearest valid treasure.</summary>
    private TileTreasureComponent AcquireNearestTarget(Vector3 playerPosition, Player player)
    {
        TileTreasureComponent[] all = FindObjectsOfType<TileTreasureComponent>();
        float best = float.MaxValue;
        TileTreasureComponent pick = null;

        foreach (var treasure in all)
        {
            if (treasure == null) continue;
            if (!treasure.hasTreasure || treasure.isDiscovered) continue;

            float d = Vector3.Distance(playerPosition, treasure.transform.position);
            if (d > maxLockDistance) continue;

            if (requireLineOfSight)
            {
                Vector3 origin = player.transform.position + Vector3.up * 1.6f;
                Vector3 dir = treasure.transform.position - origin;
                float dist = dir.magnitude;
                if (Physics.Raycast(origin, dir.normalized, out RaycastHit hit, dist))
                {
                    if (hit.transform != treasure.transform) continue;
                }
            }

            if (excludeContested)
            {
                // contest rule can be added here if project supports it
            }

            if (d < best)
            {
                best = d;
                pick = treasure;
            }
        }
        return pick;
    }

    /// <summary>Create or refresh the arrow instance above the player.</summary>
    private void CreateOrUpdateArrow(Player player)
    {
        if (arrowInstance == null)
        {
            arrowInstance = new GameObject("DirectionFeatherArrow");
            arrowRenderer = arrowInstance.AddComponent<SpriteRenderer>();
            arrowRenderer.sortingOrder = 1000;
        }

        arrowRenderer.sprite = arrowSprite;
        arrowRenderer.color = arrowColor;

        arrowInstance.transform.position = player.transform.position + Vector3.up * arrowVerticalOffset;
        arrowInstance.transform.rotation = Quaternion.LookRotation(player.transform.forward, Vector3.up);
        arrowInstance.SetActive(true);
    }

    /// <summary>Tracking loop: maintain arrow position and yaw toward target with bias.</summary>
    private IEnumerator TrackAndPointRoutine(Player player)
    {
        float startTime = Time.time;
        refreshTimer = 0f;
        currentNoiseAngleDeg = UnityEngine.Random.Range(-biasDegree, biasDegree);
        noiseGoalAngleDeg = UnityEngine.Random.Range(-biasDegree, biasDegree);

        while (player != null && (Time.time - startTime) < duration)
        {
            if (arrowInstance == null) yield break;

            arrowInstance.transform.position = player.transform.position + Vector3.up * arrowVerticalOffset;

            refreshTimer += Time.deltaTime;
            if (refreshTimer >= refreshInterval)
            {
                refreshTimer = 0f;
                TileTreasureComponent latest = AcquireNearestTarget(player.transform.position, player);
                if (latest != currentTarget) currentTarget = latest;
                noiseGoalAngleDeg = UnityEngine.Random.Range(-biasDegree, biasDegree);
            }

            float maxStep = noiseChangeRate * Time.deltaTime;
            currentNoiseAngleDeg = Mathf.MoveTowards(currentNoiseAngleDeg, noiseGoalAngleDeg, maxStep);

            Quaternion desired = arrowInstance.transform.rotation;
            if (currentTarget != null)
            {
                Vector3 toTarget = currentTarget.transform.position - player.transform.position;
                toTarget.y = 0f;
                if (toTarget.sqrMagnitude > 0.0001f)
                {
                    Vector3 dir = toTarget.normalized;
                    Quaternion noiseRot = Quaternion.AngleAxis(currentNoiseAngleDeg, Vector3.up);
                    Vector3 biasedDir = noiseRot * dir;
                    Quaternion correction = Quaternion.Euler(-90f, 0f, 0f);
                    desired = Quaternion.LookRotation(biasedDir, Vector3.up) * correction;
                }
            }

            arrowInstance.transform.rotation = Quaternion.Slerp(
                arrowInstance.transform.rotation,
                desired,
                Mathf.Clamp01(arrowSlerpSpeed * Time.deltaTime)
            );

            yield return null;
        }

        CleanupArrow();
        Destroy(gameObject);
    }

    /// <summary>Destroy arrow instance.</summary>
    private void CleanupArrow()
    {
        if (arrowInstance != null)
        {
            Destroy(arrowInstance);
            arrowInstance = null;
            arrowRenderer = null;
        }
    }
}

// ================================
// ITEM: GAUNTLET
// ================================

/// <summary>
/// Push-only melee gauntlet with helper model and lunge.
/// </summary>
public class Gauntlet : EquipmentItem
{
    [Header("Gauntlet Helper")]
    [SerializeField] private GameObject gauntletHelperPrefab;
    [SerializeField] private bool isUsing = false;

    private GameObject gauntletModel;
    private Player owner;

    protected override void Awake()
    {
        base.Awake();
        isConsumable = false;
        if (string.IsNullOrEmpty(itemName)) itemName = "Gauntlet";
        if (string.IsNullOrEmpty(itemDescription)) itemDescription = "Push-only gauntlet";
    }

    protected override bool OnUse(Player player)
    {
        if (isUsing)
        {
            owner = null;
            isUsing = false;
            if (gauntletModel != null) gauntletModel.SetActive(false);
        }
        else
        {
            owner = player;
            if (gauntletModel != null) gauntletModel.SetActive(true);
            else CreateHelper();
            isUsing = true;
        }
        return true;
    }

    /// <summary>Create and initialize the GauntletHelper.</summary>
    private void CreateHelper()
    {
        if (gauntletHelperPrefab == null) return;
        gauntletModel = Instantiate(gauntletHelperPrefab, owner.transform);
        var helper = gauntletModel.GetComponent<GauntletHelper>();
        if (helper != null) helper.Initialize(owner);
    }
}

/// <summary>
/// Helper model that lunges forward and applies push/stun on hit.
/// </summary>
[RequireComponent(typeof(Collider))]
public class GauntletHelper : MonoBehaviour
{
    [Header("Attach to Player")]
    [SerializeField] public Vector3 localOffset = new Vector3(-0.6f, 0.86f, 1.8f);
    [SerializeField] public Vector3 localEuler = Vector3.zero;
    [SerializeField] public Vector3 localScale = Vector3.one * 2.5f;

    [Header("Lunge Settings")]
    [SerializeField] public float lungeDistance = 2.7f;
    [SerializeField] public float lungeTotalSeconds = 0.7f;

    [Header("Hit Effect (Push Only)")]
    [SerializeField] public float knockbackForce = 35f;
    [SerializeField] public float knockbackDuration = 0.2f;

    [Header("Runtime Cooldowns")]
    [SerializeField] public float selfHitCooldown = 2f;
    [SerializeField] public float perTargetCooldown = 1.0f;

    [Header("Stun")]
    [SerializeField] public float stunSeconds = 3f;

    private Player owner;
    private Collider col;
    private bool _selfCooling;
    private readonly Dictionary<Player, float> _lastHitTime = new();
    private bool _swinging;
    private Vector3 _restLocalPos;

    private void Awake()
    {
        col = gameObject.GetComponent<Collider>();
        col.isTrigger = true;

        var rb = gameObject.GetComponent<Rigidbody>();
        if (rb == null) rb = gameObject.AddComponent<Rigidbody>();
        rb.isKinematic = true;
        rb.useGravity = false;
    }

    private void Reset()
    {
        var c = GetComponent<Collider>();
        if (c) c.isTrigger = true;
    }

    /// <summary>Bind to owner and set initial local transform.</summary>
    public void Initialize(Player player)
    {
        owner = player;
        AttachToPlayerCenter(owner);
    }

    private void Update()
    {
        if (owner == null || _swinging) return;

        bool pressed = false;
#if ENABLE_INPUT_SYSTEM
        pressed = (Keyboard.current != null && Keyboard.current.digit0Key.wasPressedThisFrame)
                  || (Gamepad.current != null && Gamepad.current.rightTrigger.wasPressedThisFrame);
#endif
        if (!pressed) return;

        col.enabled = true;
        StartCoroutine(LungeOutAndBack());
    }

    private void OnTriggerEnter(Collider other)
    {
        if (owner == null) return;
        if (_selfCooling) return;

        var hitPlayer = other.GetComponent<Player>() ?? other.GetComponentInParent<Player>();
        if (hitPlayer == null || hitPlayer == owner) return;

        if (_lastHitTime.TryGetValue(hitPlayer, out float lastTime))
            if (Time.time - lastTime < perTargetCooldown) return;

        _lastHitTime[hitPlayer] = Time.time;

        Vector3 knockDir = transform.forward.normalized;

        hitPlayer.StartCoroutine(PushOnly(hitPlayer, knockDir, knockbackForce, knockbackDuration));
        StartCoroutine(SelfCooldown());
        StartCoroutine(StunPlayerInput(hitPlayer, stunSeconds));
    }

    /// <summary>Self cooldown after a successful hit.</summary>
    private IEnumerator SelfCooldown()
    {
        _selfCooling = true;
        yield return new WaitForSeconds(selfHitCooldown);
        _selfCooling = false;
    }

    /// <summary>Attach to player center and ignore owner collisions.</summary>
    private void AttachToPlayerCenter(Player p)
    {
        transform.SetParent(p.transform, false);
        transform.localPosition = localOffset;
        transform.localEulerAngles = localEuler;
        transform.localScale = localScale;

        _restLocalPos = localOffset;

        if (col)
        {
            col.isTrigger = true;
            col.enabled = false;
        }

        foreach (var r in GetComponentsInChildren<Renderer>(true)) r.enabled = true;

        var ownerCols = p.GetComponentsInChildren<Collider>(true);
        foreach (var oc in ownerCols)
        {
            if (oc != null && col != null && oc != col)
                Physics.IgnoreCollision(col, oc, true);
        }
    }

    /// <summary>Animate out-and-back lunge, enabling hit window.</summary>
    private IEnumerator LungeOutAndBack()
    {
        _swinging = true;

        float half = Mathf.Max(0.0001f, lungeTotalSeconds * 0.5f);
        Vector3 outLocal = _restLocalPos + Vector3.forward * lungeDistance;

        float t = 0f;
        while (t < half)
        {
            t += Time.deltaTime;
            float a = Mathf.Clamp01(t / half);
            transform.localPosition = Vector3.Lerp(_restLocalPos, outLocal, SmoothInOut(a));
            yield return null;
        }

        t = 0f;
        while (t < half)
        {
            t += Time.deltaTime;
            float a = Mathf.Clamp01(t / half);
            transform.localPosition = Vector3.Lerp(outLocal, _restLocalPos, SmoothInOut(a));
            yield return null;
        }

        transform.localPosition = _restLocalPos;
        _swinging = false;
        col.enabled = false;
    }

    /// <summary>Smoothstep 3x^2-2x^3 easing.</summary>
    private static float SmoothInOut(float x) => x * x * (3f - 2f * x);

    /// <summary>Apply push velocity to a target for a short duration.</summary>
    private static IEnumerator PushOnly(Player target, Vector3 dir, float force, float duration)
    {
        if (target == null) yield break;

        float t = 0f;
        Vector3 push = dir.normalized * force;

        while (target != null && t < duration)
        {
            target.PushVelocity = push;
            t += Time.deltaTime;
            yield return null;
        }

        if (target != null) target.PushVelocity = Vector3.zero;
    }

    /// <summary>Temporarily disable target input, then restore.</summary>
    private static IEnumerator StunPlayerInput(Player target, float seconds)
    {
        if (target == null) yield break;

#if ENABLE_INPUT_SYSTEM
        var inputs = target.GetComponentsInChildren<UnityEngine.InputSystem.PlayerInput>(true);
        if (inputs != null)
        {
            foreach (var pi in inputs) if (pi != null) pi.DeactivateInput();
            float t = 0f;
            while (target != null && t < seconds) { t += Time.deltaTime; yield return null; }
            foreach (var pi in inputs) if (pi != null) pi.ActivateInput();
        }
#else
        yield return new WaitForSeconds(seconds);
#endif
    }
}

// ================================
// ITEM: METAL DETECTOR
// ================================

/// <summary>
/// Detects nearby treasures and provides haptic feedback based on proximity.
/// </summary>
public class MetalDetector : EquipmentItem
{
    [Header("Detection Settings")]
    [SerializeField] private float detectionRadius = 10f;

    [Header("Vibration Feedback Settings")]
    [SerializeField] private float maxVibrationIntensity = 1f;
    [SerializeField] private float minVibrationIntensity = 0.1f;
    [SerializeField] private float vibrationDuration = 0.1f;
    [SerializeField] private float minVibrationInterval = 0.1f;
    [SerializeField] private float maxVibrationInterval = 2.0f;
    [SerializeField] private float continuousVibrationDistance = 1.0f;

    [Header("Battery System")]
    [SerializeField] private float maxBattery = 10f;
    [SerializeField] private float batteryDrainPerSecond = 1f;
    [SerializeField] private float batteryRechargePerSecond = 0.5f;

    private bool isDetecting = false;
    private float currentBattery;
    private float lastVibrationTime;
    private float vibrationInterval = 0.5f;
    private Coroutine detectionCoroutine;
    private readonly List<TileTreasureComponent> detectedTreasures = new List<TileTreasureComponent>();

    /// <summary>Detection radius.</summary>
    public float DetectionRadius => detectionRadius;

    /// <summary>Current battery.</summary>
    public float CurrentBattery => currentBattery;

    /// <summary>Battery [0..1].</summary>
    public float BatteryPercentage => currentBattery / maxBattery;

    /// <summary>True if currently detecting.</summary>
    public bool IsDetecting => isDetecting;

    /// <summary>Detected treasures count.</summary>
    public int DetectedTreasureCount => detectedTreasures.Count;

    protected override void Awake()
    {
        itemName = "Metal Detector";
        itemDescription = "Detects nearby treasures with distance-based vibration.";
        cooldownTime = 0.5f;
        isConsumable = false;
        currentBattery = maxBattery;
        base.Awake();
    }

    protected override bool OnUse(Player player)
    {
        if (currentBattery <= 0f) return false;

        if (isDetecting) StopDetection();
        else StartDetection(player);

        return true;
    }

    /// <summary>Begin sensing treasures and haptics.</summary>
    private void StartDetection(Player player)
    {
        if (detectionCoroutine != null) StopCoroutine(detectionCoroutine);
        isDetecting = true;

        if (!gameObject.activeInHierarchy)
        {
            gameObject.SetActive(true);
        }

        detectionCoroutine = StartCoroutine(DetectionLoop(player));
    }

    /// <summary>Stop sensing and clear haptics.</summary>
    private void StopDetection()
    {
        isDetecting = false;

        if (detectionCoroutine != null)
        {
            StopCoroutine(detectionCoroutine);
            detectionCoroutine = null;
        }

        if (ControllerVibrationManager.Instance != null)
        {
            ControllerVibrationManager.Instance.StopAllVibrations();
        }

        detectedTreasures.Clear();
    }

    /// <summary>Main loop: drain battery, find treasures, update haptics.</summary>
    private IEnumerator DetectionLoop(Player player)
    {
        while (isDetecting && currentBattery > 0f)
        {
            currentBattery = Mathf.Max(0f, currentBattery - batteryDrainPerSecond * Time.deltaTime);
            FindNearbyTreasures(player);
            UpdateVibrationFeedback(player);
            yield return null;
        }

        if (currentBattery <= 0f) StopDetection();
    }

    /// <summary>Collect undiscovered treasures within radius.</summary>
    private void FindNearbyTreasures(Player player)
    {
        detectedTreasures.Clear();
        var allTreasures = FindObjectsOfType<TileTreasureComponent>();
        foreach (var t in allTreasures)
        {
            if (t == null || !t.hasTreasure || t.isDiscovered) continue;
            float d = Vector3.Distance(player.transform.position, t.transform.position);
            if (d <= detectionRadius) detectedTreasures.Add(t);
        }
    }

    /// <summary>Compute intensity/interval and send vibration events.</summary>
    private void UpdateVibrationFeedback(Player player)
    {
        if (detectedTreasures.Count == 0) return;

        float closest = float.MaxValue;
        foreach (var t in detectedTreasures)
        {
            float d = Vector3.Distance(player.transform.position, t.transform.position);
            if (d < closest) closest = d;
        }

        float normalized = Mathf.Clamp01(closest / detectionRadius);
        float intensity = Mathf.Lerp(maxVibrationIntensity, minVibrationIntensity, normalized);

        if (closest <= continuousVibrationDistance)
        {
            vibrationInterval = minVibrationInterval;
        }
        else
        {
            float k = Mathf.InverseLerp(continuousVibrationDistance, detectionRadius, closest);
            vibrationInterval = Mathf.Lerp(minVibrationInterval, maxVibrationInterval, k);
        }

        if (ControllerVibrationManager.Instance != null && Time.time - lastVibrationTime >= vibrationInterval)
        {
            ControllerVibrationManager.Instance.VibratePlayer(player, intensity, intensity, vibrationDuration);
            lastVibrationTime = Time.time;
        }
    }

    /// <summary>Readable detection summary (no side-effects).</summary>
    public override string GetItemInfo()
    {
        string baseInfo = base.GetItemInfo();
        if (!isDetecting)
        {
            return $"{baseInfo}\n\nMetal Detector: Inactive\nBattery: {BatteryPercentage:P0}";
        }

        if (detectedTreasures.Count == 0)
        {
            return $"{baseInfo}\n\nMetal Detector: Active - No treasures detected\nBattery: {BatteryPercentage:P0}";
        }

        float closest = float.MaxValue;
        foreach (var t in detectedTreasures)
        {
            float d = Vector3.Distance(transform.position, t.transform.position);
            if (d < closest) closest = d;
        }

        return $"{baseInfo}\n\nMetal Detector: Active - {detectedTreasures.Count} treasure(s)\nClosest: {closest:F1}m\nBattery: {BatteryPercentage:P0}";
    }

    /// <summary>Stop detection when unequipped.</summary>
    public override void OnUnequipped()
    {
        base.OnUnequipped();
        StopDetection();
    }

    /// <summary>Reset state and refill battery.</summary>
    public override void ResetItem()
    {
        base.ResetItem();
        StopDetection();
        currentBattery = maxBattery;
    }
}

// ================================
// ITEM: SWIM RING
// ================================

/// <summary>
/// Toggle a floating ring visual and allow water traversal.
/// </summary>
public class SwimRing : EquipmentItem
{
    [SerializeField] private GameObject swimRingVisual;
    [SerializeField] private float y_offset = 0.5f;
    private GameObject instantiatedVisual;

    protected override void Awake()
    {
        itemName = "SwimRing";
        itemDescription = "A swim ring that allows swimming in water.";
        isConsumable = false;
        cooldownTime = 1f;
        base.Awake();
    }

    protected override bool OnUse(Player player)
    {
        if (player == null) return false;

        if (player.EquippedSwimRing)
        {
            player.EquippedSwimRing = false;
            if (instantiatedVisual != null) instantiatedVisual.SetActive(false);
        }
        else
        {
            player.EquippedSwimRing = true;
            if (instantiatedVisual != null)
            {
                instantiatedVisual.SetActive(true);
            }
            else if (swimRingVisual != null)
            {
                instantiatedVisual = Instantiate(swimRingVisual, player.transform);
                instantiatedVisual.transform.SetParent(player.transform);
                instantiatedVisual.transform.localPosition = new Vector3(0f, y_offset, 0f);
            }
        }
        return true;
    }
}

/// <summary>
/// Water volume helper that enables collision when player has SwimRing equipped.
/// </summary>
public class SwimRingHelper : MonoBehaviour
{
    private BoxCollider solidCollider;
    private BoxCollider triggerCollider;

    private void Awake()
    {
        solidCollider = gameObject.GetComponent<BoxCollider>();
        if (solidCollider == null) solidCollider = gameObject.AddComponent<BoxCollider>();
        solidCollider.enabled = false;
        solidCollider.isTrigger = false;

        triggerCollider = gameObject.AddComponent<BoxCollider>();
        CopyColliderSettings(solidCollider, triggerCollider);
        triggerCollider.enabled = true;
        triggerCollider.isTrigger = true;
    }

    /// <summary>Copy key collider settings and expand vertical size for trigger.</summary>
    private void CopyColliderSettings(BoxCollider source, BoxCollider target)
    {
        target.center = source.center;
        target.material = source.material;
        Vector3 newSize = source.size;
        newSize.y += 5f;
        target.size = newSize;
    }

    private void OnTriggerEnter(Collider other)
    {
        Player player = other.GetComponent<Player>();
        if (player != null && player.EquippedSwimRing)
        {
            solidCollider.enabled = true;
        }
    }

    private void OnTriggerExit(Collider other)
    {
        Player player = other.GetComponent<Player>();
        if (player != null)
        {
            solidCollider.enabled = false;
        }
    }
}

// ================================
// Unrelated Code has been Removed
// ================================
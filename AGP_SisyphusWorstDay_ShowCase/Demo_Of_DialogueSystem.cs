using System.Collections;
using System.Collections.Generic;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;
#if ENABLE_INPUT_SYSTEM
using UnityEngine.InputSystem;
#endif
using TMPro;

// ================================
// DIALOGUE SEQUENCE (SO)
// ================================

[CreateAssetMenu(menuName = "Dialogue/Sequence")]
public class DialogueSequence : ScriptableObject
{
    [TextArea(1, 4)]
    public List<string> lines = new();
}

// ================================
// DIALOGUE UI
// ================================

public class DialogueUI : MonoBehaviour
{
    [Header("Wiring")]

    // Bottom panel (enable/disable to show/hide)
    [SerializeField] private GameObject rootPanel;

    // TextMeshPro text under the panel
    [SerializeField] private TMP_Text bodyText;

    [Header("Display")]

    // Fixed number of lines shown per page
    [SerializeField, Min(1)] private int maxLines = 3;

    private List<string> _lines;
    private int _pageIndex;

    // True if the panel GameObject exists and is active
    public bool IsVisible => rootPanel && rootPanel.activeSelf;

    // True if current page is the last page
    public bool IsLastPage
    {
        get
        {
            if (_lines == null) return true;
            int nextStart = (_pageIndex + 1) * maxLines;
            return nextStart >= _lines.Count;
        }
    }

    /// <summary>Open the panel and display the first page.</summary>
    public void Open(List<string> lines)
    {
        _lines = lines ?? new List<string>();
        _pageIndex = 0;
        if (rootPanel) rootPanel.SetActive(true);
        ShowCurrentPage();
    }

    /// <summary>Close the panel and clear text.</summary>
    public void Close()
    {
        if (rootPanel) rootPanel.SetActive(false);
        if (bodyText) bodyText.text = string.Empty;
        _lines = null;
        _pageIndex = 0;
    }

    /// <summary>Advance to the next page (if any).</summary>
    public void NextPage()
    {
        if (_lines == null) return;
        _pageIndex++;
        ShowCurrentPage();
    }

    private void ShowCurrentPage()
    {
        if (_lines == null || _lines.Count == 0)
        {
            if (bodyText) bodyText.text = string.Empty;
            return;
        }

        int start = _pageIndex * maxLines;
        int end = Mathf.Min(start + maxLines, _lines.Count);

        var sb = new StringBuilder(128);
        for (int i = start; i < end; i++)
            sb.AppendLine(_lines[i]);

        // Trim trailing newline so the last line sits snugly
        if (bodyText) bodyText.text = sb.ToString().TrimEnd();
    }
}

// ================================
// DIALOGUE SYSTEM
// ================================

public class DialogueSystem : MonoBehaviour
{
    [Header("Wiring")]

    // Main camera
    [SerializeField] private Camera worldCamera;

    // Fixed position/rotation for dialogue focus
    [SerializeField] private Transform dialoguePose;

    // Runtime UI controller
    [SerializeField] private DialogueUI dialogueUI;

    [Header("Camera Behavior")]

    // When true, camera moves to dialoguePose and optionally forces perspective/FOV
    [Tooltip("When true, camera moves to dialoguePose and optionally forces perspective/FOV.")]
    [SerializeField] private bool enableCameraMotion = true;

    [Header("Camera Motion Settings")]

    // Seconds to travel between camera positions
    [SerializeField, Range(0.05f, 2f)] private float moveDuration = 0.6f;

    // Easing curve for camera motion / FOV blends
    [SerializeField] private AnimationCurve ease = AnimationCurve.EaseInOut(0, 0, 1, 1);

    [Header("Dialogue Projection")]

    // If enabled, force perspective projection while chat box is visible
    [Tooltip("If enabled, force perspective projection while chat box is visible.")]
    [SerializeField] private bool forcePerspectiveDuringDialogue = true;

    // Perspective FOV used during dialogue (when forcing perspective)
    [SerializeField, Range(20f, 90f)] private float dialogueFOV = 40f;

    [Header("Auto Start (Optional)")]

    // Auto start sequence after scene load (for demo/showcase)
    [SerializeField] private bool autoStartOnSceneLoad = false;

    // Sequence to auto start (if enabled)
    [SerializeField] private DialogueSequence autoStartSequence;

    // Cached camera position for restoration
    private Vector3 _origCamPos;

    // Cached camera rotation for restoration
    private Quaternion _origCamRot;

    // Cached camera FOV for restoration
    private float _origFov;

    // True if original camera was orthographic
    private bool _origOrthographic;

    // Cached orthographic size for restoration
    private float _origOrthoSize;

    // True while a conversation is active
    private bool _isActive;

    // Active sequence lines for current conversation
    private List<string> _activeLines;

    // External read: is a conversation running
    public bool IsActive => _isActive;

    private void Awake()
    {
        if (!worldCamera) worldCamera = Camera.main;
        EnsureEventSystem();
    }

    private void Start()
    {
        if (autoStartOnSceneLoad && autoStartSequence != null)
            StartConversation(autoStartSequence);
    }

    private void Update()
    {
        if (!_isActive || !dialogueUI || !dialogueUI.IsVisible) return;

        bool clicked = false;
#if ENABLE_INPUT_SYSTEM
        clicked = Mouse.current != null && Mouse.current.leftButton.wasPressedThisFrame;
#else
        clicked = Input.GetMouseButtonDown(0);
#endif
        if (!clicked) return;

        if (dialogueUI.IsLastPage) EndConversation();
        else dialogueUI.NextPage();
    }

    /// <summary>Begin a conversation with the given sequence (no-op if already active or empty).</summary>
    public void StartConversation(DialogueSequence sequence)
    {
        if (_isActive || sequence == null || sequence.lines.Count == 0) return;

        _activeLines = sequence.lines;

        if (worldCamera)
        {
            _origCamPos = worldCamera.transform.position;
            _origCamRot = worldCamera.transform.rotation;
            _origFov = worldCamera.fieldOfView;
            _origOrthographic = worldCamera.orthographic;
            _origOrthoSize = worldCamera.orthographicSize;
        }

        StartCoroutine(Co_RunConversation());
    }

    private IEnumerator Co_RunConversation()
    {
        _isActive = true;

        if (enableCameraMotion && worldCamera)
        {
            // Switch to perspective if requested (common for isometric → dialogue)
            if (forcePerspectiveDuringDialogue && worldCamera.orthographic)
            {
                worldCamera.orthographic = false;
                worldCamera.fieldOfView = dialogueFOV;
            }

            if (dialoguePose != null)
                yield return Co_MoveCamera(worldCamera.transform, dialoguePose.position, dialoguePose.rotation, moveDuration);

            if (forcePerspectiveDuringDialogue && !_origOrthographic)
                yield return Co_LerpFOV(worldCamera, worldCamera.fieldOfView, dialogueFOV, 0.25f);
        }

        dialogueUI.Open(_activeLines);
    }

    private void EndConversation()
    {
        dialogueUI.Close();
        StartCoroutine(Co_ReturnCameraThenFinish());
    }

    private IEnumerator Co_ReturnCameraThenFinish()
    {
        if (enableCameraMotion && worldCamera)
        {
            yield return Co_MoveCamera(worldCamera.transform, _origCamPos, _origCamRot, moveDuration);

            if (_origOrthographic)
            {
                worldCamera.orthographic = true;
                worldCamera.orthographicSize = _origOrthoSize;
            }
            else
            {
                if (Mathf.Abs(worldCamera.fieldOfView - _origFov) > 0.01f)
                    yield return Co_LerpFOV(worldCamera, worldCamera.fieldOfView, _origFov, 0.2f);
                worldCamera.orthographic = false;
            }
        }

        _activeLines = null;
        _isActive = false;
    }

    private IEnumerator Co_MoveCamera(Transform cam, Vector3 targetPos, Quaternion targetRot, float duration)
    {
        Vector3 startPos = cam.position;
        Quaternion startRot = cam.rotation;
        float t = 0f;

        while (t < 1f)
        {
            t += Time.deltaTime / Mathf.Max(0.0001f, duration);
            float k = ease.Evaluate(Mathf.Clamp01(t));
            cam.SetPositionAndRotation(
                Vector3.LerpUnclamped(startPos, targetPos, k),
                Quaternion.SlerpUnclamped(startRot, targetRot, k)
            );
            yield return null;
        }
    }

    private IEnumerator Co_LerpFOV(Camera cam, float from, float to, float duration)
    {
        float t = 0f;
        while (t < 1f)
        {
            t += Time.deltaTime / Mathf.Max(0.0001f, duration);
            cam.fieldOfView = Mathf.LerpUnclamped(from, to, ease.Evaluate(Mathf.Clamp01(t)));
            yield return null;
        }
    }

    private void EnsureEventSystem()
    {
        var existing = FindObjectOfType<EventSystem>();
        if (existing != null) return;

        var go = new GameObject("EventSystem", typeof(EventSystem));
#if ENABLE_INPUT_SYSTEM
        go.AddComponent<UnityEngine.InputSystem.UI.InputSystemUIInputModule>();
#else
        go.AddComponent<StandaloneInputModule>();
#endif
    }
}

// ================================
// DIALOGUE STARTER
// ================================

[RequireComponent(typeof(Collider))]
public class DialogueStarter : MonoBehaviour
{
    [Header("Assign in Inspector")]

    // ScriptableObject with lines to display
    [SerializeField] private DialogueSequence sequence;

    // DialogueSystem that will play the sequence
    [SerializeField] private DialogueSystem dialogueSystem;

    [Header("Detection")]

    // Player tag to detect for interaction
    [SerializeField] private string playerTag = "Player";

    private bool _playerInRange;

    private void Reset()
    {
        // Ensure collider exists & is a trigger
        var col = GetComponent<Collider>();
        col.isTrigger = true;
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag(playerTag)) _playerInRange = true;
    }

    private void OnTriggerExit(Collider other)
    {
        if (other.CompareTag(playerTag)) _playerInRange = false;
    }

    private void Update()
    {
        if (!_playerInRange || dialogueSystem == null || dialogueSystem.IsActive) return;

        bool pressed = false;
#if ENABLE_INPUT_SYSTEM
        if (Keyboard.current != null) pressed = Keyboard.current.eKey.wasPressedThisFrame;
#endif
        if (!pressed) pressed = Input.GetKeyDown(KeyCode.E);

        if (pressed && sequence != null)
            dialogueSystem.StartConversation(sequence);
    }
}

// ================================
// Unrelated Code has been Removed
// ================================
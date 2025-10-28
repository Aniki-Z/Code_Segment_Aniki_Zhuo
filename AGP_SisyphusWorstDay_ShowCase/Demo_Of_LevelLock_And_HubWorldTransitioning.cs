using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class CubeActivator : MonoBehaviour
{
    // ================================
    // SCENE & UI
    // ================================

    [Header("Scene Settings")]
    [Tooltip("Target gameplay scene to load after a level is chosen.")]
    public string sceneToLoad;

    [Header("Auto Load on Arrival (next scene)")]
    [Tooltip("If true, immediately apply the chosen palette/level when the next scene finishes loading.")]
    public bool autoLoadOnArrival = true;

    [Serializable]
    public struct LevelEntry
    {
        [Tooltip("Display name shown in the selection UI.")]
        public string displayName;
        [Tooltip("Palette JSON filename")]
        public string paletteFileName;
        [Tooltip("Level JSON filename")]
        public string levelFileName;

        [Tooltip("Dialogue to play at level start.")]
        public DialogueSequence dialogue;
        [Tooltip("Dialogue to play after the level ends (before returning to menu).")]
        public DialogueSequence endDialogue;
    }

    [Header("Level Choices (paired palette + level + dialogues)")]
    public List<LevelEntry> levels = new List<LevelEntry>();

    // The watcher to re-open the correct selector after a round
    [Header("Mark this instance as the gameplay-side activator")]
    public bool isGameplayInstance = false;

    // ================================
    // PROGRESSION & LOCKS
    // ================================

    private const string PROGRESS_KEY = "CubeActivator_UnlockedMax";

    private static int GetUnlockedMax() => Mathf.Max(0, PlayerPrefs.GetInt(PROGRESS_KEY, 0));
    private static bool IsLevelUnlocked(int index) => index <= GetUnlockedMax();
    private static void UnlockUpTo(int indexInclusive)
    {
        int cur = GetUnlockedMax();
        if (indexInclusive > cur)
        {
            PlayerPrefs.SetInt(PROGRESS_KEY, indexInclusive);
            PlayerPrefs.Save();
        }
    }

    // ================================
    // STATE PASSED ACROSS SCENES
    // ================================

    private static int _lastLevelIndex = -1;

    // Pending selection (set when user clicks a button)
    private static string _pendingPaletteFile;
    private static string _pendingLevelFile;
    private static DialogueSequence _pendingDialogue;
    private static DialogueSequence _pendingEndDialogue;
    private static bool _hasPendingSelection;

    // Last applied selection (used by gameplay scene + restart)
    private static string _lastPaletteFile;
    private static string _lastLevelFile;
    private static DialogueSequence _lastDialogue;
    private static DialogueSequence _lastEndDialogue;

    // Runtime UI root + handler registration guard
    private GameObject _menuRoot;
    private bool _handlerRegistered = false;

    // ================================
    // UI
    // ================================

    /// <summary>Show or hide the select menu. (Time.timeScale is paused while open.)</summary>
    private void ToggleMenu()
    {
        if (_menuRoot == null)
        {
            BuildMenuUI();
            _menuRoot.SetActive(true);
            Time.timeScale = 0f;
        }
        else
        {
            bool next = !_menuRoot.activeSelf;
            _menuRoot.SetActive(next);
            Time.timeScale = next ? 0f : 1f;
        }
    }

    /// <summary>Force-show the level menu (used after finishing a level).</summary>
    public void ShowMenuNow()
    {
        if (_menuRoot == null) BuildMenuUI();
        _menuRoot.SetActive(true);
        Time.timeScale = 0f;
    }

    /// <summary>Builds a minimal overlay canvas with title + one button per level entry.</summary>
    private void BuildMenuUI()
    {
        if (_menuRoot != null) Destroy(_menuRoot);

        // Canvas
        _menuRoot = new GameObject("LevelSelectCanvas", typeof(Canvas), typeof(CanvasScaler), typeof(GraphicRaycaster));
        var canvas = _menuRoot.GetComponent<Canvas>();
        canvas.renderMode = RenderMode.ScreenSpaceOverlay;
        canvas.sortingOrder = 5000;

        var scaler = _menuRoot.GetComponent<CanvasScaler>();
        scaler.uiScaleMode = CanvasScaler.ScaleMode.ScaleWithScreenSize;
        scaler.referenceResolution = new Vector2(1920, 1080);

        // make sure UI can receive clicks
        EnsureEventSystem();

        // Panel
        float W = Screen.width, H = Screen.height;
        float w = Mathf.Min(Mathf.Sqrt((W * H) / 6f), W * 0.6f);
        float h = Mathf.Min(1.5f * w, H * 0.8f);

        var panel = new GameObject("Panel", typeof(Image));
        panel.transform.SetParent(_menuRoot.transform, false);
        var panelRT = panel.GetComponent<RectTransform>();
        panelRT.anchorMin = panelRT.anchorMax = new Vector2(0.5f, 0.5f);
        panelRT.pivot = new Vector2(0.5f, 0.5f);
        panelRT.sizeDelta = new Vector2(w, h);
        panelRT.anchoredPosition = Vector2.zero;
        panel.GetComponent<Image>().color = new Color(0f, 0f, 0f, 0.78f);

        // Container
        var container = new GameObject("Container", typeof(RectTransform), typeof(VerticalLayoutGroup), typeof(ContentSizeFitter));
        container.transform.SetParent(panel.transform, false);
        var contRT = container.GetComponent<RectTransform>();
        contRT.anchorMin = new Vector2(0f, 0f);
        contRT.anchorMax = new Vector2(1f, 1f);
        contRT.offsetMin = new Vector2(20f, 20f);
        contRT.offsetMax = new Vector2(-20f, -20f);

        var v = container.GetComponent<VerticalLayoutGroup>();
        v.childControlHeight = true;
        v.childControlWidth = true;
        v.childForceExpandHeight = false;
        v.childForceExpandWidth = true;
        v.spacing = Mathf.Clamp(h * 0.02f, 8f, 18f);

        var fitter = container.GetComponent<ContentSizeFitter>();
        fitter.verticalFit = ContentSizeFitter.FitMode.PreferredSize;

        // Title
        CreateUILabel(container.transform, "Title", "Choose Level",
                      Mathf.RoundToInt(Mathf.Clamp(h * 0.06f, 22f, 36f)),
                      TextAnchor.MiddleCenter, Color.white,
                      Mathf.Clamp(h * 0.09f, 40f, 70f));

        // Buttons (show lock state / disable interaction)
        float btnHeight = Mathf.Clamp(h * 0.12f, 48f, 84f);
        for (int i = 0; i < levels.Count; i++)
        {
            int captured = i;
            string label = string.IsNullOrEmpty(levels[i].displayName) ? $"Level {i + 1}" : levels[i].displayName;
            bool unlocked = IsLevelUnlocked(i);
            if (!unlocked) label += "  (Locked)";

            var btn = BuildButton(container.transform, label, btnHeight, () => OnChooseLevel(captured));
            btn.interactable = unlocked;

            if (!unlocked)
            {
                var img = btn.GetComponent<Image>();
                if (img != null) img.color = new Color(0.09f, 0.09f, 0.09f, 0.85f);
            }
        }

        // Hint
        CreateUILabel(container.transform, "Hint", "Press E again to close",
                      Mathf.RoundToInt(Mathf.Clamp(h * 0.035f, 14f, 22f)),
                      TextAnchor.MiddleCenter, new Color(1f, 1f, 1f, 0.9f),
                      Mathf.Clamp(h * 0.08f, 30f, 50f));
    }

    private Button BuildButton(Transform parent, string label, float height, Action onClick)
    {
        var go = new GameObject($"Button_{label}", typeof(Image), typeof(Button), typeof(LayoutElement));
        go.transform.SetParent(parent, false);

        var le = go.GetComponent<LayoutElement>();
        le.preferredHeight = height;
        le.minHeight = height;
        le.flexibleWidth = 1f;

        var img = go.GetComponent<Image>();
        img.color = new Color(0.18f, 0.18f, 0.18f, 0.95f);
        img.raycastTarget = true;

        var rt = go.GetComponent<RectTransform>();
        rt.anchorMin = new Vector2(0f, 1f);
        rt.anchorMax = new Vector2(1f, 1f);
        rt.pivot = new Vector2(0.5f, 1f);
        rt.sizeDelta = new Vector2(0f, height);

        CreateUILabel(go.transform, "Text", label, 22, TextAnchor.MiddleCenter, Color.white, stretch: true);

        var btn = go.GetComponent<Button>();
        btn.onClick.AddListener(() => onClick?.Invoke());
        btn.interactable = true;
        btn.navigation = Navigation.defaultNavigation;
        return btn;
    }

    private void CreateUILabel(Transform parent, string name, string text, int fontSize, TextAnchor align, Color color, float height = -1f, bool stretch = false)
    {
        // Prefer TextMeshPro if available
        var tmpType = Type.GetType("TMPro.TextMeshProUGUI, Unity.TextMeshPro");
        if (tmpType != null)
        {
            var go = new GameObject(name, tmpType);
            go.transform.SetParent(parent, false);

            var rt = go.GetComponent<RectTransform>();
            if (stretch) { rt.anchorMin = Vector2.zero; rt.anchorMax = Vector2.one; rt.sizeDelta = Vector2.zero; }
            else
            {
                rt.anchorMin = new Vector2(0f, 1f);
                rt.anchorMax = new Vector2(1f, 1f);
                rt.pivot = new Vector2(0.5f, 1f);
                if (height > 0f) rt.sizeDelta = new Vector2(0f, height);
            }

            var comp = go.GetComponent(tmpType);
            tmpType.GetProperty("text")?.SetValue(comp, text);
            tmpType.GetProperty("fontSize")?.SetValue(comp, Convert.ToSingle(fontSize));
            tmpType.GetProperty("color")?.SetValue(comp, color);

            var alignProp = tmpType.GetProperty("alignment");
            if (alignProp != null)
            {
                var enumType = alignProp.PropertyType;
                object enumValue = Enum.Parse(enumType, TMPAlignmentString(align));
                alignProp.SetValue(comp, enumValue);
            }
            return;
        }

        // Fallback to legacy UI.Text
        var tgo = new GameObject(name, typeof(Text));
        tgo.transform.SetParent(parent, false);

        var trt = tgo.GetComponent<RectTransform>();
        if (stretch) { trt.anchorMin = Vector2.zero; trt.anchorMax = Vector2.one; trt.sizeDelta = Vector2.zero; }
        else
        {
            trt.anchorMin = new Vector2(0f, 1f);
            trt.anchorMax = new Vector2(1f, 1f);
            trt.pivot = new Vector2(0.5f, 1f);
            if (height > 0f) trt.sizeDelta = new Vector2(0f, height);
        }

        var t = tgo.GetComponent<Text>();
        t.alignment = align;
        t.text = text;
        t.fontSize = fontSize;
        t.color = color;
        t.raycastTarget = false;
        t.font = Resources.GetBuiltinResource<Font>("Arial.ttf");
    }

    private string TMPAlignmentString(TextAnchor a)
    {
        switch (a)
        {
            case TextAnchor.UpperLeft: return "TopLeft";
            case TextAnchor.UpperCenter: return "Top";
            case TextAnchor.UpperRight: return "TopRight";
            case TextAnchor.MiddleLeft: return "Left";
            case TextAnchor.MiddleCenter: return "Center";
            case TextAnchor.MiddleRight: return "Right";
            case TextAnchor.LowerLeft: return "BottomLeft";
            case TextAnchor.LowerCenter: return "Bottom";
            case TextAnchor.LowerRight: return "BottomRight";
            default: return "Center";
        }
    }

    private void EnsureEventSystem()
    {
        var existing = FindObjectOfType<EventSystem>();
        if (existing != null) return;

        var go = new GameObject("EventSystem", typeof(EventSystem));
        var inputSystemUIModuleType = Type.GetType("UnityEngine.InputSystem.UI.InputSystemUIInputModule, Unity.InputSystem");
        if (inputSystemUIModuleType != null) go.AddComponent(inputSystemUIModuleType);
        else go.AddComponent<StandaloneInputModule>();
    }

    // ================================
    // USER CHOICE to LOAD SCENE
    // ================================

    /// <summary>
    /// Called by UI button: capture selection, remember dialogues, then load the next scene.
    /// </summary>
    private void OnChooseLevel(int index)
    {
        if (index < 0 || index >= levels.Count) return;

        var choice = levels[index];

        _lastLevelIndex      = index;
        _pendingPaletteFile  = choice.paletteFileName;
        _pendingLevelFile    = choice.levelFileName;
        _pendingDialogue     = choice.dialogue;
        _pendingEndDialogue  = choice.endDialogue;
        _hasPendingSelection = true;

        if (_menuRoot != null) Destroy(_menuRoot);
        Time.timeScale = 1f;

        if (string.IsNullOrEmpty(sceneToLoad)) return;

        if (!_handlerRegistered)
        {
            SceneManager.sceneLoaded += OnSceneLoaded;
            _handlerRegistered = true;
        }
        SceneManager.LoadScene(sceneToLoad);
    }

    // Runs coroutines even after scene swap
    private sealed class DeferredRunner : MonoBehaviour
    {
        private static DeferredRunner _inst;
        public static void Run(IEnumerator routine)
        {
            if (_inst == null)
            {
                var go = new GameObject("CubeActivator_DeferredRunner");
                DontDestroyOnLoad(go);
                _inst = go.AddComponent<DeferredRunner>();
            }
            _inst.StartCoroutine(routine);
        }
    }

    /// <summary>
    /// After the next scene loads: Auto-apply palette+level, start begin dialogue, then start a win/fail watcher to unlock next level.
    /// </summary>
    private void OnSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        Time.timeScale = 1f;
        SceneManager.sceneLoaded -= OnSceneLoaded;
        _handlerRegistered = false;

        string paletteToUse = null;
        string levelToUse   = null;
        DialogueSequence beginDialogue = null;
        DialogueSequence endDialogue   = null;

        if (!autoLoadOnArrival)
        {
            if (_hasPendingSelection)
            {
                _lastLevelIndex = Mathf.Clamp(_lastLevelIndex, 0, Mathf.Max(0, levels.Count - 1));
                _lastPaletteFile = _pendingPaletteFile;
                _lastLevelFile   = _pendingLevelFile;
                _lastDialogue    = _pendingDialogue;
                _lastEndDialogue = _pendingEndDialogue;

                _hasPendingSelection = false;
                _pendingPaletteFile = _pendingLevelFile = null;
                _pendingDialogue = _pendingEndDialogue = null;
            }
            return;
        }

        if (_hasPendingSelection)
        {
            paletteToUse  = _pendingPaletteFile;
            levelToUse    = _pendingLevelFile;
            beginDialogue = _pendingDialogue;
            endDialogue   = _pendingEndDialogue;
        }
        else if (levels != null && levels.Count > 0)
        {
            paletteToUse  = levels[0].paletteFileName;
            levelToUse    = levels[0].levelFileName;
            beginDialogue = levels[0].dialogue;
            endDialogue   = levels[0].endDialogue;
            _lastLevelIndex = 0;
        }

        _lastPaletteFile = paletteToUse;
        _lastLevelFile   = levelToUse;
        _lastDialogue    = beginDialogue;
        _lastEndDialogue = endDialogue;

        _hasPendingSelection = false;
        _pendingPaletteFile = _pendingLevelFile = null;
        _pendingDialogue = _pendingEndDialogue = null;

        DeferredRunner.Run(Co_ApplySelectionAfterLoad(paletteToUse, levelToUse));
    }

    // Helper for older Unity versions
    private new static T FindAnyObjectByType<T>() where T : UnityEngine.Object
    {
    #if UNITY_2023_1_OR_NEWER
        return UnityEngine.Object.FindAnyObjectByType<T>();
    #else
        return UnityEngine.Object.FindObjectOfType<T>();
    #endif
    }

    /// <summary>
    /// One-frame-late hand-off: set palette & level on scene systems, start begin dialogue, arm the unlock watcher.
    /// </summary>
    private static IEnumerator Co_ApplySelectionAfterLoad(string paletteToUse, string levelToUse)
    {
        yield return null;

        // Palette
        var palette = FindAnyObjectByType<Palette>();
        if (palette != null)
        {
            try
            {
                if (!string.IsNullOrEmpty(paletteToUse)) palette.SetFileName(paletteToUse);
                palette.LoadPalette();
            }
            catch { }
        }

        // Level data
        var dataManager = FindAnyObjectByType<DataManager>();
        if (dataManager != null)
        {
            try
            {
                if (!string.IsNullOrEmpty(levelToUse))
                {
                    var setName = typeof(DataManager).GetMethod("SetFileName", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
                    if (setName != null) setName.Invoke(dataManager, new object[] { levelToUse });
                }

                var reset = typeof(DataManager).GetMethod("ResetLevel", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
                if (reset != null) reset.Invoke(dataManager, null);
                else dataManager.gameObject.SendMessage("ResetLevel", SendMessageOptions.DontRequireReceiver);

                var loadLevel = typeof(DataManager).GetMethod("LoadLevelData", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
                if (loadLevel != null) loadLevel.Invoke(dataManager, null);
                else dataManager.gameObject.SendMessage("LoadLevelData", SendMessageOptions.DontRequireReceiver);
            }
            catch { }
        }

        // Optional UI refresh after load
        var hotbarUI = FindAnyObjectByType<HotbarUI>();
        if (hotbarUI != null) hotbarUI.UpdateCounts();

        // Begin dialogue for this level
        if (_lastDialogue != null)
        {
            var runtimeSeq = BuildRuntimeSequence(_lastDialogue, _lastPaletteFile, _lastLevelFile);
            StartDialogueInScene(runtimeSeq);
        }

        // Start watching for win/fail to handle unlocking and end-dialogue
        if (_lastLevelIndex >= 0)
        {
            DeferredRunner.Run(Co_WatchForWinAndUnlock(_lastLevelIndex));
        }
    }

    // ================================
    // UNLOCK WATCHER and DIALOG
    // ================================

    /// <summary>
    /// Watch gameplay state; when win occurs (and not fail), unlock next level, run end-dialogue, then re-open the menu.
    /// </summary>
    private static IEnumerator Co_WatchForWinAndUnlock(int playedIndex)
    {
        while (true)
        {
            var be      = FindAnyObjectByType<BlockEditor>();
            var tm      = FindAnyObjectByType<TickManager>();
            var failMgr = FindAnyObjectByType<FailStateManager>();

            bool isWin  = false;
            bool isFail = false;

            if (be != null) isWin |= be.win;
            if (tm != null) isWin |= tm.win;

            if (failMgr != null)
            {
                try { isFail = failMgr.CheckFailState(); }
                catch { isFail = false; }
            }

            if (isWin && !isFail) break;
            yield return null;
        }

        UnlockUpTo(playedIndex + 1);

        if (_lastEndDialogue != null)
        {
            var ds = FindAnyObjectByType<DialogueSystem>();
            if (ds != null)
            {
                var runtimeEnd = BuildRuntimeSequence(_lastEndDialogue, _lastPaletteFile, _lastLevelFile);
                ds.StartConversation(runtimeEnd);
                while (ds.IsActive) yield return null;
            }
        }

        var ca = FindGameplayCubeActivator();
        if (ca != null) ca.ShowMenuNow();
    }

    private static CubeActivator FindGameplayCubeActivator()
    {
    #if UNITY_2023_1_OR_NEWER
        var all = UnityEngine.Object.FindObjectsByType<CubeActivator>(FindObjectsInactive.Include, FindObjectsSortMode.None);
    #else
        var all = UnityEngine.Object.FindObjectsOfType<CubeActivator>();
    #endif
        CubeActivator fallback = null;
        foreach (var ca in all)
        {
            if (ca == null) continue;
            if (fallback == null) fallback = ca;
            if (ca.isGameplayInstance) return ca;
        }
        return fallback;
    }

    private static DialogueSequence BuildRuntimeSequence(DialogueSequence src, string paletteFile, string levelFile)
    {
        if (src == null) return null;
        var clone = ScriptableObject.CreateInstance<DialogueSequence>();
        foreach (var line in src.lines)
        {
            var s = line ?? "";
            s = s.Replace("{PALETTE}", string.IsNullOrEmpty(paletteFile) ? "default" : paletteFile);
            s = s.Replace("{LEVEL}",   string.IsNullOrEmpty(levelFile)   ? "unknown" : levelFile);
            clone.lines.Add(s);
        }
        return clone;
    }

    private static void StartDialogueInScene(DialogueSequence seq)
    {
        if (seq == null) return;
        var dialogueSystem = FindAnyObjectByType<DialogueSystem>();
        if (dialogueSystem == null) return;
        dialogueSystem.StartConversation(seq);
    }
}

// ================================
// Unrelate Code has been Removed
// ================================
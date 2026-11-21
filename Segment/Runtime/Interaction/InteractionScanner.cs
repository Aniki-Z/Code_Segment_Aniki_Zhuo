using UnityEngine;

namespace Aniki.GameplaySystems.Interaction
{
    [DisallowMultipleComponent]
    public sealed class InteractionScanner : MonoBehaviour
    {
        [SerializeField] private Transform origin;
        [SerializeField, Min(0.1f)] private float radius = 2.5f;
        [SerializeField, Range(1f, 360f)] private float viewAngle = 140f;
        [SerializeField, Min(0f)] private float angleWeight = 0.35f;
        [SerializeField] private LayerMask candidateLayers = ~0;
        [SerializeField] private QueryTriggerInteraction triggerInteraction = QueryTriggerInteraction.Collide;
        [SerializeField, Range(4, 128)] private int bufferSize = 24;

        private Collider[] overlapBuffer;

        private void Awake()
        {
            ResizeBuffer();
        }

        private void OnValidate()
        {
            radius = Mathf.Max(0.1f, radius);
            bufferSize = Mathf.Clamp(bufferSize, 4, 128);
            ResizeBuffer();
        }

        public bool TryGetBest(GameObject actor, out InteractableBehaviour best)
        {
            if (overlapBuffer == null || overlapBuffer.Length != bufferSize)
            {
                ResizeBuffer();
            }

            Transform scanOrigin = origin != null ? origin : transform;
            int hitCount = Physics.OverlapSphereNonAlloc(
                scanOrigin.position,
                radius,
                overlapBuffer,
                candidateLayers,
                triggerInteraction);

            float minimumDot = Mathf.Cos(viewAngle * 0.5f * Mathf.Deg2Rad);
            float bestScore = float.PositiveInfinity;
            best = null;

            for (int index = 0; index < hitCount; index++)
            {
                Collider hit = overlapBuffer[index];
                if (hit == null)
                {
                    continue;
                }

                InteractableBehaviour candidate = hit.GetComponentInParent<InteractableBehaviour>();
                if (candidate == null || !candidate.CanInteract(actor))
                {
                    continue;
                }

                Vector3 offset = candidate.InteractionTransform.position - scanOrigin.position;
                float distance = offset.magnitude;
                Vector3 direction = distance > 0.001f ? offset / distance : scanOrigin.forward;
                float dot = Vector3.Dot(scanOrigin.forward.normalized, direction);

                if (dot < minimumDot)
                {
                    continue;
                }

                float normalizedDistance = distance / radius;
                float normalizedAngle = 1f - Mathf.InverseLerp(minimumDot, 1f, dot);
                float score = normalizedDistance + normalizedAngle * angleWeight;

                if (score < bestScore)
                {
                    bestScore = score;
                    best = candidate;
                }
            }

            return best != null;
        }

        private void ResizeBuffer()
        {
            if (bufferSize < 4)
            {
                bufferSize = 4;
            }

            if (overlapBuffer == null || overlapBuffer.Length != bufferSize)
            {
                overlapBuffer = new Collider[bufferSize];
            }
        }

        private void OnDrawGizmosSelected()
        {
            Transform scanOrigin = origin != null ? origin : transform;
            Gizmos.DrawWireSphere(scanOrigin.position, radius);
        }
    }
}

using UnityEngine;

public class CameraController : MonoBehaviour
{
    private Vector3 initialMousePos;
    private Vector3 initialCameraPos;
    private bool isDragging = false;

    public float cameraSpeed = 5f;
    public float arrowKeySpeed = 3f;

    public float minX = -10f; // Adjust these values as needed
    public float maxX = 10f;
    public float minY = -10f;
    public float maxY = 10f;

    void Update()
    {
        if (Input.GetMouseButtonDown(0))
        {
            StartDrag();
        }
        else if (Input.GetMouseButtonUp(0))
        {
            StopDrag();
        }

        if (isDragging)
        {
            DragCamera();
        }
        else
        {
            MoveCameraWithArrowKeys();
        }
    }

    void StartDrag()
    {
        initialMousePos = Input.mousePosition;
        initialCameraPos = transform.position;
        isDragging = true;
    }

    void StopDrag()
    {
        isDragging = false;
    }

    void DragCamera()
    {
        Vector3 offset = (Input.mousePosition - initialMousePos) / Screen.height;
        Vector3 newPosition = initialCameraPos - new Vector3(offset.x, offset.y, 0f) * cameraSpeed;

        // Ensure the camera stays within bounds
        newPosition.x = Mathf.Clamp(newPosition.x, minX, maxX);
        newPosition.y = Mathf.Clamp(newPosition.y, minY, maxY);

        transform.position = newPosition;
    }

    void MoveCameraWithArrowKeys()
    {
        float horizontalInput = -Input.GetAxis("Horizontal");
        float verticalInput = -Input.GetAxis("Vertical");

        Vector3 arrowKeyMovement = new Vector3(horizontalInput, verticalInput, 0f) * arrowKeySpeed;

        // Ensure the camera stays within bounds
        Vector3 newPosition = transform.position + arrowKeyMovement * Time.deltaTime;
        newPosition.x = Mathf.Clamp(newPosition.x, minX, maxX);
        newPosition.y = Mathf.Clamp(newPosition.y, minY, maxY);

        transform.position = newPosition;
    }
}

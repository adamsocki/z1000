
enum CameraType
{
    CameraType_Orthographic,
    CameraType_Perspective
};

enum CursorMode {
    CURSOR_MODE_GAME,    // Cursor is hidden, captured for camera control
    CURSOR_MODE_UI,      // Cursor is visible, free to move
    CURSOR_MODE_COUNT
};


struct Camera
{
    CameraType type;

    mat4 projection = Identity4();
    mat4 view = Identity4();

    mat4 viewProjection;

    real32 rotation;
    vec3 forwardDir;
    vec3 position;
    vec3 upDirection = V3(0.0f, 1.0f, 0.0f);

    vec3 pos;
    vec3 front;
    vec3 up;
    vec3 right;
    
        real32 left;
        real32 right_;
        real32 top;
        real32 bottom;
        real32 nearDist;
        real32 farDist;

        real32 pitch;
        real32 yaw;
        real32 roll;

        real32 targetRotY;
        vec3 targetPos;
        real32 currentSpeed;
        real32 targetSpeed;
        real32 targetTurnSpeed;
        real32 currentTurnSpeed;
        real32 rotationSpeed;
        bool cursorCaptured;
        bool firstMouseCapture;

    CursorMode currentCursorMode;
    bool cursorModeJustChanged;
};
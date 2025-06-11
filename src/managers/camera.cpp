
void NormalizeVector(vec3* v)
{
	float length = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
	if (length > 0.0001f) {
		v->x /= length;
		v->y /= length;
		v->z /= length;
    }
}

void CrossProduct(const vec3& a, const vec3& b, vec3* result)
    {
        result->x = a.y * b.z - a.z * b.y;
        result->y = a.z * b.x - a.x * b.z;
        result->z = a.x * b.y - a.y * b.x;
    }

vec3 ScaleVector(const vec3& v, float scale)
{
	vec3 result;
	result.x = v.x * scale;
	result.y = v.y * scale;
	result.z = v.z * scale;
	return result;
}

void SetCursorMode(GLFWwindow* glfwWindow, InputManager* inputManager, Camera* cam, CursorMode newMode)
{
//        if (cam->currentCursorMode == newMode)
//        {
//            return;
//        }

	CursorMode oldMode = cam->currentCursorMode;

	cam->currentCursorMode = newMode;
	cam->cursorModeJustChanged = true;

	// mode specific changes
	switch(cam->currentCursorMode)
	{
	case CURSOR_MODE_GAME:
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		// EnterGameMode(inputManager);
		break;
	case CURSOR_MODE_UI:
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	default:
		printf("ERROR: Attempted to set invalid cursor mode: %d\n", newMode);
	break;
	}


	printf("Cursor mode changed: %d -> %d\n", oldMode, cam->currentCursorMode);

}

void UpdateCamera(WindowManager* windowManager, Camera* cam, InputManager* inputManager, Time* time)
    {
        switch (cam->currentCursorMode)
        {
            case CURSOR_MODE_GAME: {

                float moveSpeed = 15.0f * time->deltaTime;

                // Get mouse delta and calculate rotation
                vec2 mouseDelta = GetMouseDelta(inputManager);

                // Apply mouse sensitivity
                float sensitivity = 0.1f;
                float yaw = mouseDelta.x * sensitivity;
                float pitch = -mouseDelta.y * sensitivity; // Invert Y for natural feel

                // Update camera angles
                cam->yaw -= yaw;
                cam->pitch += pitch;

                // Clamp pitch to avoid camera flipping
                if (cam->pitch > 89.0f)
                    cam->pitch = 89.0f;
                if (cam->pitch < -89.0f)
                    cam->pitch = -89.0f;

                // Calculate new orientation vectors based on yaw and pitch
                // Note: We're using a Z-up coordinate system
                float yawRad = cam->yaw * (3.14159f / 180.0f);
                float pitchRad = cam->pitch * (3.14159f / 180.0f);

                // Calculate front vector (this is the direction the camera is looking)
                // Adjusted for Y-forward, Z-up coordinate system
                cam->front.x = sin(yawRad) * cos(pitchRad);
                cam->front.y = -cos(yawRad) * cos(pitchRad);
                cam->front.z = sin(pitchRad);
                NormalizeVector(&cam->front);

                // Recalculate right vector (perpendicular to front and world up)
                vec3 worldUp = {0.0f, 0.0f, 1.0f}; // Z-up world
                CrossProduct(cam->front, worldUp, &cam->right);
                NormalizeVector(&cam->right);

                // Recalculate up vector (perpendicular to front and right)
                CrossProduct(cam->right, cam->front, &cam->up);
                NormalizeVector(&cam->up);

                // Player Keyboard Movement (FPS style)
                if (InputHeld(inputManager->keyboard, Input_W)) {
                    // Move forward in XY plane (FPS style)
                    vec3 forwardXY = cam->front;
                    forwardXY.z = 0.0f; // Zero out vertical component for typical FPS movement
                    NormalizeVector(&forwardXY);
                    cam->pos = cam->pos + ScaleVector(forwardXY, moveSpeed);
                }
                if (InputHeld(inputManager->keyboard, Input_S)) {
                    // Move backward in XY plane
                    vec3 forwardXY = cam->front;
                    forwardXY.z = 0.0f;
                    NormalizeVector(&forwardXY);
                    cam->pos = cam->pos - ScaleVector(forwardXY, moveSpeed);
                	std::cout << "Cam pos: " << std::endl;
                	std::cout << cam->pos.x << std::endl;
                }
                if (InputHeld(inputManager->keyboard, Input_A)) {
                    // Strafe left
                    cam->pos = cam->pos - ScaleVector(cam->right, moveSpeed);
                }
                if (InputHeld(inputManager->keyboard, Input_D)) {
                    // Strafe right
                    cam->pos = cam->pos + ScaleVector(cam->right, moveSpeed);
                }

            	if (InputHeld(inputManager->keyboard, Input_Q)) {
            		// Strafe left
            		cam->pos = cam->pos - ScaleVector(cam->up, moveSpeed);
            	}
            	if (InputHeld(inputManager->keyboard, Input_E)) {
            		// Strafe right
            		cam->pos = cam->pos + ScaleVector(cam->up, moveSpeed);
            	}

                // // Optional: Q/E for vertical movement (often used in free-cam modes)
                // if (InputHeld(inputManager->keyboard, Input_Q)) {
                //     cam->pos.z += moveSpeed; // Move up in world space
                // }
                // if (InputHeld(inputManager->keyboard, Input_E)) {
                //     cam->pos.z -= moveSpeed; // Move down in world space
                // }
                break;
            }

            case CURSOR_MODE_UI:
            {

                break;
            }

            default:
            {
                break;
            }
        }








        if (InputPressed(inputManager->keyboard, Input_Tab)) {
            if (cam->currentCursorMode == CURSOR_MODE_GAME) {
                SetCursorMode(windowManager->glfwWindow, inputManager, cam, CURSOR_MODE_UI);
            } else {
                SetCursorMode(windowManager->glfwWindow, inputManager, cam, CURSOR_MODE_GAME);
            }
        }
    }




void InitCamera(Camera* cam, GLFWwindow* glfWwindow, InputManager* inputManager)
{
	cam->rotationSpeed = 100.0f;
	cam->targetPos = V3(0.0f, 0.0f, 0.0f);
	cam->currentSpeed = 0;
	cam->targetSpeed = 20.0f;
	cam->targetTurnSpeed = 160.0f;

	cam->pos = V3(0, 0, 0.5f);
	cam->front = V3(0, -1, 0);
	cam->up = V3(0, 0, 1);
	cam->right = V3(1, 0, 0);

	cam->cursorCaptured = false;
	cam->firstMouseCapture = true;

	cam->currentCursorMode = CURSOR_MODE_GAME;
	SetCursorMode(glfWwindow, inputManager, cam, cam->currentCursorMode);

	std::cout << "Camera Init'd" << std::endl;
}


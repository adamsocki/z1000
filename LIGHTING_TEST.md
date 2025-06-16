# Lighting Mode Testing Instructions

Your lighting system is working correctly! Here's how to test it:

## Steps to Test Lighting Modes:

1. **Run the application**: `./build/z1000`

2. **Load test level**: Press **'L'** to load the test level with lighting materials

3. **Enable level editor**: Press **Tab** to enable the level editor and show ImGui windows

4. **Open Lighting Controls**: Look for the "Lighting Controls" window in the ImGui interface

5. **Switch lighting modes**: Use the dropdown in the Lighting Controls window to switch between:
   - Simple (Color * Light)
   - Ambient Only  
   - Ambient + Diffuse
   - Ambient + Diffuse + Specular (Phong)

## Expected Visual Changes:

- **Simple Color**: Objects should be fully lit with light color * object color
- **Ambient Only**: Objects should be dimly lit uniformly (no directional lighting)
- **Diffuse**: Objects should show directional lighting with shadows/highlights
- **Specular**: Objects should show directional lighting + specular highlights

## Key Information:

- Your test level uses "Lighting - Coral" and "Lighting - Red" materials (both MATERIAL_LIGHTING type)
- Default light is positioned at (0, 5, 0) with white color
- You can adjust ambient strength, specular strength, and shininess in the UI
- Only materials with `MATERIAL_LIGHTING` type respond to lighting mode changes

## Troubleshooting:

If you still don't see changes:
1. Make sure you pressed 'L' to load the test level
2. Make sure you pressed Tab to enable the level editor
3. Check that the Lighting Controls window is visible
4. Verify the objects in your scene are using lighting materials (they should appear in coral/red colors)

The system is working correctly - the issue was likely that you needed to load the test level and enable the editor interface.
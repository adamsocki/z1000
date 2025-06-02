#!/bin/bash

# --- Configuration ---
# !! IMPORTANT: Update GLSLC_PATH if glslc is not in your system PATH !!
# Find it in your Vulkan SDK installation, e.g., /Users/<your_user>/VulkanSDK/<version>/macOS/bin/glslc
# Or maybe /usr/local/bin/glslc if installed differently (e.g., via Homebrew).
# If it's in your PATH, you can just use "glslc".
GLSLC_PATH="glslc" # ASSUMES glslc is in the system PATH - CHANGE IF NEEDED!

# !! IMPORTANT: Update these paths to match your project structure on macOS !!
# Use absolute paths or paths relative to where you run the script.
# Example: INPUT_DIR="/Users/your_username/dev/Zayn_PC/src/render/shaders"
INPUT_DIR="/Users/socki/dev/z3/src/render/shaders" # <-- EDIT THIS
OUTPUT_DIR="${INPUT_DIR}/compiled" # Output directory is inside the input dir
# --- End Configuration ---

# Check if the glslc command exists at the specified path or in the system PATH
if ! command -v "$GLSLC_PATH" &> /dev/null; then
    echo "Error: glslc command not found using path '$GLSLC_PATH'."
    echo "Please ensure the Vulkan SDK is installed and glslc is accessible."
    echo "Verify the GLSLC_PATH variable in this script is correct."
    exit 1
fi

# Check if input directory exists
if [ ! -d "$INPUT_DIR" ]; then
    echo "Error: Input directory not found: $INPUT_DIR"
    echo "Please verify the INPUT_DIR variable in this script."
    exit 1
fi

# Create output directory if it doesn't exist
# The '-p' flag creates parent directories as needed and prevents errors if it already exists.
if [ ! -d "$OUTPUT_DIR" ]; then
  echo "Creating directory: $OUTPUT_DIR"
  mkdir -p "$OUTPUT_DIR"
  if [ $? -ne 0 ]; then
    echo "Error: Failed to create directory $OUTPUT_DIR"
    exit 1
  fi
fi

echo "Compiling Vertex Shaders..."
# Loop through all .vert files in the input directory
# Using nullglob ensures the loop doesn't run if no files match
shopt -s nullglob
vertex_files=("$INPUT_DIR"/*.vert)
shopt -u nullglob

if [ ${#vertex_files[@]} -eq 0 ]; then
    echo "No .vert files found in $INPUT_DIR"
else
    for f in "${vertex_files[@]}"; do
      # Extract filename without .vert extension
      base_name=$(basename "$f" .vert)
      output_file="$OUTPUT_DIR/${base_name}_vert.spv"
      echo "  Compiling $f -> $output_file"
      # Run the compiler
      "$GLSLC_PATH" "$f" -o "$output_file"
      # Check for compilation errors
      if [ $? -ne 0 ]; then
          echo "  Error compiling $f"
      fi
    done
fi


echo "Compiling Fragment Shaders..."
# Loop through all .frag files
shopt -s nullglob
fragment_files=("$INPUT_DIR"/*.frag)
shopt -u nullglob

if [ ${#fragment_files[@]} -eq 0 ]; then
    echo "No .frag files found in $INPUT_DIR"
else
    for f in "${fragment_files[@]}"; do
      # Extract filename without .frag extension
      base_name=$(basename "$f" .frag)
      output_file="$OUTPUT_DIR/${base_name}_frag.spv"
      echo "  Compiling $f -> $output_file"
      # Run the compiler
      "$GLSLC_PATH" "$f" -o "$output_file"
      # Check for compilation errors
      if [ $? -ne 0 ]; then
          echo "  Error compiling $f"
      fi
    done
fi


echo "Shader compilation complete."

# Optional pause - uncomment if needed
# read -p "Press Enter to continue..."
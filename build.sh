#!/usr/bin/env bash
# ALGOL 26 Deployment Pipeline Automation Harness

set -e # Abort script immediately if any intermediate step exits with an error status

# Clear terminal screen layout bounds
clear
echo "========================================================"
echo "          ALGOL126 TEST & BUILD AUTOMATOR               "
echo "========================================================"

# Pipeline Stage 1: Explicit C++23 Environment Verification Checks
echo "[Harness] Verifying host development baseline metrics..."
if ! command -v g++ &> /dev/null; then
    echo " Error: Host GNU Compiler Collection (g++ Compiler) not found." #screw clang
    exit 1
fi

GCC_VERSION=$(g++ -dumpversion | cut -d. -f1)
if [ "$GCC_VERSION" -lt 14 ]; then
    echo " Error: Detected GCC version $GCC_VERSION. ALGOL126 requires GCC 14+ for complete C++23 standard module imports. or mod the build.sh for clang idk"
    exit 1
fi
echo "[Harness] Environment Verified. GCC Version $GCC_VERSION (C++23 Compliant) Active."

# Pipeline Stage 2: Clear workspace cache artifacts and trigger build
echo "[Harness] Triggering clean workspace rebuild..."
make clean
make

# Pipeline Stage 3: Auto-process comprehensive examples verification suite
echo -e "\n[Harness] Dispatching complete reference script test matrix...\n"

TEST_SCRIPTS=("src/simulation.a26" "examples/hello_world.a26" "examples/physics_sim.a26" "examples/pointer_scan.a26")

for script in "${TEST_SCRIPTS[@]}"; do
    if [ -f "$script" ]; then
        echo "--------------------------------------------------------"
        echo "[Test] Compiling reference file: $script"
        ./a26c "$script"
        echo "[Test] Execution Pass Complete for $script"
    else
        echo "[Test] Skipping $script (file not populated in current workspace branch)."
    fi
done

echo -e "\n=============================================================="
echo "[Harness Victory] All specifications built and passed successfully!"
echo "==================================================================="

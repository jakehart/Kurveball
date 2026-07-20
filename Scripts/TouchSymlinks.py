# MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#
# Recreates the symlinks of all files in the Unreal plugin directory whose
# targets have been updated since the symlink was created. This is needed
# to make MSVS and Live Coding able to see that the files have changed, since
# they rely on the "modified" timestamp.

import os
import pathlib
import time

UNREAL_PLUGIN_DIR = pathlib.Path("../Examples/UnrealCurveDemo/Plugins/KurveballPlugin/Source")
now = time.time()

print("Touching symlinks of updated files...")

for symlink in UNREAL_PLUGIN_DIR.rglob("*"):
    if not symlink.is_symlink():
        continue
    
    target = symlink.resolve()
    if not target.exists():
        print(f"Broken symlink: {symlink}")
        continue
    
    # Compare target's mtime against symlink's mtime
    target_mtime = target.stat().st_mtime
    link_mtime = symlink.lstat().st_mtime
    
    if target_mtime > link_mtime:
        target_path_str = str(target.absolute())
        symlink.unlink()  # Delete the symlink (not the target)
        os.symlink(target_path_str, symlink)  # Recreate it
        print(f"Recycled: {target.name} -> {symlink}")

input("Done.")
LICENSE_STRING="// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md"

for file in *.{cpp,h}; do
    # Use double quotes so grep treats the string as one argument
    if ! grep -q "$LICENSE_STRING" "$file"; then
        # Use double quotes for sed to expand the variable
        # Use a newline character afterward
        sed -i "1i $LICENSE_STRING" "$file"
    fi
done
#!/bin/bash
set -e

# Add all changes
git add .

# Commit with a message
commit_msg="${1:-Update all files}"
git commit -m "$commit_msg"

# Push to current branch
current_branch=$(git rev-parse --abbrev-ref HEAD)
git push origin "$current_branch"
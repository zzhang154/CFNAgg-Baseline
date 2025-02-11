#!/bin/bash

# Navigate to the ns-3 directory
cd ~/tcp-agg/ns-allinone-3.42/ns-3.42

# Initialize Git repository (if not already initialized)
if [ ! -d ".git" ]; then
  git init
  echo "Initialized empty Git repository."
fi

# Add all files to the staging area
git add .

# Commit the changes
git commit -m "TCP-Agg v-10.1-debug:
(1) Finish the trace of packet path via modifying the forwarder's callback function, see 'setup.cc' file.

(2) For every application node, we add the tag in TCPclient->send().

(3) Fix the bug of TP unit computation error before.
"

# Rename current branch to tcp-baseline (if needed)
# Check if tcp-baseline exists, if not, rename current branch
if git show-ref --verify --quiet refs/heads/tcp-baseline; then
  echo "tcp-baseline branch exists, checking it out."
  git checkout tcp-baseline
else
  echo "tcp-baseline branch does not exist, renaming current branch."
  git branch -m tcp-baseline
fi

# Add remote origin (if not already added)
if git remote get-url origin >/dev/null 2>&1; then
  echo "Remote 'origin' already configured."
else
  echo "Remote 'origin' not configured. Please enter your repository URL:"
  read REPOSITORY_URL
  git remote add origin "$REPOSITORY_URL"
  echo "Remote 'origin' added."
fi

# Push the changes to the remote repository
git push -u origin tcp-baseline

echo "All files pushed to the tcp-baseline branch."
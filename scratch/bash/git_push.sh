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
git commit -m "TCP-Agg v-13.2-release:
(1) Fix bugs for large graph (ipv4 address assignment).
(2) Pipline more things together and updates the dataset.
(3) Todo: write a function that can run TCPno-agg function. Just needs to change the aggtree. Others remains the same.
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
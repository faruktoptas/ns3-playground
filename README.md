# NS-3 Playground

A small repository to experiment with ns-3 simulations inside a container and via GitHub Actions.

This repo expects ns-3 example source files (.cc) under the `scratch/` folder. Instead of cloning the repo, contributors should add a simulation file and open a pull request — the workflow will pick up files added under `scratch/` and run them inside the container.

## Quickstart 

Add a simulation file and create a pull request on GitHub (recommended via the web UI):

1. Open the repository on GitHub: https://github.com/faruktoptas/ns3-playground
2. Click "Add file" -> "Create new file".
3. In the filename box, enter `scratch/<your_name>.cc` (for example `scratch/my_example.cc`).
4. Paste your ns-3 `.cc` content into the editor.
5. Below the editor choose "Create a new branch for this commit and start a pull request" (or set a branch name), then click "Propose new file".
6. Finish the PR creation (add title/description) and submit the PR.

What happens next
- When your pull request is opened, GitHub Actions will run the `run-simulation` job which copies any `scratch/*.cc` files into the container and builds/runs each example.
- The `build-and-push-image` job is restricted to the `build-docker` branch (it won't run for normal contributor PRs). This keeps normal PR runs lightweight.

If you prefer to use the command line and have permission to push branches, you can also create a branch locally, add `scratch/*.cc`, push the branch and open a PR — but cloning is not required for basic contributions via the web UI.

## How the runner executes examples

- The workflow will attempt to build and run every `scratch/*.cc` it finds. Name the file with a `.cc` extension and use standard ns-3 example conventions.
- If a build or run fails for one file, the workflow will continue with the remaining examples.

## Optional: test locally (advanced)

If you want to run the same container locally (this requires cloning and Docker locally):

```bash
docker build -t ns3-playground:local .
docker run --rm -v "$(pwd)":/workspace ns3-playground:local /bin/bash -lc "cd /opt/ns-3 && cp /workspace/scratch/*.cc scratch/ || true && for src in scratch/*.cc; do [ -e \"$src\" ] || continue; name=$(basename \"$src\" .cc); ./ns3 build scratch/$name && ./ns3 run scratch/$name || true; done"
```

## Notes & troubleshooting

- Make sure your `.cc` file follows ns-3 example conventions and includes any required headers.
- If the workflow cannot find your example, confirm your PR added the file under the `scratch/` path and that the filename ends with `.cc`.
- The CI logs show build/run output for each example; use those logs to debug failures.

## Contribution

Contributions are welcome. To add or update examples, add the `.cc` under `scratch/` and open a PR.

---
Updated Quickstart to use PR-based contributions (no clone required).
# ns3-playground
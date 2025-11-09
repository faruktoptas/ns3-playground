# NS-3 Playground

A small repository to experiment with ns-3 simulations inside a container and via GitHub Actions.

This repo expects you to place ns-3 example source files (.cc) into the `scratch/` folder. When files are added and pushed, the workflow will pick them up and run the simulation in a container.

## Quickstart

1. Clone the repository:

```bash
git clone https://github.com/faruktoptas/ns3-playground.git
cd ns3-playground
```

2. Add your simulation.

- Create a new ns-3 example file under `scratch/`, for example `scratch/my_example.cc`.
- Build and run will be performed by the container/workflow. Commit and push the new file:

```bash
git checkout -b my-sim
# add scratch/my_example.cc
git add scratch/my_example.cc
git commit -m "Add my ns-3 example"
git push -u origin my-sim
```

Pushing to the repository will trigger the GitHub Actions workflow defined in `.github/workflows/ns3.yml`. The workflow copies any `scratch/*.cc` file into the container and runs the example.

## How workflows behave in this repo

- `run-simulation` runs (in parallel) and will attempt to run any examples found in `scratch/` by using the container image.
- `build-and-push-image` builds and pushes the container image, but it is restricted to run only on the `build-docker` branch. If you want the image built/pushed automatically, push or open a PR from the `build-docker` branch.

If you need to change this behavior, edit `.github/workflows/ns3.yml`.

## Run locally (optional)

If you prefer to iterate locally without depending on GitHub Actions, you can build the image and run the same steps the workflow runs. The repository contains a `Dockerfile` for an image that includes ns-3.

1. Build the image locally:

```bash
docker build -t ns3-playground:local .
```

2. Run a container and execute the same commands used by the workflow (this mounts the repo into the container at `/workspace`):

```bash
docker run --rm -v "$(pwd)":/workspace ns3-playground:local /bin/bash -lc "cd /opt/ns-3 && cp /workspace/scratch/*.cc scratch/ || true && ./ns3 build scratch/example || true && ./ns3 run scratch/example || true"
```

Adjust the build/run commands for your specific example name (replace `scratch/example` with your example target).

## Notes & troubleshooting

- Make sure your `.cc` file follows ns-3 example conventions and that any required headers are present.
- The workflow mounts your repository into the container; if you add other large files they will also be mounted.
- If GitHub Actions can't find your example, check that the filename matches `scratch/*.cc` and that the workflow run logs show the copied files.

## Contribution

Contributions are welcome. If you want to change workflow behavior or add examples, open a PR.

---
Generated README to explain how to clone, add files under `scratch/`, and run simulations locally or via GitHub Actions.
# ns3-playground
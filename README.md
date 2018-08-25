# TabletDriver

Unofficial modified version of [hawku/TabletDriver](https://github.com/hawku/TabletDriver).

This version intends to provide 6.3.9w5-like axis predicting mechanics, "effectively" reducing latency.

Trace smoothing is removed. Use at your own risk.

(i found that this thing is a joke so don't expect updates lul)

# Usage

Go to official repo to get Driver, install.

Instead, run TabletDriverGUI from this version.

# Algorithms

- Linear (2 points)
- Polygon (3 points, vector rotation)
- Catmull-Rom based on Polygon
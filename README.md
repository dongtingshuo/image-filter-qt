# Image Filter (Qt)

A Qt + OpenCV desktop app that previews common image filters side-by-side and lets you tune intensity with a slider.

## Features
- Original, grayscale, invert, Gaussian blur, emboss, and cartoon filters
- Live preview with source/processed panels
- Adjustable filter intensity
- Load/save images (JPG/PNG/BMP)

## Tech Stack
- Qt Widgets (qmake)
- OpenCV
- C++17

## Build
### Prerequisites
- Qt 6 (or Qt 5 with Widgets)
- OpenCV 4

### Qt Creator
1. Open `image-filter.pro` in Qt Creator.
2. Configure your kit (Qt + compiler).
3. Build and run.

### qmake (CLI)
```bash
qmake image-filter.pro
make
```

## Project Structure
```
include/        # headers
src/            # sources
ui/             # Qt Designer files
```

## Notes
- On Windows, update the OpenCV paths in `image-filter.pro` to match your local installation.

## License
MIT License. See LICENSE.

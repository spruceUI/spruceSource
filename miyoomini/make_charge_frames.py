"""Build the Miyoo Mini charge screen's framebuffer frames from the SPRUCE theme.

spruceOS's charge_gate.sh draws these straight to /dev/fb0, with nothing to
decode a PNG at that point in boot, so each frame is stored the way the
framebuffer wants it: on a transparent screen of the panel's size, turned half
way round (every Mini's panel is mounted upside down), as B,G,R,A bytes. Only
the rows any frame draws on are kept; the gate clears the screen once and then
rewrites that band, which is a fifth of a full frame. One set per screen size:

    pip install pillow
    python3 make_charge_frames.py --size 752x560 <spruceOS>/Themes/SPRUCE/skin_752x560 \\
        <spruceOS>/spruce/miyoomini/charge/752x560
    python3 make_charge_frames.py --size 640x480 <spruceOS>/Themes/SPRUCE/skin \\
        <spruceOS>/spruce/miyoomini/charge/640x480

Writes frame-1.raw.gz to frame-5.raw.gz and band.txt
("<width> <height> <top row> <rows>"). The output is reproducible: the same
theme images give the same bytes.
"""

from __future__ import annotations

import argparse
import gzip
import sys
from collections.abc import Sequence
from dataclasses import dataclass
from pathlib import Path

from PIL import Image

SCREEN_SIZE = (752, 560)
FRAME_NAMES = tuple(f"charging-0{index}.png" for index in range(1, 6))
BAND_FILE = "band.txt"


class FrameError(Exception):
    """A theme image is missing or cannot become a charge frame."""


@dataclass(frozen=True)
class Band:
    """The rows of the screen that the charge frames draw on."""

    top: int
    rows: int


def compose(strip: Image.Image, screen_size: tuple[int, int]) -> Image.Image:
    """Center a theme image on a transparent screen and turn it half way round.

    The gate clears the rest of the screen to all-zero bytes, transparent black. The image keeps its own
    transparency so that its background matches that clear exactly: some panels show a transparent pixel
    lighter than an opaque black one, and an opaque band would stand out as a darker block.

    Args:
        strip: The theme image, any mode.
        screen_size: Width and height of the screen.

    Returns:
        An RGBA image of the screen as the framebuffer holds it.

    Raises:
        FrameError: The image does not fit on the screen.
    """
    width, height = screen_size
    if strip.width > width or strip.height > height:
        raise FrameError(f"a {strip.width}x{strip.height} image is larger than the screen, {width}x{height}")
    screen = Image.new("RGBA", screen_size, (0, 0, 0, 0))
    screen.paste(strip.convert("RGBA"), ((width - strip.width) // 2, (height - strip.height) // 2))
    return screen.rotate(180)


def band_of(frames: Sequence[Image.Image]) -> Band:
    """Find the rows that any frame draws on.

    Args:
        frames: Composed screens, as compose returns them.

    Returns:
        The smallest band of full-width rows that holds every pixel that is not fully transparent.

    Raises:
        FrameError: Every frame is fully transparent.
    """
    boxes = [box for box in (frame.getchannel("A").getbbox() for frame in frames) if box is not None]
    if not boxes:
        raise FrameError("no frame draws anything: every theme image is fully transparent")
    top = min(box[1] for box in boxes)
    bottom = max(box[3] for box in boxes)
    return Band(top=top, rows=bottom - top)


def band_bytes(frame: Image.Image, band: Band) -> bytes:
    """Return the band's full-width rows of a frame as B,G,R,A bytes."""
    rows = frame.crop((0, band.top, frame.width, band.top + band.rows))
    return rows.tobytes("raw", "BGRA")


def _load(skin_dir: Path, name: str) -> Image.Image:
    path = skin_dir / name
    try:
        with Image.open(path) as image:
            return image.convert("RGBA")
    except OSError as error:
        raise FrameError(f"cannot read the theme image {name} in {skin_dir}: {error}") from error


def build(skin_dir: Path, out_dir: Path, screen_size: tuple[int, int] = SCREEN_SIZE) -> Band:
    """Write the charge frames for one theme skin folder.

    Args:
        skin_dir: Folder holding charging-01.png to charging-05.png.
        out_dir: Folder to write the frames and band.txt to; created if absent.
        screen_size: Width and height of the screen.

    Returns:
        The band the frames cover.

    Raises:
        FrameError: An image is missing, unreadable, too large, or every image is blank.
    """
    frames = [compose(_load(skin_dir, name), screen_size) for name in FRAME_NAMES]
    band = band_of(frames)
    out_dir.mkdir(parents=True, exist_ok=True)
    for index, frame in enumerate(frames, start=1):
        # mtime=0 keeps the archive the same from run to run
        (out_dir / f"frame-{index}.raw.gz").write_bytes(gzip.compress(band_bytes(frame, band), 9, mtime=0))
    width, height = screen_size
    (out_dir / BAND_FILE).write_text(f"{width} {height} {band.top} {band.rows}\n")
    return band


def parse_size(text: str) -> tuple[int, int]:
    """Parse a WIDTHxHEIGHT argument.

    Raises:
        argparse.ArgumentTypeError: The text is not two positive whole numbers joined by "x".
    """
    width, _, height = text.partition("x")
    if not (width.isdigit() and height.isdigit() and int(width) > 0 and int(height) > 0):
        raise argparse.ArgumentTypeError(f"expected WIDTHxHEIGHT such as 752x560, got {text!r}")
    return int(width), int(height)


def main(argv: Sequence[str] | None = None) -> int:
    """Command-line entry point."""
    parser = argparse.ArgumentParser(description="Build the Miyoo Mini charge screen's frames from a theme.")
    parser.add_argument("--size", type=parse_size, default=SCREEN_SIZE, help="screen size, default 752x560")
    parser.add_argument("skin_dir", type=Path, help="theme skin folder, e.g. Themes/SPRUCE/skin_752x560")
    parser.add_argument("out_dir", type=Path, help="output folder, e.g. spruce/miyoomini/charge/752x560")
    args = parser.parse_args(argv)
    try:
        band = build(args.skin_dir, args.out_dir, args.size)
    except FrameError as error:
        print(f"make_charge_frames: {error}", file=sys.stderr)
        return 1
    print(f"wrote {len(FRAME_NAMES)} frames, rows {band.top} to {band.top + band.rows - 1}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

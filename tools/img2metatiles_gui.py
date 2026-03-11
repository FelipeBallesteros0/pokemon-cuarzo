#!/usr/bin/env python3
from __future__ import annotations

import json
import io
import subprocess
import sys
import threading
import tkinter as tk
from contextlib import redirect_stderr, redirect_stdout
import importlib.util
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

from PIL import Image, ImageTk

SCRIPT_DIR = Path(__file__).resolve().parent
EXE_DIR = Path(sys.executable).resolve().parent if getattr(sys, "frozen", False) else SCRIPT_DIR
REPO_ROOT = SCRIPT_DIR.parent

# Resolution order:
# 1) portable next to EXE (works for PyInstaller onefile/onedir)
# 2) portable next to script
# 3) repo fallback
if (EXE_DIR / "img2metatiles.py").exists():
    ROOT = EXE_DIR
    CLI_TOOL = EXE_DIR / "img2metatiles.py"
    CFG_FILE = EXE_DIR / ".img2metatiles_gui.json"
elif (SCRIPT_DIR / "img2metatiles.py").exists():
    ROOT = SCRIPT_DIR
    CLI_TOOL = SCRIPT_DIR / "img2metatiles.py"
    CFG_FILE = SCRIPT_DIR / ".img2metatiles_gui.json"
else:
    ROOT = REPO_ROOT
    CLI_TOOL = REPO_ROOT / "tools" / "img2metatiles.py"
    CFG_FILE = REPO_ROOT / ".img2metatiles_gui.json"


def load_cli_module():
    try:
        import img2metatiles as cli_mod
        return cli_mod
    except Exception:
        pass
    if not CLI_TOOL.exists():
        return None


def _parse_jasc_pal(path: Path) -> list[tuple[int, int, int]]:
    lines = [ln.strip() for ln in path.read_text(encoding="utf-8").splitlines() if ln.strip()]
    if len(lines) < 3 or lines[0] != "JASC-PAL":
        raise ValueError(f"invalid JASC-PAL: {path}")
    count = int(lines[2])
    colors: list[tuple[int, int, int]] = []
    for ln in lines[3 : 3 + count]:
        parts = ln.split()
        if len(parts) != 3:
            raise ValueError(f"invalid JASC-PAL color line: {ln}")
        colors.append((int(parts[0]), int(parts[1]), int(parts[2])))
    if not colors:
        raise ValueError(f"empty JASC-PAL: {path}")
    return [(max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b))) for (r, g, b) in colors[:16]]


def _parse_gbapal(path: Path) -> list[tuple[int, int, int]]:
    data = path.read_bytes()
    if len(data) < 32:
        raise ValueError(f"invalid gbapal size: {path}")
    colors: list[tuple[int, int, int]] = []
    for i in range(16):
        v = int.from_bytes(data[i * 2 : i * 2 + 2], "little")
        r = (v & 0x1F) * 255 // 31
        g = ((v >> 5) & 0x1F) * 255 // 31
        b = ((v >> 10) & 0x1F) * 255 // 31
        colors.append((r, g, b))
    return colors


def _load_palette_sidecar(img_path: Path) -> list[tuple[int, int, int]] | None:
    pal = img_path.with_suffix(".pal")
    gbapal = img_path.with_suffix(".gbapal")
    if pal.exists():
        return _parse_jasc_pal(pal)
    if gbapal.exists():
        return _parse_gbapal(gbapal)
    return None
    try:
        spec = importlib.util.spec_from_file_location("img2metatiles_embedded", str(CLI_TOOL))
        if spec is None or spec.loader is None:
            return None
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        return mod
    except Exception:
        return None


class App(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self.title("IMG2Metatiles GUI")
        self.geometry("1120x860")
        self.cli_module = load_cli_module()
        self.preview_size = (400, 220)
        self.preview_input_frames: list[ImageTk.PhotoImage] = []
        self.preview_output_frames: list[ImageTk.PhotoImage] = []
        self.preview_input_idx = 0
        self.preview_output_idx = 0
        self.preview_timer = None

        self.vars = {
            "input": tk.StringVar(),
            "bg_metatile_png": tk.StringVar(),
            "out_dir": tk.StringVar(value=str(ROOT / "build" / "img2metatiles_out")),
            "apply_to": tk.StringVar(),
            "mode": tk.StringVar(value="keep-order"),
            "tile_base": tk.StringVar(value="0x200"),
            "palette_slot": tk.StringVar(value="6"),
            "metatile_attr": tk.StringVar(value="0x0"),
            "transparent_key": tk.StringVar(value="magenta"),
            "quantize": tk.BooleanVar(value=False),
            "lock_input_palette": tk.BooleanVar(value=True),
            "write_palette": tk.BooleanVar(value=True),
            "anim": tk.BooleanVar(value=False),
            "frames_x": tk.StringVar(value="4"),
            "frames_y": tk.StringVar(value="2"),
            "frame_width": tk.StringVar(value="128"),
            "frame_height": tk.StringVar(value="128"),
            "anim_name": tk.StringVar(value="pool"),
            "preview_game_frames": tk.StringVar(value="12"),
            "avoid_index_zero": tk.BooleanVar(value=True),
        }

        self._build_ui()
        self._load_cfg()

    def _build_ui(self) -> None:
        frm = ttk.Frame(self, padding=10)
        frm.pack(fill=tk.BOTH, expand=True)

        row = 0
        self._path_row(frm, row, "Input PNG", "input", is_dir=False)
        row += 1
        self._path_row(frm, row, "Background Metatile 16x16 (optional)", "bg_metatile_png", is_dir=False)
        row += 1
        self._path_row(frm, row, "Output Folder", "out_dir", is_dir=True)
        row += 1
        self._path_row(frm, row, "Apply To Tileset (optional)", "apply_to", is_dir=True)
        row += 1

        ttk.Separator(frm).grid(row=row, column=0, columnspan=4, sticky="ew", pady=8)
        row += 1

        ttk.Label(frm, text="Mode").grid(row=row, column=0, sticky="w")
        ttk.Combobox(
            frm,
            state="readonly",
            values=["keep-order", "dedup"],
            textvariable=self.vars["mode"],
            width=18,
        ).grid(row=row, column=1, sticky="w")
        ttk.Label(frm, text="Tile Base").grid(row=row, column=2, sticky="w")
        ttk.Entry(frm, textvariable=self.vars["tile_base"], width=18).grid(row=row, column=3, sticky="w")
        row += 1

        ttk.Label(frm, text="Palette Slot").grid(row=row, column=0, sticky="w")
        ttk.Entry(frm, textvariable=self.vars["palette_slot"], width=18).grid(row=row, column=1, sticky="w")
        ttk.Label(frm, text="Transparent Key").grid(row=row, column=2, sticky="w")
        ttk.Combobox(
            frm,
            state="readonly",
            values=["magenta", "none"],
            textvariable=self.vars["transparent_key"],
            width=16,
        ).grid(row=row, column=3, sticky="w")
        row += 1

        ttk.Label(frm, text="Metatile Attr").grid(row=row, column=0, sticky="w")
        ttk.Entry(frm, textvariable=self.vars["metatile_attr"], width=18).grid(row=row, column=1, sticky="w")
        row += 1

        ttk.Checkbutton(frm, text="Quantize input if needed", variable=self.vars["quantize"]).grid(
            row=row, column=0, columnspan=2, sticky="w"
        )
        ttk.Checkbutton(
            frm,
            text="Lock input palette (exact colors)",
            variable=self.vars["lock_input_palette"],
        ).grid(row=row, column=2, columnspan=2, sticky="w")
        row += 1

        ttk.Checkbutton(frm, text="Write palette into apply-to", variable=self.vars["write_palette"]).grid(
            row=row, column=0, columnspan=2, sticky="w"
        )
        ttk.Checkbutton(frm, text="Animation sheet mode", variable=self.vars["anim"]).grid(
            row=row, column=2, columnspan=2, sticky="w"
        )
        ttk.Checkbutton(
            frm,
            text="Avoid visible index 0 (recommended for BG)",
            variable=self.vars["avoid_index_zero"],
        ).grid(row=row, column=0, columnspan=2, sticky="w")
        row += 1

        ttk.Label(frm, text="Frames X").grid(row=row, column=0, sticky="w")
        ttk.Entry(frm, textvariable=self.vars["frames_x"], width=18).grid(row=row, column=1, sticky="w")
        ttk.Label(frm, text="Frames Y").grid(row=row, column=2, sticky="w")
        ttk.Entry(frm, textvariable=self.vars["frames_y"], width=18).grid(row=row, column=3, sticky="w")
        row += 1

        ttk.Label(frm, text="Frame Width").grid(row=row, column=0, sticky="w")
        ttk.Entry(frm, textvariable=self.vars["frame_width"], width=18).grid(row=row, column=1, sticky="w")
        ttk.Label(frm, text="Frame Height").grid(row=row, column=2, sticky="w")
        ttk.Entry(frm, textvariable=self.vars["frame_height"], width=18).grid(row=row, column=3, sticky="w")
        row += 1

        ttk.Label(frm, text="Anim Name").grid(row=row, column=0, sticky="w")
        ttk.Entry(frm, textvariable=self.vars["anim_name"], width=18).grid(row=row, column=1, sticky="w")
        ttk.Label(frm, text="Preview Step (game frames)").grid(row=row, column=2, sticky="w")
        ttk.Entry(frm, textvariable=self.vars["preview_game_frames"], width=18).grid(row=row, column=3, sticky="w")
        row += 1

        ttk.Separator(frm).grid(row=row, column=0, columnspan=4, sticky="ew", pady=8)
        row += 1

        btns = ttk.Frame(frm)
        btns.grid(row=row, column=0, columnspan=4, sticky="w")
        ttk.Button(btns, text="Generate Tileset", command=self.run_generate).pack(side=tk.LEFT, padx=(0, 8))
        ttk.Button(btns, text="Open Output Folder", command=self.open_output).pack(side=tk.LEFT, padx=(0, 8))
        ttk.Button(btns, text="Save Config", command=self._save_cfg).pack(side=tk.LEFT, padx=(0, 8))
        ttk.Button(btns, text="Preview Input", command=self.preview_input).pack(side=tk.LEFT, padx=(0, 8))
        ttk.Button(btns, text="Preview Output", command=self.preview_output).pack(side=tk.LEFT, padx=(0, 8))
        ttk.Button(btns, text="Stop Preview", command=self.stop_preview).pack(side=tk.LEFT, padx=(0, 8))
        row += 1

        previews = ttk.Frame(frm)
        previews.grid(row=row, column=0, columnspan=4, sticky="nsew", pady=(8, 4))
        previews.columnconfigure(0, weight=1)
        previews.columnconfigure(1, weight=1)

        in_box = ttk.LabelFrame(previews, text="Input Preview")
        in_box.grid(row=0, column=0, sticky="nsew", padx=(0, 6))
        out_box = ttk.LabelFrame(previews, text="Output Preview")
        out_box.grid(row=0, column=1, sticky="nsew", padx=(6, 0))
        in_box.columnconfigure(0, weight=1)
        out_box.columnconfigure(0, weight=1)

        self.input_preview_info = ttk.Label(in_box, text="No input preview loaded")
        self.input_preview_info.grid(row=0, column=0, sticky="w", padx=6, pady=(4, 2))
        self.output_preview_info = ttk.Label(out_box, text="No output preview loaded")
        self.output_preview_info.grid(row=0, column=0, sticky="w", padx=6, pady=(4, 2))

        self.input_preview_label = ttk.Label(in_box)
        self.input_preview_label.grid(row=1, column=0, sticky="nsew", padx=6, pady=(0, 6))
        self.output_preview_label = ttk.Label(out_box)
        self.output_preview_label.grid(row=1, column=0, sticky="nsew", padx=6, pady=(0, 6))
        row += 1

        ttk.Label(frm, text="Output Log").grid(row=row, column=0, columnspan=4, sticky="w", pady=(4, 4))
        row += 1

        self.log = tk.Text(frm, height=18, wrap="word")
        self.log.grid(row=row, column=0, columnspan=4, sticky="nsew")
        frm.rowconfigure(row, weight=1)
        for c in range(4):
            frm.columnconfigure(c, weight=1)

    def _path_row(self, parent: ttk.Frame, row: int, label: str, key: str, is_dir: bool) -> None:
        ttk.Label(parent, text=label).grid(row=row, column=0, sticky="w")
        ttk.Entry(parent, textvariable=self.vars[key]).grid(row=row, column=1, columnspan=2, sticky="ew", padx=6)
        ttk.Button(
            parent,
            text="Browse",
            command=lambda: self._browse(key, is_dir),
        ).grid(row=row, column=3, sticky="w")

    def _browse(self, key: str, is_dir: bool) -> None:
        if is_dir:
            p = filedialog.askdirectory(initialdir=str(ROOT))
        else:
            p = filedialog.askopenfilename(
                initialdir=str(ROOT),
                filetypes=[("PNG files", "*.png"), ("All files", "*.*")],
            )
        if p:
            self.vars[key].set(p)

    def _append_log(self, text: str) -> None:
        self.log.insert(tk.END, text)
        self.log.see(tk.END)
        self.update_idletasks()

    def _build_args(self) -> list[str]:
        args = [
            "--input",
            self.vars["input"].get().strip(),
            "--out-dir",
            self.vars["out_dir"].get().strip(),
            "--mode",
            self.vars["mode"].get().strip(),
            "--tile-base",
            self.vars["tile_base"].get().strip(),
            "--palette-slot",
            self.vars["palette_slot"].get().strip(),
            "--transparent-key",
            self.vars["transparent_key"].get().strip(),
            "--metatile-attr",
            self.vars["metatile_attr"].get().strip(),
        ]
        bg_metatile = self.vars["bg_metatile_png"].get().strip()
        if bg_metatile:
            args += ["--bg-metatile-png", bg_metatile]
        if self.vars["quantize"].get():
            args.append("--quantize")
        if self.vars["lock_input_palette"].get():
            args.append("--lock-input-palette")
        if self.vars["anim"].get():
            args += [
                "--anim",
                "--frames-x",
                self.vars["frames_x"].get().strip(),
                "--frames-y",
                self.vars["frames_y"].get().strip(),
                "--frame-width",
                self.vars["frame_width"].get().strip(),
                "--frame-height",
                self.vars["frame_height"].get().strip(),
                "--anim-name",
                self.vars["anim_name"].get().strip(),
            ]
        if self.vars["avoid_index_zero"].get():
            args.append("--avoid-index-zero")
        apply_to = self.vars["apply_to"].get().strip()
        if apply_to:
            args += ["--apply-to", apply_to]
            if self.vars["write_palette"].get():
                args.append("--write-palette")
        return args

    def run_generate(self) -> None:
        input_path = self.vars["input"].get().strip()
        if not input_path:
            messagebox.showerror("Missing input", "Please choose an input PNG.")
            return
        if not Path(input_path).exists():
            messagebox.showerror("Input not found", f"File does not exist:\n{input_path}")
            return

        args = self._build_args()
        self._append_log("\n=== Running ===\n")
        self._append_log("img2metatiles " + " ".join(args) + "\n\n")

        def worker() -> None:
            try:
                if self.cli_module is not None and hasattr(self.cli_module, "main"):
                    old_argv = sys.argv[:]
                    out = io.StringIO()
                    err = io.StringIO()
                    code = 0
                    try:
                        sys.argv = ["img2metatiles.py"] + args
                        with redirect_stdout(out), redirect_stderr(err):
                            self.cli_module.main()
                    except SystemExit as e:
                        try:
                            code = int(e.code)
                        except Exception:
                            code = 1
                    finally:
                        sys.argv = old_argv
                    stdout = out.getvalue()
                    stderr = err.getvalue()
                else:
                    cmd = [sys.executable, str(CLI_TOOL)] + args
                    proc = subprocess.run(
                        cmd,
                        cwd=str(ROOT),
                        text=True,
                        capture_output=True,
                        check=False,
                    )
                    stdout = proc.stdout or ""
                    stderr = proc.stderr or ""
                    code = proc.returncode

                if stdout:
                    self._append_log(stdout)
                if stderr:
                    self._append_log(stderr)

                if code == 0:
                    self._append_log("\nDone.\n")
                    self.preview_output()
                    messagebox.showinfo("Success", "Tileset generated successfully.")
                else:
                    self._append_log(f"\nFailed with exit code {code}\n")
                    messagebox.showerror("Failed", "Generation failed. Check the log.")
            except Exception as e:  # pragma: no cover
                self._append_log(f"\nException: {e}\n")
                messagebox.showerror("Error", str(e))

        threading.Thread(target=worker, daemon=True).start()

    def open_output(self) -> None:
        p = self.vars["out_dir"].get().strip()
        if not p:
            return
        path = Path(p)
        if not path.exists():
            messagebox.showwarning("Not found", f"Output folder does not exist:\n{p}")
            return
        try:
            if sys.platform.startswith("win"):
                subprocess.Popen(["explorer", str(path)])
            elif sys.platform == "darwin":
                subprocess.Popen(["open", str(path)])
            else:
                subprocess.Popen(["xdg-open", str(path)])
        except Exception as e:
            messagebox.showerror("Cannot open folder", str(e))

    def _save_cfg(self) -> None:
        data = {k: v.get() for k, v in self.vars.items()}
        CFG_FILE.write_text(json.dumps(data, indent=2), encoding="utf-8")
        self._append_log(f"Saved config: {CFG_FILE}\n")

    def _load_cfg(self) -> None:
        if not CFG_FILE.exists():
            return
        try:
            data = json.loads(CFG_FILE.read_text(encoding="utf-8"))
            for k, v in data.items():
                if k in self.vars:
                    self.vars[k].set(v)
        except Exception:
            pass

    def _fit_preview_image(self, img: Image.Image) -> Image.Image:
        max_w, max_h = self.preview_size
        src = img.convert("RGBA")
        scale = min(max_w / max(1, src.width), max_h / max(1, src.height))
        scale = max(scale, 1.0)
        new_w = max(1, int(src.width * scale))
        new_h = max(1, int(src.height * scale))
        up = src.resize((new_w, new_h), Image.Resampling.NEAREST)
        canvas = Image.new("RGBA", (max_w, max_h), (24, 24, 24, 255))
        ox = (max_w - new_w) // 2
        oy = (max_h - new_h) // 2
        canvas.alpha_composite(up, (ox, oy))
        return canvas

    def _split_frames_from_sheet(self, img: Image.Image) -> list[Image.Image]:
        if not self.vars["anim"].get():
            return [img]
        try:
            fx = int(self.vars["frames_x"].get().strip())
            fy = int(self.vars["frames_y"].get().strip())
            fw = int(self.vars["frame_width"].get().strip())
            fh = int(self.vars["frame_height"].get().strip())
        except Exception:
            return [img]
        if fx <= 0 or fy <= 0 or fw <= 0 or fh <= 0:
            return [img]
        if img.width != fx * fw or img.height != fy * fh:
            return [img]
        frames: list[Image.Image] = []
        for y in range(fy):
            for x in range(fx):
                l = x * fw
                t = y * fh
                frames.append(img.crop((l, t, l + fw, t + fh)))
        return frames

    def _composite_bg_preview(self, src_img: Image.Image) -> Image.Image:
        bgp = self.vars["bg_metatile_png"].get().strip()
        if not bgp:
            return src_img
        p = Path(bgp)
        if not p.exists():
            return src_img
        try:
            bg_tile = Image.open(p)
            sidecar = _load_palette_sidecar(p)
            if sidecar and bg_tile.mode == "P":
                pal_raw: list[int] = []
                for r, g, b in sidecar:
                    pal_raw.extend([r, g, b])
                pal_raw += [0] * (768 - len(pal_raw))
                bg_tile = bg_tile.copy()
                bg_tile.putpalette(pal_raw)
            bg_tile = bg_tile.convert("RGBA")
            if bg_tile.size != (16, 16):
                return src_img
            fg = src_img.convert("RGBA")
            out = Image.new("RGBA", fg.size, (0, 0, 0, 0))
            for y in range(0, fg.height, 16):
                for x in range(0, fg.width, 16):
                    out.alpha_composite(bg_tile, (x, y))
            out.alpha_composite(fg, (0, 0))
            return out
        except Exception:
            return src_img

    def _preview_delay_ms(self) -> int:
        try:
            step = int(self.vars["preview_game_frames"].get().strip())
        except Exception:
            step = 12
        if step <= 0:
            step = 1
        # Game is ~60 FPS. step=12 -> ~200ms per preview frame.
        return max(16, int(round(1000.0 * step / 60.0)))

    def _set_input_preview(self, frames: list[Image.Image], label: str) -> None:
        self.preview_input_frames = [ImageTk.PhotoImage(self._fit_preview_image(fr)) for fr in frames]
        self.preview_input_idx = 0
        self.input_preview_info.config(text=label)
        if self.preview_input_frames:
            self.input_preview_label.configure(image=self.preview_input_frames[0])

    def _set_output_preview(self, frames: list[Image.Image], label: str) -> None:
        self.preview_output_frames = [ImageTk.PhotoImage(self._fit_preview_image(fr)) for fr in frames]
        self.preview_output_idx = 0
        self.output_preview_info.config(text=label)
        if self.preview_output_frames:
            self.output_preview_label.configure(image=self.preview_output_frames[0])

    def _tick_preview(self) -> None:
        if self.preview_input_frames:
            self.preview_input_idx = (self.preview_input_idx + 1) % len(self.preview_input_frames)
            self.input_preview_label.configure(image=self.preview_input_frames[self.preview_input_idx])
        if self.preview_output_frames:
            self.preview_output_idx = (self.preview_output_idx + 1) % len(self.preview_output_frames)
            self.output_preview_label.configure(image=self.preview_output_frames[self.preview_output_idx])
        if self.preview_input_frames or self.preview_output_frames:
            self.preview_timer = self.after(self._preview_delay_ms(), self._tick_preview)
        else:
            self.preview_timer = None

    def _ensure_preview_timer(self) -> None:
        if self.preview_timer is None and (self.preview_input_frames or self.preview_output_frames):
            self.preview_timer = self.after(150, self._tick_preview)

    def stop_preview(self) -> None:
        if self.preview_timer is not None:
            self.after_cancel(self.preview_timer)
            self.preview_timer = None

    def preview_input(self) -> None:
        p = self.vars["input"].get().strip()
        if not p:
            messagebox.showwarning("Input missing", "Choose an input PNG first.")
            return
        path = Path(p)
        if not path.exists():
            messagebox.showerror("Input not found", f"File does not exist:\n{p}")
            return
        try:
            img = Image.open(path)
            img = self._composite_bg_preview(img)
            frames = self._split_frames_from_sheet(img)
            self._set_input_preview(frames, f"{path.name} | {len(frames)} frame(s)")
            self._ensure_preview_timer()
        except Exception as e:
            messagebox.showerror("Preview error", str(e))

    def preview_output(self) -> None:
        out_dir = Path(self.vars["out_dir"].get().strip())
        anim_name = self.vars["anim_name"].get().strip() or "pool"
        if not out_dir.exists():
            messagebox.showwarning("Output missing", f"Output folder not found:\n{out_dir}")
            return
        preview_dir = out_dir / "anim_preview" / anim_name
        if preview_dir.exists():
            pngs = sorted(
                [p for p in preview_dir.glob("*.png") if p.stem.isdigit()],
                key=lambda p: int(p.stem),
            )
            if pngs:
                frames = [Image.open(p) for p in pngs]
                self._set_output_preview(frames, f"{preview_dir.name} | {len(frames)} frame(s)")
                self._ensure_preview_timer()
                return

        tiles_png = out_dir / "tiles.png"
        if tiles_png.exists():
            try:
                img = Image.open(tiles_png)
                self._set_output_preview([img], f"{tiles_png.name} | static")
                self._ensure_preview_timer()
                return
            except Exception as e:
                messagebox.showerror("Preview error", str(e))
                return
        messagebox.showwarning("No preview", "No output preview found. Generate first.")


def main() -> None:
    app = App()
    if app.cli_module is None and not CLI_TOOL.exists():
        messagebox.showerror("Missing tool", f"Missing tool: {CLI_TOOL}")
        raise SystemExit(1)
    app.mainloop()


if __name__ == "__main__":
    main()

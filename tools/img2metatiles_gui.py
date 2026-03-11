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
        self.geometry("980x700")
        self.cli_module = load_cli_module()

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
        row += 1

        ttk.Separator(frm).grid(row=row, column=0, columnspan=4, sticky="ew", pady=8)
        row += 1

        btns = ttk.Frame(frm)
        btns.grid(row=row, column=0, columnspan=4, sticky="w")
        ttk.Button(btns, text="Generate Tileset", command=self.run_generate).pack(side=tk.LEFT, padx=(0, 8))
        ttk.Button(btns, text="Open Output Folder", command=self.open_output).pack(side=tk.LEFT, padx=(0, 8))
        ttk.Button(btns, text="Save Config", command=self._save_cfg).pack(side=tk.LEFT, padx=(0, 8))
        row += 1

        ttk.Label(frm, text="Output Log").grid(row=row, column=0, columnspan=4, sticky="w", pady=(8, 4))
        row += 1

        self.log = tk.Text(frm, height=22, wrap="word")
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


def main() -> None:
    app = App()
    if app.cli_module is None and not CLI_TOOL.exists():
        messagebox.showerror("Missing tool", f"Missing tool: {CLI_TOOL}")
        raise SystemExit(1)
    app.mainloop()


if __name__ == "__main__":
    main()

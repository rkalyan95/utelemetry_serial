import json
import os
import subprocess
import tkinter as tk
from tkinter import filedialog, messagebox, simpledialog

CONFIG_FILE = os.path.join(os.path.dirname(__file__), "../telemetry_config_example.json")
SCHEMA_FILE = os.path.join(os.path.dirname(__file__), "../telemetry_schema.json")
CODEGEN_SCRIPT = os.path.join(os.path.dirname(__file__), "../telemetry_codegen.py")

DATA_TYPES = [
    "float32",
    "float64",
    "int8",
    "uint8",
    "int16",
    "uint16",
    "int32",
    "uint32",
    "string",
    "bytes",
    "struct",
]

class SensorEditor(simpledialog.Dialog):
    def __init__(self, parent, title, sensor=None):
        self.sensor = sensor or {}
        super().__init__(parent, title)

    def body(self, master):
        self.entries = {}
        required_fields = {"id", "name", "data_type"}
        fields = ["id", "name", "description", "data_type", "units", "count", "scale", "offset", "display_format", "default_value"]

        for row, field in enumerate(fields):
            label_text = field.replace("_", " ").capitalize()
            if field not in required_fields:
                label_text += " (optional)"
            label = tk.Label(master, text=label_text + ":")
            label.grid(row=row, column=0, sticky="w", padx=4, pady=2)

            if field == "data_type":
                self.entries[field] = tk.StringVar(value=self.sensor.get(field, ""))
                dropdown = tk.OptionMenu(master, self.entries[field], *DATA_TYPES, command=self.refresh)
                dropdown.grid(row=row, column=1, sticky="ew", padx=4, pady=2)
            else:
                entry = tk.Entry(master)
                entry.grid(row=row, column=1, sticky="ew", padx=4, pady=2)
                entry.insert(0, str(self.sensor.get(field, "")))
                self.entries[field] = entry

            if field == "count":
                self.count_label = label
                self.count_entry = entry
            if field == "default_value":
                self.default_label = label
                self.default_entry = entry

        master.grid_columnconfigure(1, weight=1)

        self.struct_button = tk.Button(master, text="Edit struct fields...", command=self.edit_struct_fields)
        self.struct_button.grid(row=len(fields), column=0, columnspan=2, pady=6)
        self.struct_status = tk.Label(master, text="", fg="blue")
        self.struct_status.grid(row=len(fields) + 1, column=0, columnspan=2, sticky="w", padx=4)
        self.struct_fields = self.sensor.get("struct_fields", [])
        self.refresh()
        return self.entries["id"]

    def refresh(self, *_):
        dtype = self.entries["data_type"].get()
        self.struct_button.config(state="normal" if dtype == "struct" else "disabled")
        if dtype == "bytes":
            self.count_label.grid()
            self.count_entry.grid()
        else:
            self.count_label.grid_remove()
            self.count_entry.grid_remove()
        if dtype == "string":
            self.default_label.grid()
            self.default_entry.grid()
        else:
            self.default_label.grid_remove()
            self.default_entry.grid_remove()

        if dtype == "struct":
            if self.struct_fields:
                self.struct_status.config(text=f"Struct fields: {len(self.struct_fields)} item(s). Click Edit to modify.")
            else:
                self.struct_status.config(text="Struct type requires at least one field. Click Edit struct fields to add.")
        else:
            self.struct_status.config(text="")

    def edit_struct_fields(self):
        dialog = StructFieldEditor(self, "Struct fields", self.struct_fields)
        if hasattr(dialog, "result") and dialog.result is not None:
            self.struct_fields = dialog.result
        self.refresh()

    def validate(self):
        try:
            sensor_id = int(self.entries["id"].get())
        except ValueError:
            messagebox.showwarning("Validation error", "ID must be an integer.")
            return False

        name = self.entries["name"].get().strip()
        if not name:
            messagebox.showwarning("Validation error", "Name cannot be empty.")
            return False

        data_type = self.entries["data_type"].get()
        if data_type not in DATA_TYPES:
            messagebox.showwarning("Validation error", "Select a valid data type.")
            return False

        if data_type == "struct" and not self.struct_fields:
            messagebox.showwarning("Validation error", "Struct must have at least one field.")
            return False

        if data_type == "bytes":
            count = self.entries["count"].get().strip()
            if not count.isdigit() or int(count) < 1:
                messagebox.showwarning("Validation error", "Bytes data type requires a positive count.")
                return False

        return True

    def apply(self):
        self.result = {
            "id": int(self.entries["id"].get()),
            "name": self.entries["name"].get().strip(),
            "description": self.entries["description"].get().strip(),
            "data_type": self.entries["data_type"].get(),
        }

        for key in ["units", "count", "scale", "offset", "display_format", "default_value"]:
            value = self.entries[key].get().strip()
            if value:
                if key in ("count", "id"):
                    self.result[key] = int(value)
                elif key in ("scale", "offset"):
                    self.result[key] = float(value)
                else:
                    self.result[key] = value

        if self.entries["data_type"].get() == "struct":
            self.result["struct_fields"] = self.struct_fields


class StructFieldEditor(simpledialog.Dialog):
    def __init__(self, parent, title, fields):
        self.fields = list(fields)
        super().__init__(parent, title)

    def body(self, master):
        self.listbox = tk.Listbox(master, height=8, width=50)
        self.listbox.grid(row=0, column=0, columnspan=3, pady=4, padx=4, sticky="nsew")
        self.refresh_list()

        add_btn = tk.Button(master, text="Add field", command=self.add_field)
        add_btn.grid(row=1, column=0, pady=4, padx=4)
        edit_btn = tk.Button(master, text="Edit field", command=self.edit_field)
        edit_btn.grid(row=1, column=1, pady=4, padx=4)
        remove_btn = tk.Button(master, text="Remove field", command=self.remove_field)
        remove_btn.grid(row=1, column=2, pady=4, padx=4)

        master.grid_columnconfigure(0, weight=1)
        return self.listbox

    def refresh_list(self):
        self.listbox.delete(0, tk.END)
        for idx, field in enumerate(self.fields, start=1):
            self.listbox.insert(tk.END, f"{idx}. {field['name']} ({field['data_type']})")

    def add_field(self):
        field = self.edit_struct_field({"name": "", "data_type": "uint8", "count": 1})
        if field:
            self.fields.append(field)
            self.refresh_list()

    def edit_field(self):
        selection = self.listbox.curselection()
        if not selection:
            return
        index = selection[0]
        field = self.fields[index]
        updated = self.edit_struct_field(field)
        if updated:
            self.fields[index] = updated
            self.refresh_list()

    def remove_field(self):
        selection = self.listbox.curselection()
        if not selection:
            return
        del self.fields[selection[0]]
        self.refresh_list()

    def edit_struct_field(self, field):
        dialog = StructFieldDialog(self, "Struct field", field)
        return dialog.result

    def apply(self):
        self.result = self.fields


class StructFieldDialog(simpledialog.Dialog):
    def __init__(self, parent, title, field):
        self.field = field
        super().__init__(parent, title)

    def body(self, master):
        tk.Label(master, text="Name:").grid(row=0, column=0, sticky="w", padx=4, pady=2)
        self.name_entry = tk.Entry(master)
        self.name_entry.grid(row=0, column=1, sticky="ew", padx=4, pady=2)
        self.name_entry.insert(0, self.field.get("name", ""))

        tk.Label(master, text="Data type:").grid(row=1, column=0, sticky="w", padx=4, pady=2)
        self.dtype_var = tk.StringVar(value=self.field.get("data_type", "uint8"))
        tk.OptionMenu(master, self.dtype_var, *DATA_TYPES).grid(row=1, column=1, sticky="ew", padx=4, pady=2)

        tk.Label(master, text="Count:").grid(row=2, column=0, sticky="w", padx=4, pady=2)
        self.count_entry = tk.Entry(master)
        self.count_entry.grid(row=2, column=1, sticky="ew", padx=4, pady=2)
        self.count_entry.insert(0, str(self.field.get("count", "")))

        master.grid_columnconfigure(1, weight=1)
        return self.name_entry

    def validate(self):
        if not self.name_entry.get().strip():
            messagebox.showwarning("Validation error", "Field name cannot be empty.")
            return False
        if self.count_entry.get().strip():
            try:
                if int(self.count_entry.get().strip()) < 1:
                    raise ValueError
            except ValueError:
                messagebox.showwarning("Validation error", "Count must be a positive integer.")
                return False
        return True

    def apply(self):
        self.result = {
            "name": self.name_entry.get().strip(),
            "data_type": self.dtype_var.get(),
        }
        count = self.count_entry.get().strip()
        if count:
            self.result["count"] = int(count)


def load_config(path):
    try:
        with open(path, "r", encoding="utf-8") as handle:
            return json.load(handle)
    except Exception as exc:
        messagebox.showerror("Load error", f"Unable to open config: {exc}")
        return None


def save_config(config, path):
    try:
        with open(path, "w", encoding="utf-8") as handle:
            json.dump(config, handle, indent=2)
            handle.write("\n")
        return True
    except Exception as exc:
        messagebox.showerror("Save error", f"Unable to save config: {exc}")
        return False


def run_codegen(config_path, out_dir):
    try:
        result = subprocess.run(["py", CODEGEN_SCRIPT, "generate", config_path, "--out-dir", out_dir], capture_output=True, text=True)
        if result.returncode != 0:
            messagebox.showerror("Generate error", result.stderr or result.stdout)
            return False
        messagebox.showinfo("Generate complete", result.stdout)
        return True
    except Exception as exc:
        messagebox.showerror("Generate error", str(exc))
        return False


def build_main_window(root):
    root.title("Telemetry Config UI")
    root.geometry("900x600")

    top_frame = tk.Frame(root, padx=10, pady=10)
    top_frame.pack(fill="x")

    tk.Button(top_frame, text="Load JSON", command=lambda: load_json(root)).pack(side="left", padx=4)
    tk.Button(top_frame, text="Save JSON", command=lambda: save_json(root)).pack(side="left", padx=4)
    tk.Button(top_frame, text="Generate C Files", command=lambda: generate_files(root)).pack(side="left", padx=4)
    tk.Button(top_frame, text="Add Sensor", command=lambda: add_sensor(root)).pack(side="left", padx=4)
    tk.Button(top_frame, text="Edit Sensor", command=lambda: edit_sensor(root)).pack(side="left", padx=4)
    tk.Button(top_frame, text="Remove Sensor", command=lambda: remove_sensor(root)).pack(side="left", padx=4)

    list_frame = tk.Frame(root, padx=10, pady=10)
    list_frame.pack(fill="both", expand=True)

    tk.Label(list_frame, text="Sensors:").pack(anchor="w")
    sensor_list = tk.Listbox(list_frame, name="sensor_list", selectmode="browse")
    sensor_list.pack(fill="both", expand=True, side="left")

    scrollbar = tk.Scrollbar(list_frame, orient="vertical", command=sensor_list.yview)
    scrollbar.pack(side="right", fill="y")
    sensor_list.config(yscrollcommand=scrollbar.set)

    root.config(menu=build_menu(root))
    root.sensor_list = sensor_list
    root.config_data = None

    return root


def build_menu(root):
    menu = tk.Menu(root)
    file_menu = tk.Menu(menu, tearoff=0)
    file_menu.add_command(label="Load JSON", command=lambda: load_json(root))
    file_menu.add_command(label="Save JSON", command=lambda: save_json(root))
    file_menu.add_separator()
    file_menu.add_command(label="Exit", command=root.quit)
    menu.add_cascade(label="File", menu=file_menu)
    help_menu = tk.Menu(menu, tearoff=0)
    help_menu.add_command(label="About", command=lambda: messagebox.showinfo("About", "Telemetry Config UI\nBuilt for telemetry JSON editing."))
    menu.add_cascade(label="Help", menu=help_menu)
    return menu


def refresh_sensor_list(root):
    root.sensor_list.delete(0, tk.END)
    if not root.config_data:
        return
    for sensor in root.config_data.get("sensors", []):
        display = f"{sensor['id']}: {sensor['name']} ({sensor['data_type']})"
        if sensor.get("description"):
            display += f" - {sensor['description']}"
        root.sensor_list.insert(tk.END, display)


def load_json(root):
    path = filedialog.askopenfilename(title="Open telemetry JSON", filetypes=[("JSON files", "*.json"), ("All files", "*")], initialdir=os.path.dirname(CONFIG_FILE))
    if not path:
        return
    config = load_config(path)
    if config is not None:
        root.config_data = config
        root.config_path = path
        refresh_sensor_list(root)


def save_json(root):
    if not root.config_data:
        messagebox.showwarning("No config", "No telemetry configuration loaded.")
        return
    path = filedialog.asksaveasfilename(title="Save telemetry JSON", defaultextension=".json", filetypes=[("JSON files", "*.json")], initialfile=os.path.basename(root.config_path or CONFIG_FILE), initialdir=os.path.dirname(root.config_path or CONFIG_FILE))
    if not path:
        return
    if save_config(root.config_data, path):
        root.config_path = path
        messagebox.showinfo("Saved", f"Configuration saved to {path}")


def add_sensor(root):
    editor = SensorEditor(root, "Add sensor")
    if editor.result:
        root.config_data.setdefault("sensors", []).append(editor.result)
        refresh_sensor_list(root)


def edit_sensor(root):
    if not root.config_data:
        messagebox.showwarning("No config", "Load a configuration before editing.")
        return
    index = root.sensor_list.curselection()
    if not index:
        messagebox.showwarning("Select sensor", "Choose a sensor to edit.")
        return
    sensor = root.config_data["sensors"][index[0]]
    editor = SensorEditor(root, "Edit sensor", sensor=sensor)
    if editor.result:
        root.config_data["sensors"][index[0]] = editor.result
        refresh_sensor_list(root)


def remove_sensor(root):
    if not root.config_data:
        return
    index = root.sensor_list.curselection()
    if not index:
        messagebox.showwarning("Select sensor", "Choose a sensor to remove.")
        return
    del root.config_data["sensors"][index[0]]
    refresh_sensor_list(root)


def generate_files(root):
    if not root.config_data:
        messagebox.showwarning("No config", "Load or create a configuration first.")
        return
    path = filedialog.asksaveasfilename(title="Save telemetry JSON for generation", defaultextension=".json", filetypes=[("JSON files", "*.json")], initialfile=os.path.basename(root.config_path or CONFIG_FILE), initialdir=os.path.dirname(root.config_path or CONFIG_FILE))
    if not path:
        return
    if not save_config(root.config_data, path):
        return
    output_dir = filedialog.askdirectory(title="Choose output folder", initialdir=os.path.join(os.path.dirname(__file__), "..", "generatedconfigs"))
    if not output_dir:
        return
    run_codegen(path, output_dir)


def main():
    root = tk.Tk()
    build_main_window(root)
    if os.path.exists(CONFIG_FILE):
        config = load_config(CONFIG_FILE)
        if config is not None:
            root.config_data = config
            root.config_path = CONFIG_FILE
            refresh_sensor_list(root)
    root.mainloop()

if __name__ == "__main__":
    main()

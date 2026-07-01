import tkinter as tk
from tkinter import ttk
import serial
import struct
import threading
import time

def calculate_crc16(data: bytes) -> int:
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 1: crc = (crc >> 1) ^ 0xA001
            else:         crc >>= 1
    return crc & 0xFFFF

class TelemetryApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Dynamic Handshake Control Console")
        self.root.geometry("820x660")
        
        # State Control Variables
        self.device_registry = {}  # Format: { id: {"name": str, "format": str} }
        self.ser = None
        self.serial_connected = False
        self.handshake_done = False
        self.kill_listen_thread = False
        self.listen_thread = None  

        self.create_widgets()
        self.attempt_serial_connection()

    def create_widgets(self):
        # Hardware Link Controls Frame
        input_frame = ttk.LabelFrame(self.root, text=" Hardware Link Controls ", padding=10)
        input_frame.pack(fill="x", padx=15, pady=5)

        self.btn_disconnect = ttk.Button(input_frame, text="🛑 Disconnect Port", command=self.disconnect_serial)
        self.btn_disconnect.grid(row=0, column=0, padx=5, pady=5)

        self.btn_restart = ttk.Button(input_frame, text="🔄 Restart & Sync Loop", command=self.restart_and_sync)
        self.btn_restart.grid(row=0, column=1, padx=5, pady=5)

        # Dropdown for Compiler Alignment Choice
        ttk.Label(input_frame, text="Struct Alignment Mode:").grid(row=0, column=2, padx=(20, 5), pady=5)
        self.alignment_var = tk.StringVar(value="32-bit ARM (Cortex-M)")
        self.combo_alignment = ttk.Combobox(
            input_frame, 
            textvariable=self.alignment_var, 
            values=["Standard Tightly Packed (=)", "32-bit ARM (Cortex-M)"],
            state="readonly",
            width=24
        )
        self.combo_alignment.grid(row=0, column=3, padx=5, pady=5)

        self.status_label = ttk.Label(self.root, text="Initializing...", font=("Arial", 10, "bold"))
        self.status_label.pack(anchor="w", padx=15, pady=2)

        # Device Tracking Grid
        table_frame = ttk.LabelFrame(self.root, text=" Devices Registered by Controller ", padding=10)
        table_frame.pack(fill="x", padx=15, pady=5)

        self.tree = ttk.Treeview(table_frame, columns=("ID", "Name", "Format", "Handshake"), show="headings", height=4)
        self.tree.heading("ID", text="Device ID")
        self.tree.heading("Name", text="Device Name")
        self.tree.heading("Format", text="Data Format Layout")
        self.tree.heading("Handshake", text="Status Confirmation")
        
        self.tree.column("ID", width=80, anchor="center")
        self.tree.column("Name", width=120, anchor="center")
        self.tree.column("Format", width=140, anchor="center")
        self.tree.column("Handshake", width=230, anchor="center")
        self.tree.pack(fill="x")

        # Live Text Feed
        stream_frame = ttk.LabelFrame(self.root, text=" Live Telemetry Data Feed (From Main Loop) ", padding=10)
        stream_frame.pack(fill="both", expand=True, padx=15, pady=10)

        self.log_text = tk.Text(stream_frame, wrap="word", background="#1E1E1E", foreground="#FFFFFF", font=("Consolas", 10))
        self.log_text.pack(fill="both", expand=True, side="left")
        
        scrollbar = ttk.Scrollbar(stream_frame, command=self.log_text.yview)
        scrollbar.pack(fill="y", side="right")
        self.log_text.config(yscrollcommand=scrollbar.set)

    def log_message(self, message):
        self.log_text.insert(tk.END, message + "\n")
        self.log_text.see(tk.END)

    def attempt_serial_connection(self):
        port_target = 'COM12'
        self.status_label.config(text=f"Connecting to {port_target}...", foreground="orange")
        self.handshake_done = False
        self.kill_listen_thread = False
        self.root.update_idletasks()
        
        try:
            self.ser = serial.Serial(
                port=port_target, baudrate=115200,
                bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE, timeout=0.05
            )
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            self.serial_connected = True
            self.status_label.config(text=f"CONNECTED to {port_target}! Syncing...", foreground="green")
            
            self.listen_thread = threading.Thread(target=self.serial_listen_loop, daemon=True)
            self.listen_thread.start()
                
        except Exception as e:
            self.serial_connected = False
            self.status_label.config(text=f"❌ UART LINK ERROR: {str(e).split('\n')[0]}", foreground="red")

    def disconnect_serial(self):
        self.kill_listen_thread = True
        self.serial_connected = False
        self.handshake_done = False
        if self.ser and self.ser.is_open:
            try:
                self.ser.close()
            except Exception:
                pass
        self.status_label.config(text="DISCONNECTED: Port closed. System idle.", foreground="red")

    def restart_and_sync(self):
        self.disconnect_serial()
        
        if self.listen_thread and self.listen_thread.is_alive():
            self.listen_thread.join(timeout=0.5) 
        
        self.device_registry.clear()
        for item in self.tree.get_children():
            self.tree.delete(item)
            
        self.log_text.insert(tk.END, "\n--- SYSTEM RESET: Commencing Handshake Sync ---\n")
        self.log_text.see(tk.END)
        
        self.attempt_serial_connection()

    def update_ui_table(self, dev_id, name, format_str, status):
        for item in self.tree.get_children():
            if self.tree.item(item)['values'][0] == dev_id:
                self.tree.delete(item)
        self.tree.insert("", "end", values=(dev_id, name, format_str, status))

    def serial_listen_loop(self):
        raw_data = bytearray()
        HANDSHAKE_SYNCH = 0x55
        TELEMETRY_SYNCH = 0xAA
        last_poll_time = 0

        while self.serial_connected and not self.kill_listen_thread:
            try:
                if not self.handshake_done and (time.time() - last_poll_time > 0.2):
                    if self.ser and self.ser.is_open:
                        self.ser.write(b"\xAA")
                        last_poll_time = time.time()

                waiting = self.ser.in_waiting if (self.ser and self.ser.is_open) else 0
                if waiting > 0:
                    raw_data.extend(self.ser.read(waiting))
                else:
                    time.sleep(0.01)
                    continue

                while len(raw_data) > 0 and not self.kill_listen_thread:
                    h_idx = raw_data.index(HANDSHAKE_SYNCH) if HANDSHAKE_SYNCH in raw_data else 999999
                    t_idx = raw_data.index(TELEMETRY_SYNCH) if TELEMETRY_SYNCH in raw_data else 999999

                    if h_idx == 999999 and t_idx == 999999:
                        raw_data.clear()
                        break

                    # Process Handshake Registration Packet (0x55)
                    if h_idx < t_idx:
                        self.handshake_done = True  
                        sync_idx = h_idx
                        if b"\r\n" not in raw_data[sync_idx:]: break 
                            
                        end_idx = raw_data.index(b"\r\n", sync_idx)
                        packet = raw_data[sync_idx:end_idx]
                        
                        if len(packet) < 4:
                            del raw_data[:sync_idx + 1]
                            continue

                        dev_id = packet[1]
                        payload_bytes = packet[2:-2]
                        raw_string = payload_bytes.decode('ascii', errors='ignore').strip()
                        received_crc, = struct.unpack("<H", packet[-2:])

                        crc_data_block = bytes([HANDSHAKE_SYNCH, dev_id]) + payload_bytes
                        if calculate_crc16(crc_data_block) == received_crc:
                            
                            dev_name = raw_string
                            fmt_str = "None"
                            
                            if ":" in raw_string:
                                parts = raw_string.split(":", 1)
                                dev_name = parts[0]
                                raw_fmt = parts[1]
                                
                                # Strip explicit user prefixes if provided; otherwise store pure layout characters
                                if raw_fmt and raw_fmt[0] in ('@', '=', '<', '>', '!'):
                                    fmt_str = raw_fmt[1:]
                                else:
                                    fmt_str = raw_fmt
                            
                            self.device_registry[dev_id] = {"name": dev_name, "format": fmt_str}
                            
                            self.ser.write(struct.pack("<H", received_crc))
                            time.sleep(0.03)
                            mcu_ack = self.ser.read(2)
                            
                            status_str = "Linked & Verified (ACK Success)" if mcu_ack == b"\xAA\x55" else "Linked (Pacing Window)"
                            self.root.after(0, self.update_ui_table, dev_id, dev_name, fmt_str, status_str)
                        else:
                            self.root.after(0, self.update_ui_table, dev_id, "Unknown", "Unknown", "PC Checksum Error")
                            
                        del raw_data[:end_idx + 2]

                    # Process Live Telemetry Packet (0xAA)
                    else:
                        sync_idx = t_idx
                        if len(raw_data) - sync_idx < 4: break

                        info_type = raw_data[sync_idx + 1] 
                        msg_id = raw_data[sync_idx + 2]    
                        msg_len = raw_data[sync_idx + 3]   
                        
                        total_size = 4 + msg_len + 2 

                        if len(raw_data) - sync_idx < total_size: break

                        p_start = sync_idx + 4
                        p_end = p_start + msg_len
                        payload = bytes(raw_data[p_start:p_end])
                        received_crc, = struct.unpack("<H", raw_data[p_end:p_end+2])

                        crc_block = bytes([msg_id, msg_len, info_type]) + payload
                        
                        if calculate_crc16(crc_block) == received_crc:
                            dev_profile = self.device_registry.get(msg_id, {"name": f"Device ID: {msg_id}", "format": "None"})
                            dev_name = dev_profile["name"]
                            timestamp = time.strftime("%H:%M:%S")
                            parsed_output = ""
                            
                            if info_type == 0x10: 
                                try:
                                    raw_fmt = dev_profile["format"]
                                    if raw_fmt != "None":
                                        mode = self.alignment_var.get()
                                        
                                        if mode == "32-bit ARM (Cortex-M)":
                                            # Enforce Little-Endian sizing but absorb structural padding automatically
                                            fmt_string = "<" + raw_fmt
                                            expected_size = struct.calcsize(fmt_string)
                                            payload_len = len(payload)
                                            
                                            if payload_len > expected_size:
                                                padding_bytes = payload_len - expected_size
                                                fmt_string += ("x" * padding_bytes)
                                        else:
                                            # Force standard strict packed rules
                                            fmt_string = "<" + raw_fmt
                                            
                                        unpacked_data = struct.unpack(fmt_string, payload[:struct.calcsize(fmt_string)])
                                        val_strings = [f"{v:.3f}" if isinstance(v, float) else str(v) for v in unpacked_data]
                                        parsed_output = f"Struct Data: ({', '.join(val_strings)})"
                                    else:
                                        parsed_output = f"Unregistered Struct Data Hex: {payload.hex().upper()}"
                                except Exception as err:
                                    parsed_output = f"Alignment Unpack Error: {str(err)}"
                                    
                            elif info_type == 0x01: # FLOAT
                                if len(payload) == 4:
                                    val, = struct.unpack("<f", payload)
                                    parsed_output = f"Float: {val:.3f}"
                            elif info_type == 0x05: # STRING
                                parsed_output = f"Text: {payload.decode('ascii', errors='ignore').strip('\x00')}"
                            elif info_type == 0x03: # INT32
                                if len(payload) == 4:
                                    val, = struct.unpack("<i", payload)
                                    parsed_output = f"Int32: {val}"
                            elif info_type == 0x06: # INT16
                                if len(payload) == 2:
                                    val, = struct.unpack("<h", payload)
                                    parsed_output = f"Int16: {val}"
                            elif info_type == 0x04: # INT8
                                if len(payload) == 1:
                                    val, = struct.unpack("<b", payload)
                                    parsed_output = f"Int8: {val}"
                            elif info_type == 0x02: # BYTE ARRAY
                                parsed_output = f"Byte Array: [{', '.join(f'0x{b:02X}' for b in payload)}]"
                            else: 
                                parsed_output = f"Raw Hex [Type 0x{info_type:02X}]: {payload.hex().upper()}"

                            self.root.after(0, self.log_message, f"[{timestamp}] Node '{dev_name}' -> {parsed_output}")
                        
                        del raw_data[:sync_idx + total_size]

            except Exception:
                self.serial_connected = False
                break

if __name__ == "__main__":
    root = tk.Tk()
    app = TelemetryApp(root)
    root.mainloop()

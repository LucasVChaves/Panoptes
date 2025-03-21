import requests
import cv2 
import numpy as np
import tkinter as tk
from PIL import Image, ImageTk  
from threading import Thread, Event
from queue import Queue, Empty, Full
import time

class StreamViewer:
    def __init__(self, url, max_fps=30):
        self.url = url
        self.queue = Queue(maxsize=3)  
        self.root = None
        self.panel = None
        self.stop_event = Event()
        self.last_frame_time = 0
        self.max_fps = max_fps
        self.frame_interval = 1 / max_fps

    def fetch_stream(self):
        session = requests.Session()
        try:
            response = session.get(self.url, stream=True, timeout=2)
            if response.status_code != 200:
                print("Erro de conexão:", response.status_code)
                return

            boundary = self.get_boundary(response.headers['Content-Type'])
            buffer = b''
            
            while not self.stop_event.is_set():
                data = response.raw.read(65536)
                if not data:
                    break
                
                buffer += data
                while True:
                    a = buffer.find(boundary)
                    if a == -1:
                        break
                    
                    b = buffer.find(boundary, a + len(boundary))
                    if b == -1:
                        break
                    
                    frame = buffer[a + len(boundary):b]
                    buffer = buffer[b:]
                    
                    header, image_data = frame.split(b'\r\n\r\n', 1)
                    if image_data.startswith(b'\xff\xd8'):
                        try:
                            self.queue.put_nowait(image_data)
                        except Full:
                            pass
                
        except Exception as e:
            print("Erro no stream:", e)
        finally:
            session.close()

    def get_boundary(self, content_type):
        boundary = content_type.split('boundary=')[-1].strip()
        return b'--' + boundary.encode()

    def update_display(self):
        try:
            image_data = self.queue.get_nowait()
            
            nparr = np.frombuffer(image_data, np.uint8)
            frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            
            img = Image.fromarray(frame)
            imgtk = ImageTk.PhotoImage(image=img)
            
            if self.panel is None:
                self.panel = tk.Label(self.root, image=imgtk)
                self.panel.image = imgtk
                self.panel.pack(fill='both', expand=True)
            else:
                self.panel.configure(image=imgtk)
                self.panel.image = imgtk
            
            now = time.time()
            elapsed = now - self.last_frame_time
            if elapsed < self.frame_interval:
                time.sleep(self.frame_interval - elapsed)
            self.last_frame_time = time.time()
            
        except Empty:
            pass
        except Exception as e:
            print("Erro no display:", e)
        
        if not self.stop_event.is_set():
            self.root.after(1, self.update_display)

    def start(self):
        self.root = tk.Tk()
        self.root.geometry("800x600")
        self.root.title("Panoptes - POF Intensity Tool")
        
        net_thread = Thread(target=self.fetch_stream)
        net_thread.daemon = True
        net_thread.start()
        
        self.update_display()
        self.root.mainloop()

    def on_close(self):
        self.stop_event.set()
        self.root.destroy()

if __name__ == "__main__":
    # TODO: Perguntar IP quando iniciar o programa
    ESP_IP = "192.168.0.11"
    viewer = StreamViewer(f"http://{ESP_IP}", max_fps=16)
    viewer.start()

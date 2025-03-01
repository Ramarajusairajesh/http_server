# HTTP Server (C++ & Python) 🚀

A simple **HTTP server** implemented in both **C++ and Python**, serving static files and logging client requests.

---

## 📌 Features
✅ **C++ & Python Implementations**  
✅ **Serves static files from a predefined directory**  
✅ **Logs client IP, timestamp, and requested path**  
✅ **Custom 404 page (`404.html`)**  
✅ **Multi-client support**  

---

## 🛠️ Installation & Usage

### **1️⃣ Clone the Repository**
```sh
git clone https://github.com/Ramarajusairajesh/http_server.git
cd http_server
```
2️⃣ Running the C++ Server
```sh
Copy
Edit
cd cpp
g++ http_server.cpp -o server
./server
Now, the C++ server is running on http://localhost:8080 🎉
```

3️⃣ Running the Python Server
```sh
Copy
Edit
cd python
python3 server.py
Now, the Python server is running on http://127.0.0.1:8080 🎉
```

📜 Logging Format (logs.txt)
Each request is logged as:

```css
Copy
Edit
[YYYY-MM-DD HH:MM:SS.mmm] [Client_IP] Requested: /path
Example:
```
```less
Copy
Edit
[2025-02-28 14:53:12.123] [192.168.1.42] Requested: /
[2025-02-28 14:53:15.456] [192.168.1.50] Requested: /home
[2025-02-28 14:53:18.789] [10.0.0.25] Requested: /auth
```
📂 Project Structure
bash
Copy
Edit
http_server/
```│── cpp/              # C++ implementation
│   ├── http_server.cpp
│   ├── logs.txt
│   ├── templates/    # Static files for C++ server
│   │   ├── index.html
│   │   ├── 404.html
│── python/           # Python implementation
│   ├── server.py
│   ├── logs.txt
│   ├── browny-v1.0/  # Static files for Python server
│   │   ├── index.html
│   │   ├── 404.html
│── README.md         # Project documentation
```
TODO: HTTPS

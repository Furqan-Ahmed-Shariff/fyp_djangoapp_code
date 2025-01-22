# Django Backend for Smart Bin Project

This repository contains the Django backend for the **Smart Bin Project**, which automates waste segregation into dry and wet categories. The backend consists of two apps, `receive_img` and `image_val`, both implementing APIs with **Django Rest Framework (DRF)**. The project uses **JSON Web Token (JWT)** authentication for staff users and **API key authentication** for the ESP32 device.

---

## 📂 Apps Overview

### 1. **receive_img**
This app handles:
- Accepting waste images from the ESP32 device.
- Authenticating the ESP32 device using API key authentication.
- Classifying waste into "Dry" or "Wet" using the **YOLOv11 model**.
- Storing image data in a database (`Wastes`).

### 2. **image_val**
This app provides:
- Verification of previously classified images by staff users.
- Fetching the next image from the database for staff verification.

---

## ⚙️ How It Works

### **Workflow**

1. **ESP32 Device Interaction (receive_img):**
   - The ESP32 device sends a waste image to the `receive_img` API.
   - API authenticates the device using an API key.
   - The image is passed through the YOLOv11 model to classify it as "Dry" or "Wet".
   - The classification result determines the waste bin direction:
     - **Dry Waste:** HTTP response `"right"`
     - **Wet Waste:** HTTP response `"left"`
   - The image and its classification result are stored in the SQLite database (`Wastes` table).

2. **Staff Verification (image_val):**
   - Staff users log in using **JWT authentication**.
   - Staff can verify the classification of previously processed images.
   - The API fetches and returns the next unverified image for review.

---

## 📄 Database Design

**Table: Wastes**
- `category` (string): "Dry" or "Wet"
- `photo` (image): Uploaded waste image
- `validated` (boolean): Verification status of the image (default: `False`)

---

## 🔑 Authentication

### **ESP32 Authentication**
- Uses **API Key Authentication**.
- Only authorized devices with valid API keys can access the `receive_img` API.

### **Staff User Authentication**
- Uses **JWT Authentication** for secure login and access.
- Only authenticated staff users can verify waste classifications.

---

## 📂 API Endpoints

### **App: receive_img**
1. **POST /api/receive_img/upload/**
   - **Purpose:** Accepts an image from the ESP32 device.
   - **Authentication:** API Key.
   - **Request Body:**
     ```json
     {
       "api_key": "device_api_key",
       "image": "<image_file>"
     }
     ```
   - **Response:**
     - `"right"` for dry waste.
     - `"left"` for wet waste.

---

### **App: image_val**
2. **POST /api/image_val/verify/**
   - **Purpose:** Verifies the classification of the previous image and fetches the next unverified image.
   - **Authentication:** JWT (for staff users).
   - **Request Body:**
     ```json
     {
       "image_id": "<image_id>",
       "verification_status": true/false
     }
     ```
   - **Response:** The next unverified image (if available).
     ```json
     {
       "image_id": "<next_image_id>",
       "photo_url": "<url_of_next_image>"
     }
     ```

---

## 🛠️ Setup and Installation

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/your-username/smart-bin-django.git
   cd smart-bin-django

from ultralytics import YOLO
import cv2
import time

# Load model
model = YOLO("best.pt")

# Firebase
import firebase_admin
from firebase_admin import credentials, db

cred = credentials.Certificate("serviceAccountKey.json")

firebase_admin.initialize_app(cred, {
    'databaseURL': "https://carparking-56a23-default-rtdb.firebaseio.com/"
})

ref = db.reference("parking")

# Parking slots
parking_slots = {
    "A": (173, 42,322, 156),
    "B": (344, 37,490, 174),
    "C":  (508, 56,621, 196)
}

# Webcam
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)

if not cap.isOpened():
    print("Camera not opening")
    exit()

last_update = 0  # for Firebase delay

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.resize(frame, (640, 480))

    results = model(frame, conf=0.25)

    empty_count = 0
    slot_status = {}

    # Loop through slots (FIXED)
    for name, slot in parking_slots.items():
        x1, y1, x2, y2 = slot
        occupied = False

        for box in results[0].boxes:
            bx1, by1, bx2, by2 = map(int, box.xyxy[0])

            if (bx1 < x2 and bx2 > x1 and by1 < y2 and by2 > y1):
                occupied = True
                break

        if occupied:
            color = (0, 0, 255)
            slot_status[name] = "occupied"
        else:
            color = (0, 255, 0)
            slot_status[name] = "empty"
            empty_count += 1

        cv2.rectangle(frame, (x1, y1), (x2, y2), color, 2)

    # 🔥 Send data to Firebase (every 2 sec)
    if time.time() - last_update > 2:
        ref.set(
            {
            "total_slots": len(parking_slots),
            "empty_slots": empty_count,
            "occupied_slots": len(parking_slots) - empty_count,
            "slots": slot_status
            }
        )
        last_update = time.time()

    # Display
    cv2.putText(frame, f"Empty: {empty_count}", (20, 40),
                cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

    cv2.imshow("Parking Detection", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
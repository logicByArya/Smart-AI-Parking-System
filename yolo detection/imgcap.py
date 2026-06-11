import cv2

cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)

if not cap.isOpened():
    print("Camera not opening")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        break

    cv2.imshow("Press 's' to save image", frame)

    key = cv2.waitKey(1)

    # Press 's' to save image
    if key & 0xFF == ord('s'):
        cv2.imwrite("parking.jpg", frame)
        print("Image saved as parking.jpg")
        break

    # Press 'q' to quit without saving
    if key & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
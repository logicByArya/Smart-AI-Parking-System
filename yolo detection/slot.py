import cv2

points = []

def mouse_click(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        print(f"{x}, {y}")
        points.append((x, y))

img = cv2.imread("parking.jpg")

cv2.namedWindow("Image")
cv2.setMouseCallback("Image", mouse_click)

while True:
    cv2.imshow("Image", img)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break


cv2.destroyAllWindows()
import cv2

img = cv2.imread("parking.jpg")

for slot in parking_slots:
    x1, y1, x2, y2 = slot
    cv2.rectangle(img, (x1, y1), (x2, y2), (255, 0, 0), 2)

cv2.imshow("Slots", img)
cv2.waitKey(0)
cv2.destroyAllWindows()
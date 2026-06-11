import cv2

# Define slots FIRST
parking_slots = [
    (173, 42,322, 156),
    (344, 37,490, 174),
    (508, 56,621, 196)

]

img = cv2.imread("parking.jpg")

if img is None:
    print("Error: Image not found")
    exit()

for slot in parking_slots:
    x1, y1, x2, y2 = slot
    cv2.rectangle(img, (x1, y1), (x2, y2), (255, 0, 0), 2)

cv2.imshow("Slots", img)
cv2.waitKey(0)
cv2.destroyAllWindows()

# See https://www.geeksforgeeks.org/python-opencv-roberts-edge-detection/
# https://stackoverflow.com/questions/66417094/implement-roberts-edge-operator-for-edge-detection-in-python-opencv
import cv2
import numpy as np
from scipy import ndimage
from matplotlib import pyplot as plt

roberts_cross_v = np.array([[1, 0], [0, -1]])
roberts_cross_h = np.array([[0, 1], [-1, 0]])

#img = cv2.imread("'C:\\Users\\JuanJose\\Pictures\\casa.jpg'", 0).astype('float64')
#img /= 255.0
#img0 = cv2.imread("C:\\Users\\JuanJose\\Pictures\\casa.jpg", )
the_folder = "C:\\Users\\JuanJose\\Documents\\ArticulosMios\\ProyectoManzanas\\Manzanas\\"
the_file = "man_a_8_6.bmp"

img0 = cv2.imread(the_folder+the_file,)

# converting to gray scale
img = cv2.cvtColor(img0, cv2.COLOR_BGR2GRAY)

vertical = ndimage.convolve(img, roberts_cross_v)
horizontal = ndimage.convolve(img, roberts_cross_h)

edged_img = np.sqrt(np.square(horizontal) + np.square(vertical))

plt.subplot (1,2,1),plt.imshow (img,cmap = 'gray')
plt.title ('Original'), plt.xticks ( []), plt.yticks ( [])

plt.subplot (1,2,2),plt.imshow (edged_img,cmap = 'gray')
plt.title ('Roberts'), plt.xticks ( []), plt.yticks ( [])

plt.show ()

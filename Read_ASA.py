import numpy as np
import cv2

asa = np.fromfile('/home/juanjose/Imágenes/APICAL.ASA', dtype='uint8')

cv2.imwrite('api1.png', asa[:62208].reshape(243,256))

asa2 = asa[62208:124416]
# len(asa2)
#62208
cv2.imwrite('api2.png', asa2.reshape(243,256) )
 
asa3 = asa[124416:186624]
cv2.imwrite('api3.png', asa3.reshape(243,256) )

asa4 = asa[186624:248832]
cv2.imwrite('api4.png', asa4.reshape(243,256) )

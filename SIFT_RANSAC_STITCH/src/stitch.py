# FURKAN ŞAHİN 250201042 



import cv2
import numpy as np
import copy


# Loading the images
img0 = cv2.imread('data/goldengate/goldengate-00.png')
img1 = cv2.imread('data/goldengate/goldengate-01.png')
img2 = cv2.imread('data/goldengate/goldengate-02.png')
img3 = cv2.imread('data/goldengate/goldengate-03.png')
img4 = cv2.imread('data/goldengate/goldengate-04.png')
img5 = cv2.imread('data/goldengate/goldengate-05.png')

# Creating an array which holds images
imagesList = [img0, img1, img2, img3, img4, img5]

# Converting images to grayscale
gray0 = cv2.cvtColor(img0, cv2.COLOR_BGR2GRAY)
gray1 = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
gray2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)
gray3 = cv2.cvtColor(img3, cv2.COLOR_BGR2GRAY)
gray4 = cv2.cvtColor(img4, cv2.COLOR_BGR2GRAY)
gray5 = cv2.cvtColor(img5, cv2.COLOR_BGR2GRAY)

# Creating an array which holds grayscales
grayList = [gray0, gray1, gray2, gray3, gray4, gray5]

panoramas = [] # holds created panoramas



def stitch_pictures():
     # deep copy arrays
    copyGrayList = copy.deepcopy(grayList)
    copyImagesList = copy.deepcopy(imagesList)


    # Creates panoramas according to center images image2 and image3 like that :
    # left: image0 + right: image2 = panorama0_2
    # left: image2 + right: image4 = panorama2_4
    # left: image1 + right: image3 = panorama1_3
    # left: image3 + right: image5 = panorama3_5

    for i in range(4):
        # Initiate SIFT detector
        sift = cv2.SIFT_create()
       
        # find key points and descriptor for right image and left image
        kp1, des1 = sift.detectAndCompute(
            copyGrayList[i+2], None)  # right image
        kp2, des2 = sift.detectAndCompute(copyGrayList[i], None)  # left image

        # assign values
        imgLeft = copyImagesList[i]
        grayLeft = copyGrayList[i]
        imgRight = copyImagesList[i+2]
        grayRight = copyGrayList[i+2]

        # get a panorama image which is created using right image and left image
        pano = createPanoramas(imgLeft, imgRight, grayLeft,
                      grayRight, kp1, kp2, des1, des2)
                      
        panoramas.append(pano) # save created panorama to the array
        #cv2.imwrite("data/panorama_"+str(i)+"_"+str(i+2)+".png", pano)

        
        


def stitch_panoramas():

    # Stitch panoramas 
    # left: panorama0_2 + right: panorama1_3 = pano
    # left: panorama2_4 + right: panorama3_5 = pano2
    # left: pano + right: pano2 = panorama


    # Initiate SIFT detector
    sift = cv2.SIFT_create()
    
    # find key points and descriptor for images in panoramas array  holds created panoramas)

    grayLeftPanorama = cv2.cvtColor(panoramas[0], cv2.COLOR_BGR2GRAY) # get panorama0_2 graysacle
    grayRightPanorama = cv2.cvtColor(panoramas[1], cv2.COLOR_BGR2GRAY) # get panorama1_3 grayscale
    kp1, des1 = sift.detectAndCompute(grayRightPanorama, None)  # right image
    kp2, des2 = sift.detectAndCompute(grayLeftPanorama, None)  # left image

    imgLeft = panoramas[0] # get panorama0_2
    grayLeft = grayLeftPanorama 
    imgRight = panoramas[1] # get panorama1_3
    grayRight = grayRightPanorama

    # stitch panorama0_2 and panorama1_3
    pano = createPanoramas(imgLeft, imgRight, grayLeft,grayRight, kp1, kp2, des1, des2)
   


    grayLeftPanorama = cv2.cvtColor(panoramas[2], cv2.COLOR_BGR2GRAY) # get panorama2_4 graysacle
    grayRightPanorama = cv2.cvtColor(panoramas[3], cv2.COLOR_BGR2GRAY) # get panorama3_5 grayscale
    kp1, des1 = sift.detectAndCompute(grayRightPanorama, None)  # right image
    kp2, des2 = sift.detectAndCompute(grayLeftPanorama, None)  # left image

    imgLeft = panoramas[2] # get panorama2_4
    grayLeft = grayLeftPanorama
    imgRight = panoramas[3] # get panorama3_5
    grayRight = grayRightPanorama

    # stitch panorama2_4 and panorama3_5
    pano2 = createPanoramas(imgLeft, imgRight, grayLeft,grayRight, kp1, kp2, des1, des2)



    grayLeftPanorama = cv2.cvtColor(pano, cv2.COLOR_BGR2GRAY) # get pano (0123) graysacle
    grayRightPanorama = cv2.cvtColor(pano2, cv2.COLOR_BGR2GRAY) # get pano2 (2345) graysacle
    kp1, des1 = sift.detectAndCompute(grayRightPanorama, None)  # right image
    kp2, des2 = sift.detectAndCompute(grayLeftPanorama, None)  # left image

    imgLeft = pano # get pano (0123)
    grayLeft = grayLeftPanorama
    imgRight = pano2 # get pano (2345)
    grayRight = grayRightPanorama

    # stitch pano and pano2
    pano3 = createPanoramas(imgLeft, imgRight, grayLeft,grayRight, kp1, kp2, des1, des2)
    cv2.imwrite("data/panorama.png",pano3)



def createPanoramas(imgLeft, imgRight, grayLeft, grayRight, kp1, kp2, des1, des2):

    # Create a BFMatcher object.
    bf = cv2.BFMatcher()

    # Find matching points
    matches = bf.knnMatch(des1, des2,k=2)


    all_matches = []
    for m, n in matches:
        all_matches.append(m)

    # Mark the mathces on the image 
    draw_params = dict(matchColor=(0, 255, 0), singlePointColor=None, flags=2)
    img3 = cv2.drawMatches(imgRight, kp1, imgLeft, kp2,
                           all_matches[:2300], None, **draw_params)

    # Apply ratio test
    good = []
    for m, n in matches:
        if m.distance < 0.70 * n.distance:
            good.append(m)

    # Find Homograhy Matrice
    MIN_MATCH_COUNT = 10
    if len(good) > MIN_MATCH_COUNT:
        src_pts = np.float32(
            [kp1[m.queryIdx].pt for m in good]).reshape(-1, 1, 2)
        dst_pts = np.float32(
            [kp2[m.trainIdx].pt for m in good]).reshape(-1, 1, 2)

        HM, mask = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC, 5.0)
        h, w = grayRight.shape
        pts = np.float32([[0, 0], [0, h-1], [w-1, h-1],
                          [w-1, 0]]).reshape(-1, 1, 2)

        # apply perspective transform                  
        dst = cv2.perspectiveTransform(pts, HM)
        grayLeft = cv2.polylines(
            grayLeft, [np.int32(dst)], True, 255, 3, cv2.LINE_AA)
        #cv2.imwrite("data/original_image_overlapping.png", grayLeft)
    else:
        print("Not enought matches are found - %d/%d",
              (len(good)/MIN_MATCH_COUNT))

    # apply warp process
    dst = cv2.warpPerspective(
        imgRight, HM, (imgLeft.shape[1] + imgRight.shape[1], imgLeft.shape[0]))
    dst[0:imgLeft.shape[0], 0:imgLeft.shape[1]] = imgLeft

    # apply trim
    def trim(frame):
        # crop top
        if not np.sum(frame[0]):
            return trim(frame[1:])
        # crop top
        if not np.sum(frame[-1]):
            return trim(frame[:-2])
        # crop top
        if not np.sum(frame[:, 0]):
            return trim(frame[:, 1:])
        # crop top
        if not np.sum(frame[:, -1]):
            return trim(frame[:, :-2])
        return frame
    return trim(dst)





stitch_pictures()
stitch_panoramas()
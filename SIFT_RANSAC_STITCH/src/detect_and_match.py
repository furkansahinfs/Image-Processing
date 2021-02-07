# FURKAN ŞAHİN 250201042 


import cv2
import matplotlib.pyplot as plt
import numpy as np
import copy
import json



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


# To write values to the txt :
def to_txt():

    # deep copy arrays
    copyGrayList=copy.deepcopy(grayList)
    copyImagesList=copy.deepcopy(imagesList)

    for i in range(6):

        sift = cv2.SIFT_create()
        kp, des = sift.detectAndCompute(copyImagesList[i],None) #get keypoints and descriptor

        keypoints = {}  
        count = 0
        # for each keypoint object in kp array, reach each object and save its parametres in json format.
        # Example = 
        # {"Kp_0": [{"x": 2.360567092895508}, {"y": 802.5349731445312}, {"size": 1.954025149345398}, 
        # {"angle": 209.7701416015625}, {"response": 0.013668788596987724}, {"octave": 6095359}],
        # "Kp_1": [{"x": 2.360567092895508}, {"y": 802.5349731445312}, {"size": 1.954025149345398}, 
        # {"angle": 310.74261474609375}, {"response": 0.013668788596987724}, {"octave": 6095359}] ....}

        for j in kp:
            keypoints['Kp_%d'%count] = []  
            keypoints['Kp_%d'%count].append({'x': j.pt[0]})
            keypoints['Kp_%d'%count].append({'y': j.pt[1]})
            keypoints['Kp_%d'%count].append({'size': j.size})
            keypoints['Kp_%d'%count].append({'angle': j.angle})
            keypoints['Kp_%d'%count].append({'response': j.response})
            keypoints['Kp_%d'%count].append({'octave': j.octave})
            count+=1


        # merge keypoints json and des array in a new json and save that json to txt.
        keypoints_descriptor = {}  
        keypoints_descriptor['value1'] = []  
        keypoints_descriptor['value1'] .append({'des': des.tolist()})
        keypoints_descriptor['value2'] = []  
        keypoints_descriptor['value2'] .append(keypoints)

        with open('data/sift_'+str(i)+'.txt', 'w') as jfile:  
            json.dump(keypoints_descriptor, jfile)




def interest_point():

    # deep copy arrays
    copyGrayList=copy.deepcopy(grayList)
    copyImagesList=copy.deepcopy(imagesList)

    for i in range(6):
      
        # Applying SIFT detector
        sift = cv2.SIFT_create()
        kp = sift.detect(copyGrayList[i], None) #get keypoints 

        # Mark the keypoint on the image 
        img = cv2.drawKeypoints(copyGrayList[i], kp, copyImagesList[i],flags=cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
        cv2.imwrite('data/sift_keypoints_'+str(i)+'.png', img) # save image
        
        
        

def tentative():

    # deep copy arrays
    copyGrayList=copy.deepcopy(grayList)
    copyImagesList=copy.deepcopy(imagesList)

    for i in range(5):

        sift = cv2.SIFT_create()
        kp1, des1 = sift.detectAndCompute(copyImagesList[i],None) #get keypoints and descriptor for image i in array
        kp2, des2 = sift.detectAndCompute(copyImagesList[i+1],None) #get keypoints and descriptor for image i+1 in array

        # create BFMatcher 
        bf = cv2.BFMatcher()
        matches = bf.knnMatch(des1,des2,k=2)

        # Apply ratio test
        good = []
        for m,n in matches:
            if m.distance < 0.5*n.distance:
                good.append(m)

        
        matcherJson = {}  
        count = 0
        # for each matches object in good array, reach each object and save its parametres in json format.
        # Example = 
        # {"Matches_0": [{"distance": 95.0631332397461}, {"trainIdx": 67}, {"queryIdx": 925}, {"imgIdx": 0}], 
        # "Matches_1": [{"distance": 191.3400115966797}, {"trainIdx": 2}, {"queryIdx": 1280}, {"imgIdx": 0}], .... }
        for j in good:
            matcherJson['Matches_%d'%count] = []  
            matcherJson['Matches_%d'%count].append({'distance': j.distance})
            matcherJson['Matches_%d'%count].append({'trainIdx': j.trainIdx})
            matcherJson['Matches_%d'%count].append({'queryIdx': j.queryIdx})
            matcherJson['Matches_%d'%count].append({'imgIdx': j.imgIdx})
            count+=1

        #save matcherJson to txt.
        with open('data/tentative_correspondences_'+str(i)+'-'+str(i+1)+'.txt', 'w') as jfile:  
            json.dump(matcherJson, jfile) 

        # Mark the mathces on the image 
        img3 = cv2.drawMatches(copyImagesList[i], kp1, copyImagesList[i+1], kp2, good, copyImagesList[i+1],flags=2)
        cv2.imwrite('data/tentative_correspondences_'+str(i)+'-'+str(i+1)+'.png', img3) #save image
       


interest_point()
to_txt()
tentative()
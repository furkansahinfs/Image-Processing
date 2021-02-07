# FURKAN ŞAHİN 250201042 


import cv2
import matplotlib.pyplot as plt
import numpy as np
import copy
import json


# Loading the images
img0 = cv2.imread('data/goldengate/goldengate-00.png',0)
img1 = cv2.imread('data/goldengate/goldengate-01.png',0)
img2 = cv2.imread('data/goldengate/goldengate-02.png',0)
img3 = cv2.imread('data/goldengate/goldengate-03.png',0)
img4 = cv2.imread('data/goldengate/goldengate-04.png',0)
img5 = cv2.imread('data/goldengate/goldengate-05.png',0)

# Creating an array which holds images
imagesList = [img0, img1, img2, img3, img4, img5]

# Read kp and des values from saved txt.
def read_kp_des(i):
    kp = []  # holds keypoint objects
    des = [] # holds descriptor

    with open('data/sift_'+str(i)+'.txt') as jfile:  
        data = json.load(jfile)
    index = 1

    while(data.__contains__('value%d'%index)):

        if(data.__contains__('value1')):
            pt = data['value1'][0]['des']
            des = pt
        
        kp_index = 0

        if(data.__contains__('value2')):
            while(data['value2'][0].__contains__('Kp_%d'%kp_index)):
                pt = cv2.KeyPoint(x=data['value2'][0]['Kp_%d'%kp_index][0]['x'],y=data['value2'][0]['Kp_%d'%kp_index][1]['y'], _size=data['value2'][0]['Kp_%d'%kp_index][2]['size']
                ,_angle=data['value2'][0]['Kp_%d'%kp_index][3]['angle'],_response=data['value2'][0]['Kp_%d'%kp_index][4]['response'],_octave=data['value2'][0]['Kp_%d'%kp_index][5]['octave'])
                kp.append(pt)
                kp_index+=1

        index+=1

    return (kp,des)

# Read matches values from saved txt 
def read_matches(i,j):
    matches = []   # holds matches objects.
    with open('data/tentative_correspondences_'+str(i)+'-'+str(j)+'.txt') as jfile:  
        data = json.load(jfile)
    index = 0
    while(data.__contains__('Matches_%d'%index)):
        pt = cv2.DMatch(_distance=data['Matches_%d'%index][0]['distance'],_trainIdx=data['Matches_%d'%index][1]['trainIdx'],
        _queryIdx=data['Matches_%d'%index][2]['queryIdx'], _imgIdx=data['Matches_%d'%index][3]['imgIdx'])
        matches.append(pt)
        index+=1
    return matches


def ransac():
    # deep copy array
    copyImagesList=copy.deepcopy(imagesList)

    for i in range(5):
        # Initiate SIFT detector
        sift = cv2.SIFT_create()
        kp1,des1 = read_kp_des(i) # get kp,des values from sift_i.txt for image i in array
        kp2,des2 = read_kp_des(i+1) # get kp,des values from sift_i.txt for image i+1 in array
        good = read_matches(i,i+1) # get matches values from tentative_correspondences_i-j.txt 

        MIN_MATCH_COUNT = 10
        # Find Homograhy Matrice
        if len(good)>MIN_MATCH_COUNT:
            src_pts = np.float32([ kp1[m.queryIdx].pt for m in good ]).reshape(-1,1,2)
            dst_pts = np.float32([ kp2[m.trainIdx].pt for m in good ]).reshape(-1,1,2)

            HM, mask = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC,5.0)
            matchesMask = mask.ravel().tolist()

            # Save Homography Matrice to txt in json format.
            homographyMatrices = {}  
            homographyMatrices['h_'+str(i)+'-'+str(i+1)] = []  
            homographyMatrices['h_'+str(i)+'-'+str(i+1)] .append({'matrice': HM.tolist()})

            with open('data/h_'+str(i)+'-'+str(i+1)+'.txt', 'w') as jfile:  
                json.dump(homographyMatrices, jfile)

            # apply perspective transform
            h,w = copyImagesList[i].shape
            pts = np.float32([ [0,0],[0,h-1],[w-1,h-1],[w-1,0] ]).reshape(-1,1,2)
            dst = cv2.perspectiveTransform(pts,HM)

            # place poly lines on image
            img2 = cv2.polylines(copyImagesList[i+1],[np.int32(dst)],True,255,3, cv2.LINE_AA)

        else:
            print(i)
            print("Not enough matches are found - %d/%d" % (len(good),MIN_MATCH_COUNT))
            matchesMask = None               
      
        
        # Mark the mathces on the image 
        draw_params = dict(matchColor = (0,255,0), # draw matches in green color
                        singlePointColor = None,
                        matchesMask = matchesMask, # draw only inliers
                        flags = 2)

        img3 = cv2.drawMatches(copyImagesList[i],kp1,copyImagesList[i+1],kp2,good,None,**draw_params)
        cv2.imwrite('data/inliers_'+str(i)+'-'+str(i+1)+'.png', img3) # save image

        # Save inlier object to the txt in json format
        inliers = {}  
        inliers['inlier_'+str(i)+'-'+str(i+1)] = []  
        inliers['inlier_'+str(i)+'-'+str(i+1)] .append({'inlier': draw_params})

        with open('data/inliers_'+str(i)+'-'+str(i+1)+'.txt', 'w') as jfile:  
            json.dump(inliers, jfile)




ransac()
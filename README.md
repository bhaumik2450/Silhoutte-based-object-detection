# Silhoutte-based-object-detection

This repository contains the code for the paper titled "Silhouette-Based Real-Time Object Detection and Tracking" available at following link:
https://link.springer.com/chapter/10.1007/978-981-10-7895-8_16

# Description
Object detection and tracking in the video sequence is a challenging task and time-consuming process. Intrinsic factors like pose, appearance, variation in scale and extrinsic factors like variation in illumination, occlusion, and clutter are major challenges in object detection and tracking. The main objective of the tracking algorithm is accuracy and speed in each frame. We propose the best combination of detection and tracking algorithm which performs very efficiently in real time. In proposed algorithm, object detection task is performed from given sketch using Fast Directional Chamfer Matching (FDCM) which is capable of handling some amount of deformation in edges. To deal with the articulation condition, part decomposition algorithm is used in the proposed algorithm. Combination of these two parts is capable enough to handle deformation in shape automatically. Amount of time taken to perform this algorithm depends on the size and edge segment in the input frame. For object tracking, Speeded up Robust Features (SURF) algorithm is used because of its rotation invariant and fast performance features. The proposed algorithm works in all situations without the prior knowledge about number of frames.

# Keywords
Convexity defects
Fast directional chamfer matching 
Part decomposition 
Speeded up robust features 

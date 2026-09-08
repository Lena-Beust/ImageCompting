_INSA RENNES - Computer Science - Semester 8_
# Images and videos computing : Project

## Objectives
This project aims to create an application capable of identifying objects in images from the Niclcon base. The objects to be recognized are pictograms drawn by hand by people in charge of managing emergency services during large-scale operations. The pictograms are gathered in forms, which are scanned. Each form contains 22 sheets, and each gathers 35 drawings refering to 14 kinds of pictograms.
The main goal is to extract the pictograms and, for each, create two files : 
- the snippet of the pictograme -> .png file
- the informations about the pictograme : its associated label, its size, its scripter, its identifier and the number of the page in the form. There are 14 labels (e.g. fire, road, warning...) and 3 differents sizes (small, medium, large) -> .txt file

## Used methods
Differents essentials steps are requiered to achieve this project. The main ones are the following :
1. Warpping : The sheet aren't supposed to be perfectly scanned. We assumed that we have to redress them.
2. Detection of the code of the sheet : Each sheet contains a zone that refers to the identifier of the sheet, coded in binary. This zone has to be detected, analyzed and converted.
3. Detection of the label : In the left side of each sheet, the label of the associated drawings can be found.
4. Detection of the size : Just as the label, the size of the pictograms has to be recognize.
5. Extraction of the drawings : Finally, the hand drawn pictograms are extrated.
6. Construction of the .png and .txt files for each drawing.

### Warpping
The sheet contains two crosses each, if it was well scan. To redress the images, we choose to detect them and rotate the sheet. Indeed, the main concern about the scanned forms is that they are lightly rotated. 

With the function *matchTemplate* from OpenCV, the two crosses are detected and we can create a line between them. A target line, from a perfect scanned sheet, is useful to calculate the angle between these two lines. The sheet is rotated according this specific angle. 

To enhance the search and detection of the cross, we restrain the area of searching to the two corner, where the crosses are expected to be. They are in the right upper and the left down corners. The methods acts like a mask to ignore the useless parts of the sheet.

		┌────────────────────────────┐
		│ ✗✗✗✗✗     ✗✗✗✗✗   ✓✓✓✓✓    │ 
		│ ✗✗✗✗✗     ✗✗✗✗✗   ✓✓✓✓✓    │
		│ ✗✗✗✗✗     ✗✗✗✗✗   ✓✓✓✓✓    │
		│                            │
		│ ✗✗✗✗✗    ✗✗✗✗✗    ✗✗✗✗     │ 
		│ ✗✗✗✗✗   ✗✗✗✗✗     ✗✗✗✗     │
		│                            │
		│ ✓✓✓✓✓    ✗✗✗✗✗   ✗✗✗✗✗     │
		│ ✓✓✓✓✓    ✗✗✗✗✗   ✗✗✗✗✗     │
		│ ✓✓✓✓✓    ✗✗✗✗✗   ✗✗✗✗✗     │
		└────────────────────────────┘
		✗ : ignored area
		✓ : searched area

Sometimes, only one cross is detected. In that case, we adapt the rotation using the center and the wanted angle with the target line.

This step of the projet take the unchanged images of the scanned sheet as entries and return the redresses images, less each last sheet of the forms. Indeed, a form contains 21 sheets with the hand drawn pictograms and a last one that we don't have to analyse.

### Extract Drawing 
The retriveSymbol class is responsible for automatically detecting and extracting drawings from an input image.
Its role in the global project is to locate the square boxes containing drawings, extract each drawing individually, and return them in a clean and ordered form so they can later be classified by icon type.

#### Preprocessing
To make box detection easier, the image is simplified in several steps :
- Blue color removal
  The image is converted to HSV color space and blue areas are removed.  
  This step removes the drawings themselves, which may overlap the box borders, allowing clearer detection of the boxes.
- Grayscale conversion  
  The cleaned image is converted to grayscale.
- Noise reduction 
  A Gaussian blur is applied to reduce noise and small artifacts.
- Edge detection  
  Canny edge detection is used to highlight the contours of the boxes.
- Morphological operations  
  Dilation and morphological closing are applied to strengthen and close the box contours, producing well-defined shapes.
#### Square Detection
The `detect_squares()` method analyzes the processed image to find valid boxes :
- Contours are detected using `findContours`
- Each contour is filtered using the following criteria:
    - Area must be large enough to be a box
    - Area must not be too large**, otherwise it is considered a text zone and the image is marked invalid
    - The contour must have 4 vertices
    - The width/height ratio must be close to 1 (square-like shape)
      Invalid or irrelevant shapes are discarded.
#### Drawing Extraction
For each valid square :
- A bounding rectangle is computed
- A small margin is removed to avoid capturing the box borders
- The region of interest is extracted
- Each extracted drawing is stored along with its position


#### Sorting
Extracted drawings are sorted :
1. From top to bottom (Y coordinate)
2. From left to right (X coordinate)


## Performances results
The performances of ours algorithms can be quantitatively and qualitatively studies, on the inital and final data bases.

### Initial data base
The inital base contains 804 scanned sheet from 34 differents scripters. Only 770 sheet have to consider. The others are not sheet with hand drawn pictograms.

For the first step, the warpping, the 770 sheet have been analyzed and only one was considered not well redressed. Indeed, the chosen treatment was to rotate the sheet. One of them requiered a translation instead of a rotation though.


